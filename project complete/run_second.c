/*
this file includes the main function of the second run of the assembler and all
functions assisting it. the file holds operations neccesary for: inserting label
addresses into corresponding cells in the instruction array; storing and orgenizing
the data for the ".ent" and ".ext" files; and creating all the output files as
the last part of the assembler.
*/

#include "assembler.h"

#define LINERR(num) *errNum = num;\
		    return 1;
#define TABLE (*table)
#define ENTRYS (*entrys)
#define EXTRNS (*extrns)

int readLine2nd(char*, progG*, symbl**, Elbl**, Elbl**, int, int*);
int cmdLbl(char*, char*, progG*, symbl**, Elbl**, int, int*);
void printOb(FILE*, progG*);
void printEfile(FILE*, Elbl*);

/*the main function of the second run, calling all other functions.
reads directly from the pre_assembled ".am" file, and sends the line
for interepretation. whilst orgenizing all the data read in the given parameters.*/
void run2nd(FILE *fp, char **nameFile, progG *pg, symbl** table, int errFlag)
{
	FILE *outfp = NULL;/*pointer for output files*/
	char line[LINE]="", *c=NULL;/*line holder string, and general use char pointer*/
	Elbl *entrys = NULL;/*list for entrys*/
	Elbl *extrns = NULL;/*list for externals*/
	int errNum=-1;/*error type*/
	int LC=0;/*line counter*/
	
	while(fgets(line, LINE, fp))/*read file*/
	{
		LC++;/*advance line counter*/
		if(readLine2nd(line, pg, table, &entrys, &extrns, errFlag, &errNum))/*error found*/
		{
			prnErr(errNum, line, LC, *nameFile);/*print error details*/
			errNum=-1;/*reset error indicator to no error*/
			errFlag = 1;/*flag error*/
		}
	}
	if(errFlag) return;/*all errors printed, move to next file without making the output files*/
	
	/*no errors, make output files*/
	c = strstr(*nameFile, ".am");
	strcpy(c, ".ob");/*change name*/
	outfp = fopen(*nameFile, "w");/*creating object file*/
	printOb(outfp, pg);
	fclose(outfp);
	if(entrys)/*if there are entries for .ent file*/
	{
		*nameFile = (char*)realloc(*nameFile, sizeof(char)*(strlen(*nameFile)+1));/*make name space bigger for ".ent"*/
		c = strstr(*nameFile, ".ob");
		if(c) strcpy(c, ".ent");/*change name*/
		outfp = fopen(*nameFile, "w");/*creating entries file*/
		printEfile(outfp, entrys);
		fclose(outfp);
		if(extrns)
		{
			c = strstr(*nameFile, ".ent");
			if(c) strcpy(c, ".ext");/*change name*/
			outfp = fopen(*nameFile, "w");/*creating externals file*/
			printEfile(outfp, extrns);
			fclose(outfp);
		}
	}
	else if(extrns)
	{
		*nameFile = (char*)realloc(*nameFile, sizeof(char)*(strlen(*nameFile)+1));/*make name space bigger for ".ext"*/
		c = strstr(*nameFile, ".ob");
		if(c) strcpy(c, ".ext");/*change name*/
		outfp = fopen(*nameFile, "w");/*creating externals file*/
		printEfile(outfp, extrns);
		fclose(outfp);
	}
	freeElbl(entrys);
	freeElbl(extrns);
	
	return;
}

