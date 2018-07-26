/*
 * structs.c
 *
 *  Created on: Jul 15, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "project.h"

/* Add opcode to a linked list of opcodes */
struct Opcode *register_opcode (opcode *head, int code, char *name) {
	opcode *currItem;
	opcode *newcode = (opcode*)malloc(sizeof(opcode));
	newcode->code = code;
	newcode->name = name;

	if (!head)
		return newcode;

	currItem = head;
	while(currItem->next)
		currItem = currItem->next;
	currItem->next = newcode;
	return head;
}

/* Add all opcodes to a linked list */
void register_all_opcodes() {
	opcodes = register_opcode(opcodes, MOV, "mov");
	opcodes = register_opcode(opcodes, CMP, "cmp");
	opcodes = register_opcode(opcodes, ADD, "add");
	opcodes = register_opcode(opcodes, SUB, "sub");
	opcodes = register_opcode(opcodes, NOT, "not");
	opcodes = register_opcode(opcodes, CLR, "clr");
	opcodes = register_opcode(opcodes, LEA, "lea");
	opcodes = register_opcode(opcodes, INC, "inc");
	opcodes = register_opcode(opcodes, DEC, "dec");
	opcodes = register_opcode(opcodes, JMP, "jmp");
	opcodes = register_opcode(opcodes, BNE, "bne");
	opcodes = register_opcode(opcodes, RED, "red");
	opcodes = register_opcode(opcodes, PRN, "prn");
	opcodes = register_opcode(opcodes, JSR, "jsr");
	opcodes = register_opcode(opcodes, RTS, "rts");
	opcodes = register_opcode(opcodes, STOP, "stop");

}

struct OperationLine *new_operation_line (char bits[]) {
	operationLine* newdata = (operationLine*)malloc(sizeof(operationLine));
	strcpy(newdata->bits, bits);
	newdata->ic = IC++;

	return newdata;
}

struct OperationLine *add_operation_line (operationLine *head, char bits[]) {
	operationLine *currItem;
	operationLine *newItem = new_operation_line(bits);
	if (!head)
		return newItem;
	currItem = head;
	while(currItem->next)
		currItem = currItem->next;
	currItem->next = newItem;
	return head;
}

struct OperationLine *update_operation_labelname (operationLine *head, char *labelname, int ic) {
	operationLine *currItem = head;
	while(currItem->next && currItem->ic != ic)
		currItem = currItem->next;
	if (currItem->ic == ic)
		currItem->labelname = labelname;
	return head;
}

struct OperationLine *set_operation_label_value (operationLine *head, char bits[], int ic) {
	operationLine *currItem = head;
	while (currItem && currItem->ic != ic)
		currItem = currItem->next;
	if (currItem->ic == ic) {
		strcpy(currItem->bits, bits);
		currItem->labelname = NULL;
	}
	return head;
}

struct DataLine *new_data (char *data, char string, char *label, int type) {
	dataLine* newdata = (dataLine*)malloc(sizeof(dataLine));

	newdata->data = data;
	newdata->string = string;
	newdata->label = label;
	newdata->type = type;
	newdata->dc = DC++;
	newdata->next = NULL;


	return newdata;
}

struct Label *new_label (char *name, int line) {
	label* newlabel = (label*)malloc(sizeof(label));

	newlabel->line = line;
	newlabel->name = name;

	return newlabel;
}

struct Label *add_label (label *head, char *name, int line) {
	label *currItem;
	label *newItem = new_label(name, line);

	if (!head)
		return newItem;
	currItem = head;
	while(currItem->next)
		currItem = currItem->next;
	currItem->next = newItem;
	return head;
}

struct Label *update_label_line (label *head, char *name, int line) {
	label *currItem = head;
	while(currItem->next && strcmp(currItem->name, name) != 0)
		currItem = currItem->next;
	if (strcmp(currItem->name, name) == 0)
		currItem->line = line;
	return head;
}

struct DataLine *add_data (dataLine *head, char *data, char string, char *label, int type) {
	dataLine *currItem;
	dataLine *newItem = new_data(data, string, label, type);
	IC++;
	if (!head)
		return newItem;
	currItem = head;
	while(currItem->next)
		currItem = currItem->next;
	currItem->next = newItem;
	return head;
}

int get_opcode (char *ch) {
	opcode *tmp = opcodes;
	while(tmp) {
		if (strcmp(ch,tmp->name) == 0)
			return tmp->code;
		tmp = tmp->next;
	}
	return -1;
}

struct DataLine *search_data (dataLine *head,char *label) {

	dataLine *cursor = head;
    while(cursor!=NULL)
    {
        if(strcmp(cursor->label, label) == 0)
            return cursor;
        cursor = cursor->next;
    }
    return NULL;
}

void printKeys(dataLine *head) {
	dataLine *curr = head;

	while (curr) {
		printf("line: %d, data: %s, string: %c, label: %s\n", curr->dc, curr->data, curr->string, curr->label);
		curr = curr->next;
	}
	putchar('\n');
}

void printOperations(operationLine *head) {
	operationLine *curr = head;

	while (curr) {
			printf("%d: %s", curr->ic, curr->bits);
			if (curr->labelname)
				printf ("\tLabel %s value missing", curr->labelname);
			printf ("\n");
			curr = curr->next;
		}
		putchar('\n');
}

void printLabels(label *head) {
	label *curr = head;

	while (curr) {
		printf("%s\t\t%d\n", curr->name, curr->line);
		curr = curr->next;
	}
	putchar('\n');
}


