SHELL = /bin/bash
CFLAGS = -Wall -export-dynamic -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

new: *.c *.h
	if [ ! -d "objects" ]; then mkdir objects; fi
	$(CC) -c csv_test.c -o objects/csv_test.o $(CFLAGS) $(LDFLAGS)
	$(CC) -c spreadsheet.c -o objects/spreadsheet.o $(CFLAGS) $(LDFLAGS)
	$(CC) -c utils/expression.c -o objects/expression.o $(CFLAGS) $(LDFLAGS)
	$(CC) -c utils/infix.c -o objects/infix.o
	$(CC) -c utils/cstack.c -o objects/cstack.o
	$(CC) -c utils/stack.c -o objects/stack.o
	$(CC) -c utils/odsreader.c -o objects/odsreader.o `xml2-config --cflags --libs`
	$(CC) objects/*.o -o csv_test $(CFLAGS) $(LDFLAGS) `xml2-config --cflags --libs`

clean:
	rm -rf objects
	rm -rf csv_test
