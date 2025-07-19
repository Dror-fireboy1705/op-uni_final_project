/*
this file includes functions to help with organizing the data
in the instruction and data arrays, that are used to print
the ".ob" file correctly.
*/

#include "assembler.h"

#define LDEXP(num) ldexp(1,num)
#define LINERR(num) *errNum = num;\
		    return 1;

/*the function receives the required input to add the first word
of an instruction to the instruction array. it returns 0 if
addition was succesful and 1 otherwise.*/
int addInst(progG *pg, int Ncmd, int adType1, int adType2)
{
	/*insert values to last cell in array*/
	ia[ic] = LDEXP(signA);/*ARE sign*/
	ia[ic] += LDEXP(adType1);/*type of first operand*/
	ia[ic] += LDEXP(adType2);/*type of second operand*/
	ia[ic] += ldexp(Ncmd, opCode);/*code of command*/
	
	/*create next cell*/
	ia = (word*)realloc(ia,sizeof(word)*(++ic+1));/*make array one cell bigger*/
	if(!(ia)) return 1;/*allocation failed*/
	ia[ic] = 0;/*initialize new cell*/
	
	return 0;
}

/*the function recieves the required input to add a word
for an operand of an instruction to the instruction array.
it returns 0 if adding was succesful and 1 otherwise.*/
int addOprd(progG *pg, int opNum, int opType)
{
	/*insert values to last cell in array*/
	ia[ic] = LDEXP(opType);/*ARE sign of operand*/
	ia[ic] += ldexp(opNum,tgtAdInst);/*value of operand*/
	
	/*create next cell*/
	ia = (word*)realloc(ia, sizeof(word)*(++ic+1));/*make array one cell bigger*/
	if(!(ia)) return 1;/*allocation failed*/
	ia[ic] = 0;/*initialize new cell*/
	
	return 0;
}

/*the function is a special case of "addOprd". specifically, when
both operands are a register or a register's contents. in which case,
it is required to add both operands in one word of code.*/
int addOpRegs(progG *pg, int opNum1, int opNum2, int opType)
{
	/*insert values to last cell in array*/
	ia[ic] = LDEXP(opType);/*ARE sign(for registers always "A")*/
	ia[ic] += ldexp(opNum1,tgtAdInst);/*value of first register*/
	ia[ic] += ldexp(opNum2,tgtAdInst);/*value of second register*/
	/*registers were moved appropriatley when to fit in their locations before given to function*/
	
	/*create next cell*/
	ia = (word*)realloc(ia, sizeof(word)*(++ic+1));/*make array one cell bigger*/
	if(!(ia)) return 1;/*allocation failed*/
	ia[ic] = 0;/*initialize new cell*/
	
	return 0;
}

/*the fuction is equivalent to "addOprd". it does the
same thing for data words, in the data array.*/
int addData(progG *pg, int val)
{
	/*insert values to last cell*/
	da[dc] = val;
	/*create next cell*/
	da = (word*)realloc(da, sizeof(word)*(++dc+1));/*make array one cell bigger*/
	if(!(da)) return 1;/*allocation failed*/
	da[dc] = 0;/*initialize new cell*/
	
	return 0;
}

