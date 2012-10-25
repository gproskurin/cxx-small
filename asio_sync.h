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

#ifndef asio_sync_h
#define asio_sync_h

#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/mpl/bool_fwd.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include "zero_ref_notifier.h"

/*

Used for running async boost::asio operations and wait for completion, with
optional timeout. Also can run specified function periodically (in caller's
thread) while async operation is in progress.

*/

namespace asio_sync {

	typedef boost::function< void() > periodical_func_t;
	const boost::posix_time::time_duration default_periodical_time = boost::posix_time::seconds(1);

	template <typename Socket, typename Op>
	void run_op(
		boost::system::error_code& ec, // result error code for operation
		Socket& sock,
		const Op&, // instance of one of the following classes (op_read, op_write, ...)
		const boost::posix_time::time_duration&, // timeout for the whole operation
		const periodical_func_t& = 0, // periodical function, disabled if zero
		const boost::posix_time::time_duration& = default_periodical_time // period for calling periodical function
	);


	// "operation" classes. Used to start async operation. Only function async_start is used in run_op()

	class op_read {
		const boost::asio::mutable_buffer& buf_;
	public:
		typedef boost::mpl::true_ handler_has_bytes_transferred;

		op_read(const boost::asio::mutable_buffer& buf) : buf_(buf) {}

		template <typename Socket, typename Handler>
		void async_start(Socket& sock, const Handler& h) const
		{
			boost::asio::async_read(sock, boost::asio::buffer(buf_), h);
		}
	};

	class op_wait_read {
	public:
		typedef boost::mpl::true_ handler_has_bytes_transferred;

		template <typename Socket, typename Handler>
		void async_start(Socket& sock, const Handler& h) const
		{
			sock.async_read_some(boost::asio::null_buffers(), h);
		}
	};

	class op_write {
		const boost::asio::const_buffer& buf_;
	public:
		typedef boost::mpl::true_ handler_has_bytes_transferred;

		op_write(const boost::asio::const_buffer& buf) : buf_(buf) {}

		template <typename Socket, typename Handler>
		void async_start(Socket& sock, const Handler& h) const
		{
			boost::asio::async_write(sock, boost::asio::buffer(buf_), h);
		}
	};

	template <typename Proto>
	class op_connect {
		typedef typename Proto::endpoint endpoint_t;
		const endpoint_t ep_;
	public:
		typedef boost::mpl::false_ handler_has_bytes_transferred;
		typedef typename boost::asio::basic_stream_socket<Proto> socket_t;

		op_connect(const endpoint_t& ep) : ep_(ep) {}

		typename endpoint_t::protocol_type protocol() const { return ep_.protocol(); }

		template <typename Socket, typename Handler>
		void async_start(Socket &sock, Handler h) const
		{
			BOOST_MPL_ASSERT(( boost::is_same<Socket, socket_t> ));
			sock.async_connect(ep_, h);
		}
	};

	typedef boost::optional<boost::system::error_code> opt_ec_t;

	inline
	void set_ec(
			opt_ec_t& ec_res,
			const boost::system::error_code& ec,
			zero_ref_notifier_t::ref_object_type)
	{
		ec_res = ec;
	}

	template <typename Item>
	void set_ec_and_cancel_item(
			opt_ec_t& ec_res,
			const boost::system::error_code& ec,
			Item& item,
			zero_ref_notifier_t::ref_object_type)
	{
		ec_res = ec;
		boost::system::error_code cancel_ec; // ignored
		item.cancel(cancel_ec);
	}

	// special version of run_op() for op_connect/stream_descriptor
	template <typename Proto>
	void run_op(
			boost::system::error_code& ec,
			boost::asio::posix::stream_descriptor& sd,
			const op_connect<Proto>& op,
			const boost::posix_time::time_duration& timeout,
			const periodical_func_t& periodical_func = 0,
			const boost::posix_time::time_duration& periodical_time = default_periodical_time)
	{
		typename op_connect<Proto>::socket_t sock(sd.get_io_service());
		const int new_fd = ::dup(sd.native_handle());
		if (new_fd == -1) {
			ec.assign(errno, boost::system::system_category());
			return;
		}
		sock.assign(op.protocol(), new_fd);
		run_op(ec, sock, op, timeout, periodical_func, periodical_time);
	}

	namespace detail {

