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
gboolean getSomeDelay(gpointer data) {
	static short calls = 0;
	Spreadsheet *tmp = (Spreadsheet *)data;
	if(calls > 0) {
		calls = 0;
		tmp->context = gtk_widget_get_style_context(tmp->activecell);
		gtk_style_context_remove_class(tmp->context, "focus");
		tmp->activecell = gtk_grid_get_child_at(GTK_GRID(tmp->grid), 0, 0);
		return FALSE;
	}
	calls++;
	return TRUE;
}
void displayMessage(Spreadsheet *sp, const char *msg) {
	gtk_label_set_text(GTK_LABEL(sp->message), msg);
	sp->context = gtk_widget_get_style_context(sp->activecell);
	gtk_style_context_remove_class(sp->context, "active");
	sp->context = gtk_widget_get_style_context(sp->message);
	gtk_style_context_add_class(sp->context, "focus");
	/* 
	 * NEVER EVER USE sleep() FUNCTION TO ADD DELAY WITH GTK+
	 * IT WILL CAUSE GUI UNRESPONSIVE
	 */
	sp->activecell = sp->message;
	g_timeout_add_seconds(2.3, getSomeDelay, sp);
}
/*
 * Cell focus and click handlers 
 */
void cellClicked(GtkWidget *widget, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	int i, j;
	const char *label1, *label2;
	char str[256];
	if(tmp->activecell == tmp->message) {
		return;
	}
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
				sprintf(str, "Active cell coordinates are : (%d, %d)", i + 1, j + 1);
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
	if(tmp->activecell == tmp->message)
		return FALSE;
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
				sprintf(str, "Active cell coordinates are : (%d, %d)", i + 1, j + 1);
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
				if(i == 0)
					buf = gtk_button_new_with_label(" ");
				else
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
				if(i == 0)
					buf = gtk_button_new_with_label(" ");
				else
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
	FILE *fp;
	int maxcol = 0, row = 0, col = 0, i = 0;
	char str[1024];
	GtkWidget *buf;
	generateDataFromODS(filename);
	fp = fopen("utils/data", "r");
	if(fp == NULL) {
		gtk_label_set_text(GTK_LABEL(sp->message), "Unable to get data from given .ods file !");
		return;
	}
	fscanf(fp, "%[^\n]\n", str);
	maxcol = atoi(str);
	while(fscanf(fp, "%c", &str[i]) != -1 && maxcol) {
		if(str[i] == '\n') {
			str[i] = '\0';
			/*
			if(row < 2) {
				printf("%s\n", str);
			}
			*/
			buf = gtk_button_new_with_label(str);
			sp->context = gtk_widget_get_style_context(buf);
			gtk_style_context_add_class(sp->context, "cell");
			gtk_grid_attach(GTK_GRID(sp->grid), buf, col, row, 1, 1);
			g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), sp);
			g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), sp);
			col++;
			if(col >= maxcol) {
				col = 0;
				row++;
			}
			i = -1;/* Sp after incrementing it outside the if makes it zero */
		}
		i++;
	}
	sp->max.col = maxcol;
	sp->max.row = row;
	gtk_container_add(GTK_CONTAINER(sp->scroll), sp->grid);
	remove("utils/data");
	return ;
}
/*
 * Function sorts the spreadheet according to direction and type
 * direction = ROW / COL (Sort on row / sort on column)
 * type = ASC / DES (Ascending / Descending)
 */
