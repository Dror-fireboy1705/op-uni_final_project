/*
this file includes the main function of the pre_assembler and all functions assisting it.
the file holds all operations neccesary for: eliminating note lines and empty lines;
which are irrelevent for the assembler; and the process of laying the macros defined
in a file throughout it.
*/

#include "assembler.h"

#define ERRFOUND(num,code,line) pre_errFound(num,code,line,amFile,name_am,&errFlag,read)
#define READPARAM char*, int*, macro**, int*
#define LINERR(num) 	*errNum = num;\
			return NULL;
#define TABLE (*table)

char* pre_readLine(READPARAM);
char* pre_readErr(READPARAM);
int vldMacro(char*, macro*);
void pre_errFound(int, char*, int, FILE*, char*, int*);

/*the main function of the pre_assembler. calling all other functions
of the process. the function itself is responsible for reading directly
form the file; opening the output ".am" file; and dealing with errors.*/
FILE* macLay(FILE *fp, char *name, macro **table)
{
	int macrFlag = 0, errFlag = 0, errNum=-1;/*all variables neccesary for errors*/
	int LC = 0;/*line counter*/
	char line[LINE] = "";/*string holding current line being read*/
	char *c=NULL;/*general use char pointer*/
	FILE *amFile=NULL;/*file pointer for the output ".am" file*/
	char* (*read)(READPARAM) = pre_readLine;/*function pointer used to redirect the reading of a line when error occurs*/
	
	/*file name ".am"*/
	c = strrchr(name, 's');/*find the 's' in ".as"*/
	*c = 'm';/*replace to make ".am"*/
	amFile = fopen(name, "w");/*create outpput ".am" file*/
	
	while(fgets(line, LINE, fp))/*read file*/
	{
		LC++;/*advance line counter*/
		c = read(line, &macrFlag, table, &errNum);/*call readline function*/
		if(c) fprintf(amFile, "%s",c);/*if read return non-NULL then there is no error. print to file*/
		else /*return NULL => error. enter "error mode"*/
		{
			pre_errFound(errNum,line,LC,amFile,name,&errFlag);
			read = pre_readErr;
			errNum = -1;/*reset error indicator to no error*/
		}
	}
	if(macrFlag)/*macro defention without end*/
	{
		errNum = noEndMac;
		pre_errFound(errNum,line,LC,amFile,name,&errFlag);
	}
	
	/*closing operations*/
	if(errFlag) return NULL;
	return amFile;/*return pointer to output file used later*/
}

/*the function recieves a string holding current line to read, and pointers to
variables used to connect between lines. the function interprets the line,
and returns the fitting string to printf to ouput file, or NULL if error occurs.*/
char* pre_readLine(char *line, int *macrFlag, macro **table, int *errNum)
{
	char cur[LINE]="";/*current word*/
	macro *ptr=NULL;/*general purpose macro pointer*/
	char *c=NULL, *p=NULL;/*general purpose char pointer, pointer for strNext*/
	
	c = strchr(line, '\n');
	if(c == NULL) {LINERR(lineMaxLen)}/*line longer than allowed(if \n not found than the line must be longer than 80)*/
	
	/*empty line and note line check*/
	if(line[0] == ';') return "";/*note line*/
	if(strNext(line, cur, &p) == EOF) return "";/*empty line*/
	
	if(saved(cur) == macr)/*start of macro*/
	{
		if(*macrFlag) {LINERR(nestedMac)}/*declare macro in macro defenition(error)*/
		
		*macrFlag = 1;/*signal inside macro*/
		if(strNext(line, cur, &p) == EOF) {LINERR(invldMac)}/*macro defenition without macro name*/
		if(!vldMacro(cur, TABLE)) {LINERR(invldMac)}/*macro name invalid*/
		if(!(TABLE = addMac(TABLE, cur, errNum))) return NULL;/*error occured in addition of macro(error number registered in function)*/
		if(strNext(line, cur, &p) != EOF) {LINERR(unknownCmd)}/*writing after "macr (name)"*/
		return "";
	}
	if(saved(cur) == endmacr)/*end macro*/
	{
		if(*macrFlag)/*end of macro defenition*/
		{
			*macrFlag = 0;/*signal out of macro*/
			if(strNext(line, cur, &p) != EOF) {LINERR(unknownCmd)}/*writing after "endmacr"*/
			return "";
		}
		LINERR(unmtchdEndMac)
	}
	if((ptr = findMac(TABLE, cur)))/*name of macro*/
	{
		return ptr->code;/*print into file macro code lines*/
	}
	
	/*non-pre_assembler relevent code*/
	if(*macrFlag)/*is code of macr*/
	{
		ptr = lastMac(TABLE);
		if(addLineMac(ptr, line)) {LINERR(allocFail)}/*add line to macro(if allocation failed, signal error)*/
		return "";
	}
	
	return line;
}

