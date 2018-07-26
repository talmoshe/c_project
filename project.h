/*
 * project.h
 *
 *  Created on: Jul 15, 2018
 *      Author: Tal Moshe
 *      Email: talmoshe24@gmail.com
 */

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#define OPS 16
#define MAXLINE 80
#define MAXWORD 40
#define MAXWORDS 5
#define LINEBITS 14
#define REGISTERS 7
#define DEBUG 0
#define LOG(X,Y) {if (DEBUG) printf(X,Y);}
#define LOG0(X) {if (DEBUG) printf(X);}
#define LOG2(X,Y,Z) {if (DEBUG) printf(X,Y,Z);}

enum {DATA,STRING};
enum OPCODE {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,STOP};
enum DECODE {ABSOLUTE,EXTERNAL,RELOCATABLE};

int IC, DC;
char op1addr[OPS][4];
char op2addr[OPS][4];

struct DataLine *dataset;
struct OperationLine *operations;
struct Opcode *opcodes;
struct Label *labels;
struct Label *entries;
struct Label *externs;

#endif /* SRC_PROJECT_H_ */
