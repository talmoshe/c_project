/*
 * parse.c
 *
 *  Created on: Jul 23, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "parse.h"
#include "project.h"
#include "structs.h"

/**
 * shrink_line - Removes extra blank and tailing spaces between comma and brackets
 * and returns a new shrinked string
 */
void shrink_line(char *str) {
    int i = 0, j = 0;


    while(str[i] != '\0') {
    	 if(isspace(str[i])) {
    		 str[j++] = ' '; /* Shrink multiple spaces */
    	     	 while(isspace(str[i]))
    	         i++;
    	} else if (str[i] == '(') { /* Remove blanks inside brackets */
    		if (isspace(str[j-1]))  /* Remove blanks before brackets */
    		    j--;
    		str[j++] = str[i++];
    		while (str[i] != ')')
    				if (isspace(str[i]))
    					i++;
    				else
    					str[j++] = str[i++];
    	} else if (str[i] == ',') { /* Remove blanks between commas */
    		if (isspace(str[j-1]))  /* Remove blanks before comma */
    			j--;
    		str[j++] = str[i++];
			while(isspace(str[i])) /* Remove blanks after comma */
				i++;
		}

    	if (str[i] != '(' && str[i] != ',')
    		str[j++] = str[i++];
    	else
    		str[j] = str[i];
    }
    str[j] = '\0';
}


/**
 * parse_string - Handles the .string text
 */

void parse_string (char *string, char *label, int len) {
	int i=0;
	char *p = (char*)malloc(sizeof(char)*2);

	if (!(string[0] == '\"')) {
		printf (".string must start with \"\n");
		return;
	}

	string++;
	i=1;
	while (string[0] != '\"' && string[0] != '\0') {
		sscanf (string, "%c", p);
		dataset = add_data(dataset, "", *p, (i<2 ? label : ""), STRING);
		string++;
		i++;
	}

	if (string[0] == '\"')
		dataset = add_data(dataset, "", '\0', "", STRING);
	else
		printf (".string must end with \"\n");
}

/**
 * parse_data - Handles the .data text
 */
void parse_data (char *data, char *label) {
	char *token = strtok(data, ",");

	if (search_data(dataset, label) == NULL) { /* Verify label doesn't exist */
		dataset = add_data(dataset, token, '0', label, DATA); /* Insert first token with label */
		token = strtok(NULL, ",");

		while (token != NULL) { /* Insert all other tokens */
			dataset = add_data(dataset, token, '0', "", DATA);
			token = strtok(NULL, ",");
		}
	} else
		printf ("Label %s already exists, skipping\n", label);
}

/*
 * get_addressing - Gets instruction parameter and returns the addressing number (0,1,2,3)
 */
char *get_addressing (char *params) {
	char *ch = malloc(MAXWORD);
	if (params[0] == 'r' && (params[1] >= '1' && params[1] <= '8')) { /* This is a register */
			ch = "3";
	} else if (params[0] == '#') { /* This is a value */
		while (params++ && (params[0] == '-' || (params[0] >= '0' && params[0] <= '9')))
			;
		if (!params)
			ch = "0";
	} else if (strchr(params, ',') == NULL && strchr(params, '(') == NULL && strchr(params, ')') == NULL) { /* This is a label */
		ch = "1";
	} else if (strchr(params, '(') != NULL && strchr(params, ')') != NULL) {
		ch = "2";
	} else {
		ch =  "-";
	}
	return ch;
}

/*
 * getRegister - gets instruction parameter.
 * If parameter is valid register - return its number. Else return -1
 */
int getRegister (char *param) {
	char regnum = param[1];
	if ((strlen(param) == 2) && (param[0] == 'r') && (atoi(param) >= 0) && (atoi(param) <= REGISTERS))
		return atoi(&regnum);
	else
		return -1;
}

/*
 * bit2str - Gets an integer for number and another integer for number of bits
 * and returns assembly code of bits */
