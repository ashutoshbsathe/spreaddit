#ifndef __SPREADSHEET_H
#define __SPREADSHEET_H
#define ASC  1000
#define DESC 1001
typedef struct pos {
	int row;
	int col;
} pos;
typedef struct spreadsheet {
	GtkEntry *formula;
	GtkBuilder *builder;
	GtkStyleContext *context;
	GtkWidget *scroll;
	GtkWidget *activecell;
	GtkWidget *grid;
	GtkWidget *message;
	pos active, max;
} Spreadsheet;
void init(Spreadsheet *sp, GtkBuilder *builder);
void cellClicked(GtkWidget *widget, gpointer data);
gboolean cellFocussed(GtkWidget *widget, GtkDirectionType direction, gpointer data);
void addGridFromCSVFile(Spreadsheet *sp, char *filename);
GtkWidget *getActiveCell(Spreadsheet *sp);
GtkWidget *getFileView(Spreadsheet *sp);
void addGridFromODSFile(Spreadsheet *sp, char *filename);
void sortGrid(Spreadsheet *sp, int type);
void displayMessage(Spreadsheet *sp, const char *msg);
char *getAnswerFromFormula(const char *formula);
#endif
