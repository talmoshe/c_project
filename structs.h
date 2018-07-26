/*
 * structs.h
 *
 *  Created on: Jul 15, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#ifndef SRC_STRUCTS_H_
#define SRC_STRUCTS_H_

typedef struct DataLine {
	int dc;
	char *data;
	char string;
	char *label;
	int type;
	struct DataLine *next;
} dataLine;

typedef struct Opcode {
	int code;
	char *name;
	struct Opcode *next;
} opcode;

typedef struct Label {
	int line;
	char *name;
	struct Label *next;
} label;

typedef struct OperationLine {
	int ic;
	char bits[14];
	char *labelname;
	struct OperationLine *next;
} operationLine;

struct DataLine *new_data (char *, char , char *, int);
struct OperationLine *new_operation_line (char[]);
struct OperationLine *add_operation_line (operationLine*, char[]);
struct Label *add_label (label*, char*, int);
struct Label *new_label (char*, int);
struct Label *update_label_line (label*, char*, int);
struct OperationLine *set_operation_label_value (operationLine*, char[], int);
struct OperationLine *update_operation_labelname (operationLine*, char*, int);
struct DataLine *add_data (dataLine *, char*, char, char*, int);
struct DataLine *search_data (dataLine *,char *);
struct Opcode *register_opcode (opcode *, int , char *);
void printKeys(dataLine *);
void printOperations(operationLine*);
void printLabels(label*);
void register_all_opcodes();
int get_opcode (char *);

#endif /* SRC_STRUCTS_H_ */
