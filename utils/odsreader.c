#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
FILE *fp;
void parseTable(xmlDocPtr doc, xmlNodePtr cur) {
	xmlChar *key;
	xmlNodePtr tmp;
	xmlChar *str = NULL;
	int i = 0, j = 0;
	cur = cur->xmlChildrenNode;
	while(cur != NULL) {
		tmp = cur->xmlChildrenNode;
		if(tmp == NULL) {
			str = xmlGetProp(cur, "number-columns-repeated");
			if(str != NULL) {
				j = atoi(str);
				while(j > 0) {
					fprintf(fp, " \n");
					j--;
					i++;
				}
			}
			else {
				fprintf(fp, " \n");
				i++;
			}
			cur = cur->next;
			continue;
		}
		if((!xmlStrcmp(tmp->name, (const xmlChar *)"p"))) {
			key = xmlNodeListGetString(doc, tmp->xmlChildrenNode, 1);
			fprintf(fp, "%s\n", key);
			i++;
			xmlFree(key);
		}
		cur = cur->next;
	}
	return;
}
void generateDataFromODS(char *filename) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	char command[1024];
	xmlChar *str = NULL;
	fp = fopen("utils/data", "w+");
	if(fp == NULL) {
		perror("FATAL ERROR : Unable to access intermidiate data file");
		exit(errno);
	}
	sprintf(command, "/bin/sh ./utils/uncompressods.sh %s", filename);
	system(command);
	doc = xmlParseFile("utils/uncompressed/content.xml");

	if(doc == NULL) {
		perror("FATAL ERROR : Unable to open xml file");
		exit(errno);
	}
	cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	while(cur != NULL) {
		if((!xmlStrcmp(cur->name, (const xmlChar *)"body")))
			break;
		cur = cur->next;
	}
	cur = cur->xmlChildrenNode;
	while(cur != NULL) {
		if((!xmlStrcmp(cur->name, (const xmlChar *)"spreadsheet"))) {
			break;
		}
		cur = cur->next;
	}
	cur = cur->xmlChildrenNode;
	while(cur != NULL) {
		if((!xmlStrcmp(cur->name, (const xmlChar *)"table"))) {
			break;
		}
		cur = cur->next;
	}
	cur = cur->xmlChildrenNode;
	while(cur != NULL) {
		if((!xmlStrcmp(cur->name, (const xmlChar *)"table-column"))) {
			str = xmlGetProp(cur, "number-columns-repeated");
			fprintf(fp, "%s\n", str);
		}
		if((!xmlStrcmp(cur->name, (const xmlChar *)"table-row"))) {
			parseTable(doc, cur);
		}
		cur = cur->next;
	}
	sprintf(command, "/bin/sh ./utils/clean.sh");
	system(command);
	fclose(fp);
	return;
}
