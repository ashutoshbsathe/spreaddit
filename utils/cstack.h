#include <stdlib.h>
typedef struct cnode{
	char val;
	struct cnode *ptr;
}cnode;
typedef cnode *cstack;
int cisempty(cstack *s);
int cisfull(cstack *s);
void cinit(cstack *s);
void cpush(cstack *s, char x);
char cpop(cstack *s);

