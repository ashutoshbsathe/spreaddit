#ifndef __STACK_H
#define __STACK_H
#include <stdlib.h>
typedef struct node{
	int val;
	struct node *ptr;
}node;
typedef node *stack;
int isempty(stack *s);
int isfull(stack *s);
void stackinit(stack *s);
void push(stack *s, int x);
int pop(stack *s);
#endif
