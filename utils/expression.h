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
