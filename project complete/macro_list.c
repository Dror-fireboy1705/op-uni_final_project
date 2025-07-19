/*
this file includes functions to orgenize and deal with macros during
the pre_assembler, which are defined as recursive lists.
*/

#include "assembler.h"

/*the function gets the head of a list of macros and a name for a new macro.
the function adds the new macro to the list and returns the head of the list.
the function returns NULL if an error occured.*/
macro* addMac(macro *head, char *name, int *errNum)
{
	macro *node = head;
	if(!node)/*list empty*/
	{
		node = (macro*)malloc(sizeof(macro));/*allocate space for first macro*/
		if(!node)/*allocation failed*/
		{
			*errNum = allocFail;
			return NULL;
		}
		strcpy(node->name, name);/*insert name*/
		node->code = NULL;/*initialize other values*/
		node->next = NULL;
		return node;
	}
	
	while(node->next)/*get to end of list*/
	{
		if(!strcmp(node->name, name))/*check if macro with the same name has been defined*/
		{
			/*signal error and return NULL*/
			*errNum = usedMac;
			return NULL;
		}
		node=node->next;
	}
	node->next = (macro*)malloc(sizeof(macro));/*allocate space for new macro*/
	if(!node->next)/*allocation failed*/
	{
		*errNum = allocFail;
		return NULL;
	}
	node=node->next;
	strcpy(node->name, name);/*insert name*/
	node->code = NULL;/*initialize othe values*/
	node->next=NULL;
	
	return head;
}

/*the function gets a specific macro and a string of code.
the function adds the line of code to the given macro,
and return 0 for no errors or the error number, if occured.*/
int addLineMac(macro *node, char *line)
{
	if(!(node->code))/*first line of macro*/
	{
		(node->code) = (char*)calloc(strlen(line)+1, sizeof(char));/*allocate space for line*/
		if(!(node->code)) return allocFail;/*allocation failed*/
		strcpy(node->code, line);/*copy code*/
		return 0;
	}
	node->code = (char*)realloc(node->code, sizeof(char)*(strlen(node->code)+strlen(line)));/*allocate new space for extra line*/
	if(!(node->code)) return allocFail;/*allocation failed*/
	strcat(node->code, line);/*copy code to end of previous code*/
	return 0;
}

/*the function recieves the head of a macro list and the name of a macro in the list.
the function returns a pointer to the macro with the given name, or NULL
if no such macro was found.*/
macro* findMac(macro *head, char *name)
{
	macro *ptr = head;
	while(ptr)/*go through list*/
	{
		if(!strcmp(ptr->name, name)) return ptr;/*macro found*/
		ptr=ptr->next;
	}
	return NULL;/*macro not found*/
}

/*the function recieves the head of a macro list and returns the last macro in the list.*/
macro* lastMac(macro *head)
{
	macro *ptr = head;
	if(!ptr) return NULL;/*list empty*/
	while(ptr->next) ptr=ptr->next;/*move forward until last macro*/
	return ptr;/*return the last macro*/
}

/*the function frees the allocated space for all macros in the given list(defined recursively)*/
void freeMac(macro *node)
{
	if(!node) return;/*exit condition(end of list)*/
	freeMac(node->next);/*move to next macro*/
	free(node);/*free current macro*/
}
