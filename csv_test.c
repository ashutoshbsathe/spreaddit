#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#include "spreadsheet.h"
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
		gtk_label_set_text(GTK_LABEL(sheet->message), "Please select a cell before making changes !\n");
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
		gtk_label_set_text(GTK_LABEL(sheet->message), "Please select a cell before making changes !");
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
	Spreadsheet *tmp = (Spreadsheet *)data;
	if(tmp->activecell != NULL) {
		label = gtk_entry_get_text(tmp->formula);
		gtk_button_set_label(GTK_BUTTON(tmp->activecell), label);
	}
	else
		gtk_label_set_text(GTK_LABEL(tmp->message), "Please select a cell before applying changes !");
}
void saveClicked(GtkWidget *widget, gpointer data) {
	Spreadsheet *sheet = (Spreadsheet *)data;
	sortGrid(sheet, 1000);
	gtk_widget_show_all(window);
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
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
