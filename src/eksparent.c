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

EksParent *eks_parent_new(const char *name, EksParentType ptype, EksParent *topParent, EksParent *extras)
{
	EksParent *thisParent=malloc(sizeof(EksParent));
	if(thisParent==NULL)
	{
		eks_error_message("Failed to allocate space for the child!");
		return NULL;
	}
	
	thisParent->firstExtras=extras;
	thisParent->firstChild=NULL;
	thisParent->custom=NULL;
	
	if(topParent)
		thisParent->upperEksParent=topParent;
	else
		thisParent->upperEksParent=thisParent;
	
	eks_parent_set_string(thisParent,name,ptype);
	
	thisParent->nextChild=thisParent;
	thisParent->prevChild=thisParent;
	
	return thisParent;
}

/**
	Get the string (char*) from a parent object.
	
	@param tempEksParent
		the parent to get its child from
	@return
		returns the 'string' or NULL if failed
*/
char *eks_parent_get_string(EksParent *tempEksParent)
{
	if(tempEksParent)
	{
		if(tempEksParent->type==EKS_PARENT_VALUE_STRING)
		{
			return g_strdup(tempEksParent->name);
		}
		else if(tempEksParent->type==EKS_PARENT_VALUE_INT)
		{
			return eks_int_to_string(tempEksParent->iname);
		}
		else if(tempEksParent->type==EKS_PARENT_VALUE_DOUBLE)
		{
			return g_strdup_printf("%f", tempEksParent->dname);
		}
		
		eks_error_message("Unknown type/value!");
	}
	
	eks_error_message("The parent was NULL!");
	return NULL;
}

/**
	Get the int from a parent object.
	
	@param tempEksParent
		the parent to get its child from
	@return
		returns the int
*/
intptr_t eks_parent_get_int(EksParent *tempEksParent)
{
	if(tempEksParent->type!=EKS_PARENT_VALUE_INT)
	{
		eks_warning_message("The type is not an int!");
	}
	
	if(tempEksParent)
		return tempEksParent->iname;
	else
	{
		eks_error_message("The parent was NULL!");
		return NULL;
	}
}

/**
	Get the double from a parent object.
	
	@param tempEksParent
		the parent to get its child from
	@return
		returns the double
*/
double eks_parent_get_double(EksParent *tempEksParent)
{
	if(tempEksParent->type!=EKS_PARENT_VALUE_DOUBLE)
	{
		eks_warning_message("The type is not a double!");
	}
	
	if(tempEksParent)
		return tempEksParent->dname;
	else
	{
		eks_error_message("The parent was NULL!");
		return 0.0;
	}
}

