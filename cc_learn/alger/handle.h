/**/

template <class T>
class Handle {
	T* ptr;
	int* refcount;
public:
	Handle(T* t) : ptr(t), refcount(new int(1)) {}
	Handle(const Handle& r) : ptr(r.ptr), refcount(r.refcount)
		{ ++(*refcount); }
	T& operator=(const Handle& r) {
		if (ptr==r.ptr) return *this;
		if (--(*refcount)==0) {
			delete ptr;
			delete refcount;
		}
		++(*r.refcount);
		refcount=r.refcount;
		ptr=r.ptr;
		return *this;
	}
	~Handle() { if (--(*refcount)==0) {delete ptr; delete refcount;} }
};

/**/
