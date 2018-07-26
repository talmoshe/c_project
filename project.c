/*
 * project.c
 *
 *  Created on: Jul 13, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "project.h"
#include "structs.h"
#include "parse.h"

char op1addr[OPS][4]={"013","013",	"013",	"013",	"",		"", 	"1", 	"",		"", 	"", 	"", 	"", 	"", 	"", 	"", 	""};
char op2addr[OPS][4]={"13", "013",	"13",	"13",	"13",	"13", 	"13", 	"13",	"13", 	"123", 	"123", 	"13", 	"013", 	"123", 	"", 	""};


int main(int argc, char *argv[]) {

	FILE *fptr;
	char filename[MAXLINE];
	char *line;
	int i;
	IC = 100;
	DC = 0;

	if(argc==1) { /* Check args */
		printf("\nNo Argument Passed\n");
		exit(1);
	}

	register_all_opcodes();

	for (i = 1; i < argc; ++i) { /* Go over all files in arguments */

		sprintf(filename, "%s%s", argv[i], ".as");

		if ((fptr = fopen(filename, "r")) == NULL) { /* Open the file for reading */
			printf("Error! File %s doesn't exist\n", filename);
			continue;
		}
		line = (char*)malloc(MAXLINE);
		while (fgets(line, MAXLINE, fptr) != NULL) { /* Read the file line by line */
			shrink_line(line);
			LOG("\nParsing line - |%s| \n", line);
			parseline(line);
		}

		fclose(fptr);

		if (DEBUG) {
			/* printKeys(dataset); */
			printOperations(operations);
			printf("Labels:\n");
			printLabels(labels);
			printf("Entries:\n");
			printLabels(entries);
			printf("Externs:\n");
			printLabels(externs);
		}
		fill_label_values();
		printOperations(operations);

		printf ("Labels:\n");
		printLabels(labels);
		printf ("Entries:\n");
		printLabels(entries);
		printf ("Externs:\n");
		printLabels(externs);

		sprintf(filename, "%s%s", argv[i], ".ext");
		writeLabelsToFile(filename, fptr, externs);
		sprintf(filename, "%s%s", argv[i], ".ent");
		writeLabelsToFile(filename, fptr, entries);
	}

	return 0;
}

