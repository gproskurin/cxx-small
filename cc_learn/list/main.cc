/*
*/

#include <string>

#include "list.h"
#include "locks.h"

int main(int argc, char *argv[])
{
	Lock* lock = new Pthread_Lock_RW;
	List<std::string> lst;
	for (int i=1; i<argc; ++i) {
		lock->x_lock();
		lst.insert_tail(argv[i]);
		lock->unlock();
	}

	lock->s_lock();
	lst.print();
	lock->unlock();

	lst.reverse();
	lst.print();
	lst.sort();
	lst.print();
}