char *bit2str(int num, int bits) {
	int i;
	char *bitstring = (char*)malloc(sizeof(char)*bits+1);
	for (i = 0; i < bits; i++) {
		if ((num & (1u<<(bits-1-i))) != 0)
			bitstring[i] = '/';
		else
			bitstring[i] = '.';
	}
	bitstring[i] = '\0';
	return bitstring;
}

/* add_to_lines gets dynamic number of arguments to build a line of bit codes
 * First argument holds the number of arguments
 *
 * Each additional pair of arguments holds:
 * 1) Integer
 * 2) Number of bits for the integer
 * For example: "1,6" pair will generate "...../"
 *
 * All the pairs together will build the n bit of the instruction line *
 */
char *generate_bits(int num, ...) {
	   va_list valist;
	   int i,j,a,b;
	   char *bits = (char*)malloc(sizeof(char)*LINEBITS);
	   char *chtmp = (char*)malloc(sizeof(char)*LINEBITS);
	   j=0;

	   va_start(valist, num);
	   for (i = 0; i < num; i+=2) {
		   a = va_arg(valist, int);
		   b = va_arg(valist, int);
		   LOG2("%d,%d|", a,b);
		   chtmp = bit2str(a, b);
		   while (chtmp[0] != '\0') {
			   bits[j++] = chtmp[0];
			   chtmp++;
		   }
	   }
	   va_end(valist);
	   bits[j] = '\0';
	   LOG("%s\n", bits);
	   return bits;
}

void add_to_operations (char *bits) {
	operations = add_operation_line(operations, bits);
}

/*
 * handle_op_param gets 2 parameters:
 * 1) Instruction parameter
 * 2) Number: 1 - opsrc, 2 - opdst
 *
 * The data in the parameter is cooked into binary instruction line
 * In case param is register, it will be coded into bits 2-3 or 4-5 according to maram_num
 */
void handle_op_param(char *param, int param_num) {
	switch (atoi(get_addressing(param))) {
		case 0: /* Immediate addressing */
			param[0] = ' ';
			add_to_operations(generate_bits(6, (atoi(param)>0 ? 0 : -1), 4, atoi(param), 8, 0, 2));
			break;
		case 1: /* Direct addressing - value is empty for now */
			add_to_operations(generate_bits(6, 0, 4, 0, 8, 0, 2));
			operations = update_operation_labelname(operations, param, IC-1);
			break;
		case 3: /* Register addressing */
			if (getRegister(param) >= 0)
				add_to_operations(generate_bits(6, (param_num==1?getRegister(param):0), 6, (param_num==2?getRegister(param):0), 6, 0,2));
			break;
	}
}
/*
 * handle_operation gets instruction code and string that contains the instruction parameters
 * The data is cooked into multiple binary instruction lines
 */
