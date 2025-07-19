/*
this file includes the main function and calls the entire rest of the project.
*/

#include "assembler.h"

/*the function goes thorugh all the files given as arguments in command line
and calls the rest of the functions in the project to compile each file appropriately.*/
int main(int argc, char *argv[])
{
	FILE *fp=NULL;/*pointer to current file*/
	char *fName=NULL;/*name of current file*/
	macro *tableMac = NULL;/*recursive list for macros*/
	symbl *tableSym = NULL;/*recursive list for labels*/
	progG *pg = NULL;/*program guide*/
	int errFlag=0;/*flag an error for the second run of assembler*/
	int i=1;
	
	for(; argc>1; argc--, i++)/*go through all given arguments*/
	{
		pg = newProgG();/*restart program guide for new file*/
		
		/*copy name of file*/
		fName = (char*)malloc(sizeof(char)*(strlen(argv[i])+strlen(".as")+1));
		strcpy(fName, argv[i]);
		strcat(fName, ".as");
		
		/*start assembling*/
		fp = fopen(fName, "r");
		if(!fp)/*file doesn't exist*/
		{
			/*signal error and move to next file*/
			prnErr(invldFile,"",0,fName);
			continue;
		}
		fp = macLay(fp, fName, &tableMac);/*pre-assembler*/
		if(!fp) continue;/*error found in pre_assembler, move to next file*/
		
		fp = freopen(fName, "r", fp);/*file recieved in open mode "w". change to "r"*/
		if(!fp)/*opening of file failed*/
		{
			/*freopen failed, signal error and move to next file*/
			printf("computer fault, error occured with file \"%s\"", fName);
			continue;
		}
		errFlag = run1st(fp, fName, pg, &tableSym, &tableMac);/*first run of assembler*/
		
		if(fseek(fp, 0, 0))/*start file from beginning again for second run*/
		{
			/*fseek failed, signal error and move to next file*/
			printf("computer fault, error occured with file \"%s\"", fName);
			continue;
		}
		run2nd(fp, &fName, pg, &tableSym, errFlag);/*second run of assembler and creation of output files*/
		
		freeAll(&fp, &fName, &tableMac, &tableSym, &pg);/*free all allocated space*/
	}
	
	return 0;
}

