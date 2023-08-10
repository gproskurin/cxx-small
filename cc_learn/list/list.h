/*
*/

template<class T>
class List {
	struct Node {
		T data;
		Node* next;
		Node* prev;
	};
	Node* head;
	Node* tail;
	bool empty() const;
	void node_insert_tail(Node*);
	void node_insert_head(Node*);
	void node_remove_first();
	void node_remove_last();
	void node_remove(Node*);

public:
	void insert_tail(const T&);
	void insert_head(const T&);
	void print() const;
	void sort();
	void reverse();
	List();
	~List();
};

/**/