/*the function receives the current line string, and pointers to variables with
important information. the function interprets the line and performs one of the 2 actions:
insert a label address into the corresponding spot in the instruction array,
or check an entry declaration and store data appropriately.
the function returns 1 if an error is found and 0 otherwise.*/
int readLine2nd(char *line, progG *pg, symbl **table, Elbl **entrys, Elbl **extrns, int errFlag, int *errNum)
{
	char cur[LINE]="";/*current word in line*/
	char *c=NULL, *p=NULL;/*general purpose char pointer, pointer for strNext*/
	symbl *lbl=NULL;/*pointer for label declared as entry*/
	
	strNext(line,cur,&p);/*read next word in line*/
	if((c = strchr(cur, ':')))/*declaration of label*/
	{
		/*move to next word in line*/
		if(strNext(line, cur,&p) == EOF) return 0;/*label with no command(error would have been reported in run 1)*/
	}
	if((saved(cur) == data) || (saved(cur) == string) || (saved(cur) == extrn)) return 0;/*data, string or extern(skip line)*/
	if(saved(cur) == entry)/*entry*/
	{
		if(strNext(line, cur, &p) == EOF) {LINERR(invldLbl)}/*entry decleration with no label*/
		if(!(lbl = findSym(TABLE, cur))) {LINERR(unmtchdEnt)}/*undefined entry declaration*/
		if(!(ENTRYS = addElbl(ENTRYS, cur, lbl->ads, signR, errNum))) return 1;/*error occured in function*/
	}
	else if(saved(cur) != CMDNUM)/*command*/
	{
		if(cmdLbl(cur, line, pg, table, extrns, errFlag, errNum)) return 1;/*error occured in function*/
	}
	else return 0;/*unknown command, dealt with in first run*/
	
	return 0;
}

/*the function recieves the string of a command and the line string.
the function checks if one of the command operands is a label, if so
the function inserts it's address into the corresponding cell in
the instruction array. if the label is an external label the function
stores the line as an apearence point for the ".ext" file.
the function returns 1 if an erro is found or 0 otherwise.*/
int cmdLbl(char *cmd, char *line, progG *pg, symbl **table, Elbl **extrns, int errFlag, int *errNum)
{
	char op[LINE]="";/*current operand dealt with*/
	char *c=NULL, *p=NULL;/*general purpose char pointer, pointer for strNext*/
	symbl *lbl=NULL;
	int inct=0;/*in-function instruction "counter"*/
	
	/*seperating operands if there is more than one*/
	c = strchr(line, ',');
	if(c) *c = ' ';
	
	p = (strstr(line, cmd) + strlen(cmd));/*point to after command*/
	if(strNext(line, op, &p) == EOF) return 0;/*no operands*/
	if((saved(op) == SAVENUM) && (op[0] != '#') && (op[0] != '*'))/*operand is label*/
	{/*first operand*/
		if(!(lbl = findSym(TABLE, op))) {LINERR(undecLbl)}/*label not declared in file*/
		if(!errFlag)/*no error so far(if an error occured no point in dealing with output data)*/
		{
			inct = labelAds(pg, lbl);/*insert label address into instruction array*/
			if(lbl->ARE == signE)/*extern appearence in file*/
			{
				if(!(EXTRNS = addElbl(EXTRNS, lbl->name, inct+100, signE, errNum))) return 1;/*error occured in function*/
			}
		}
	}
	if(strNext(line, op, &p) == EOF) return 0;/*one operand*/
	if((saved(op) == SAVENUM) && (op[0] != '#') && (op[0] != '*'))/*operand is label*/
	{/*second operand*/
		if(!(lbl = findSym(TABLE, op))) {LINERR(undecLbl)}/*label not declared in file*/
		if(!errFlag)/*no error so far(if an error occured no point in dealing with output data)*/
		{
			inct = labelAds(pg, lbl);/*insert label address into instruction array*/
			if(lbl->ARE == signE)/*extern appearence in file*/
			{
				if(!(EXTRNS = addElbl(EXTRNS, lbl->name, inct+100, signE, errNum))) return 1;/*error occured in function*/
			}
		}
	}
	/*if there are more operands it would have been dealt with in first run*/
	
	return 0;
}

/*the function prints the appropriate data into the ".ob" file
in the manner specified in maman 14.*/
void printOb(FILE *fp, progG *pg)
{
	int i=0;
	fprintf(fp, "%d %d\n", ic, dc);/*headline*/
	for(; i<ic; i++)/*print instructions*/
	{
		fprintf(fp, "%04d %05o\n", (i+100), ia[i]);
	}
	for(i=0; i<dc; i++)/*print data*/
	{
		fprintf(fp, "%04d %05o\n", (i+ic+100), da[i]);
	}
}

/*the function prints the appropriate data into the ".ent"
or ".ext" file in the manner specified in maman 14.*/
void printEfile(FILE *fp, Elbl *lbls)
{
	while(lbls)/*go over every Elbl data*/
	{
		fprintf(fp, "%s %04d\n", lbls->name, lbls->line);
		lbls = lbls->next;
	}
}
