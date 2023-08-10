/*
*/

#include <iostream>

#include "list.h"

template<class T>
List<T>::List(): head(0), tail(0)
{
}

template<class T>
List<T>::~List()
{
	while (!empty()) {
		Node* first = head;
		node_remove_first();
		delete first;
	}
}

template<class T>
bool List<T>::empty() const
{
	if (head==0 && tail==0)
		return true;
	if (head!=0 && tail!=0)
		return false;
	throw("empty(): error");
}

template<class T>
void List<T>::print() const
{
	for (Node* curr = head; curr!=0; curr=curr->next) {
		std::cout << curr->data << '\n';
	}
	std::cout << "---\n";
}

template<class T>
void List<T>::node_insert_tail(Node* node)
{
	node->next = 0;
	node->prev = tail;
	if (tail!=0)
		tail->next = node;
	tail = node;
	if (head==0)
		head = tail;
}

template<class T>
void List<T>::node_insert_head(Node* node)
{
	node->prev = 0;
	node->next = head;
	if (head!=0)
		head->prev = node;
	head = node;
	if (tail==0)
		tail = head;
}

template<class T>
void List<T>::insert_tail(const T& info)
{
	Node* node = new Node;
	node->data = info;
	node_insert_tail(node);
}

template<class T>
void List<T>::insert_head(const T& info)
{
	Node* node = new Node;
	node->data = info;
	node_insert_head(node);
}

template<class T>
void List<T>::node_remove_first()
{
	if (head==tail) {
		head = tail = 0;
		return;
	}
	head = head->next;
	head->prev = 0;
}

template<class T>
void List<T>::node_remove_last()
{
	if (head==tail) {
		head = tail = 0;
		return;
	}
	tail = tail->prev;
	tail->next = 0;
}

template<class T>
void List<T>::node_remove(Node* node)
{
	if (node==head) {
		node_remove_first();
		return;
	}
	if (node==tail) {
		node_remove_last();
		return;
	}
	Node* before = node->prev;
	Node* after = node->next;
	before->next = after;
	after->prev = before;
}

template<class T>
void List<T>::sort()
{
	if (head==tail)	// size==1 or empty
		return;
	Node* min = head;
	for (Node* curr=head->next; curr!=0; curr=curr->next) {
		if (curr->data < min->data)
			min = curr;
	}
	node_remove(min);
	sort();
	node_insert_head(min);
}

template<class T>
void List<T>::reverse()
{
	if (head==tail)
		return;
	Node* first = head;
	node_remove_first();
	reverse();
	node_insert_tail(first);
}

/**/
