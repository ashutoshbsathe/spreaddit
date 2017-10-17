#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "spreadsheet.h"
#define PERMS 0666
#define YES 1
#define NO  0
/*
 * Initializes the spreadsheet with proper values
 * This needs you to send a GtkBuilder as well
 * Function will take appropriate values from the layout files
 * (Layout files are fixed for all spreadsheets)
 */
void init(Spreadsheet *sp, GtkBuilder *builder){
	sp->activecell = NULL;
	sp->formula = GTK_ENTRY(gtk_builder_get_object(builder, "formulaEntry"));
	sp->scroll = GTK_WIDGET(gtk_builder_get_object(builder, "fileView"));
	sp->message = GTK_WIDGET(gtk_builder_get_object(builder, "messageHolder"));
	sp->grid = gtk_grid_new();
	sp->active.row = sp->active.col = sp->max.col = sp->max.row = -1;
	printf("Successfully Initialised Spreadsheet\n");
}
/*
 * Cell focus and click handlers 
 */
void cellClicked(GtkWidget *widget, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	int i, j;
	const char *label1, *label2;
	char str[256];
	label1 = gtk_button_get_label(GTK_BUTTON(widget));
	if(tmp->activecell != NULL) {
		tmp->context = gtk_widget_get_style_context(tmp->activecell);
		gtk_style_context_remove_class(tmp->context, "active");
	}
	tmp->context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(tmp->context, "active");
	gtk_entry_set_text(tmp->formula, label1);
	tmp->activecell = widget;
	for(i = 0; i <= tmp->max.row; i++) {
		for(j = 0; j <= tmp->max.col; j++) {
			if(gtk_grid_get_child_at(GTK_GRID(tmp->grid), j, i) == tmp->activecell) {
				sprintf(str, "Active cell coordinates are : (%d, %d)", j + 1, i + 1);
				label2 = &str[0]; 
				/* 
				 * So now a constant character pointer points to str 
				 * This is needed by gtk_label_set_text()
				 */
				tmp->active.row = i;
				tmp->active.col = j;
				gtk_label_set_text(GTK_LABEL(tmp->message), label2);
				return;
			}
		}
	}
}
gboolean cellFocussed(GtkWidget *widget, GtkDirectionType direction, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	int i, j;
	const char *label1, *label2;
	char str[256];
	label1 = gtk_button_get_label(GTK_BUTTON(widget));
	if(tmp->activecell != NULL) {
		tmp->context = gtk_widget_get_style_context(tmp->activecell);
		gtk_style_context_remove_class(tmp->context, "active");
	}
	tmp->context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(tmp->context, "active");
	gtk_entry_set_text(tmp->formula, label1);
	tmp->activecell = widget;
	for(i = 0; i <= tmp->max.row; i++) {
		for(j = 0; j <= tmp->max.col; j++) {
			if(gtk_grid_get_child_at(GTK_GRID(tmp->grid), j, i) == tmp->activecell) {
				sprintf(str, "Active cell coordinates are : (%d, %d)", j + 1, i + 1);
				label2 = &str[0]; 
				/* 
				 * So now a constant character pointer points to str 
				 * This is needed by gtk_label_set_text()
				 */
				tmp->active.row = i;
				tmp->active.col = j;
				gtk_label_set_text(GTK_LABEL(tmp->message), label2);
				return FALSE;
			}
		}
	}
	return FALSE;
}
/*
 * Loads CSV file into spreadsheet's grid view
 * Function will try to open the file and add the file to 
 * spreadsheet
 *
 * Function returns 0 on successful opening and adding the file
 * It will return errno if something goes wrong
 */
#define START 10
#define QUOTE 20
#define COMMA 30
#define END   40
void addGridFromCSVFile(Spreadsheet *sp, char *filename){
	int fd, i = 0, newRow = NO, newCol = NO, textDelimiter = NO, row = 0, col = 0, numRead = 0;
	short laststate = START;
	char cell[1024];
	char errormsg[128];
	GtkWidget  *buf;
	fd = open(filename, O_RDONLY, PERMS);
	if(fd == -1) {
		sprintf(errormsg, "Unable to open file : %s", strerror(errno));
		gtk_label_set_text(GTK_LABEL(sp->message), (const char *)&errormsg[0]);
		return;
	}
	sp->max.col = sp->max.row = 0;
	while(1) {
		if(newCol == YES) {
			col++;
			if(sp->max.col < col)
				sp->max.col = col;
			i = 0;
			newCol = NO;
			textDelimiter = NO;
		}
		if(newRow == YES) {
			row++;
			col = 0;
			i = 0;
			newRow = NO;
			sp->max.row = row;
			textDelimiter = NO;
		}
		numRead = read(fd, &cell[i], sizeof(char)); /* Yeah, read file character by character */
		if(numRead == 0)
			break;
		switch(cell[i]) {
			case '"' : /* This will enable text within "" to be stored as it is */
				if(textDelimiter == YES)
					textDelimiter = NO;
				else {
					textDelimiter = YES;
					i--; /* We don't want to show '"' in our label */
				}
				laststate = QUOTE;
				break;
			case '\n' : /* String has ended and line of csv file also has ended, so give string to button and increment row */
				if(laststate == QUOTE)
					i--;
				cell[i] = '\0';
				buf = gtk_button_new_with_label(cell);
				sp->context = gtk_widget_get_style_context(buf);
				gtk_style_context_add_class(sp->context, "cell");
				gtk_grid_attach(GTK_GRID(sp->grid), buf, col, row, 1, 1);
				g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), sp);
				g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), sp);
				newRow = YES;
				laststate = END;
				break;
			case ',' : /* String has ended and a column of csv file has been read, so give string to button and increment col*/
				if(textDelimiter == YES)
					break;
				if(laststate == QUOTE)
					i--;
				cell[i] = '\0';
				buf = gtk_button_new_with_label(cell);
				sp->context = gtk_widget_get_style_context(buf);
				gtk_style_context_add_class(sp->context, "cell");
				gtk_grid_attach(GTK_GRID(sp->grid), buf, col, row, 1, 1);
				g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), sp);
				g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), sp);
				newCol = YES;
				newRow = NO;
				laststate = COMMA;
				break;
			default:
				break;
		}
		i++;
	}
	gtk_container_add(GTK_CONTAINER(sp->scroll), sp->grid);
	return ;
}
/*
 * Returns active cell from the spreadsheet (Simple !!)
 */
GtkWidget *getActiveCell(Spreadsheet *sp){
	return sp->activecell;
}
/*
 * Returns fileView i.e. scrolled window from the spreadsheet
 */
GtkWidget *getFileView(Spreadsheet *sp){
	return sp->scroll;
}
/*
 * Loads ODS files into spreadsheet's grid view
 * Function will try to decompress the file given to it
 * And then will keep using XML parser from "utils"to get next string
 */
void addGridFromODSFile(Spreadsheet *sp, char *filename){
}
/*
 * Function sorts the spreadheet according to direction and type
 * direction = ROW / COLUMN (Sort on row / sort on column)
 * type = ASC / DES (Ascending / Descending)
 */
void sortGrid(Spreadsheet *sp, int direction, int type){
}
