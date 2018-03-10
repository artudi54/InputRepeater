#include "stdafx.h"
#include "InputRepeater.hpp"

#include <iostream>

struct Node{
	int val = 0;
	Node *next = nullptr;
	Node* add(int val) {
		next = new Node;
		next->val = val;
		return next;
	}
	void print() {
		const Node *me = this;
		while (me != nullptr) {
			std::cout << me->val << ' ';
			me = me->next;
		}
		std::cout << '\n';
	}
};

Node* getmax(Node *head) {
	Node prev;
	prev.next = head;
	Node *prevMax = &prev;
	for (Node *it = &prev; it->next != nullptr; it = it->next)
		if (it->next->val > prevMax->next->val)
			prevMax = it;
	Node *max = prevMax->next;
	if (head->next != max) {
		std::swap(head->next, max->next);
		prevMax->next = head;
	}
	else {
		head->next = max->next;
		max->next = head;
	}
	return max;
}

Node *ssort(Node *head) {
	Node prev;
	prev.next = head;
	for (Node *it = &prev; it->next != nullptr; it = it->next) {
		Node *max = getmax(it->next);
		it->next = max;
	}
	return prev.next;
}

void print(int(&tab)[4]) {
	return;
}


//todo: review noexcept
int main(int argc, char *argv[]) {
	QApplication application(argc, argv);
	InputRepeater window;
	window.show();
	return application.exec();
}
