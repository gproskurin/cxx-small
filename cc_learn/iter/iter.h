/**/

template <class T>
class Iterator {
public:
	virtual T* first() = 0;
	virtual T* next() = 0;
	virtual bool more() = 0;
	virtual T* operator*() = 0;
	virtual T* operator->() = 0;
};

/**/
