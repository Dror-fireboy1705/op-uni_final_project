/*
the program is meant to perform the first part of the compilation process for the assembly language.
this consists of: laying the macros over the file; translating the english commands into computer language;
and registering data required for the next stages such as entry declarations and external usage.
*/

/*---used libraries---*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* --defines(macros)-- */

/*defenitions of specific numbers used. the meaning of each coming from the details in maman 14*/
#define WORD 15 /*number of bits in a "word"(defined in maman)*/
#define LINE 82 /*room for 80 chararacter line, '\n' and '\0'*/
#define LABEL 32 /*room for 31 charachter name and '\0'*/
#define SAVENUM 14 /*number of saved words*/
#define CMDNUM 16 /*number of commands(shown in function "command")*/
#define MAXNUM 4096 /*(2^12) size of computer RAM*/

/*most of the "ease of use" macros are declared in the file they are used in.
aside from the ones below used in many of the files in the program(used for the program guide defined later)*/
#define ia (pg->IA)
#define da (pg->DA)
#define ic (pg->IC)
#define dc (pg->DC)

/* _-enums(for lists of numeric values)-_ */

enum cmd {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop};/*numbers of the commands*/
enum saveWrd {r0, r1, r2, r3, r4, r5, r6, r7, macr, endmacr, data, string, entry, extrn};/*numbers of the saved words in the language*/
enum typeErr {invldFile, fileSize, allocFail, lineMaxLen, unknownCmd, nestedMac, unmtchdEndMac, invldMac, usedMac, noEndMac, invldLbl, usedLbl, undecLbl, invldOprd, unmtchdEnt};/*numbers for all types of errors*/
enum binLoc {signE, signR, signA, tgtAdInst, tgtAdDrct, tgtAdRegI, tgtAdRegD ,ognAdInst, ognAdDrct, ognAdRegI, ognAdRegD, opCode};/*location of part in binary representation of word(used when creating the computer language code)*/
enum regLoc {regTgt, regOgn=3};/*used to differ between type of register in command*/
enum I_D {inst, dat};/*used to differ between labels declared before instructions(commands) and before data input(.data/.string)*/


/*
	-___complex variables___-
*/

/*recursive list for macro storage*/
typedef struct macroNode
{
	char name[LABEL];/*macro name is as long as a label name*/
	char *code; /*inner code within defenition of macro*/
	struct macroNode *next;
} macro;

/*recursive list for label storage*/
typedef struct symblNode
{
	char name[LABEL];
	int ads; /*address*/
	int ARE;/*R-relocatable or E-external*/
	int ID;/*instruction or data*/
	struct symblNode *next;
} symbl;

typedef unsigned int word; /*code words for instructions and data*/

/*recursive list for storage of entrys or externals*/
typedef struct entrys_externals
{
	char name[LABEL];
	int line;/*where entry defined/where external appears*/
	struct entrys_externals *next;
} Elbl; /*entry/external labels*/

/*struct to hold all valuable information used often in assembler process(guiding the program along)*/
typedef struct program_guide
{
	int DC, IC; /*instruction counter, data counter*/
	word *DA, *IA; /*data coding array, instruction coding array*/
	/*i saw the pdf instructed to have an "int L;" here. but my implementation does not require one. so i did not write it*/
} progG;


/*
	--_*_functions_*_--
*/


/*assembler stages*/
FILE* macLay(FILE*, char*, macro**);/*pre-assembler*/
int run1st(FILE*, char*, progG*, symbl**, macro**);/*first run*/
void run2nd(FILE*, char**, progG*, symbl**, int);/*second run*/

/*assist functions*/
void prnErr(int, char*, int, char*);
progG* newProgG();
int strNext(char*, char*, char**);
int saved(char*);
int command(char*);
int vldLabel(char*);
int isNum(char*);
int neg(int);
void freeAll(FILE**, char **, macro**, symbl**, progG**);

/*macro_list functions*/
macro* addMac(macro*, char*, int*);
int addLineMac(macro*, char*);
macro* findMac(macro*, char*);
macro* lastMac(macro*);
void freeMac(macro*);

/*symbl_list functions*/
symbl* addSym(symbl*, char*, int, int, int, macro*, int*);
symbl* findSym(symbl*, char*);
void sepData(symbl*, progG*);
void freeSym(symbl*);

/*word_array functions*/
int addInst(progG*, int, int ,int);
int addOprd(progG*, int, int);
int addOpRegs(progG*, int, int, int);
int addData(progG*, int);
int ognOprd(char*, int, int*, int*, int*, int*);
int tgtOprd(char*, int, int*, int*, int*, int*);
int labelAds(progG*, symbl*);

/*Elbls functions*/
Elbl* addElbl(Elbl*, char*, int, int, int*);
void freeElbl(Elbl*);
