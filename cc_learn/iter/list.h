/**/

template <class T>
class List {
	struct Node {
		Node *next;
		T item;
	};
	Node *head;
public:
	class Iterator {
		Node* pos;
		Iterator(Node* n) : pos(n) {}
	public:
		Iterator() : pos(0) {}
		T& operator*() { return pos->item; }
		T* operator->() { return &pos->item; }
		Iterator& operator=(Iterator* i) { pos=i->pos; return *this; }
		Iterator& operator=(Node* node) { pos=node; return *this; }
		Iterator& operator++() { pos=pos->next; return *this; }
		Iterator operator++(int) {
			Node* old=pos;
			pos=pos->next;
			return Iterator(old);
		}
		operator Node*() { return pos; }
	};

	Node* first() { return head; }
	void insert_first(const T& t) {
		Node* node = new Node;
		node->next = head;
		node->item = t;
		head = node;
	}
	bool empty() { return head==0; }
	List() : head(0) {}
	~List() {}
};

/**/
