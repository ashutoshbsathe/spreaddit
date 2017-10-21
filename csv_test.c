#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "spreadsheet.h"
#include "utils/expression.h"
GtkWidget *window;
void insertrowbelowcell(GtkWidget *widget, gpointer data) {
	int i;
	Spreadsheet *tmp = (Spreadsheet *)data;
	GtkWidget *buf;
	if(tmp->active.row < 0) {
		gtk_label_set_text(GTK_LABEL(tmp->message), "Please select a cell first !");
		return;
	}
	gtk_grid_insert_row(GTK_GRID(tmp->grid), tmp->active.row + 1);
	for(i = 0; i < tmp->max.col; i++) {
		buf = gtk_button_new_with_label(" ");
		tmp->context = gtk_widget_get_style_context(buf);
		gtk_style_context_add_class(tmp->context, "cell");
		gtk_grid_attach(GTK_GRID(tmp->grid), buf, i, tmp->active.row + 1, 1, 1);
		g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), tmp);
		g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), tmp);
	}
	tmp->max.row++;
	gtk_widget_show_all(window);
	return;
}
void insertrowabovecell(GtkWidget *widget, gpointer data) {
	int i;
	Spreadsheet *tmp = (Spreadsheet *)data;
	GtkWidget *buf;
	if(tmp->active.row < 0) {
		gtk_label_set_text(GTK_LABEL(tmp->message), "Please select a cell first !");
		return;
	}
	gtk_grid_insert_row(GTK_GRID(tmp->grid), tmp->active.row );
	for(i = 0; i < tmp->max.col; i++) {
		buf = gtk_button_new_with_label(" ");
		tmp->context = gtk_widget_get_style_context(buf);
		gtk_style_context_add_class(tmp->context, "cell");
		gtk_grid_attach(GTK_GRID(tmp->grid), buf, i, tmp->active.row, 1, 1);
		g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), tmp);
		g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), tmp);
	}
	tmp->max.row++;
	tmp->active.row++;
	cellClicked(tmp->activecell, tmp);
	gtk_widget_show_all(window);
	return;
}
void insertcoltorightcell(GtkWidget *widget, gpointer data) {
	int i;
	Spreadsheet *tmp = (Spreadsheet *)data;
	GtkWidget *buf;
	if(tmp->active.row < 0) {
		gtk_label_set_text(GTK_LABEL(tmp->message), "Please select a cell first !");
		return;
	}
	gtk_grid_insert_column(GTK_GRID(tmp->grid), tmp->active.col + 1);
	for(i = 0; i < tmp->max.row; i++) {
		buf = gtk_button_new_with_label(" ");
		tmp->context = gtk_widget_get_style_context(buf);
		gtk_style_context_add_class(tmp->context, "cell");
		gtk_grid_attach(GTK_GRID(tmp->grid), buf, tmp->active.col + 1, i, 1, 1);
		g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), tmp);
		g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), tmp);
	}
	tmp->max.col++;
	gtk_widget_show_all(window);
	return;
}
void insertcoltoleftcell(GtkWidget *widget, gpointer data) {
	int i;
	Spreadsheet *tmp = (Spreadsheet *)data;
	GtkWidget *buf;
	if(tmp->active.row < 0) {
		gtk_label_set_text(GTK_LABEL(tmp->message), "Please select a cell first !");
		return;
	}
	gtk_grid_insert_column(GTK_GRID(tmp->grid), tmp->active.col);
	for(i = 0; i < tmp->max.row; i++) {
		buf = gtk_button_new_with_label(" ");
		tmp->context = gtk_widget_get_style_context(buf);
		gtk_style_context_add_class(tmp->context, "cell");
		gtk_grid_attach(GTK_GRID(tmp->grid), buf, tmp->active.col , i, 1, 1);
		g_signal_connect(buf, "clicked", G_CALLBACK(cellClicked), tmp);
		g_signal_connect(buf, "focus", G_CALLBACK(cellFocussed), tmp);
	}
	tmp->max.col++;
	tmp->active.col++;
	cellClicked(tmp->activecell, tmp);
	gtk_widget_show_all(window);
	return;
}
void boldClicked(GtkWidget *widget, gpointer data) {
	char str[1024], tmp[1024];
	Spreadsheet *sheet = (Spreadsheet *)data;
	GtkWidget *label;
	if(sheet->activecell == NULL) {
		displayMessage(sheet, "Please select a cell before making changes !");
		return;
	}
	sheet->context = gtk_widget_get_style_context(sheet->activecell);
	if(gtk_style_context_has_class(sheet->context, "italic") == TRUE) {
		strcpy(tmp, "<i>");
		strcat(tmp, gtk_button_get_label(GTK_BUTTON(sheet->activecell)));
		strcat(tmp, "</i>");
	}
	else
		strcpy(tmp, gtk_button_get_label(GTK_BUTTON(sheet->activecell)));
	if(gtk_style_context_has_class(sheet->context, "bold") == TRUE) {
		strcpy(str, tmp);
		gtk_style_context_remove_class(sheet->context, "bold");
	}
	else {
		strcpy(str, "<b>");
		strcat(str, tmp);
		strcat(str, "</b>");
		gtk_style_context_add_class(sheet->context, "bold");
	}
	label = gtk_bin_get_child(GTK_BIN(sheet->activecell));
	gtk_label_set_markup(GTK_LABEL(label), str);
	return;
}
void italicClicked(GtkWidget *widget, gpointer data) {
	char str[1024], tmp[1024];
	Spreadsheet *sheet = (Spreadsheet *)data;
	GtkWidget *label;
	if(sheet->activecell == NULL) {
		displayMessage(sheet, "Please select a cell before making changes !");
		return;
	}
	sheet->context = gtk_widget_get_style_context(sheet->activecell);
	if(gtk_style_context_has_class(sheet->context, "bold") == TRUE) {
		strcpy(tmp, "<b>");
		strcat(tmp, gtk_button_get_label(GTK_BUTTON(sheet->activecell)));
		strcat(tmp, "</b>");
	}
	else
		strcpy(tmp, gtk_button_get_label(GTK_BUTTON(sheet->activecell)));
	if(gtk_style_context_has_class(sheet->context, "italic") == TRUE) {
		strcpy(str, tmp);
		gtk_style_context_remove_class(sheet->context, "italic");
	}
	else {
		strcpy(str, "<i>");
		strcat(str, tmp);
		strcat(str, "</i>");
		gtk_style_context_add_class(sheet->context, "italic");
	}
	label = gtk_bin_get_child(GTK_BIN(sheet->activecell));
	gtk_label_set_markup(GTK_LABEL(label), str);
	return;
}
void applyClicked(GtkWidget *widget, gpointer data) {
	const char *label;
	char *actual;
	int i, j;
	Spreadsheet *tmp = (Spreadsheet *)data;
	if(tmp->activecell != NULL) {
		label = gtk_entry_get_text(tmp->formula);
		if(label[0] == ' ' )
			label = &label[1];
		if(strcasecmp(label, "=sort(ASC)") == 0 || strcasecmp(label, "= sort(ASC)") == 0) {
			sortGrid(tmp, ASC);
			gtk_widget_show_all(window);
		}
		else if (strcasecmp(label, "=sort(DESC)") == 0) {
			sortGrid(tmp, DESC);
			gtk_widget_show_all(window);
		}
		else if(strlen(label) > 1 && label[0] == '"'&& label[1] == '=' && label[strlen(label) - 1] == '"') {
			actual = (char *)malloc(strlen(label));
			for(i = 0, j = 2; j <= strlen(label); i++, j++) {
				if(j == strlen(label) - 1)
					continue;
				actual[i] = label[j];
			}
			actual[i] = '\0';
			label = &actual[0];
			gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
		}
		else if(strlen(label) > 0 && label[0] == '=') {
			/*
			 * It's formula man !!
			 */
			printf("Getting answer from formula\n");
			actual = getAnswerFromFormula(tmp, &label[1]);
			if(actual == NULL) {
				displayMessage(tmp, "Invalid syntax in formula ! Please check your formula again ! Also check if all cells contain valid integer data");
				return;
			}
			label = &actual[0];
			gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
		}
		else if(label[0] == '=') {
			displayMessage(tmp, "Invalid syntax in formula ! Please check your formula again !");
		}
		else
			gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
	}
	else
		displayMessage(tmp, "Please select a cell before applying changes !");
}
void openODSClicked(GtkWidget *widget, gpointer data) {
	static char *filename = NULL;
	Spreadsheet *sheet = (Spreadsheet *)data;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	if(filename == NULL) {
		dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
		res = gtk_dialog_run (GTK_DIALOG (dialog));
		if (res == GTK_RESPONSE_ACCEPT) {
			GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
			filename = gtk_file_chooser_get_filename (chooser);
		}
		gtk_widget_destroy(dialog);
		if(filename[strlen(filename) - 1] == 's' && filename[strlen(filename) - 2] == 'd' && filename[strlen(filename) - 3] == 'o' && filename[strlen(filename) - 4] == '.')
			printf("File extension is valid(.ods)\n");
		else {
			printf("WARNING : Filename does not contain .ods\nWARNING : This may lead to unexpected output\nWARNING : Please open another file !\n");
			displayMessage(sheet, "Fileextension in not ods ! This may lead to unexpected behaviour ! Please open another file !");
		}
		addGridFromODSFile(sheet, filename);
		displayMessage(sheet, "Selected file is now open in editor");
	}
	else {
		displayMessage(sheet, "You already have one file open in the editor !");
	}
	gtk_widget_show_all(window);
}
void openClicked(GtkWidget *widget, gpointer data) {
	static char *filename = NULL;
	Spreadsheet *sheet = (Spreadsheet *)data;
	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	if(filename == NULL) {
		dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(window), action, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
		res = gtk_dialog_run (GTK_DIALOG (dialog));
		if (res == GTK_RESPONSE_ACCEPT) {
			GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
			filename = gtk_file_chooser_get_filename (chooser);
		}
		gtk_widget_destroy(dialog);
		if(filename[strlen(filename) - 1] == 'v' && filename[strlen(filename) - 2] == 's' && filename[strlen(filename) - 3] == 'c' && filename[strlen(filename) - 4] == '.')
			printf("File extension is valid(.csv)\n");
		else {
			printf("WARNING : Filename does not contain .csv\nWARNING : This may lead to unexpected output\nWARNING : Please open another file !\n");
			displayMessage(sheet, "Fileextension in not csv ! This may lead to unexpected behaviour ! Please open another file !");
		}
		addGridFromCSVFile(sheet, filename);
		displayMessage(sheet, "Selected file is now open in editor");
	}
	else {
		displayMessage(sheet, "You already have one file open in the editor !");
	}
	gtk_widget_show_all(window);
}
void saveClicked(GtkWidget *widget, gpointer data) {
	Spreadsheet *sheet = (Spreadsheet *)data;
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	FILE *fp;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;
	unsigned int i, j, k, l, countofcomma = 0, countofquotes = 0;
	static char *filename = NULL;
	const char *label;
	char *actual;
	dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(window), action, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
	chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	gtk_file_chooser_set_filename(chooser, "Untitled document");
	if(filename == NULL) {
		res = gtk_dialog_run(GTK_DIALOG(dialog));
		if(res == GTK_RESPONSE_ACCEPT) {
			filename = gtk_file_chooser_get_filename(chooser);
			printf("I get filename as %s\n", filename);
		}
	
		gtk_widget_destroy(dialog);
		if(filename[strlen(filename) - 1] == 'v' && filename[strlen(filename) - 2] == 's' && filename[strlen(filename) - 3] == 'c' && filename[strlen(filename) - 4] == '.')
			printf("Saving as .csv\n");
		else {
			strcat(filename, ".csv");
			printf("Filename does not contain .csv ...\nAdding .csv to filename...\n");
		}
	}
	fp = fopen(filename, "w");
	for(i = 0; i < sheet->max.row; i++) {
		for(j = 0; j < sheet->max.col; j++) {
			label = gtk_button_get_label(GTK_BUTTON(gtk_grid_get_child_at(GTK_GRID(sheet->grid), j, i)));
			for(k = 0, countofcomma = 0, countofquotes = 0; k < strlen(label); k++) {
				if(label[k] == ',')
					countofcomma++;
				if(label[k] == '"')
					countofquotes++;
			}
			if(countofcomma == 0 && countofquotes == 0) {
				actual = (char *)malloc(strlen(label) + 1);
				strcpy(actual, label);
				puts(actual);
			}
			else {
				/*
				 * 1 for storing NUL character
				 * 2 for forward and backward double quotes
				 * each double quote will be replaced with double double quotes so they ac as text delimiters
				 */
				actual = (char *)malloc(strlen(label) + countofquotes + 3);
				strcpy(actual, "\"");
				strcat(actual, label);
				strcat(actual, "\"");
				for(k = 1; k < strlen(label); k++) {
					if(actual[k] == '"') {
						/*
						 * Shift the string one unit forward
						 */
						for(l = k; actual[l] != '\0'; l++);
						for(l = l + 1; l > k; l--)
							actual[l] = actual[l - 1];
						k = k + 2;
					}
				}
				puts(actual);
			}
			fprintf(fp, "%s", actual);
			if(j == sheet->max.col - 1)
				fprintf(fp, "\n");
			else
				fprintf(fp, ",");
			free(actual);
		}
	}
	fclose(fp);
	displayMessage(sheet, "Successfully saved the file !");
	return;
}
void deleteRow(GtkWidget *widget, gpointer data) {
	Spreadsheet *sheet = (Spreadsheet *)data;
	if(sheet->max.row <= 1) {
		displayMessage(sheet, "Cannot delete last row !!");
		return;
	}
	gtk_grid_remove_row(GTK_GRID(sheet->grid), sheet->active.row);
	sheet->activecell = sheet->message;
	displayMessage(sheet, "Successfully deleted the row");
	sheet->max.row--;
	return;
}
void deleteCol(GtkWidget *widget, gpointer data) {
	Spreadsheet *sheet = (Spreadsheet *)data;
	if(sheet->max.col <= 1) {
		displayMessage(sheet, "Cannot delete last column !!");
		return;
	}
	gtk_grid_remove_column(GTK_GRID(sheet->grid), sheet->active.col);
	sheet->activecell = sheet->message;
	displayMessage(sheet, "Successfully deleted the column");
	sheet->max.col--;
	return;
}
gboolean keyPressHandler(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	Spreadsheet *sp = (Spreadsheet *)data;
	static const char *label = NULL;
	switch(event->keyval) {
		case GDK_KEY_c : 
			if(sp->activecell == NULL)
				displayMessage(sp, "Please select a cell before attempting to copy the content");
			else {
				gtk_label_set_text(GTK_LABEL(sp->message), "Copied the contents of the current cell");
				label = gtk_button_get_label(GTK_BUTTON(sp->activecell));
			}
			break;
		case GDK_KEY_v : 
			if(label == NULL)
				displayMessage(sp, "Please select a cell and press Ctrl + C to copy it's content");
			else if (sp->activecell == NULL)
				displayMessage(sp, "Please select a cell before attempting to paste the data");
			else
				gtk_button_set_label(GTK_BUTTON(sp->activecell), label);
			break;
		default    :
			break;
	}
	return FALSE;
}
void dateformat1(GtkWidget *widget, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	const char *label = NULL;
	if(tmp->activecell == NULL){
		displayMessage(tmp, "Please select a cell before changing date format");
		return;
	}
	label = formatdate(gtk_button_get_label(GTK_BUTTON(tmp->activecell)), DDMMYYYY);
	if(label == NULL) {
		displayMessage(tmp, "Selected cell does not contain a valid date string");
		return;
	}
	gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
	gtk_widget_show_all(window);
	return;
}
void dateformat2(GtkWidget *widget, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	const char *label = NULL;
	if(tmp->activecell == NULL){
		displayMessage(tmp, "Please select a cell before changing date format");
		return;
	}
	label = formatdate(gtk_button_get_label(GTK_BUTTON(tmp->activecell)), DD1MM1YYYY);
	if(label == NULL) {
		displayMessage(tmp, "Selected cell does not contain a valid date string");
		return;
	}
	gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
	gtk_widget_show_all(window);
	return;
}
void dateformat3(GtkWidget *widget, gpointer data) {
	Spreadsheet *tmp = (Spreadsheet *)data;
	const char *label = NULL;
	if(tmp->activecell == NULL){
		displayMessage(tmp, "Please select a cell before changing date format");
		return;
	}
	label = formatdate(gtk_button_get_label(GTK_BUTTON(tmp->activecell)), DMONTHYYYY);
	if(label == NULL) {
		displayMessage(tmp, "Selected cell does not contain a valid date string");
		return;
	}
	gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
	gtk_widget_show_all(window);
	return;
}
int main(int argc, char*argv[]) {
	GtkCssProvider *provider;
	GtkStyleContext *context;
	GtkBuilder *builder;
	GtkWidget *tmp;
	GError *error = NULL;
	Spreadsheet sheet;
	gtk_init(&argc, &argv);
	provider = gtk_css_provider_new();
	gtk_css_provider_load_from_file(provider, g_file_new_for_path("res/styles/styles.css"), &error);
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "res/layouts/trial2.xml", &error);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
	gtk_builder_connect_signals(builder, NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	init(&sheet, builder);
	//addGridFromCSVFile(&sheet, argv[1]);
	addGridFromODSFile(&sheet, argv[1]);
	context = gtk_widget_get_style_context(window);
	gtk_style_context_add_class(context, "window");
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "boldButton"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "formatting-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(boldClicked), &sheet);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "italicButton"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "formatting-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(italicClicked), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "applyButton")), "clicked", G_CALLBACK(applyClicked), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "formulaEntry")), "activate", G_CALLBACK(applyClicked), &sheet);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "insertRowBelow"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "tool-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(insertrowbelowcell), &sheet);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "insertRowAbove"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "tool-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(insertrowabovecell), &sheet);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "insertColRight"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "tool-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(insertcoltorightcell), &sheet);
	tmp = GTK_WIDGET(gtk_builder_get_object(builder, "insertColLeft"));
	context = gtk_widget_get_style_context(tmp);
	gtk_style_context_add_class(context, "tool-button");
	g_signal_connect(tmp, "clicked", G_CALLBACK(insertcoltoleftcell), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "saveButton")), "clicked", G_CALLBACK(saveClicked), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "deleteRow")), "clicked", G_CALLBACK(deleteRow), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "deleteCol")), "clicked", G_CALLBACK(deleteCol), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "openODSbutton")), "clicked", G_CALLBACK(openODSClicked), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "openButton")), "clicked", G_CALLBACK(openClicked), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "dateformat1")), "clicked", G_CALLBACK(dateformat1), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "dateformat2")), "clicked", G_CALLBACK(dateformat2), &sheet);
	g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "dateformat3")), "clicked", G_CALLBACK(dateformat3), &sheet);
	if(argc > 1) {
		if(argc > 2) {
			printf("Unknown option %s\n", argv[1]);
			return EINVAL;
		}
		if(argv[1][strlen(argv[1]) - 1] == 's' && argv[1][strlen(argv[1]) - 2] == 'd' && argv[1][strlen(argv[1]) - 3] == 'o' && argv[1][strlen(argv[1]) - 4] == '.')
			addGridFromODSFile(&sheet, argv[1]);
		else if(argv[1][strlen(argv[1]) - 1] == 'v' && argv[1][strlen(argv[1]) - 2] == 's' && argv[1][strlen(argv[1]) - 3] == 'c' && argv[1][strlen(argv[1]) - 4] == '.')
			addGridFromCSVFile(&sheet, argv[1]);
		else {
			printf("Unknown file format for %s\n", argv[1]);
			displayMessage(&sheet, "Unknown file format");
		}
	}
	else
		displayMessage(&sheet, "Click on open button to open a file");
	g_signal_connect(window, "key-release-event", G_CALLBACK(keyPressHandler), &sheet);
	printf("%s", formatdate("20-02-1999", 50));
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
