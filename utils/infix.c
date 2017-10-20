#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "stack.h"
#include "cstack.h"
/* Reads a line of input from the user, till \n
 * and stores it in the array arr and makes arr
 * a string, and returns no. of characters read
 */
int readline(char *arr, int n) {
	char ch;
	int i = 0;
	while((ch = getchar()) != '\n' && i < n)
		arr[i++] = ch;
	arr[i] = '\0';
	return i;
}

#define OPERATOR	100
#define	OPERAND		200
#define	END			300
#define	ERROR		400
typedef struct token{
	int type;
	union data {
		int num;
		char op;
	}data;
}token;

/* input: a postfix string, possibly with errors
 * output: the 'next' token from string, separated on 
 *         space or operator
 *  	type: OPERAND, OPERATOR, END, ERROR
 */

enum states {START, DIG, OP, STOP, ERR, SPC, BRACKET};

token getnext(char *str, int *restart) {
	static int currstate = START;
	int nextstate;
	static int i = 0;
	token t;
	static int sum = 0;
	static char currchar, currop;
	if(*restart == 1) {
		i = 0;
		currstate = START;
		*restart = 0;
	}
	while(1) {
		currchar = str[i];
		switch(currstate) {
			case START:
				switch(currchar) {
					case '0': case '1': case '2':
					case '3': case '4': case '5':
					case '7': case '8': case '9':
					case '6':
						nextstate = DIG;
						sum = currchar - '0';
						break;
					case '+': case '-': case '*':
					case '/': case '%': case ')':
					case '(':
						nextstate = OP;
						currop = currchar;	
						break;
					case ' ': case '\t':
						nextstate = SPC;
						break;
					case '\0':
						nextstate = STOP;
						break;
					default:
						nextstate = ERR;
						break;
				}
				break;
			case DIG:
				switch(currchar) {
					case '0': case '1': case '2':
					case '3': case '4': case '5':
					case '7': case '8': case '9':
					case '6':
						nextstate = DIG;
						sum = sum * 10 + currchar - '0';
						break;
					case '+': case '-': case '*':
					case '/': case '%': case ')':
					case '(':
						nextstate = OP;
						t.type = OPERAND;
						t.data.num = sum;
						i++;
						currop = currchar;
						currstate = nextstate;
						return t;	
						break;
					case ' ': case '\t':
						nextstate = SPC;
						t.type = OPERAND;
						t.data.num = sum;
						i++;
						currstate = nextstate;
						return t;	
						break;
					case '\0':
						nextstate = STOP;
						t.type = OPERAND;
						t.data.num = sum;
						i++;
						currstate = nextstate;
						return t;	
						break;
					default:
						nextstate = ERR;
						t.type = OPERAND;
						t.data.num = sum;
						i++;
						currstate = nextstate;
						return t;	
						break;
				}

				break;
			case OP:
				switch(currchar) {
					case '0': case '1': case '2':
					case '3': case '4': case '5':
					case '7': case '8': case '9':
					case '6':
						nextstate = DIG;
						sum = currchar - '0';
						t.type = OPERATOR;
						t.data.op = currop;
						currop = currchar;
						i++;
						currstate = nextstate;
						return t;
						break;
					case '+': case '-': case '*':
					case '/': case '%': case '(':
					case ')':
						nextstate = OP;
						t.type = OPERATOR;
						t.data.op = currop;
						currop = currchar;
						i++;
						currstate = nextstate;
						return t;
						break;
					case ' ': case '\t':
						nextstate = SPC;
						t.type = OPERATOR;
						t.data.op = currop;
						i++;
						currstate = nextstate;
						return t;
						break;
					case '\0':
						nextstate = STOP;
                        t.type = OPERATOR;
                        t.data.op = currop;
                        i++;
						currstate = nextstate;
						return t;
						break;
					default:
						nextstate = ERR;
						t.type = OPERATOR;
						t.data.op = currop;
						i++;
						currstate = nextstate;
						return t;
						break;
				}
				break;
			case SPC:
				switch(currchar) {
					case '0': case '1': case '2':
					case '3': case '4': case '5':
					case '7': case '8': case '9':
					case '6':
						nextstate = DIG;
						sum = currchar - '0';
						break;
					case '+': case '-': case '*':
					case '/': case '%': case ')':
					case '(':
						nextstate = OP;
						currop = currchar;
						break;
					case ' ': case '\t':
						nextstate = SPC;
						break;
					case '\0':
						nextstate = STOP;
						break;
					default:
						nextstate = ERR;
						break;
				}
				break;
			case STOP:
				t.type = END;
				return t;
				break;
			case ERR:
				t.type = ERROR;
				return t;
				break;
		}
		currstate = nextstate;
		i++;

	}
}
char ctop(cstack *s) {
	char temp = cpop(s);
	cpush(s, temp);
	return temp;
}
int precedence(char op) {
	if(op == '(' || op == ')')
		return 100;
	if(op == '%')
		return 70;
	if(op == '*' || op == '/')
		return 50;
	if(op == '+' || op == '-')
		return 30;
	return 0;
}
/* 
 * This function is similar to inttopost function written in class, instead of making a string from
 * proper operand and all , it will directly evaluate them 
 *
 * This function gives the integer answer, so if the division answer is not an integer, only integer
 * part will be considered for further calculation
 */
