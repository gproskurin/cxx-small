/**/

template <class T>
class MP {
	T* ptr;
public:
	MP() : ptr(new T) {}
	MP(const MP& mp) : ptr(new T(*mp.ptr)) {}
	MP& operator=(const MP& mp) {
		if (&mp != this) {
			delete ptr;
			ptr = new T(*mp.ptr);
		}
		return *this;
	}
	T* operator->() const { return ptr; }
	const T& operator*() const { return *ptr; }
	~MP() { delete ptr; }
};

/**/
