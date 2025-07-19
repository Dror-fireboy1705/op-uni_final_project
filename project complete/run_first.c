/*
this file includes the main function of the first run of the assembler and all
functions assisting it. the file holds operations neccesary for: creating the words
of instrcutions and data; storing data about labels; and orgenizing most of the
data for the output ".ob" file.
*/

#include "assembler.h"

#define LINERR(num) *errNum = num;\
		    return 1;
#define TABLESYM (*tableSym)
#define TABLEMAC (*tableMac)

int readLine1st(char*, progG*, symbl**, macro**, int*);
int cmdFound(char*, char*, progG*, int*);
int datFound(char*, char*, progG*, int*);
int addNumDat(char*, progG*, int*);

/*the main function of the first run, calling all other functions.
reads directly from the pre_assembled ".am" file, and sends the line
for interepretation. whilst holding all neccesary in the given parameters.
the function returns 1 if an error was found during the first run and 0 otherwise.*/
int run1st(FILE *fp, char *nameFile, progG *pg, symbl **tableSym, macro **tableMac)
{
	char line[LINE] = "";/*current line being read*/
	int errFlag = 0, errNum=-1, LC=0;/*error data and line counter*/
	
	while(fgets(line, LINE, fp))/*read file*/
	{
		if((ic+dc) >= MAXNUM)/*file too big for computer detailed in maman 14*/
		{
			prnErr(fileSize, NULL, 0, nameFile);/*signal error*/
			return 1;
		}
		LC++;/*advance line counter*/
		if(readLine1st(line, pg, tableSym, tableMac, &errNum))/*error found*/
		{
			prnErr(errNum, line, LC, nameFile);/*print error details*/
			errNum=-1;/*reset error indicator to no error*/
			errFlag = 1;/*flag an error for second run*/
		}
	}
	sepData(TABLESYM, pg);/*seperate data labels from instruction labels*/
	
	return errFlag;
}

/*the function recieves a string holding current line, and pointers to variables
used to hold details regarding the whole file. the function interprets the
line and it's meaning and stores the appropriate information in the given parameters.
the function returns 1 if an error was found and 0 otherwise.*/
int readLine1st(char *line, progG *pg, symbl **tableSym, macro **tableMac, int *errNum)
{
	char cur[LINE]="", nameLbl[LABEL]="";/*current word, name of label(if found)*/
	char *c=NULL, *p=NULL;/*pointer to find ':', pointer for strNext*/
	
	strNext(line,cur,&p);/*read first word of line*/
	if((c = strchr(cur, ':')))/*charachter ':' found, declaration of label*/
	{
		if(!vldLabel(cur)) {LINERR(invldLbl)}
		
		strcpy(nameLbl, cur);/*save name of label*/
		strNext(line,cur,&p);/*move to command*/
		if((saved(cur) == data) || (saved(cur) == string))/*after label data or string*/
		{
			if(!(TABLESYM = addSym(TABLESYM, nameLbl, dc+100, signR, dat, TABLEMAC, errNum))) return 1;/*error occured in function*/
			if(datFound(cur, line, pg, errNum)) return 1;
		}
		else if((saved(cur) == entry) || (saved(cur) == extrn))/*after label entry or extern*/
		{
			if(saved(cur) == extrn)
			{
				if(strNext(line, cur, &p) == EOF) {LINERR(invldLbl)}/*extrn with no name of label*/
				if(strlen(cur) >= LABEL) {LINERR(invldLbl)}/*name of label bigger then 31 charachters*/
				if(!(TABLESYM = addSym(TABLESYM, cur, signE, signE, inst, TABLEMAC, errNum))) return 1;/*error occured in function*/
			}
			/*if entry, do nothing(entrys will be dealt with in second run)*/
		}
		else if(command(cur) != CMDNUM)/*after label command*/
		{
			if(!(TABLESYM = addSym(TABLESYM, nameLbl, ic+100, signR, inst, TABLEMAC, errNum))) return 1;/*error occured in function*/
			if(cmdFound(cur, line, pg, errNum)) return 1;/*error occured in function*/
		}
		else {LINERR(unknownCmd)}/*not a recognized word*/
	}
	
	else/*no label(same thing without saving a label in list)*/
	{
		if((saved(cur) == data) || (saved(cur) == string))/*data or string*/
		{
			if(datFound(cur, line, pg, errNum)) return 1;/*error occured in function*/
		}
		else if((saved(cur) == entry) || (saved(cur) == extrn))/*entry or extern*/
		{
			if(saved(cur) == extrn)
			{
				if(strNext(line, cur,&p) == EOF) {LINERR(invldLbl)}/*extrn with no name of label*/
				if(strlen(cur) >= LABEL) {LINERR(invldLbl)}/*name of label bigger then 31 charachters*/
				if(!(TABLESYM = addSym(TABLESYM, cur, signE, signE, inst, TABLEMAC, errNum))) return 1;/*error occured in function*/
			}
			/*if entry, do nothing(entrys will be dealt with in second run)*/
		}
		else if(command(cur) != CMDNUM)/*command*/
		{
			if(cmdFound(cur, line, pg, errNum)) return 1;/*error occured in function*/
		}
		else {LINERR(unknownCmd)}/*not a recognized word*/
	}
	return 0;
}