void handle_operation(int opcode, char *params) {

	char *param1 = (char *)malloc(sizeof(char *)*MAXWORD);
	char *param2 = (char *)malloc(sizeof(char *)*MAXWORD);
	char *param3 = (char *)malloc(sizeof(char *)*MAXWORD);
	char *p1addr = (char*)malloc(MAXWORD);
	char *p2addr = (char*)malloc(MAXWORD);
	char *p3addr = (char*)malloc(MAXWORD);

	switch (opcode) {

		/* Handle instructions with 2 params */
		case MOV:
		case CMP:
		case ADD:
		case SUB:
		case LEA:

			param1 = strtok(params,",");
			param2 = strtok(NULL,",");
			p1addr = get_addressing (param1);
			p2addr = get_addressing (param2);

			if ((strchr(op1addr[opcode], p1addr[0]) != NULL) && (strchr(op2addr[opcode], p2addr[0]) != NULL)) { /* If addressing matches */

				add_to_operations(generate_bits(8, opcode, 8, atoi(p1addr), 2, atoi(p2addr), 2, 0, 2)); /* Add line for the instruction */

				if (getRegister(param1) > 0 && getRegister(param2) > 0)
					add_to_operations(generate_bits(6, getRegister(param1), 6, getRegister(param2), 6, 0,2)); /* Add single line for 2 registers */
				else {
					handle_op_param(param1, 1); /* Add line for param 1 */
					handle_op_param(param2, 2); /* Add line for param 2 */
				}
			} else {
				printf ("Illegal addressing (%d,%d)for params (%s,%s) with opcode %d\n", atoi(p1addr), atoi(p2addr), param1, param2, opcode);
			}
			break;

		/* Handle instructions with 1 param */
		case NOT:
		case CLR:
		case INC:
		case DEC:
		case RED:
		case PRN:
			p2addr = get_addressing (params);
			if (strchr(op2addr[opcode], p2addr[0]) != NULL) {

				add_to_operations(generate_bits(8, opcode, 8, 0, 2, atoi(p2addr), 2, 0, 2));
				handle_op_param(params, 2);

			} else {
				printf ("Illegal addressing (%d) for opcode %d: %s\n", atoi(p2addr), opcode, params);
			}
			break;

		case JMP:
		case BNE:
		case JSR:
			p2addr = get_addressing (params);
			if (strchr(op2addr[opcode], p2addr[0]) != NULL) {
				if (atoi(p2addr) == 1) {
					add_to_operations(generate_bits(8, opcode, 8, 0, 2, atoi(p2addr), 2, 0, 2));
					handle_op_param(params, 2);
				} else {
					param1 = strtok(params,"(");
					param2 = strtok(NULL,",");
					param3 = strtok(NULL,")");
					p2addr = get_addressing (param2);
					p3addr = get_addressing (param3);

					add_to_operations(generate_bits(12, atoi(p2addr), 2, atoi(p3addr), 2, opcode, 4, 0, 2, 2, 2, 0, 2));
					handle_op_param(param1, 2);

					if (getRegister(param2) > 0 && getRegister(param3) > 0)
						add_to_operations(generate_bits(6, getRegister(param2), 6, getRegister(param3), 6, 0,2)); /* Add single line for 2 registers */
					else {
						handle_op_param(param2, 1); /* Add line for param 1 */
						handle_op_param(param3, 2); /* Add line for param 2 */
					}

				}
			} else {
				printf ("Illegal addressing (%d) for opcode %d: %s\n", atoi(p2addr), opcode, params);
			}
			break;

		/* Handle instructions without params */
		case RTS:
		case STOP:
			add_to_operations(generate_bits(4, opcode, 8, 0, 6));
			break;

		default:
			printf ("Illegal operation code %d\n", opcode);
			break;
	}
}

/*
 * parseLine - Gets text line of code and cooks it into binary code
 */

