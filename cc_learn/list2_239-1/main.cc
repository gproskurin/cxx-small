/*
*/

#include <string>
#include <iostream>

#include "list.h"

using namespace DoubleList;

int main(int argc, char *argv[])
{
	List* lst = create();
	for (int i=1; i<argc; ++i) {
		insert_tail(lst, argv[i]);
	}

	print(lst);
	reverse(lst);
	print(lst);
	sort(lst);
	print(lst);

	destroy(lst);
}
