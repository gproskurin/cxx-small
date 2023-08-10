#include <string>
#include <iostream>

#include "list.h"

using namespace std;

int main(int argc, char *argv[])
{
	List<string> lst;
	for(int i=1; i<argc; ++i) {
		lst.insert_first(argv[i]);
	}

	List<string>::Iterator i;
	i = lst.first();
	while(i!=0) {
		cout << *(i++) << '\n';
	}
}