#define ASC     1000
#define DES     1001
#define ACTVSTR 200
#define ACTVNUM 201
#define STR     202
typedef struct node {
	short type;
	union Data {
		const char *label;
		int num;
	}data;
	short bold;
	short italic;
} node;
int comparerows (const void *one, const void *two) {
	node **p = (node **)one;
	node **q = (node **)two;
	node *a = (node *)(*p);
	node *b = (node *)(*q);
	int ret = 0;
	int i = 0;
	while(a[i].type != ACTVNUM && a[i].type != ACTVSTR)
		i++;
	if(a[i].type == ACTVNUM)
		ret = a[i].data.num - b[i].data.num;
	else
		ret = strcmp(a[i].data.label, b[i].data.label);
	return ret;
}
char *inttoarr(int num) {
	char *buf = (char *)malloc(64);
	sprintf(buf, "%d", num);
	return buf;
}
void boldClicked(GtkWidget *widget, gpointer data);
void italicClicked(GtkWidget *widget, gpointer data);
void sortGrid(Spreadsheet *sp, int type, int *read) {
	node **arr = NULL;
	GtkWidget *tmp, *grid;
	int i, j, flag = 0, num;
	GList *children, *iter;
	const char *label;
	
	arr = (node **)malloc(sizeof(node *) * sp->max.row);
	if(arr == NULL) {
		displayMessage(sp, "Sufficient memory is not available for sorting");
	}
	printf("Inside the sortGrid\n");
	for(i = 0; i < sp->max.row; i++) {
		arr[i] = (node *)malloc(sizeof(node) * sp->max.col);
		for(j = 0; j < sp->max.col; j++) {
			tmp = gtk_grid_get_child_at(GTK_GRID(sp->grid), j, i);
			arr[i][j].data.label = gtk_button_get_label(GTK_BUTTON(tmp));
			sp->context = gtk_widget_get_style_context(tmp);
			if(gtk_style_context_has_class(sp->context, "bold") == TRUE) {
				arr[i][j].bold = YES;
			}
			else
				arr[i][j].bold = NO;
			if(gtk_style_context_has_class(sp->context, "italic") == TRUE)
				arr[i][j].italic = YES;
			else
				arr[i][j].italic = NO;
			arr[i][j].type = STR;
		}
	}
	/*
	 * Checking of all the members of the active column are numeric strings
	 */
	for(i = 0; i < sp->max.row; i++) {
		if(strcmp(arr[i][sp->active.col].data.label, " ") == 0) {
			displayMessage(sp, "Cannot sort the column with empty cells");
			return;
		}
	}
	for(i = 0; i < sp->max.row; i++) {
		j = 0;
		if(arr[i][sp->active.col].data.label[j] == '-')
			j++;
		flag = 0;
		for(; j < strlen(arr[i][sp->active.col].data.label); j++) {
			if(arr[i][sp->active.col].data.label[j] < '0' || arr[i][sp->active.col].data.label[j] > '9') {
				flag = 1;
				break;
			}
		}
		if(flag == 1)
			break;
	}
	if(flag == 0) {
		for(i = 0; i < sp->max.row; i++) {
			num = atoi(arr[i][sp->active.col].data.label);
			arr[i][sp->active.col].data.num = num;
			arr[i][sp->active.col].type = ACTVNUM;
		}
	}
	else {
		for(i = 0; i < sp->max.row; i++) {
			arr[i][sp->active.col].type = ACTVSTR;
		}
	}
	qsort(arr, sp->max.row, sizeof(arr[0]),  comparerows);
	grid = gtk_grid_new();
	for(i = 0; i < sp->max.row; i++) {
		for(j = 0; j < sp->max.col; j++) {
			/*
			if(arr[i][j].type == ACTVNUM)
				printf("%d", arr[i][j].data.num);
			else
				printf("%s", arr[i][j].data.label);
			if(j == sp->max.col - 1)
				printf("\n");
			else
				printf(",");
			*/
			if(arr[i][j].type == ACTVNUM) 
				label = inttoarr(arr[i][j].data.num);
			else
				label = arr[i][j].data.label;
			tmp = gtk_button_new_with_label(label);
			sp->context = gtk_widget_get_style_context(tmp);
			gtk_style_context_add_class(sp->context, "cell");
			if(arr[i][j].bold == YES) {
				boldClicked(tmp, sp);
			}
			if(arr[i][j].italic == YES)
				italicClicked(tmp, sp);
			gtk_grid_attach(GTK_GRID(grid), tmp, j, i, 1, 1);
			g_signal_connect(tmp, "clicked", G_CALLBACK(cellClicked), sp);
			g_signal_connect(tmp, "focus", G_CALLBACK(cellFocussed), sp);
		}
	}
	sp->activecell = NULL;
	sp->active.row = -1;
	sp->active.col = -1;
	children = gtk_container_get_children(GTK_CONTAINER(sp->scroll));
	for(iter = children; iter != NULL; iter = g_list_next(iter))
		  gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);
	sp->grid = grid;
	gtk_container_add(GTK_CONTAINER(sp->scroll), sp->grid);
}
//void sortGrid(Spreadsheet *sp, int direction, int type){
//	int i, string = NO, j = 0, minindex, compare;
//	int *array = (int *)malloc(sizeof(int) * sp->max.row);
//	const char *label;
//	if(direction == ROW) {
//		/*
//		 * First of all check if the active row contains all non empty strings otherwise we don't sort them
//		 */
//		for(i = 0; i < sp->max.col; i++) {
//			label = gtk_button_get_label(GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(sp->grid), i, sp->active.row)));
//			if(strcmp(label, " ") == 0) {
//				displayMessage(sp, "Cannot sort the element since few cells are empty in this row");
//				//gtk_label_set_text(GTK_LABEL(sp->message), "Cannot sort the element since few cells are empty in this row");
//				return;
//			}
//		}
//		/*
//		 * Check if row contains all the numbers
//		 */
//		for(i = 0; i < sp->max.col; i++) {
//			label = gtk_button_get_label(GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(sp->grid), i, sp->active.row)));
//			j = 0;
//			if(label[j] == '-')
//				j++;
//			for(;j < strlen(label); j++) {
//				if(label[j] < '0' || label[j] > '9') {
//					printf("Unacceptable character %c %d\n", label[j], label[j]);
//					string = YES;
//					break;
//				}
//			}
//			if(string == YES)
//				break;
//			else {
//				array[i] = atoi(label);
//				printf("Array[%d] = %d\n", i, array[i]);
//			}
//		}
//		printf("String = %d\n", string);
//		if(type == ASC) {
//			for(i = 0; i < sp->max.col; i++) {
//				minindex = i;
//				for(j = i + 1; j < sp->max.col; j++) {
//					if(string == YES)
//						compare = strcmp(gtk_button_get_label(GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(sp->grid), i, sp->active.row))), gtk_button_get_label(GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(sp->grid),j, sp->active.row))));
//					else {
//						printf("Comparing %d wiht %d\n", array[j], array[i]);
//						compare = array[j] - array[i];
//					}
//					if(compare < 0)
//						minindex = j;
//				}
//				printf("Swap column %d with column %d\n", i, minindex);
//				swap(sp, COL, i, minindex);
//				if(i != minindex && string == 0) {
//					array[i] = array[i] + array[minindex];
//					array[minindex] = array[i] - array[minindex];
//					array[i] = array[i] - array[minindex];
//					for(j = 0; j < sp->max.col; j++) {
//						printf("%d ", array[j]);
//					}
//					printf("\n");
//				}
//			}
//		}
//		else {
//		}
//	}
//}
