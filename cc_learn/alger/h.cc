#include "handle.h"

class Big {
	long double arr[1000000];
};

int main()
{
	for(;;) {
		{ Handle<Big> h(new Big);
			Big* bp = new Big;
		  Handle<Big> h1(bp);
		}
	}
}
