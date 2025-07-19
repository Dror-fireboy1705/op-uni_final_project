/*
this file includes many function used throughout the project.
for ease of use; efficient code writing; and to ease reading the code.
*/

#include "assembler.h"

/*the function gets a codde for a type of error that has been found
in the compiled file, and prints a fitting messege to the user.*/
void prnErr(int errNum, char *code, int numLine, char *nameFile)
{
	char *c = strchr(code, '\n');
	if(c) *c = '\0';/*get rid of '\n' in printing error*/
	c = strstr(nameFile, ".am");
	if(c) *c = '\0';/*get rid of ".am" in printing name of file*/
	
	switch(errNum)
	{
		case invldFile:
			printf("file \"%s\" not found\n",nameFile);
			goto end;
		case fileSize:
			printf("file \"%s\" is too big for computer\n",nameFile);
			goto end;
		case allocFail:
			printf("file \"%s\" line %d: computer fault, allocation of space failed\n\t\"%s\"\n\n",nameFile,numLine,code);
			goto end;
		case lineMaxLen:
			printf("file \"%s\" line %d: line longer than accepted maximum\n\t\"%s\"\n\n",nameFile,numLine,code);
			goto end;
		case unknownCmd:
			printf("file \"%s\" line %d: unknown command\n\t\"%s\"\n\n",nameFile,numLine,code);
			goto end;
		case nestedMac:
			printf("file \"%s\" line %d: macro declaration inside macro defenition\n\n\t\"%s\"\n\n",nameFile,numLine,code);
			goto end;
		case unmtchdEndMac:
			printf("file \"%s\" line %d: endmacr not associated with macro\n\t\"%s\"\n\n",nameFile,numLine, code);
			goto end;
		case invldMac:
			printf("file \"%s\" line %d: invalid macro name\n\t\"%s\"\n\n",nameFile,numLine,code);
			goto end;
		case noEndMac:
			printf("file \"%s\": end of file within a macro defenition\n",nameFile);
			goto end;
		case invldLbl:
			printf("file \"%s\" line %d: invalid name of label\n\t\"%s\"\n\n",nameFile, numLine, code);
			goto end;
		case usedLbl:
			printf("file \"%s\" line %d: label name is already in use in file\n\t\"%s\"\n\n",nameFile, numLine, code);
			goto end;
		case undecLbl:
			printf("file \"%s\" line %d: unrecognzied symbol\n\t\"%s\"\n\n",nameFile, numLine, code);
			goto end;
		case invldOprd:
			printf("file \"%s\" line %d: statement inoperable due to invalid operands\n\t\"%s\"\n\n",nameFile, numLine, code);
			goto end;
		case unmtchdEnt:
			printf("file \"%s\" line %d: label declared as entry but not defined in file\n\t\"%s\"\n\n",nameFile, numLine, code);
			goto end;
	}
	end:
	*c = '.';/*change name back*/
}

/*the function allocates space for the program guide and its contents
and initializes everything*/
progG* newProgG()
{
	progG *pg = (progG*)malloc(sizeof(progG));
	pg->DC=0;
	pg->IC=0;
	pg->DA=(word*)calloc(1,sizeof(word));
	pg->IA=(word*)calloc(1,sizeof(word));
	return pg;
}

/*the function gets an input string to read from, a holder string to hold the specific information
and a char pointer address that points at the part of the first string that is for reading.
the function stores the next word (seperated by white spaces) of the first string, in the given holder string.
the function returns 1 if the read was succesful or EOF otherwise.*/
int strNext(char *strMain, char *strPart, char **p)
{
	int n=0;/*variable for ouput of sscanf*/
	if(!(*p))/*p is null(never visited input string)*/
	{
		n = sscanf(strMain, "%s", strPart);/*scan first word in input*/
		*p = (strMain + strlen(strPart)+1);/*move pointer to after word scanned*/
		return n;
	}
	n = sscanf(*p, "%s", strPart);/*scan the next word in input*/
	*p = (strstr(*p, strPart) + strlen(strPart));/*move pointer to after word scanned*/
	return n;
}

/*the function recieves a string and returns the number of saved language word it represents.
if the string represents no language saved word, the function returns the number of saved words there are.*/
int saved(char *s)
{
			/*  0	  1	2     3     4	  5	6     7      8	      9		 10	   11	     12		13*/
	char *saveWrd[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "macr", "endmacr", ".data", ".string", ".entry", ".extern"};/*array of saved word strings*/
	int i=0;
	for(; i<SAVENUM; i++)/*go over saved word strings*/
	{
		if(!strcmp(s, saveWrd[i])) return i;/*check for match*/
	}
	return SAVENUM;/*14 means no recognizable saved word*/
}

/*the function is equivalent to the function "saved". it does the same things for the commands of the language.*/
int command(char *s)
{
			/*    0	     1	    2	   3	  4      5      6      7      8      9	   10	  11     12	13     14      15*/
	char cmd[][WORD] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};/*array of commands*/
	int i=0;
	for(;i<CMDNUM;i++)/*go over command strings*/
	{
		if(!strcmp(s, cmd[i])) return i;/*check for match*/
	}
	return CMDNUM;/*no recognizable command*/
}

/*the function recives a string and return 1 if it could represent a valid label,
according to the instructions given in maman 14. it returns 0 otherwise.*/
int vldLabel(char *s)
{
	char *c = strchr(s, ':');
	int i=0;
	
	if(*(c+1) != '\0') return 0;/*colon in the middle of label*/
	*c = '\0';/*check only label name itself(without the colon)*/
	if(strlen(s) >= LABEL) return 0;/*label name longer than 31 charachters*/
	if(saved(s) != SAVENUM) return 0;/*label name is a language saved word*/
	if(command(s) != CMDNUM) return 0;/*label name is a langauge command*/
	for(; i<strlen(s);i++)/*check each charachter of string*/
	{
		if(!(isalpha(s[i]) || isdigit(s[i]))) return 0;/*charachter not a letter or digit in label name*/
	}
	
	return 1;/*valid label*/
}

/*the function is equivalent to the function "vldLabel". it does the same for numbers*/
int isNum(char *s)
{
	int i=0;
	for(;i<strlen(s);i++)/*check if each charachter in the string is a digit*/
	{
		if(!isdigit(s[i])) return 0;
	}
	return 1;
}

/*the function recieves a negative number and makes it a negtive number in a way
that could be printed according to the instructions of maman 14.*/
int neg(int num)
{
	return ((-num)&((int)ldexp(1,WORD)-1));
}

/*the function frees all the allocated space in the given parameters
and initializes each of them.*/
void freeAll(FILE **fp, char **fName, macro **tableMac, symbl **tableSym, progG **pg)
{
	fclose(*fp);
	*fp = NULL;
	freeMac(*tableMac);
	*tableMac = NULL;
	freeSym(*tableSym);
	*tableSym = NULL;
	free((*pg)->IA);
	free((*pg)->DA);
	free(*pg);
	*pg = NULL;
	free(*fName);
	*fName = NULL;
}
