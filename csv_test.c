#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <gtk/gtk.h>
#define YES 1
#define NO 0
int main(int argc, char*argv[]) {
    int fd, i = 0, numRead, newRow = NO, newCol = NO, row = 0, col = 0, textDelimiter = NO;
    char cell[1024]; /*Will contain content of 1 cell as a string*/

    GtkWidget *window;
    GtkWidget *myGrid;
    GtkWidget *scroll;
    GtkWidget ***button;
    button = (GtkWidget ***)malloc(sizeof(GtkWidget **) * (row + 1));
    button[0] = (GtkWidget **)malloc(sizeof(GtkWidget *) * (col + 1));

    scanf("%s", cell);
    fd = open(cell, O_RDWR, 0666);
    if(fd == -1) {
        perror("Unable to open file.");
        return errno;
    }

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 650, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_set_title(GTK_WINDOW(window), "Spreaddit - A lightweight spreadsheet editor !");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    myGrid = gtk_grid_new();

    while(1) {
        if(newRow == YES) {
            row++;
            col = 0;
            button = (GtkWidget ***)realloc(button, sizeof(GtkWidget **) * (row + 1));
            button[row] = (GtkWidget **)malloc(sizeof(GtkWidget *) * (col + 1));
            i = 0;
            newRow = NO;
            textDelimiter = NO;
        }
        if(newCol == YES) {
            col++;
            button[row] = (GtkWidget **)realloc(button[row], sizeof(GtkWidget *) * (col + 1));
            i = 0;
            newCol = NO;
            textDelimiter = NO;
        } 
        numRead = read(fd, &cell[i], sizeof(char));
        if(numRead == 0)
            break;
        switch(cell[i]) {
            case '"'  : /*This will enable text within "" to be stored as it is */
                if(i == 0) {
                    textDelimiter = YES;
                    i = -1;
                    break;
                }
                if(cell[i - 1] == '"')
                    i--;/* We don't want to show "" in the program */
                break;
            case '\n': /*String has ended and line of the csv file also has ended, given string to button and increment row*/
                cell[i] = '\0';
                puts(cell);
                button[row][col] = gtk_button_new_with_label(cell);
                gtk_grid_attach(GTK_GRID(myGrid), button[row][col], col, row, 1, 1);
                newRow = YES;
                break;
            case ',' : /*String has ended and column of csv file is read, give strnig to button and increment col*/
                if(cell[i - 1] == '"') {
                    textDelimiter = NO;
                    i--;
                }
                if(textDelimiter == YES)
                    break;
                cell[i] = '\0';
                puts(cell);
                button[row][col] = gtk_button_new_with_label(cell);
                gtk_grid_attach(GTK_GRID(myGrid), button[row][col], col, row, 1, 1);
                newRow = NO;
                newCol = YES;
                break;
            default : 
                break;
        }
        i++;
    }
    gtk_container_add(GTK_CONTAINER(scroll), myGrid);
    gtk_container_add(GTK_CONTAINER(window), scroll);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