/*the function recieves a string meant to represent an the origin operand of a word
and the number of command the operand is for, as well as pointers to integers,
meant for storage of the operand data. the function reads the operand string
and figures the type of operand it is, and what values are fitting for it.
the function insert each value into the appropriate integer variable through
the given pointers. the function returns 0 if read was succesful and 1 otherwise.*/
int ognOprd(char *op, int Ncmd, int *opNum, int *typeOp, int *adType, int *errNum)
{
	if(((saved(op) > r7) && (saved(op) != SAVENUM)) || (command(op) != CMDNUM)) {LINERR(invldOprd)}/*language saved word or command given as operand*/
	if(*op == '#')/*number operand*/
	{
		/*check if number is valid operand for command*/
		if(!(Ncmd<=sub)) {LINERR(invldOprd)}
		
		op++;/*move to number section of operand*/
		if(*op == '-')/*number is meant to be negative*/
		{
			op++;/*skip to digits of operand*/
			if(isNum(op) && (atol(op) <= (MAXNUM/2)+1))/*operand is a real number and not too big for computer*/
			{
				/*store information of operand*/
				*opNum = (neg(atol(op)) & (MAXNUM-1));
				*typeOp = signA;
				*adType = ognAdInst;
			}
			else {LINERR(invldOprd)}
		}
		else/*number is meant to be positive*/
		{
			if(*op == '+') op++;/*skip to digits of operand*/
			if(isNum(op) && (atol(op) <= MAXNUM/2))/*operand is a real number and not too big for computer*/
			{
				/*store information of operand*/
				*opNum = atol(op);
				*typeOp = signA;
				*adType = ognAdInst;
			}
			else {LINERR(invldOprd)}
		}
	}
	else if(*op == '*')/*operand is register content*/
	{
		/*check if register content is valid operand*/
		if(!(Ncmd<=sub)) {LINERR(invldOprd)}
		
		op++;/*move to operand name*/
		if(saved(op) > r7) {LINERR(invldOprd)}/*not register name*/
		
		/*store information of operand*/
		*opNum = ldexp(saved(op),regOgn);
		*typeOp = signA;
		*adType = ognAdRegI;
	}
	else if(saved(op) <= r7)/*operand is register*/
	{
		/*check if register is valid operand*/
		if(!(Ncmd<=sub)) {LINERR(invldOprd)}
		
		/*store information of operand*/
		*opNum = ldexp(saved(op),regOgn);
		*typeOp = signA;
		*adType = ognAdRegD;
	}
	else/*operand is label(or unrecognized symbol which will be discoverd in second run)*/
	{/*labels are valid operand for every command*/
		/*insert values marking cell for label address*/
		*opNum = 0;
		*typeOp = WORD;
		*adType = ognAdDrct;
	}
	return 0;
}

/*the function is equivalent to "ognOprd". it does the same thing for the target operand
of a word instead of the origin operand. and insert values accordingly.*/
int tgtOprd(char *op, int Ncmd, int *opNum, int *typeOp, int *adType, int *errNum)
{
	if(((saved(op) > r7) && (saved(op) != SAVENUM)) || (command(op) != CMDNUM)) {LINERR(invldOprd)}/*language saved word given as operand*/
	if(*op == '#')/*number operand*/
	{
		/*check if number is valid operand for command*/
		if(!(Ncmd==cmp || Ncmd==prn)) {LINERR(invldOprd)}
		
		op++;
		if(*op == '-')/*number is meant to be negative*/
		{
			op++;/*skip to digits of operand*/
			if(isNum(op) && (atol(op) <= (MAXNUM/2)+1))/*operand is a real number and not too big for computer*/
			{
				/*store information of operand*/
				*opNum = (neg(atol(op)) & (MAXNUM-1));
				*typeOp = signA;
				*adType = tgtAdInst;
			}
			else {LINERR(invldOprd)}
		}
		else/*number is meant to be positive*/
		{
			if(*op == '+') op++;/*skip to digits of operand*/
			if(isNum(op) && (atol(op) <= MAXNUM/2))/*operand is a real number and not too big for computer*/
			{
				/*store information of operand*/
				*opNum = atol(op);
				*typeOp = signA;
				*adType = tgtAdInst;
			}
			else {LINERR(invldOprd)}
		}
	}
	else if(*op == '*')/*operand is register content*/
	{
		/*register content is valid target operand for every command with target operand*/
		if(!(Ncmd<=sub)) {LINERR(invldOprd)}
		
		op++;/*move to operand name*/
		if(saved(op) > r7) {LINERR(invldOprd)}/*not register name*/
		
		/*store information of operand*/
		*opNum = ldexp(saved(op),regTgt);
		*typeOp = signA;
		*adType = tgtAdRegI;
	}
	else if(saved(op) <= r7)/*operand is register*/
	{
		/*check if register is valid operand*/
		if((Ncmd==jmp) || (Ncmd==bne) || (Ncmd==jsr)) {LINERR(invldOprd)}
		
		/*store information of operand*/
		*opNum = ldexp(saved(op),regTgt);
		*typeOp = signA;
		*adType = tgtAdRegD;
	}
	else/*operand is label(or unrecognized symbol which will be discoverd in second run)*/
	{/*labels are valid operand for every command*/
		/*insert values marking cell for label address*/
		*opNum = 0;
		*typeOp = WORD;
		*adType = tgtAdDrct;
	}
	return 0;
}

/*the function recives a label and places it's address into the
first cell marked for label address in the instruction array.
the function returns the index of the cell found.*/
int labelAds(progG *pg, symbl *lbl)
{
	int i=0;
	while(ia[i] != LDEXP(WORD)) i++;/*get to first spot with cell marked for label address*/
	ia[i] = LDEXP(lbl->ARE)+ldexp(lbl->ads, tgtAdInst);/*insert label address correctly*/
	return i;
}
