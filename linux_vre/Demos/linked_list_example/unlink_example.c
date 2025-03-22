#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#define DATASZ	0x10


struct listentry {
	struct listentry* next;
	struct listentry* prev;
	char data[DATASZ];
};

void unlink(struct listentry* e) {
	(e->next)->prev = e->prev;
	(e->prev)->next = e->next;
}

void link(struct listentry* head, struct listentry* e) {
	e->next = head;
	e->prev = head->prev;
	(head->prev)->next = e;
	head->prev = e;
}

void print_list(struct listentry* head) {
	struct listentry* e = head;
	
	do {
		printf("%c (next %c) (prev %c)\n", e->data[0], e->next->data[0], e->prev->data[0]);
		e = e->next;
	} while (e != head);
}

int main() {
	struct listentry head = {0};
	struct listentry* a = NULL;
	struct listentry* b = NULL;
	struct listentry* c = NULL;

	// init the head for a circular double linked list 
	head.next = &head;
	head.prev = &head;
	memset(&(head.data), 'H', DATASZ);

	// add some things to the list
	a = malloc(sizeof(struct listentry));
	memset(&(a->data), 'A', DATASZ);
	link(&head, a);

	b = malloc(sizeof(struct listentry));
	memset(&(b->data), 'B', DATASZ);
	link(&head, b);

	c = malloc(sizeof(struct listentry));
	memset(&(c->data), 'C', DATASZ);
	link(&head, c);

	print_list(&head);

	// Corrupt B
	b->next = (struct listentry*)0x4141414141414141;
	b->prev = (struct listentry*)0x3431343134313431;

	// unlink B
	unlink(b);

	print_list(&head);

	return 0;
}
