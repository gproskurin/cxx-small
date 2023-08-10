/*
*/

#include <string>

namespace DoubleList {
	// implementation
	struct Node {
		std::string data;
		Node *next;
		Node *prev;
	};

	// interface
	struct List {
		Node *head;
		Node *tail;
	};

	List *create();
	void destroy(List*);
	void print(const List*);
	void reverse(List*);
	bool empty(const List*);
	void insert_tail(List*, const std::string&);
	void insert_head(List*, const std::string&);
	void sort(List*);

	// exceptions
	class Error {};
};

/**/