void parseline (char *line) {
	int is_label = 0;
	int is_data = 0;
	int is_string = 0;
	int is_extern = 0;
	int is_entry = 0;
	int opcode = 0;

	char *cmd1 = (char *)malloc(MAXLINE*sizeof(char *));
	char *cmd2 = (char *)malloc(MAXLINE*sizeof(char *));
	char *cmd3 = (char *)malloc(MAXLINE*sizeof(char *));

	char *op = (char *)malloc(MAXLINE*sizeof(char *));
	char *params = (char *)malloc(MAXLINE*sizeof(char *));

	if (*line != ';') {
		sscanf(line,"%s %s %s", cmd1,cmd2,cmd3); /* Get command into strings */

		is_label = (cmd1[strlen(cmd1)-1] == ':'); /* Is there a label? */

		if (is_label) {
			cmd1[strlen(cmd1)-1] = '\0';
			labels = add_label(labels, cmd1, IC);

		}

		/* Set oprand to *op and parameters to *params according to existance of label */
		strcpy(op, (is_label ? cmd2 : cmd1));
		strcpy(params, (is_label ? cmd3 : cmd2));

		is_data = (strcmp(op, ".data") ? 0 : 1); /* Is it .data? */
		is_string = (strcmp(op, ".string") ? 0 : 1); /* Is it .string? */
		is_entry = (strcmp(op, ".entry") ? 0 : 1); /* Is it .entry? */
		is_extern = (strcmp(op, ".extern") ? 0 : 1); /* Is it .extern? */

		if (is_data) 					/* Handle .data */
			parse_data(params, cmd1);
		else if (is_string) 				/* Handle .string */
			parse_string(params, cmd1, strlen(params));
		else if (is_entry)
			entries = add_label(entries, params, 0); /* Handle .entry */
		else if (is_extern)
			externs = add_label(externs, params, 0); /* Handle .extern */
		else { 							/* Handle operation */
			opcode = get_opcode(op);
			LOG("Opcode = %d\n", opcode);

			if (opcode < 0) {
				printf ("Invalid opcode %s, ignoring.\n", op);
				return;
			} else {
				handle_operation(opcode, params);
			}
		}
	} else {
		LOG0("This is a comment line\n");
	}
}
/*
 * fill_label_values updates the missing label nume numbers in the instructions object
 */

void fill_label_values() {
	operationLine *currOp = operations;
	char *tmp;
	int added = 0;

	while (currOp) {
		if (currOp->labelname) { /* Check if line is tagged with unresolved label */
			label *currExt = externs;
			label *currLabel = labels;
			label *currEnt = entries;
			added = 0;

			while (currExt && !added) { /* Check if matches extern label */
				if (strcmp(currExt->name, currOp->labelname) == 0) {
					if (currExt->line == 0) {
						LOG2("Updating existing extern label %s with value %d\n", currExt->name, currOp->ic);
						externs = update_label_line(externs, currExt->name, currOp->ic);
					} else {
						LOG2("Creating new extern label %s with value %d\n", currExt->name, currOp->ic);
						externs = add_label(externs, currOp->labelname, currOp->ic);
					}
					operations = set_operation_label_value(operations, "............./", currOp->ic);
					added = 1;
				}
				currExt = currExt->next;
			}

			while (currEnt && !added) { /* Check if matches extern label */
				if (strcmp(currEnt->name, currOp->labelname) == 0) {
					if (currEnt->line == 0) {
						LOG2("Updating existing entry label %s with value %d\n", currEnt->name, currOp->ic);
						entries = update_label_line(entries, currEnt->name, currOp->ic);
					} else {
						LOG2("Creating new entry label %s with value %d\n", currEnt->name, currOp->ic);
						entries = add_label(entries, currOp->labelname, currOp->ic);
					}
					operations = set_operation_label_value(operations, "............./", currOp->ic);
					added = 1;
					break;
				}
				currEnt = currEnt->next;
			}

			if (!added) {
				while (currLabel) { /* Check if matches regular label */
					if (strcmp(currLabel->name, currOp->labelname) == 0) {
						tmp = generate_bits(6, 0,4,currLabel->line, 8, 2, 2);
						operations = set_operation_label_value(operations, tmp, currOp->ic);
						LOG2("Updating line %d with value %s\n", currOp->ic, tmp);
						break;
					}
					currLabel = currLabel->next;
				}
			}

		}
		currOp = currOp->next;
	}
}

/*
 * writeLabelsToFile creates file and writes labels and line numbers into it
 */
void writeLabelsToFile(char *filename, FILE *fptr, label *labels) {
	label *curr;
	if ((fptr = fopen(filename, "w")) == NULL) { /* Open file for writing */
		printf("Error! Can't open file %s for writing\n", filename);
	} else {
		curr = labels;
		while (curr) {
			fprintf (fptr, "%s\t%d\n", curr->name, curr->line);
			LOG2("Added to %s line %s\n", filename, curr->name);
			curr = curr->next;
		}
	}
	fclose(fptr);
}
