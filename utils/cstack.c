#include <stdlib.h>
#include "cstack.h"
void cpush(cstack *s, char val) {
	cnode *curr = *s, *next = *s;
	if(curr != NULL) {
		next = curr->ptr;
		while(next != NULL) {
			next = next -> ptr;
			curr = curr -> ptr;
		}
		next = (cnode *)malloc(sizeof(cnode));
		next->val = val;
		next->ptr = NULL;
		curr->ptr = next;
	}
	else {
		curr = (cnode *)malloc(sizeof(cnode));
		(curr->val) = val;
		(curr->ptr) = NULL;
		*s = curr;
	}
}
char cpop(cstack *s) {
	char ret;
	cnode *curr = *s, *next = *s;
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
int cisfull(cstack *s) {
	return 0;
}
void cinit(cstack *s) {
	*s = NULL;
}
int cisempty(cstack *s) {
	return *s == NULL;
}
