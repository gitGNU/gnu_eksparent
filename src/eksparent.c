/**
	@file
	@author Florian Evaldsson
	@version 0.1
	
	@section LICENSE
	
	Copyright (c) 2015 Florian Evaldsson <florian.evaldsson@telia.com>


	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	@section DESCRIPTION

	This file contains all the commonly used functions by the user. 
	Mostly functions to handle the eksparent and such.
*/

#include "eksparent.h"
#include "misc.h"

/**
	This function defines the defaults of the EksParent structure.
	
	@param thisParent
		The EksParent Structure to set the values from
	@param name
		the name to the EksParent structure
	@param ptype
		the type of the object
	@return
		will return 1 if it succeded
*/
static void eks_parent_set_base(EksParent *thisParent, EksParentType ptype)
{
	if(ptype==EKS_PARENT_TYPE_COMMENT)
	{
		thisParent->structure=-1;
	}
	else if(ptype==EKS_PARENT_TYPE_VALUE)
	{
		if(thisParent->upperEksParent==NULL || thisParent->upperEksParent==thisParent)
		{
			thisParent->structure=0;
			thisParent->upperEksParent=thisParent;
		}
		else
		{
			thisParent->structure=thisParent->upperEksParent->structure+1;
		}
	}
	else
	{
		thisParent->structure=-10;
	}
	
	thisParent->custom=NULL;
	thisParent->firstChild=NULL;
}

/**
	create a new parent
	
	@param name
		the name to use
	@param ptype
		the type to use
	@param topParent
		the toplevel parent to use
	@param extras
		the extras to use
	@return
		returns the new eksparent
*/
EksParent *eks_parent_new(const char *name, EksParentType ptype, EksParent *topParent, EksParent *extras)
{
	EksParent *thisParent=malloc(sizeof(EksParent));
	if(thisParent==NULL)
	{
		eks_error_message("Failed to allocate space for the child!");
		return NULL;
	}
	
	if(topParent)
		thisParent->upperEksParent=topParent;
	else
		thisParent->upperEksParent=thisParent;
	
	thisParent->firstExtras=extras;
	
	eks_parent_set_base(thisParent,ptype);
	eks_parent_set_string(thisParent,name);
	
	thisParent->nextChild=thisParent;
	thisParent->prevChild=thisParent;
	
	return thisParent;
}

/**
	Get the string (char*) from a parent object.
	
	@param thisParent
		the parent to get its child from
	@return
		returns the 'string' or NULL if failed
*/
char *eks_parent_get_string(EksParent *thisParent)
{
	if(thisParent)
	{
		if(thisParent->type==EKS_PARENT_VALUE_STRING)
		{
			return g_strdup(thisParent->name);
		}
		else if(thisParent->type==EKS_PARENT_VALUE_INT)
		{
			return eks_int_to_string(thisParent->iname);
		}
		else if(thisParent->type==EKS_PARENT_VALUE_DOUBLE)
		{
			return g_strdup_printf("%f", thisParent->dname);
		}
		
		eks_error_message("Unknown type/value!");
	}
	
	eks_error_message("The parent was NULL!");
	return NULL;
}

/**
	Get the int from a parent object.
	
	@param thisParent
		the parent to get its child from
	@return
		returns the int
*/
intptr_t eks_parent_get_int(EksParent *thisParent)
{
	if(thisParent->type!=EKS_PARENT_VALUE_INT)
	{
		eks_warning_message("The type is not an int!");
	}
	
	if(thisParent)
		return thisParent->iname;
	else
	{
		eks_error_message("The parent was NULL!");
		return NULL;
	}
}

/**
	Get the double from a parent object.
	
	@param thisParent
		the parent to get its child from
	@return
		returns the double
*/
double eks_parent_get_double(EksParent *thisParent)
{
	if(thisParent->type!=EKS_PARENT_VALUE_DOUBLE)
	{
		eks_warning_message("The type is not a double!");
	}
	
	if(thisParent)
		return thisParent->dname;
	else
	{
		eks_error_message("The parent was NULL!");
		return 0.0;
	}
}

/**
	Do something foreach child
	
	@param thisParent
		The parent to do the foreach on
	@param func
		The function to run
	@param inparam
		The input parameter
*/
void eks_parent_foreach_child(EksParent *thisParent,void *func,void *inparam)
{
	void (*typefunction)(EksParent*,void*);
	
	typefunction=func;

	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return;
	}
	
	EksParent *loopUnit=firstUnit;
	EksParent *sloopUnit;

	do
	{
		sloopUnit=loopUnit->nextChild;
		
		typefunction(loopUnit,inparam);
		
		loopUnit=sloopUnit;
	}while(loopUnit!=firstUnit);
	
	return;
}