/*the function recieves the word of a command and the line the command is in.
the function interprets the meaning of the command and stores appropriate
computer code for the command and it's operands in the program guide.
the function returns 1 if an error was found and 0 otherwise.*/
int cmdFound(char *cmd, char *line, progG *pg, int *errNum)
{
	char op1[LINE]="", op2[LINE]="";/*no operand can be longer than 80 characters(line length maximum)*/
	char *c=NULL, *p=NULL;/*general purpose char pointer, pointer for strNext*/
	int Ncmd = command(cmd);/*number of command*/
	int opNum1=0, opNum2=0, opType1=0, opType2=0, adType1=0, adType2=0;/*numbers and other information of operands*/
	
	if(Ncmd<=lea)/*two operands required*/
	{
		c = strchr(line, ',');
		if(c) *c = ' ';/*seperating first operand from comma*/
		
		/*first operand*/
		p = (strstr(line, cmd) + strlen(cmd));
		if(strNext(line, op1, &p)==EOF) {LINERR(invldOprd)}/*no operand*/
		if(ognOprd(op1, Ncmd, &opNum1, &opType1, &adType1, errNum)) return 1;/*error with operand*/
		
		/*second operand*/
		if(strNext(line, op2, &p)==EOF) {LINERR(invldOprd)}/*no operand*/
		if(tgtOprd(op2, Ncmd, &opNum2, &opType2, &adType2, errNum)) return 1;/*error with operand*/
		
		/*check if there are more operands(which is an error)*/
		if(strNext(line, op2, &p) != EOF) {LINERR(invldOprd)}
		
		/*insert into program guide*/
		if(addInst(pg, Ncmd, adType1, adType2)) {LINERR(allocFail)}
		if((adType1 == ognAdRegI || adType1 == ognAdRegD) && (adType2 == tgtAdRegI || adType2 == tgtAdRegD))/*both operands are registers*/
		{
			if(addOpRegs(pg, opNum1, opNum2, opType1)) {LINERR(allocFail)}
		}
		else
		{
			if(addOprd(pg, opNum1, opType1)) {LINERR(allocFail)}
			if(addOprd(pg, opNum2, opType2)) {LINERR(allocFail)}
		}
	}
	else if(Ncmd<=jsr)/*one operand required*/
	{
		/*read operand*/
		p = (strstr(line, cmd) + strlen(cmd));
		if(strNext(line, op1, &p) == EOF) {LINERR(invldOprd)}/*no operand*/
		if(tgtOprd(op1, Ncmd, &opNum1, &opType1, &adType1, errNum)) return 1;/*error with operand*/
		
		/*check if there are more operands(which is an error)*/
		if(strNext(line, op1, &p) != EOF) {LINERR(invldOprd)}
		
		/*insert into program guide*/
		if(addInst(pg, Ncmd, adType1, -1)) {LINERR(allocFail)}
		if(addOprd(pg, opNum1, opType1)) {LINERR(allocFail)}
	}
	else /*"if(Ncmd<=stop) no operand required*/
	{
		/*check if there are operands(which is an error)*/
		p = (strstr(line, cmd) + strlen(cmd));
		if(strNext(line, op1, &p) != EOF) {LINERR(invldOprd)}
		
		if(addInst(pg, Ncmd, -1, -1)) {LINERR(allocFail)}
	}
	
	return 0;
}

