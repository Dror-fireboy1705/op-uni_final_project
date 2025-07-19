/*
this file includes functions to orgenize and deal with labels during
the assembler, which are defined as recursive lists.
*/

#include "assembler.h"

/*the function gets the head of a list of labels and a name for a new label.
the function adds the new label to the list and returns the head of the list.
the function returns NULL if an error occured.*/
symbl* addSym(symbl *head, char *name, int ads, int are, int id, macro *tableMac, int *errNum)
{
	symbl *node = head;
	if(!node)/*list empty*/
	{
		node = (symbl*)malloc(sizeof(symbl));
		if(!node)/*allocation failed*/
		{
			*errNum = allocFail;
			return NULL;
		}
		strcpy(node->name, name);/*insert name*/
		node->ads = ads;/*initialize other values*/
		node->ARE = are;
		node->ID = id;
		node->next = NULL;
		return node;
	}
	
	while(node->next)/*get to end of list*/
	{
		if(!strcmp(node->name, name))/*check if label with the same name has been defined*/
		{
			*errNum = usedLbl;/*label with the same name has been defined*/
			return NULL;
		}
		if(findMac(tableMac, name))/*check if macro with the same name has been defined*/
		{
			*errNum = usedLbl;/*macro with the same name has been defined*/
			return NULL;
		}
		node=node->next;
	}
	node->next = (symbl*)malloc(sizeof(symbl));
	if(!node->next) return NULL;/*allocation failed*/
	node=node->next;
	strcpy(node->name, name);/*insert name*/
	node->ads = ads;/*initialize other values*/
	node->ARE = are;
	node->ID = id;
	node->next=NULL;
	
	return head;
}

/*the function recieves the head of a label list and the name of a label in the list.
the function returns a pointer to the label with the given name, or NULL
if no such label was found.*/
symbl* findSym(symbl *head, char *name)
{
	symbl *ptr = head;
	while(ptr)/*go through list*/
	{
		if(!strcmp(ptr->name, name)) return ptr;/*symbol found*/
		ptr=ptr->next;
	}
	return NULL;/*symbol not found*/
}

/*the function recieves the head of a label list, and checks every label
in the list. if that label is detailed as a data label, the function adds
the IC of the program guide to the address of the label. that is to
seperate the data labels from the instruction labels, as required in the maman.*/
void sepData(symbl *head, progG *pg)
{
	symbl *node = head;
	while(node)/*go through list*/
	{
		if(node->ID == dat) node->ads += ic;/*if data symbol, update address to seperate from instructions*/
		node = node->next;
	}
}

/*the function frees the allocated space for all labels in the given list(defined recursively)*/
void freeSym(symbl *head)
{
	if(!head) return;/*exit condition(end of list)*/
	freeSym(head->next);/*move to next label*/
	free(head);/*free current label*/
}
