/*
this file includes functions to orgenize and deal with entrys and
externals during the assembler, which are defined as recursive lists.
Elbl is short for "entry or external label", and will be used often
throughout this file.
*/

#include "assembler.h"

/*the function gets the head of a list of Elbls and a name for a new Elbl.
the function adds the new Elbl to the list and returns the head of the list.
the function returns NULL if an error occured.*/
Elbl* addElbl(Elbl *head, char *name, int line, int type, int *errNum)
{
	Elbl *node = head;
	if(!node)/*list empty*/
	{
		node = (Elbl*)malloc(sizeof(Elbl));
		if(!node)/*allocation failed*/
		{
			*errNum = allocFail;
			return NULL;
		}
		strcpy(node->name, name);/*insert name*/
		node->line = line;/*insert location line*/
		node->next=NULL;/*initialize other values*/
		return node;
	}
	
	if(type == signR)/*entrys*/
	{
		while(node->next)/*get to end of list*/
		{
			if(!strcmp(node->name, name))
			{
				*errNum = usedLbl;/*label with the same name has been declared as entry*/
				return NULL;
			}
			node = node->next;/*advance to next*/
		}
	}
	else/*type == signE(externals)*/
	{
		while(node->next) node = node->next;/*get to end of list*/
	}
	node->next = (Elbl*)malloc(sizeof(Elbl));
	if(!node->next) return NULL;/*allocation failed*/
	node=node->next;
	strcpy(node->name, name);/*insert name*/
	node->line = line;/*insert location line*/
	node->next=NULL;/*initialize other values*/
	
	return head;
}

/*the function frees the allocated space for all Elbls in the given list(defined recursively)*/
void freeElbl(Elbl *head)
{
	if(!head) return;/*exit condition(end of list)*/
	freeElbl(head->next);/*move to next Elbl*/
	free(head);/*free current Elbl*/
}