int infixeval(char *infix) {
	cstack operator; /* For storing operators like +, - , * , /, % */
	stack operand; /* For storing operators */
	token t;
	int preccurr, prectop, res = 0, restart = 1, x , y, lasttype = START;
	char ch;

	cinit(&operator);
	stackinit(&operand);

	while(1) {
		t = getnext(infix, &restart);
		//myPrintf(t);
		/* In this infix type of expresison, no 2 consecutive tokens are operands */
		switch(t.type) {
			case OPERAND : 
				if(isempty(&operand))
			   		res = t.data.num;
				if(lasttype == OPERAND)
					return INT_MIN;
				push(&operand, t.data.num);
				lasttype = OPERAND;
				break;
			case OPERATOR :
				lasttype = OPERATOR;
				if(!cisempty(&operator) && t.data.op != '(' && t.data.op != ')') {
					preccurr = precedence(t.data.op);
					prectop = precedence(ctop(&operator));
					while(preccurr <= prectop) {
						if(ctop(&operator) == '(' || ctop(&operator) == ')')
							break;
						ch = cpop(&operator);
						if(!isempty(&operand))
							x = pop(&operand);
						else
							return INT_MIN;
						if(!isempty(&operand)) {
							y = pop(&operand);
							switch(ch) {
								case '+' : res = x + y;
										   break;
								case '-' : res = y - x;
										   break;
								case '*' : res = x * y;
										   break;
								case '/' : res = y / x;
										   break;
								case '%' : res = x % y;
										   break;
							}
							push(&operand, res);
							if(!cisempty(&operator))
								prectop = precedence(ctop(&operator));
							else
								break;
						}
						else
							break;
					}
				}
				if(t.data.op == ')') {
					if(cisempty(&operator))
						return INT_MIN;
					while(ctop(&operator) != '(') {
						/* Do the computation for bracket */
						ch = cpop(&operator);
						if(!isempty(&operand))
							res = x = pop(&operand); /* Let's say we have only one operand in our stack. so we need to modify res before breaking */
						else
							break;
						if(!isempty(&operand)) {
							y = pop(&operand);
							switch(ch) {
								case '+' : res = x + y;
										   break;
								case '-' : res = y - x;
										   break;
								case '*' : res = x * y;
										   break;
								case '/' : res = y / x;
										   break;
								case '%' : res = y % x;
										   break;
							}
							push(&operand, res);
							if(!cisempty(&operator))
								prectop = precedence(ctop(&operator));
							else {
								return INT_MIN; /* Since the operator stack became empty before hitting '(' */
							}
						}
						else {
							return INT_MIN;
						}
					}
					cpop(&operator); /* So the '(' gets removed from the stack */
				}
				else 
					cpush(&operator, t.data.op);
				break;
			case END :
				while(!cisempty(&operator)) {
					ch = cpop(&operator);
					if(ch == '(' || ch == ')') /* Means that there are stray brackets in the expression */
						return INT_MIN;
					if(!isempty(&operand))
						res = x = pop(&operand); /* Let's say we only have one operand on stack, then result won't change */
					else
						return INT_MIN;
					if(!isempty(&operand)) {
						y = pop(&operand);
						switch(ch) {
							case '+' : res = x + y;
									   break;
							case '-' : res = y - x;
									   break;
							case '*' : res = x * y;
									   break;
							case '/' : res = y / x;
									   break;
							case '%' : res = y % x;
									   break;
						}
					}
					else
						return INT_MIN;
					push(&operand, res);
				}
				return res;	
				break;
			case ERROR :
				return INT_MIN;
		}
	}
	return INT_MIN;
}
/* infix evaluation without converting it to postfix
 * This is main function calling rest of the function
 * this is capable of handling brackets as well :
 * for eg. : 1 + 2 + ( 3 + 4 ) * 5
 * ans will be 38
 *
 * Please note that there should be spaces before and after every operator and operand (including brackets)
 */
/*
int main(int argc, char *argv[]) {
	char line[128], *p;
	int x, y;
	while(x = readline(line, 128)) {
		if(line != NULL) {
			y = infixeval(line);
			if(y != INT_MIN)
				printf("%d\n", y);
			else
				fprintf(stderr, "Error in expression\n");
		}
		else
			fprintf(stderr, "Error in expression\n");
	}
	//printf("%d\n", y);
	return 0;
}
*/