/*the function is equivalnt to "pre_readLine". with one segnificant difference,
the function is only returns NULL. the function simply reads through the
lines of the given file, and prints regarding found errors.*/
char* pre_readErr(char *line, int *macrFlag, macro **table, int *errNum)
{
	char cur[LINE]="";/*current word*/
	char *c=NULL, *p=NULL;/*general purpose char pointer, pointer for strNext*/
	
	if((strNext(line, cur, &p) == EOF) || (line[0] == ';')) return NULL;/*empty line and note line can't have errors in them*/
	
	c = strchr(line, '\n');
	if(c == NULL) {LINERR(lineMaxLen)}/*line longer than allowed(if \n not found than the line must be longer than 80)*/
	
	if(saved(cur) == macr)/*start of macro*/
	{
		if(*macrFlag) {LINERR(nestedMac)}/*declare macro in macro defenition(error)*/
		
		*macrFlag = 1;/*signal inside macro*/
		if(strNext(line, cur, &p) == EOF) {LINERR(invldMac)}/*macro defenition without macro name*/
		if(!vldMacro(cur, TABLE)) {LINERR(invldMac)}
		if(!(TABLE = addMac(TABLE, cur, errNum))) {LINERR(allocFail)}/*addition of macro to table failed*/
		if(strNext(line, cur, &p) != EOF) {LINERR(unknownCmd)}/*writing after "macr (name)"*/
		return NULL;
	}
	if(saved(cur) == endmacr)/*end macro*/
	{
		if(*macrFlag) /*end of macro defenition*/
		{
			*macrFlag = 0;/*signal out of macro*/
			if(strNext(line, cur, &p) != EOF) {LINERR(unknownCmd)}/*writing after "endmacr"*/
			return NULL;
		}
		LINERR(unmtchdEndMac)
	}
	
	/*non-pre_assembler relevent code(do nothing)*/

	return NULL;
}

/*the function recives a string representing the name of a macro,
and returns 1 if the name is valid or 0 otherwise.*/
int vldMacro(char *macName, macro *table)
{
	int i=0;
	if(strlen(macName) > LABEL) return 0;/*name too long for macro*/
	if(!isalpha(*macName)) return 0;/*name starts with non letter charachter*/
	if(saved(macName) != SAVENUM) return 0;/*name is a saved word*/
	if(command(macName) != CMDNUM) return 0;/*name is a command*/
	if(findMac(table, macName)) return 0;/*name already used as a macro*/
	for(; i<strlen(macName); i++)
	{
		if(!isgraph(*(macName+i))) return 0;/*non-printing charachter in name*/
		if(isspace(*(macName+i))) return 0;/*white charachter in name*/
	}
	
	return 1;/*valid name*/
}

/*the function does all the neccesary operations for when an error occurs:
printing the erro to the user; closing the output file and signaling an error
for the remainder of the pre_assembler.*/
void pre_errFound(int errNum, char *code, int numLine, FILE *am, char *name, int *errFlag)
{
	prnErr(errNum, code, numLine, name);/*print error details*/
	if(!(*errFlag))/*first error*/
	{
		/*if the .am file is still open(first error), close and remove the .am file*/
		fclose(am);
		remove(name);
	}
	*errFlag = 1;
}