		typedef boost::function<void()> void_func_t;

		inline
		void run_2_func(void_func_t f1, void_func_t f2)
		{
			f1();
			f2();
		}

	}

	// How it works if timer is used.
	// Initially, we start two async requests: wait for timer and operation
	// itself. In handlers, each request cancels other request. If one request
	// finishes before other starts, cancelling of not started request is
	// no-op. For example, if timer expires before async operation is started,
	// timer handler tries to cancel operation (no-op), so operation will be
	// started without timer. To prevent this, we start both requests in one
	// task, posted to io_service via strand and use the same strand for
	// handlers. If we are sure than only one thread does run() for io_service,
	// strand is not created, we use "implicit strand".

	// For now, strand is not necessary, since only one thread does run() for io_service.
	const bool use_strand = false;

	template <typename Socket, typename Op>
	void run_op(
			boost::system::error_code& ec,
			Socket& sock,
			const Op& op,
			const boost::posix_time::time_duration& timeout,
			const periodical_func_t& periodical_func,
			const boost::posix_time::time_duration& periodical_time)
	{
		zero_ref_notifier_t::self_ptr_t handlers_waiter = zero_ref_notifier_t::create();
		boost::asio::io_service& io = sock.get_io_service();
		boost::asio::deadline_timer timer_timeout(io);
		opt_ec_t ec_op, ec_tm;

		boost::scoped_ptr<boost::asio::io_service::strand> strand_ptr;

		try {
			const bool use_timer = !timeout.is_special();
			if (use_timer) {
				timer_timeout.expires_from_now(timeout);

				if (use_strand)
					strand_ptr.reset(new boost::asio::io_service::strand(io));

				typedef boost::function< void(const boost::system::error_code&) > timer_handler_t;

				typedef typename boost::mpl::if_<
					typename Op::handler_has_bytes_transferred, 
						boost::function < void (const boost::system::error_code&, size_t) >,
						boost::function < void (const boost::system::error_code&) >
					>::type op_handler_t;

				const timer_handler_t timer_handler = boost::bind(
					&set_ec_and_cancel_item<Socket>,
					boost::ref(ec_tm),
					boost::asio::placeholders::error,
					boost::ref(sock),
					handlers_waiter->get_ref_object()
				);

				const op_handler_t op_handler = boost::bind(
					&set_ec_and_cancel_item<boost::asio::deadline_timer>,
					boost::ref(ec_op),
					boost::asio::placeholders::error,
					boost::ref(timer_timeout),
					handlers_waiter->get_ref_object()
				);

				const detail::void_func_t tm_starter = boost::bind(
					&boost::asio::deadline_timer::async_wait<timer_handler_t>,
					&timer_timeout,
					strand_ptr ? strand_ptr->wrap(timer_handler) : timer_handler 
				);

				const detail::void_func_t op_starter = boost::bind(
					&Op::template async_start<Socket, op_handler_t>,
					&op,
					boost::ref(sock),
					strand_ptr ? strand_ptr->wrap(op_handler) : op_handler
				);

				if (strand_ptr)
					strand_ptr->post(boost::bind(&detail::run_2_func, tm_starter, op_starter));
				else
					io.post(boost::bind(&detail::run_2_func, tm_starter, op_starter));

			} else {
				op.async_start(
					sock,
					boost::bind(
						&set_ec,
						boost::ref(ec_op),
						boost::asio::placeholders::error,
						handlers_waiter->get_ref_object()
					)
				);
			}

			// wait
			if (periodical_func) {
				while(!handlers_waiter->timed_wait(periodical_time)) {
					periodical_func();
				}
			} else {
				handlers_waiter->wait();
			}


			// deal with errors

			assert(ec_op);
			assert(!use_timer || ec_tm);

			// no timer - just return ec_op as error code
			// OR
			// operation was not canceled (error in operation itself or just success) - ec_op is error code
			if (!use_timer || ec_op.get()!=boost::asio::error::operation_aborted) {
				ec = ec_op.get();
				return;
			}

			// operation was cancelled
			ec = (
				ec_tm.get() ?
					ec_tm.get() : // some error in timer, return it as operation error
					boost::asio::error::timed_out // no error in timer, return timeout
			);
		} catch (...) {
			handlers_waiter->wait();
			throw;
		}
	}

} // namespace asio_sync

#endif