/*the function is equivalent to "cmdFound". it does the same for
the data command ".data" and ".string".*/
int datFound(char *cmd, char *line, progG *pg, int *errNum)
{
	char numLine[LINE];/*array for seperating the numbers(strtok changes its given array)*/
	char *p=NULL;/*pointer for strNext*/
	char *cur=NULL, curArr[LINE]="";/*char pointer and char array for where a pointer does not work(LINE is big enough for every possible entrance to curArr)*/
	char *p1=NULL, *p2=NULL;/*pointers for quotes of string*/
	
	if(saved(cmd) == data)
	{
		cur = (strstr(line, cmd) + strlen(cmd));/*point to number after command*/
		strcpy(numLine, cur);/*copy number after command into a different space to keep line unchanged*/
		cur = strtok(numLine, ",");/*take in nexxt parameter*/
		if(!cur)/*at most 1 parameter*/
		{
			if(strNext(numLine, cur, &p) == EOF) {LINERR(invldOprd)}/*no parameter*/
		}
		/*process first parameter*/
		if(addNumDat(cur, pg, errNum)) return 1;/*error in function*/
		
		/*more than one parameter*/
		while((cur = strtok(NULL, ",")))/*take in next paramater(and check if one is recieved)*/
		{
			/*process parameter*/
			if(addNumDat(cur, pg, errNum)) return 1;/*error in function*/
		}
	}
	else /*saved(cmd) == string*/
	{
		p1 = strchr(line, '"');
		if(!p1) {LINERR(invldOprd)}/*no quotes*/
		p2 = strrchr(line, '"');
		if(p1 == p2) {LINERR(invldOprd)}/*start of string with no end*/
		if(sscanf(p2+1, "%s", curArr) != EOF) {LINERR(invldOprd)}/*writing after end of string*/
		while(++p1 != p2)/*go over all charachters in string*/
		{
			if(addData(pg, *p1)) {LINERR(allocFail)};/*process charachters one at a time*/
		}
		if(addData(pg, '\0')) {LINERR(allocFail)};/*process charachters one at a time*/
	}
	
	return 0;
}

/*the function recieves a string representing a number
found in ".data" command, and adds it's data to the data
array in the program guide. the function return 1 if an
error occured and a 0 otherwise.*/
int addNumDat(char *num, progG *pg, int *errNum)
{
	char *c=NULL;/*pointer for parts of the number string*/
	
	while(isspace(*num)) num++;/*skip spaces*/
	c = strchr(num, '\n');/*if last number in command*/
	if(c) *c = '\0';/*get rid of "\n"*/
	if(*num == '-')/*number is meant to be negative*/
	{
		num++;/*skip to number digits*/
		if(isNum(num) && (atol(num) <= (MAXNUM/2)+1))/*string is a real number and not too big for computer*/
		{
			if(addData(pg, neg(atol(num)))) {LINERR(allocFail)}/*add number data to array*/
		}
		else {LINERR(invldOprd)}
	}
	else/*number is meant to be positive*/
	{
		if(*num == '+') num++;/*skip to number digits*/
		if(isNum(num) && (atol(num) <= MAXNUM/2))/*string is a real number and not too big for computer*/
		{
			if(addData(pg, atol(num))) {LINERR(allocFail)}/*add number data to array*/
		}
		else {LINERR(invldOprd)}
	}
	
	return 0;
}