/**
	Do something foreach child
	
	@param theParent
		The parent to do the foreach on
	@param func
		The function to run
	@param inparam
		The input parameter
*/
void eks_parent_foreach_child(EksParent *theParent,void *func,void *inparam)
{
	void (*typefunction)(EksParent*,void*);
	
	typefunction=func;

	EksParent *firstUnit=theParent->firstChild;
	
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
	
	@param tempEksParent
		the parent to count children from
	@return
		the amount of children
*/
size_t eks_parent_get_child_amount(EksParent *tempEksParent)
{
	size_t amount=0;

	EksParent *firstUnit=tempEksParent->firstChild;
	
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
	Will create the first child for the parent.
	
	@param topParent
		the toplevel parent
	@return
		the child
*/
void eks_parent_new_first_child(EksParent *topParent)
{
	if(topParent)
	{	
		EksParent *thisParent;
		if((thisParent=malloc(sizeof(EksParent)))==NULL)
		{
			eks_error_message("Failed to allocate space for the child!");
			return;
		}
		thisParent->name=NULL;
		thisParent->type=EKS_PARENT_VALUE_STRING;
		thisParent->firstExtras=NULL;
		thisParent->custom=NULL;
		thisParent->structure=0;
		thisParent->firstChild=NULL;
		
		thisParent->upperEksParent=topParent;
		thisParent->nextChild=thisParent;
		thisParent->prevChild=thisParent;
		
		//dont mix up with this one!
		topParent->firstChild=thisParent;
		
		return;
	}
	
	eks_error_message("Could not create the child! %s",topParent->name);
	return;
}

/**
	This function defines the EksParent structure.
	
	@param tempEksParent
		The EksParent Structure to set the values from
	@param name
		the name to the EksParent structure
	@param ptype
		the type of the object
	@return
		will return 1 if it succeded
*/
static void eks_parent_set_base(EksParent *tempEksParent, EksParentType ptype)
{
	if(ptype==EKS_PARENT_TYPE_COMMENT)
	{
		tempEksParent->structure=-1;
		tempEksParent->firstChild=NULL;
	}
	else if((tempEksParent->upperEksParent==NULL || tempEksParent->upperEksParent==tempEksParent) && ptype==EKS_PARENT_TYPE_VALUE)
	{
		tempEksParent->structure=0;
		tempEksParent->upperEksParent=tempEksParent;
	}
	else if(ptype==EKS_PARENT_TYPE_VALUE || ptype==EKS_PARENT_TYPE_TEXT)
	{
		tempEksParent->structure=tempEksParent->upperEksParent->structure+1;
		
		if(ptype==EKS_PARENT_TYPE_TEXT)
			tempEksParent->firstChild=NULL;
	}
}

int eks_parent_set_string(EksParent *tempEksParent, const char *name, EksParentType ptype)
{
	if(tempEksParent)
	{
		int nameLen=name?strlen(name):0;
		if(name!=NULL && nameLen>0)
		{
			int type=eks_string_to_double(name,&tempEksParent->dname,&tempEksParent->iname);
			
			tempEksParent->type=type;
			
			if(type==0)
				tempEksParent->name=g_strndup(name,nameLen);
		}
		else
		{
			tempEksParent->name=NULL;
			
			tempEksParent->type=EKS_PARENT_VALUE_STRING;
		}
		
		eks_parent_set_base(tempEksParent, ptype);
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

int eks_parent_set_int(EksParent *tempEksParent, intptr_t name, EksParentType ptype)
{
	if(tempEksParent)
	{
		tempEksParent->iname=name;
			
		tempEksParent->type=EKS_PARENT_VALUE_INT;
		
		eks_parent_set_base(tempEksParent, ptype);
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

int eks_parent_set_double(EksParent *tempEksParent, double name, EksParentType ptype)
{
	if(tempEksParent)
	{
		tempEksParent->dname=name;
			
		tempEksParent->type=EKS_PARENT_VALUE_DOUBLE;
		
		eks_parent_set_base(tempEksParent, ptype);
		
		return 1;
	}
	
	eks_error_message("Could not set string!");
	return 0;
}

/**
	This function will fix the parents children structure, that means that it will set the structure number to the parents+1
	
	@param parentToFix
		The EksParent to fix
	@return
		void
*/
void eks_parent_fix_structure(EksParent *parentToFix)
{
	if(parentToFix && parentToFix->structure>=0 && parentToFix->firstChild && parentToFix->upperEksParent!=parentToFix)
	{
		parentToFix->structure=parentToFix->upperEksParent->structure+1;
		
		EksParent *firstUnit=parentToFix->firstChild;
		
		EksParent *loopUnit=firstUnit;

		do
		{
			eks_parent_fix_structure(loopUnit);
		
			loopUnit=loopUnit->nextChild;
		}while(loopUnit!=firstUnit);
	}
	else if(parentToFix->structure==0 && parentToFix->upperEksParent==parentToFix)
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
	
	@param thisEksParent
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

	@param thisEksParent NO_FREE
		EksParent to clone
	@return NEW
		a pointer to the cloned parent structure
*/
EksParent *eks_parent_clone(EksParent *thisEksParent)
{
	//define parent
	EksParent *newEksParent;
	
	//assign some memory
	if((newEksParent=calloc(1,sizeof(EksParent))))
	{
		newEksParent->name=g_strndup(thisEksParent->name,strlen(thisEksParent->name));
		newEksParent->structure=thisEksParent->structure;
		
		newEksParent->nextChild=newEksParent;
		newEksParent->prevChild=newEksParent;
		
		if(thisEksParent->structure==0 || thisEksParent->upperEksParent==thisEksParent)
		{
			newEksParent->upperEksParent=newEksParent;
		}
		
		//do it on the bottom ones as well
		if(thisEksParent->structure>=0)
		{
			EksParent *firstUnit=thisEksParent->firstChild;
	
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
	
	@param tempEksParent NO_FREE
		The EksParent Structure to check the child from
	@param pos .
		the position, 0= first child
	@return
		will return 1 if it exists
*/
int eks_parent_check_child(EksParent *tempEksParent,int pos)
{
	return (tempEksParent->firstChild && (tempEksParent->structure>0 || (tempEksParent->structure==0 && tempEksParent->upperEksParent==tempEksParent)));
}

/**
	This function will get the EksParentstructure from a child from a pos.
	
	@param tempEksParent NO_FREE
		The EksParent Structure to get the child from
	@param pos .
		the position, 0= first child
	@return
		this will give the pointer to the EksParent
*/

EksParent* eks_parent_get_child(EksParent *tempEksParent,int pos)
{
	EksParent *firstUnit=tempEksParent->firstChild;
	
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
	
	eks_error_message("Your position is probably wrong! Pos[%d] in-EksParent name[%s]",pos,tempEksParent->name);
	
	return NULL;
}

/**
	Get the first child from the EksParent.

	@param tempEksParent NO_FREE
		the parent to get the child from
	@return .
		returns the parent
*/

EksParent* eks_parent_get_first_child(EksParent *tempEksParent)
{
	if(tempEksParent->firstChild)
	{
		return tempEksParent->firstChild;
	}
	
	eks_error_message("Failed to get child!");// There is [%d] children!",tempEksParent->amount);
	return NULL;
}

/**
	Get the last child from the EksParent.

	@param tempEksParent NO_FREE
		the parent to get the child from
	@return .
		returns the parent
*/

EksParent* eks_parent_get_last_child(EksParent *tempEksParent)
{
	if(tempEksParent->firstChild)
	{
		return tempEksParent->firstChild->prevChild;
	}
	
	eks_error_message("Failed to get child!");// There is [%d] children!",tempEksParent->amount);
	return NULL;
}

/**
	Go up a number of parents

	@param tempEksParent NO_FREE
		the parent to get the parent from
	@param amount
		number of times to climb (positive number = upwards, negative = downwards (will create new children if not exists, goes through last child))
	@return .
		returns the parent
*/

EksParent *eks_parent_climb_parent(EksParent *tempEksParent,int amount)
{
	EksParent *debugparent=tempEksParent;
	
	uint8_t madetwice=0;
	
	if(amount<0)
	{
		amount=-amount;
	
		for(int i=0;i<amount;i++)
		{
			if(tempEksParent->firstChild)
			{
				tempEksParent=tempEksParent->firstChild->prevChild;
			}
			else
			{
				eks_parent_add_children(tempEksParent,1);
				EksParent *newParent=tempEksParent->firstChild->prevChild;
				eks_parent_set_string(newParent,NULL,EKS_PARENT_TYPE_TEXT);
			}
		}
	}
	else
	{
		for(int i=0;i<amount;i++)
		{
			tempEksParent=tempEksParent->upperEksParent;
		
			//if its the toplevel parent
			if(tempEksParent==tempEksParent->upperEksParent)
			{
				if(madetwice==1)
				{
					eks_warning_message("You climbed the parent twice! Amount[%d] Name[%s] Structure[%d]",amount,debugparent->name,debugparent->structure);
				}
			
				madetwice++;
			}
		}
	}
	
	return tempEksParent;
}

/**
	This function will get the EksParentstructure from a child from a name.
	
	@param tempEksParent
		The EksParent Structure to get the child from
	@param tempName
		The childs name
	@return
		this will give the pointer to the EksParent
*/
EksParent* eks_parent_get_child_from_name(EksParent *tempEksParent,const char *tempName)
{
	if(!tempEksParent->firstChild)
	{
		eks_error_message("No first child?");
		return NULL;
	}
	
	if(tempEksParent->structure>=0)
	{
		EksParent *firstUnit=tempEksParent->firstChild;
		EksParent *loopUnit=firstUnit;
	
		do
		{
			if(strcmp(loopUnit->name,tempName)==0)
			{
				return loopUnit;
			}
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

	@param tempEksParent
		the parent
	@param ptype
		the type
	@return
		1 if they are the same 0 if they arent.
*/
int eks_parent_compare_type(EksParent *tempEksParent, EksParentType ptype)
{
	return ((tempEksParent->structure>=0 && (ptype==EKS_PARENT_TYPE_VALUE || (ptype==EKS_PARENT_TYPE_TEXT && tempEksParent->firstChild==NULL))) || (ptype==EKS_PARENT_TYPE_COMMENT && tempEksParent->structure==-1));
}

/**
	Get the amount of children from a specific type

	@param tempEksParent
		in-parent
	@param ptype
		type of object
	@return
		number of objects
*/
int eks_parent_get_amount_from_type(EksParent *tempEksParent, EksParentType ptype)
{
	int retamount=0;
	
	EksParent *firstUnit=tempEksParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child?");
		return -1;
	}
		
	EksParent *loopUnit=firstUnit;

	do
	{
		//EksParent *child=eks_parent_get_child(tempEksParent,i);
		
		if(eks_parent_compare_type(loopUnit,ptype))
			retamount++;
			
		loopUnit=loopUnit->nextChild;
	}while(loopUnit!=firstUnit);
	
	return retamount;
}

/**
	Get a specific child from the pos where the type is from, see eks_parent_get_amount_from_type
	
	@param tempEksParent
		the in-parent
	@param pos
		position from where we want it
	@param ptype
		type of object
*/
EksParent *eks_parent_get_child_from_type(EksParent *tempEksParent,int pos, EksParentType ptype)
{
	EksParent *firstUnit=tempEksParent->firstChild;
	
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
	
	eks_error_message("Your position is probably wrong! Pos[%d] in-EksParent name[%s] Type[%d]",pos,tempEksParent->name,ptype);
	
	return NULL;
}

/**
	almost the same as eks_parent_get_name, but here you will get the object first
	
	@param tempEksParent
		the current-level-parent
	@param pos
		The position in the parent object
	@param ptype
		The type of the subparent
	@return
		the char* to the name of that object
*/
char *eks_parent_get_information_from_type(EksParent *tempEksParent,int pos, EksParentType ptype)
{
	EksParent *tempParent=eks_parent_get_child_from_type(tempEksParent,pos,ptype);
	
	if(tempParent)
		return tempParent->name;
		
	eks_error_message("something went wrong!");
	return NULL;
}

/**
	Add children to a EksParent structure.
	
	@param tempEksParent
		The EksParent structure to add some children to.
	@param num
		The amount of children to add.
	@return
		void
*/
void eks_parent_add_children(EksParent *tempEksParent,int num)
{
	if(tempEksParent)
	{
		if(num>0)
		{
			if(!tempEksParent->firstChild)
			{
				eks_parent_new_first_child(tempEksParent);
				num--;
			}
	
			EksParent *firstParent=tempEksParent->firstChild;
	
			for(int i=0;i<num;i++)
			{
				EksParent *newParent=malloc(sizeof(EksParent));
		
				newParent->name=NULL;
				newParent->type=EKS_PARENT_VALUE_STRING;
				newParent->custom=NULL;
				newParent->firstExtras=NULL;
				newParent->structure=0;
				newParent->firstChild=NULL;
		
				newParent->upperEksParent=tempEksParent;
		
				newParent->prevChild=firstParent->prevChild;
				newParent->nextChild=firstParent;
				firstParent->prevChild->nextChild=newParent;
				firstParent->prevChild=newParent;
			}
	
		}
		else
		{
			eks_error_message("invalid number of new children?\n");
		}
	}
	else
	{
		eks_error_message("You need to initiate the parent first!");
	}
}

/**
	Add a child from a type

	@param tempParent
		the parent to add some children into
	@param name
		the name of the new child
	@param ptype
		the type of the new child
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child_from_type(EksParent *tempParent,char *name, EksParentType ptype)
{
	eks_parent_add_children(tempParent,1);
	EksParent *newParent=eks_parent_get_last_child(tempParent);
	eks_parent_set_string(newParent,name,ptype);
	return newParent;
}

/**
	Internal function that correctly destroys everything below the current parent
	
	@param tempEksParent
		the temp parent to destroy
*/
static void eks_parent_destroy_below(EksParent *tempEksParent)
{
	EksParent *firstUnit=tempEksParent->firstChild;
	
	if(tempEksParent->structure>=0 && firstUnit)
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
	eks_parent_destroy(tempEksParent->firstExtras,EKS_TRUE);
	
	if(tempEksParent->type==EKS_PARENT_VALUE_STRING)
		free(tempEksParent->name);
	free(tempEksParent->custom);
	free(tempEksParent);
}

/**
	This is the main free function for the parent structure.
	
	@param tempEksParent FREE
		The EksParent structure to free
	@param recursive .
		If you want to free all the children to the EksParent structure.
	@return
		void
*/
void eks_parent_destroy(EksParent *tempEksParent,EksBool recursive)
{
	if(tempEksParent)
	{
		//link the next child and prev child correctly
		tempEksParent->nextChild->prevChild=tempEksParent->prevChild;
		tempEksParent->prevChild->nextChild=tempEksParent->nextChild;
		
		if(tempEksParent->upperEksParent->firstChild==tempEksParent)
		{
			if(tempEksParent->nextChild!=tempEksParent && tempEksParent->prevChild!=tempEksParent)
				tempEksParent->upperEksParent->firstChild=tempEksParent->nextChild;
			else
				tempEksParent->upperEksParent->firstChild=NULL;
		}
		
		//if you want to destroy everything below
		if(recursive)
			eks_parent_destroy_below(tempEksParent);
	}
}

/**
	Add custom information for the parent.
	This can be whatever.

	@param tempEksParent
		the parent to insert the content into
	@param
		content, the content to insert.
*/
void eks_parent_custom_set(EksParent *tempEksParent,void *content,int pos)
{
	void **vector=tempEksParent->custom;

	size_t len=0;
	
	if(vector)
		while(*vector)
		{
			len++;	
			vector++;
		}
	
	vector=tempEksParent->custom;
	
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
	
	tempEksParent->custom=vector;
}

/**
	Get the content from a custom position
	
	@param tempEksParent
		the parent to read from
	@param pos
		the position in the vector
*/
void *eks_parent_custom_get(EksParent *tempEksParent,int pos)
{
	void **vector=tempEksParent->custom;

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
	
	return ((void**)(tempEksParent->custom))[pos];
}

/**
	Will dump the contents of a parent structure.
	
	@param topLevelEksParent NO_FREE
		The parent to dump the information from
	@return NEW
		The output text
*/
char *eks_parent_dump_text(EksParent *topLevelEksParent)
{
	//FIX FIX FIX (works now but not as i want)
	char *returnString=calloc(1,sizeof(char));
	char *StructureString=NULL;
	
	char *tabString=NULL;
	
	char *thisname=eks_parent_get_string(topLevelEksParent);
	
	if(topLevelEksParent->structure>=0)
	{
		if(topLevelEksParent->firstChild==NULL)
		{
			tabString=malloc(sizeof(char)*(topLevelEksParent->upperEksParent->structure+1));
			memset(tabString,'\t',topLevelEksParent->upperEksParent->structure);
			tabString[topLevelEksParent->upperEksParent->structure]='\0';
		
			void *temp=returnString;
			returnString=g_strconcat(returnString,tabString,thisname,"\n",NULL);
			free(temp);
		}
		else
		{
			//add the hashtag signs, for the structure level.
			if(topLevelEksParent->structure>0)
			{
				//if the word does not contain \n or ' '
				//if(thisname)
				//{
					if((StructureString=malloc(sizeof(char)*(topLevelEksParent->structure+1)))==NULL)
					{
						eks_error_message("Failed to allocate space for the returning string!");
						return NULL;
					}
		
					memset(StructureString,'#',topLevelEksParent->structure);
					StructureString[topLevelEksParent->structure]='\0';
			
					if((tabString=malloc(sizeof(char)*(topLevelEksParent->structure)))==NULL)
					{
						eks_error_message("Failed to allocate space for the tabs!");
						return NULL;
					}
					memset(tabString,'\t',topLevelEksParent->structure-1);
					tabString[topLevelEksParent->structure-1]='\0';
			
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
			EksParent *firstUnit=topLevelEksParent->firstChild;
	
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
	else if(topLevelEksParent->structure==-1)
	{
		tabString=malloc(sizeof(char)*(topLevelEksParent->upperEksParent->structure+1));
		memset(tabString,'\t',topLevelEksParent->upperEksParent->structure);
		tabString[topLevelEksParent->upperEksParent->structure]='\0';
		
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
