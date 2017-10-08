#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>

#define YES 1
#define NO 0

#define START 1
#define QUOTE 2
#define END	  3
#define COMMA 4

GtkEntry *formula;
GtkBuilder *builder;
GtkWidget *message;
GtkCssProvider *provider;
GtkStyleContext *context;
GtkWidget *scroll;
GtkWidget *window;
GtkWidget *activecell = NULL, *myGrid;
GError *error = NULL;
typedef struct sizeValues {
	int maxRow;
	int maxCol;
} sizeValues;
sizeValues val = {0, 0};
sizeValues active = {5, 5};
void buttonClicked(GtkWidget *widget, gpointer data) {
	const char *label1;
	const char *label2;
	char str[512];
	int i, j;
	label1 = gtk_button_get_label(GTK_BUTTON(widget));
	if(activecell != NULL) {
		context = gtk_widget_get_style_context(activecell);
		gtk_style_context_remove_class(context, "active");
	}
	context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(context, "active");
	formula = GTK_ENTRY(gtk_builder_get_object(builder, "formulaEntry"));
	gtk_entry_set_text(formula, label1);
	activecell = widget;
	for(i = 0; i <= val.maxRow; i++) {
		for(j = 0; j <= val.maxCol; j++) {
			if(gtk_grid_get_child_at(GTK_GRID(myGrid), j, i) == activecell) {
				sprintf(str, "Active cell coordinates are (%d, %d)", j + 1, i + 1);
				label2 = &str[0];
				active.maxRow = i;
				active.maxCol = j;
				gtk_label_set_text(GTK_LABEL(message), label2);
				return;
			}
		}
	}
}
gboolean buttonFocussed(GtkWidget *widget, GtkDirectionType direction, gpointer user_data) {
	const char *label;
	const char *label2;
	char str[512];
	int i, j;
	label = gtk_button_get_label(GTK_BUTTON(widget));
	if(activecell != NULL) {
		context = gtk_widget_get_style_context(activecell);
		gtk_style_context_remove_class(context, "active");
	}
	context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(context, "active");
	formula = GTK_ENTRY(gtk_builder_get_object(builder, "formulaEntry"));
	gtk_entry_set_text(formula, label);
	activecell = widget;
	for(i = 0; i <= val.maxRow; i++) {
		for(j = 0; j <= val.maxCol; j++) {
			if(gtk_grid_get_child_at(GTK_GRID(myGrid), j, i) == activecell) {
				sprintf(str, "Active cell coordinates are (%d, %d)", j + 1, i + 1);
				label2 = &str[0];
				active.maxRow = i;
				active.maxCol = j;
				gtk_label_set_text(GTK_LABEL(message), label2);
				return FALSE;
			}
		}
	}
	return FALSE;
}
static void insertrowbelowcell(GtkWidget *widget, gpointer data) {
	int i;
	GtkWidget *buf;
	g_print("%d\n", active.maxRow);
	if(active.maxRow < 0) {
		gtk_label_set_text(GTK_LABEL(message), "Please select a cell first !\n");
		return;
	}
	gtk_grid_insert_row(GTK_GRID(myGrid), active.maxRow + 1);
	for(i = 0; i <= val.maxCol; i++) {
		buf = gtk_button_new_with_label(" ");
		context = gtk_widget_get_style_context(buf);
		gtk_style_context_add_class(context, "cell");
		gtk_grid_attach(GTK_GRID(myGrid), buf, i, active.maxRow + 1, 1, 1);
		g_signal_connect(buf, "clicked", G_CALLBACK(buttonClicked), NULL);
		g_signal_connect(buf, "focus", G_CALLBACK(buttonFocussed), NULL);
	}
	gtk_widget_show_all(window);
	return;
}
void boldClicked(GtkWidget *widget, gpointer data) {
	char str[1024];
	GtkWidget *label;
	if(activecell == NULL) {
		g_print("Please select a tile before making changes !\n");
		return;
	}
	strcpy(str, "<b>");
	strcat(str, gtk_button_get_label(GTK_BUTTON(activecell)));
	strcat(str, "</b>");
	context = gtk_widget_get_style_context(activecell);
	if(gtk_style_context_has_class(context, "bold") == TRUE) {
		label = gtk_bin_get_child(GTK_BIN(activecell));
		gtk_label_set_text(GTK_LABEL(label), gtk_button_get_label(GTK_BUTTON(activecell)));
		gtk_style_context_remove_class(context, "bold");
		return;
	}
	label = gtk_bin_get_child(GTK_BIN(activecell));
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_style_context_add_class(context, "bold");
}
void italicClicked(GtkWidget *widget, gpointer data) {
	char str[1024];
	GtkWidget *label;
	if(activecell == NULL) {
		g_print("Please select a tile before making changes !\n");
		return;
	}
	strcpy(str, "<i>");
	strcat(str, gtk_button_get_label(GTK_BUTTON(activecell)));
	strcat(str, "</i>");
	context = gtk_widget_get_style_context(activecell);
	if(gtk_style_context_has_class(context, "italic") == TRUE) {
		label = gtk_bin_get_child(GTK_BIN(activecell));
		gtk_label_set_text(GTK_LABEL(label), gtk_button_get_label(GTK_BUTTON(activecell)));
		gtk_style_context_remove_class(context, "italic");
		return;
	}
	label = gtk_bin_get_child(GTK_BIN(activecell));
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_style_context_add_class(context, "italic");
}
void underlineClicked(GtkWidget *widget, gpointer data) {
	char str[1024];
	GtkWidget *label;
	if(activecell == NULL) {
		g_print("Please select a tile before making changes !\n");
		return;
	}
	strcpy(str, "<u>");
	strcat(str, gtk_button_get_label(GTK_BUTTON(activecell)));
	strcat(str, "</u>");
	context = gtk_widget_get_style_context(activecell);
	if(gtk_style_context_has_class(context, "underline") == TRUE) {
		label = gtk_bin_get_child(GTK_BIN(activecell));
		gtk_label_set_text(GTK_LABEL(label), gtk_button_get_label(GTK_BUTTON(activecell)));
		gtk_style_context_remove_class(context, "underline");
		return;
	}
	label = gtk_bin_get_child(GTK_BIN(activecell));
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_style_context_add_class(context, "underline");
}
void applyClicked(GtkWidget *widget, gpointer data) {
	const char *label;
	if(activecell != NULL) {
		label = gtk_entry_get_text(formula);
		gtk_button_set_label(GTK_BUTTON(activecell), label);
	}
	else
		g_print("Please select a cell first\n");
}
int main(int argc, char*argv[]) {
	int fd, i = 0, numRead, newRow = NO, newCol = NO, row = 0, col = 0, textDelimiter = NO;
	char laststate = START;
	char cell[1024]; /*Will contain content of 1 cell as a string*/

	GtkWidget *buf;
	GtkWidget *insertrowbelow;
	provider = gtk_css_provider_new();
	gtk_css_provider_load_from_file(provider, g_file_new_for_path("res/styles/styles.css"), &error);

	scanf("%s", cell); /* Using the same array to store filename as well :P */
	fd = open(cell, O_RDWR, 0666);
	if(fd == -1) {
		perror("Unable to open file.");
		return errno;
	}

	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "res/layouts/trial2.xml", &error);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
	scroll = GTK_WIDGET(gtk_builder_get_object(builder, "fileView"));
	message = GTK_WIDGET(gtk_builder_get_object(builder, "messageHolder"));
	insertrowbelow = GTK_WIDGET(gtk_builder_get_object(builder, "insertRowBelow"));
	gtk_builder_connect_signals(builder, NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	myGrid = gtk_grid_new();

	while(1) {
		if(newCol == YES) {
			col++;
			if(val.maxCol < col) 
				val.maxCol = col;
			i = 0;
			newCol = NO;
			textDelimiter = NO;
		}
		if(newRow == YES) {
			row++;
			col = 0;
			i = 0;
			newRow = NO;
			val.maxRow = row - 1;
			textDelimiter = NO;
		}
		numRead = read(fd, &cell[i], sizeof(char)); /* Yeah, read file character by character */
		if(numRead == 0) {
			break;
		}
				
		switch(cell[i]) {
			case '"'  : /*This will enable text within "" to be stored as it is */
				if(textDelimiter == YES)
					textDelimiter = NO;
				else {
					textDelimiter = YES;
					i--;
				}
				laststate = QUOTE;
				break;
			case '\n': /*String has ended and line of the csv file also has ended, given string to button and increment row*/
				if(laststate == QUOTE)
					i--;
				cell[i] = '\0';
				buf = gtk_button_new_with_label(cell);
				context = gtk_widget_get_style_context(buf);
				gtk_style_context_add_class(context, "cell");
				gtk_grid_attach(GTK_GRID(myGrid), buf, col, row, 1, 1);
				g_signal_connect(buf, "clicked", G_CALLBACK(buttonClicked), NULL);
				g_signal_connect(buf, "focus", G_CALLBACK(buttonFocussed), NULL);
				newRow = YES;
				laststate = END;
				break;
			case ',' : /*String has ended and column of csv file is read, give strnig to button and increment col*/
				if(textDelimiter == YES) {
					break;
				}
				if(laststate == QUOTE)
					i--;
				cell[i] = '\0';
				buf = gtk_button_new_with_label(cell);	
				context = gtk_widget_get_style_context(buf);
				gtk_style_context_add_class(context, "cell");	
				gtk_grid_attach(GTK_GRID(myGrid), buf, col, row, 1, 1);
				g_signal_connect(buf, "clicked", G_CALLBACK(buttonClicked), NULL);
				g_signal_connect(buf, "focus", G_CALLBACK(buttonFocussed), NULL);
				newRow = NO;
				newCol = YES;
				laststate = COMMA;
				break;
			default : 
				break;
		}
		i++;
	}
	gtk_container_add(GTK_CONTAINER(scroll), myGrid);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "boldButton")), "clicked", G_CALLBACK(boldClicked), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "italicButton")), "clicked", G_CALLBACK(italicClicked), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "ulineButton")), "clicked", G_CALLBACK(underlineClicked), NULL);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "applyButton")), "clicked", G_CALLBACK(applyClicked), NULL);
	g_signal_connect(insertrowbelow, "clicked", G_CALLBACK(insertrowbelowcell), NULL);
	printf("Initialized the insertrowbelow button\n");
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
