CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` `pkg-config --cflags glib-2.0`

new: *.c *.h
	mkdir objects
	$(CC) -c csv_test.c $(CFLAGS) $(LDFLAGS) -o objects/csv_test.o
	$(CC) -c spreadsheet.c $(CFLAGS) $(LDFLAGS) -o objects/spreadsheet.o
	$(CC) objects/*.o $(CFLAGS) $(LDFLAGS) -o csv_test

clean:
	rm -rf objects
	rm -rf csv_test
