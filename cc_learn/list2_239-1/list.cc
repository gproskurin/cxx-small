/*
*/

#include <iostream>
#include <stdlib.h>

#include "list.h"

namespace DoubleList {

List *create()
{
	List* new_list = new List;
	new_list->head = new_list->tail = 0;
	return new_list;
}

void print(const List* pl)
{
	for (Node* node = pl->head; node!=0; node=node->next)
		std::cout << node->data << '\n';
	std::cout << "---\n";
}

void node_remove_first(List* pl)
{
	if (empty(pl)) {
		std::cerr << "remove_first() from empty list\n";
		exit(1);
	}
	if (pl->head==pl->tail) { // if size==1
		pl->head = pl->tail = 0;
		return;
	}
	pl->head = pl->head->next;
	pl->head->prev = 0;
}

void node_remove_last(List* pl)
{
	if (empty(pl)) {
		std::cerr << "remove_last() from empty list\n";
		exit(1);
	}
	if (pl->head==pl->tail) { // if size==1
		pl->head = pl->tail = 0;
		return;
	}
	pl->tail = pl->tail->prev;
	pl->tail->next = 0;
}

void node_remove(List* pl, Node* elm)
{
	if (empty(pl) || elm==0) {
		std::cerr << "remove() from empty list\n";
		exit(1);
	}

	if (elm==pl->head) {
		node_remove_first(pl);
		return;
	}
	if (elm==pl->tail) {
		node_remove_last(pl);
		return;
	}

	Node* before = elm->prev;
	Node* after = elm->next;
	before->next = after;
	after->prev = before;
}

void node_insert_tail(List *pl, Node* elm)
{
	elm->next=0;
	elm->prev=pl->tail;

	if (empty(pl)) {
		pl->head = pl->tail = elm;
	} else {
		pl->tail->next=elm;
		pl->tail=elm;
	}
}

void insert_tail(List *pl, const std::string& str)
{
	Node* new_node = new Node;
	new_node->data=str;
	node_insert_tail(pl, new_node);
}

void node_insert_head(List *pl, Node* elm)
{
	elm->prev=0;
	elm->next=pl->head;
	if (empty(pl)) {
		pl->head = pl->tail = elm;
	} else {
		pl->head->prev = elm;
		pl->head = elm;
	}
}

void insert_head(List *pl, const std::string& str)
{
	Node* new_node = new Node;
	new_node->data=str;
	node_insert_head(pl, new_node);
}

bool empty(const List *pl)
{
	if (pl->head==0 && pl->tail==0)
		return true;
	if (pl->head!=0 && pl->tail!=0)
		return false;
	std::cerr << "empty() error\n";
	exit(1);
}

void sort(List *pl)
{
	if (pl->head==pl->tail)	// empty or size==1
		return;

	// ALG
	// 1. find minimum
	// 2. remove minimum
	// 3. sort remaining list
	// 4. insert minimum to head

	Node* min = pl->head;
	for (Node* curr=pl->head->next; curr!=0; curr=curr->next) {
		if (curr->data < min->data)
			min = curr;
	}
	node_remove(pl, min);
	sort(pl);
	node_insert_head(pl, min);
}

void reverse_swap_pointers(List *pl)
{
	if (pl->head==pl->tail) // empty or size==1
		return;

	Node* curr=pl->head;
	while (curr) {
		// swap next and prev pointers in node
		Node* tmp = curr->prev;
		curr->prev = curr->next;
		curr->next = tmp;

		curr = curr->prev;	// next of unswapped pointers
	}
	// swap head and tail
	Node* tmp = pl->head;
	pl->head = pl->tail;
	pl->tail = tmp;
}

void reverse_recursive(List *pl)
{
	if (pl->head==pl->tail) // empty or size==1
		return;

	// ALG:
	// 1. remove(not destroy) first node
	// 2. reverse remaining list
	// 3. insert first node (removed in [1]) to tail

	Node* first = pl->head;
	node_remove_first(pl);
	reverse(pl);
	node_insert_tail(pl, first);
}

void reverse(List *pl)
{
	//reverse_recursive(pl);
	reverse_swap_pointers(pl);
}

void destroy(List *pl)
{
	Node* node=pl->head;
	while (node) {
		Node* old_node = node;
		node = node->next;
		delete old_node;
	}
	delete pl;
}

} //namespace DoubleList
/**/
