#include <stdlib.h>
#include "stack.h"
void push(stack *s, int val) {
	node *curr = *s, *next = *s;
	if(curr != NULL) {
		next = curr->ptr;
		while(next != NULL) {
			next = next -> ptr;
			curr = curr -> ptr;
		}
		next = (node *)malloc(sizeof(node));
		next->val = val;
		next->ptr = NULL;
		curr->ptr = next;
	}
	else {
		curr = (node *)malloc(sizeof(node));
		(curr->val) = val;
		(curr->ptr) = NULL;
		*s = curr;
	}
}
int pop(stack *s) {
	int ret;
	node *curr = *s, *next = *s;
	next = next -> ptr;
	if(next != NULL) {
		while(next -> ptr != NULL) {
			next = next -> ptr;
			curr = curr -> ptr;
		}
		ret = next->val;
		curr->ptr = NULL;
	}
	else {
		ret = curr -> val;
		*s = NULL;
	}
	return ret;
}
int isfull(stack *s) {
	return 0;
}
void stackinit(stack *s) {
	*s = NULL;
}
int isempty(stack *s) {
	return *s == NULL;
}
