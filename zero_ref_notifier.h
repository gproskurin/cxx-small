/*-
 * Copyright (c) Gennady Proskurin <gpr@mail.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef zero_ref_notifier_h
#define zero_ref_notifier_h

#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>

/*

Usage example:

// create object itself
zero_ref_notifier_t::self_ptr_t zp = zero_ref_notifier_t::create();

// pass references to different handlers
boost::create_thread(thr_func, zp->get_ref_object());
boost::asio::async_read(..., boost::bind(..., zp->get_ref_object()));

zp->wait();
// After wait(), it is garanteed that all objects created by ref_object_type()
// are destroyed.  It means, for example, that all threads are completed
// (joined), all boost::asio handlers are invoked (or destroyed).


Thread safety: the same as for shared_ptr.
- simultaneous call to ref_object_type() is safe (it just creates copies of internal shared_ptr)
- ref_object_type objects (returned by get_ref_object()) are safe to copy by multiple threads
- simultaneous call to wait() and get_ref_object() is not safe, since wait() calls reset() for shared_ptr
For details, see boost documentation about shared_ptr thread safety.

Warning:
wait() and timed_wait() functions reset internal state of class, so it cannot
be used to generate reference objects anymore, i.e. function get_ref_object()
cannot be called.

Implementation:
For reference counting, shared_ptr is used with custom destructor. When custom
destructor is called, it means that reference count is zero, and we do
notification in this case.

*/

class zero_ref_notifier_t : public boost::enable_shared_from_this<zero_ref_notifier_t> {
	struct notify_data_t;
	typedef boost::shared_ptr<notify_data_t> notify_data_ptr_t;

public:
	typedef boost::shared_ptr<zero_ref_notifier_t> self_ptr_t;

	// reference object (of type ref_object_type) references two things:
	// 1. this object itself (shared_ptr<zero_ref_notifier_t>), to prevent it to
	// be destroyed before all reference objects
	// 2. internal shared_ptr (notify_data_ptr_), used as reference counter for reference objects
	typedef std::pair< self_ptr_t, notify_data_ptr_t > ref_object_type;

	ref_object_type get_ref_object() { return ref_object_type(shared_from_this(), notify_data_ptr_); }

	// block until all notify_data_ptr_t components of ref_object_type disappear
	void wait();

	template <typename TimeType>
	bool timed_wait(const TimeType&);

	static self_ptr_t create() { return self_ptr_t(new zero_ref_notifier_t); }

private:
	zero_ref_notifier_t() : notify_data_ptr_(&notify_data_, &notify_data_t::notify) {}

	struct notify_data_t {
		boost::mutex mtx_;
		boost::condition_variable cv_;
		bool it_happened_;

		notify_data_t() : it_happened_(false) {}
		static void notify(notify_data_t*);
	};
	notify_data_t notify_data_;
	notify_data_ptr_t notify_data_ptr_;
};

// This is custom destructor for shared_ptr. It just notifies.
inline
void zero_ref_notifier_t::notify_data_t::notify(notify_data_t* p)
{
	boost::mutex::scoped_lock lk(p->mtx_);
	p->it_happened_ = true;
	p->cv_.notify_all();
}

inline
void zero_ref_notifier_t::wait()
{
	// Destroy our copy, so only copies returned by
	// get_ref_object() remain somewhere.
	// When all that copies are destroyed, custom destructor is called
	// (notify_data_t::notify) which do notification.
	notify_data_ptr_.reset();

	// wait for notification
	boost::mutex::scoped_lock lk(notify_data_.mtx_);
	while (!notify_data_.it_happened_)
		notify_data_.cv_.wait(lk);
}

template <typename TimeType>
bool zero_ref_notifier_t::timed_wait(const TimeType& tm)
{
	notify_data_ptr_.reset();

	boost::mutex::scoped_lock lk(notify_data_.mtx_);
	if (notify_data_.it_happened_)
		return true;
	notify_data_.cv_.timed_wait(lk, tm);
	return notify_data_.it_happened_;
}

#endif
