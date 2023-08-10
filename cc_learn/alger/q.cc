/**/

#include <stdlib.h>
#include <list>
#include <iostream>

using namespace std;

class Int {
	int value;
public:
	Int(const int i) : value(i) {}
	operator int() const { return value; }
	void set(const int i) { value=i; }
	int get() { return value; }
	//bool operator<(const Int& i) { return value<i.value; }
};

#include "mp.h"

int main(int argc, char *argv[])
{
	typedef MP<Int> IntPtr;
	list<IntPtr> lst;
	for (int i=1; i<argc; ++i) {
		Int n = atoi(argv[i]);
		IntPtr mp(n);
		//mp->set(n);
		lst.push_back(mp);
	}

	lst.sort();
	for (list<IntPtr>::const_iterator i=lst.begin(); i!=lst.end(); ++i) {
		IntPtr mp = *i;
		cout << *mp << '\n';
	}
}

/**/
