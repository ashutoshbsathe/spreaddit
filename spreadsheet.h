typedef struct pos {
	int row;
	int col;
} pos;
typedef struct spreadsheet {
	GtkEntry *formula;
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
void sortGrid(Spreadsheet *sp, int direction, int type);
