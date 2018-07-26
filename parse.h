/*
 * parse.h
 *
 *  Created on: Jul 23, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#ifndef SRC_PARSE_H_
#define SRC_PARSE_H_

#include "structs.h"

/**
 * shrink_line - Removes extra blank and tailing spaces between comma and brackets
 */
void shrink_line(char*);

/**
 * parse_string - Handles the .string text
 */
void parse_string (char*, char*, int);

/**
 * parse_data - Handles the .data text
 */
void parse_data (char*, char*);

/*
 * get_addressing - Gets instruction parameter and returns the addressing number (0,1,2,3)
 */
char *get_addressing (char*);

/*
 * getRegister - gets instruction parameter.
 * If parameter is valid register - return its number. Else return -1
 */
int getRegister (char*);

/*
 * bit2str - Gets an integer for number and another integer for number of bits
 * and returns assembly code of bits
 */
char *bit2str(int, int);

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
char *generate_bits(int, ...);

/*
 * handle_op_param gets 2 parameters:
 * 1) Instruction parameter
 * 2) Number: 1 - opsrc, 2 - opdst
 *
 * The data in the parameter is cooked into binary instruction line
 * In case param is register, it will be coded into bits 2-3 or 4-5 according to maram_num
 */
void handle_op_param(char*, int);

/*
 * handle_operation gets instruction code and string that contains the instruction parameters
 * The data is cooked into multiple binary instruction lines
 */
void handle_operation(int, char*);

/*
 * parseLine - Gets text line of code and cooks it into binary code
 */
void parseline (char*);

void fill_label_values();

void writeLabelsToFile(char*, FILE*, label*);

#endif /* SRC_PARSE_H_ */
