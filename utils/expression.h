#ifndef __EXPRESSION_H
#define __EXPRESSION_H
#include <gtk/gtk.h>
#include <string.h>
#include "spreadsheet.h"
#define POS 300
#define INT 400
#define OPR 500
#define ENT 600
#define ERR 700
typedef struct formulatoken {
	int type;
	union formula {
		pos position;
		char op;
		int num;
	} formuladata;
}formulatoken;
formulatoken getnexttoken(const char *formula, int *restart);
#define DDMMYYYY 50
#define DD1MM1YYYY 53
#define DMONTHYYYY 54
char *formatdate(const char *str, int format);
#endif