/**
	Get the amount of children (all types) from a parent object.
	
	@param thisParent
		the parent to count children from
	@return
		the amount of children
*/
size_t eks_parent_get_child_amount(EksParent *thisParent)
{
	size_t amount=0;

	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return 0;
	}
	
	EksParent *loopUnit=firstUnit;

	do
	{
		amount++;
	
		loopUnit=loopUnit->nextChild;
	}while(loopUnit!=firstUnit);
	
	return amount;
}

/**
	Set the name as a string.
	
	@param thisParent
		the input parent
	@param name
		the input string
	@param ptype
		the type of object, eg text or comment
	@return
		0=fail,1=successful

*/
int eks_parent_set_string(EksParent *thisParent, const char *name)
{
	if(thisParent)
	{
		int nameLen=name?strlen(name):0;
		if(name!=NULL && nameLen>0)
		{
			int type=eks_string_to_double(name,&thisParent->dname,&thisParent->iname);
			
			thisParent->type=type;
			
			if(type==EKS_PARENT_VALUE_STRING)
				thisParent->name=g_strndup(name,nameLen);
		}
		else
		{
			thisParent->name=NULL;
			
			thisParent->type=EKS_PARENT_VALUE_STRING;
		}
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

/**
	Set the name as a int.
	
	@param thisParent
		the input parent
	@param name
		the input integer
	@param ptype
		the type of object, eg text or comment
	@return
		0=fail,1=successful

*/
int eks_parent_set_int(EksParent *thisParent, intptr_t name)
{
	if(thisParent)
	{
		thisParent->iname=name;
			
		thisParent->type=EKS_PARENT_VALUE_INT;
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

/**
	Set the name as a double.
	
	@param thisParent
		the input parent
	@param name
		the input double
	@param ptype
		the type of object, eg text or comment
	@return
		0=fail,1=successful

*/
int eks_parent_set_double(EksParent *thisParent, double name)
{
	if(thisParent)
	{
		thisParent->dname=name;
			
		thisParent->type=EKS_PARENT_VALUE_DOUBLE;
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

/**
	This function will fix the parents children structure, that means that it will set the structure number to the parents+1
	
	@param thisParent
		The EksParent to fix
	@return
		void
*/
void eks_parent_fix_structure(EksParent *thisParent)
{
	if(thisParent && thisParent->structure>=0 && thisParent->firstChild && thisParent->upperEksParent!=thisParent)
	{
		thisParent->structure=thisParent->upperEksParent->structure+1;
		
		EksParent *firstUnit=thisParent->firstChild;
		
		EksParent *loopUnit=firstUnit;

		do
		{
			eks_parent_fix_structure(loopUnit);
		
			loopUnit=loopUnit->nextChild;
		}while(loopUnit!=firstUnit);
	}
	else if(thisParent->structure==0 && thisParent->upperEksParent==thisParent)
	{
		eks_error_message("The parent have its topEksParent as its parent, it might crash after this!");
	}
}

/**
	Use this function if you want to insert a topEksParent into another EksParents structure, 
	This function will insert afterwards
	it will fix so that the EksParents structure is correct.
	
	@param child
		The child as reference
	@param inEksParent
		the parent you want to insert
	@return
		returns 1 if it was successful
*/
int eks_parent_from_child_insert_prev(EksParent *child,EksParent *inEksParent)
{
	if(inEksParent && child->upperEksParent)
	{
		inEksParent->upperEksParent=child->upperEksParent;
		
		if(inEksParent->nextChild==inEksParent || inEksParent->prevChild==inEksParent)
		{
			inEksParent->prevChild=child->prevChild;
			inEksParent->nextChild=child;
			child->prevChild->nextChild=inEksParent;
			child->prevChild=inEksParent;
		}
		else
		{
			eks_error_message("NOT DONE YET!");
			
			///@deprecated
			inEksParent->prevChild=child->prevChild;
			inEksParent->nextChild=child;
			child->prevChild->nextChild=inEksParent;
			child->prevChild=inEksParent;
		}
	
		eks_parent_fix_structure(inEksParent);

		return 1;
	}
		
	eks_error_message("this did not work!");
	
	return 0;
}

/**
	Use this function if you want to insert a topEksParent into another EksParents structure, 
	This function will insert afterwards
	it will fix so that the EksParents structure is correct.
	
	@param thisParent
		The EksParent as reference
	@param childNum
		the child you want to insert it into
	@param inEksParent
		the parent you want to insert
	@return
		returns 1 if it was successful
*/
int eks_parent_insert(EksParent *topParent,EksParent *inEksParent)
{
	if(topParent)
	{
		if(topParent->firstChild)
			eks_parent_from_child_insert_prev(topParent->firstChild,inEksParent);
		else
		{
			topParent->firstChild=inEksParent;
			
			inEksParent->upperEksParent=topParent;
			
			eks_parent_fix_structure(inEksParent);
		}
		
		return 1;
	}
	
	eks_error_message("topParent is NULL!");
	
	return 0;
}

/**
	Will clone a parent

	@param thisParent NO_FREE
		EksParent to clone
	@return NEW
		a pointer to the cloned parent structure
*/
EksParent *eks_parent_clone(EksParent *thisParent)
{
	//define parent
	EksParent *newEksParent;
	
	//assign some memory
	if((newEksParent=calloc(1,sizeof(EksParent))))
	{
		if(thisParent->type==EKS_PARENT_VALUE_INT)
			newEksParent->iname=thisParent->iname;
		else if(thisParent->type==EKS_PARENT_VALUE_DOUBLE)
			newEksParent->dname=thisParent->dname;
		else
			newEksParent->name=g_strdup(thisParent->name);
			
		newEksParent->structure=thisParent->structure;
		
		newEksParent->nextChild=newEksParent;
		newEksParent->prevChild=newEksParent;
		
		if(thisParent->structure==0 || thisParent->upperEksParent==thisParent)
		{
			newEksParent->upperEksParent=newEksParent;
		}
		
		//do it on the bottom ones as well
		if(thisParent->structure>=0)
		{
			EksParent *firstUnit=thisParent->firstChild;
	
			if(!firstUnit)
			{
				goto end_parent_loop;
			}
	
			EksParent *loopUnit=firstUnit;
			EksParent *newTempUnit;
			EksParent *prevTempUnit=NULL;

			do
			{
				newTempUnit=eks_parent_clone(loopUnit);
				newTempUnit->upperEksParent=newEksParent;
				
				if(prevTempUnit!=NULL)
				{
					prevTempUnit->nextChild=newTempUnit;
					newTempUnit->prevChild=prevTempUnit;
				}
				else
				{
					newEksParent->firstChild=newTempUnit;
				}
				
				//if only child
				if(loopUnit==loopUnit->nextChild)
				{
					newTempUnit->nextChild=newTempUnit;
					newTempUnit->prevChild=newTempUnit;
					
					goto end_parent_loop;
				}
				
				loopUnit=loopUnit->nextChild;
				prevTempUnit=newTempUnit;
				
			}while(loopUnit!=firstUnit);
			
			prevTempUnit->nextChild=newEksParent->firstChild;
			newEksParent->firstChild->prevChild=prevTempUnit;
		}
		
		end_parent_loop:
		
		return newEksParent;
	}
	else
	{
		eks_error_message("Could not allocate space for the new child!");
		return NULL;
	}
}

/**
	This function will check the EksParentstructure for a child from a pos.
	
	@param thisParent NO_FREE
		The EksParent Structure to check the child from
	@param pos .
		the position, 0= first child
	@return
		will return 1 if it exists
*/
int eks_parent_check_child(EksParent *thisParent,int pos)
{
	return (thisParent->firstChild && (thisParent->structure>0 || (thisParent->structure==0 && thisParent->upperEksParent==thisParent)));
}

/**
	This function will get the EksParentstructure from a child from a pos.
	
	@param thisParent NO_FREE
		The EksParent Structure to get the child from
	@param pos .
		the position, 0= first child
	@return
		this will give the pointer to the EksParent
*/

EksParent* eks_parent_get_child(EksParent *thisParent,int pos)
{
	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return NULL;
	}
		
	EksParent *loopUnit=firstUnit;

	do
	{
		if(pos<=0)
		{
			return loopUnit;
		}
		
		pos--;
		
		loopUnit=loopUnit->nextChild;
	}while(loopUnit!=firstUnit);
	
	eks_error_message("Your position is probably wrong! Pos[%d] in-EksParent name[%s]",pos,thisParent->name);
	
	return NULL;
}

/**
	Get the first child from the EksParent.

	@param thisParent NO_FREE
		the parent to get the child from
	@return .
		returns the parent
*/

EksParent* eks_parent_get_first_child(EksParent *thisParent)
{
	if(thisParent)
	{
		return thisParent->firstChild;
	}
	
	eks_error_message("Failed to get child!");// There is [%d] children!",thisParent->amount);
	return NULL;
}

/**
	Get the last child from the EksParent.

	@param thisParent NO_FREE
		the parent to get the child from
	@return .
		returns the parent
*/

EksParent* eks_parent_get_last_child(EksParent *thisParent)
{
	if(thisParent && thisParent->firstChild)
	{
		return thisParent->firstChild->prevChild;
	}
	
	eks_error_message("Failed to get child!");// There is [%d] children!",thisParent->amount);
	return NULL;
}

/**
	Go up a number of parents

	@param thisParent NO_FREE
		the parent to get the parent from
	@param amount
		number of times to climb (positive number = upwards, negative = downwards (will create new children if not exists, goes through last child))
	@return .
		returns the parent
*/

EksParent *eks_parent_climb_parent(EksParent *thisParent,int amount)
{
	EksParent *debugparent=thisParent;
	
	uint8_t madetwice=0;
	
	if(amount<0)
	{
		amount=-amount;
	
		for(int i=0;i<amount;i++)
		{
			if(thisParent->firstChild)
			{
				thisParent=thisParent->firstChild->prevChild;
			}
			else
			{
				eks_parent_add_child(thisParent,NULL,EKS_PARENT_TYPE_VALUE,NULL);
			}
		}
	}
	else
	{
		for(int i=0;i<amount;i++)
		{
			thisParent=thisParent->upperEksParent;
		
			//if its the toplevel parent
			if(thisParent==thisParent->upperEksParent)
			{
				if(madetwice==1)
				{
					char *tmpname=eks_parent_get_string(debugparent);
					eks_warning_message("You climbed the parent twice! Amount[%d] Name[%s] Structure[%d]",amount,tmpname,debugparent->structure);
					free(tmpname);
				}
			
				madetwice++;
			}
		}
	}
	
	return thisParent;
}

/**
	This function will get the EksParentstructure from a child from a name.
	
	@param thisParent
		The EksParent Structure to get the child from
	@param tempName
		The childs name
	@return
		this will give the pointer to the EksParent
*/
EksParent* eks_parent_get_child_from_name(EksParent *thisParent,const char *tempName)
{
	if(!thisParent->firstChild)
	{
		eks_error_message("No first child?");
		return NULL;
	}
	
	if(thisParent->structure>=0)
	{
		EksParent *firstUnit=thisParent->firstChild;
		EksParent *loopUnit=firstUnit;
	
		do
		{
			char *testname=eks_parent_get_string(loopUnit);
			if(strcmp(testname,tempName)==0)
			{
				return loopUnit;
			}
			free(testname);
			loopUnit=loopUnit->nextChild;
		}while(loopUnit!=firstUnit);
		
		eks_error_message("No such object found [%s]!",tempName);
	}
	else
	{
		eks_error_message("You tried to get the child from a value or comment!");
	}
	return NULL;
}

/**
	Checks if the parent have that type

	@param thisParent
		the parent
	@param ptype
		the type
	@return
		1 if they are the same 0 if they arent.
*/
int eks_parent_compare_type(EksParent *thisParent, EksParentType ptype)
{
	return ((thisParent->structure>=0 && ptype==EKS_PARENT_TYPE_VALUE) || (ptype==EKS_PARENT_TYPE_COMMENT && thisParent->structure==-1));
}

/**
	Get the amount of children from a specific type

	@param thisParent
		in-parent
	@param ptype
		type of object
	@return
		number of objects
*/
int eks_parent_get_amount_from_type(EksParent *thisParent, EksParentType ptype)
{
	int retamount=0;
	
	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return -1;
	}
		
	EksParent *loopUnit=firstUnit;

	do
	{
		//EksParent *child=eks_parent_get_child(thisParent,i);
		
		if(eks_parent_compare_type(loopUnit,ptype))
			retamount++;
			
		loopUnit=loopUnit->nextChild;
	}while(loopUnit!=firstUnit);
	
	return retamount;
}

/**
	Get a specific child from the pos where the type is from, see eks_parent_get_amount_from_type
	
	@param thisParent
		the in-parent
	@param pos
		position from where we want it
	@param ptype
		type of object
*/
EksParent *eks_parent_get_child_from_type(EksParent *thisParent,int pos, EksParentType ptype)
{
	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return NULL;
	}
		
	EksParent *loopUnit=firstUnit;

	do
	{
		if(eks_parent_compare_type(loopUnit,ptype))
		{
			if(pos<=0)
			{
				return loopUnit;
			}
			
			pos--;
		}
		
		loopUnit=loopUnit->nextChild;
	}while(loopUnit!=firstUnit);
	
	eks_error_message("Your position is probably wrong! Pos[%d] in-EksParent name[%s] Type[%d]",pos,thisParent->name,ptype);
	
	return NULL;
}

/**
	almost the same as eks_parent_get_name, but here you will get from a type such as a comment or value
	
	@param thisParent
		the current-level-parent
	@param pos
		The position in the parent object
	@param ptype
		The type of the subparent
	@return
		the char* to the name of that object
*/
char *eks_parent_get_from_type(EksParent *thisParent,int pos, EksParentType ptype)
{
	EksParent *tempParent=eks_parent_get_child_from_type(thisParent,pos,ptype);
	
	if(tempParent)
		return eks_parent_get_string(tempParent);
		
	eks_error_message("something went wrong!");
	return NULL;
}

/**
	Add a child to an existing eksparent
	
	@param thisParent
		the parent to add some children into
	@param name
		the name of the new child
	@param ptype
		the type of the new child
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child(EksParent *thisParent,char *name, EksParentType ptype, EksParent *extras)
{
	EksParent *newParent;

	if(thisParent)
	{
		EksParent *firstParent=thisParent->firstChild;
	
		if(!firstParent)
		{
			newParent=eks_parent_new(name,ptype,thisParent,extras);
			thisParent->firstChild=newParent;
			return newParent;
		}
		else
		{
			newParent=malloc(sizeof(EksParent));
		
			newParent->upperEksParent=thisParent;
		
			newParent->firstExtras=extras;
		
			eks_parent_set_base(newParent,ptype);
			eks_parent_set_string(newParent,name);

			newParent->prevChild=firstParent->prevChild;
			newParent->nextChild=firstParent;
			firstParent->prevChild->nextChild=newParent;
			firstParent->prevChild=newParent;
		
			return newParent;
		}
	}
	else
	{
		eks_error_message("You need to initiate the parent first!");
		return NULL;
	}
}

/**
	Internal function that correctly destroys everything below the current parent
	
	@param thisParent
		the temp parent to destroy
*/
static void eks_parent_destroy_below(EksParent *thisParent)
{
	EksParent *firstUnit=thisParent->firstChild;
	
	if(thisParent->structure>=0 && firstUnit)
	{
		EksParent *loopUnit=firstUnit;
		EksParent *sloopUnit;

		do
		{
			sloopUnit=loopUnit->nextChild;
		
			eks_parent_destroy_below(loopUnit);
		
			loopUnit=sloopUnit;
		}while(loopUnit!=firstUnit);
	}

	//free the core
	eks_parent_destroy(thisParent->firstExtras,EKS_TRUE);
	
	if(thisParent->type==EKS_PARENT_VALUE_STRING)
		free(thisParent->name);
	free(thisParent->custom);
	free(thisParent);
}

/**
	This is the main free function for the parent structure.
	
	@param thisParent FREE
		The EksParent structure to free
	@param recursive .
		If you want to free all the children to the EksParent structure.
	@return
		void
*/
void eks_parent_destroy(EksParent *thisParent,EksBool recursive)
{
	if(thisParent)
	{
		//link the next child and prev child correctly
		thisParent->nextChild->prevChild=thisParent->prevChild;
		thisParent->prevChild->nextChild=thisParent->nextChild;
		
		if(thisParent->upperEksParent->firstChild==thisParent)
		{
			if(thisParent->nextChild!=thisParent && thisParent->prevChild!=thisParent)
				thisParent->upperEksParent->firstChild=thisParent->nextChild;
			else
				thisParent->upperEksParent->firstChild=NULL;
		}
		
		//if you want to destroy everything below
		if(recursive)
			eks_parent_destroy_below(thisParent);
	}
}

/**
	Add custom information for the parent.
	This can be whatever.

	@param thisParent
		the parent to insert the content into
	@param
		content, the content to insert.
*/
void eks_parent_custom_set(EksParent *thisParent,void *content,int pos)
{
	void **vector=thisParent->custom;

	size_t len=0;
	
	if(vector)
		while(*vector)
		{
			len++;	
			vector++;
		}
	
	vector=thisParent->custom;
	
	if(pos<0)
	{
		len++;
		vector=realloc(vector,sizeof(void*)*(len+1));
		
		vector[len-1]=content;
		vector[len]=NULL;
	}
	else
	{
		if(len<(pos+1))
		{
			vector=realloc(vector,sizeof(void*)*(pos+1+1));
			vector[pos]=content;
			vector[pos+1]=NULL;
		}
		else
			vector[pos]=content;
	}
	
	thisParent->custom=vector;
}

/**
	Get the content from a custom position
	
	@param thisParent
		the parent to read from
	@param pos
		the position in the vector
*/
void *eks_parent_custom_get(EksParent *thisParent,int pos)
{
	void **vector=thisParent->custom;

	int len=0;
	
	while(*vector)
	{
		len++;	
		vector++;
	}
	
	if(pos>len)
	{
		eks_error_message("Your position is too high! len: %d, pos: %d",len,pos);
		return NULL;
	}
	
	return ((void**)(thisParent->custom))[pos];
}

/**
	Will dump the contents of a parent structure.
	
	@param thisParent NO_FREE
		The parent to dump the information from
	@return NEW
		The output text
*/
char *eks_parent_dump_text(EksParent *thisParent)
{
	//FIX FIX FIX (works now but not as i want)
	char *returnString=calloc(1,sizeof(char));
	char *StructureString=NULL;
	
	char *tabString=NULL;
	
	char *thisname=eks_parent_get_string(thisParent);
	
	if(thisParent->structure>=0)
	{
		if(thisParent->firstChild==NULL)
		{
			tabString=malloc(sizeof(char)*(thisParent->upperEksParent->structure+1));
			memset(tabString,'\t',thisParent->upperEksParent->structure);
			tabString[thisParent->upperEksParent->structure]='\0';
		
			void *temp=returnString;
			returnString=g_strconcat(returnString,tabString,thisname,"\n",NULL);
			free(temp);
		}
		else
		{
			//add the hashtag signs, for the structure level.
			if(thisParent->structure>0)
			{
				//if the word does not contain \n or ' '
				//if(thisname)
				//{
					if((StructureString=malloc(sizeof(char)*(thisParent->structure+1)))==NULL)
					{
						eks_error_message("Failed to allocate space for the returning string!");
						return NULL;
					}
		
					memset(StructureString,'#',thisParent->structure);
					StructureString[thisParent->structure]='\0';
			
					if((tabString=malloc(sizeof(char)*(thisParent->structure)))==NULL)
					{
						eks_error_message("Failed to allocate space for the tabs!");
						return NULL;
					}
					memset(tabString,'\t',thisParent->structure-1);
					tabString[thisParent->structure-1]='\0';
			
					void *temp=returnString;
					if(thisname)
						returnString=g_strconcat(returnString,tabString,StructureString,thisname,"\n",NULL);
					else
						returnString=g_strconcat(returnString,tabString,StructureString,"\n",NULL);
					free(temp);
				//else
				//{
				//	returnString=g_strconcat(returnString,"#",thisname,"{\n",NULL);
				//}
			}
			else
			{
				//if it is a comment (should be improved, this will also include the variables!)
				void *temp=returnString;
				returnString=g_strconcat(returnString,"//",thisname,"\n",NULL);
				free(temp);
			}
		
			//do it for all the sub-children
			EksParent *firstUnit=thisParent->firstChild;
	
			if(!firstUnit)
			{
				goto skip_unit;
			}
		
			EksParent *loopUnit=firstUnit;

			do
			{
				char *temp=eks_parent_dump_text(loopUnit);
				void *temp2=returnString;
				returnString=g_strconcat(returnString,temp,NULL);
				free(temp2);
				free(temp);
			
				loopUnit=loopUnit->nextChild;
			}while(loopUnit!=firstUnit);
		
			skip_unit: ;
		}
	}
	else if(thisParent->structure==-1)
	{
		tabString=malloc(sizeof(char)*(thisParent->upperEksParent->structure+1));
		memset(tabString,'\t',thisParent->upperEksParent->structure);
		tabString[thisParent->upperEksParent->structure]='\0';
		
		void *temp=returnString;
		returnString=g_strconcat(returnString,tabString,"/*",thisname,"*/\n",NULL);
		free(temp);
	}
	
	free(StructureString);
	free(tabString);
	free(thisname);
	
	return returnString;
}

#ifndef CC_COMPILING_EKS
	#include "./eksparser.c"
#endif
