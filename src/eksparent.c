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
	
	thisParent->rt=0;
	thisParent->span=0;
	thisParent->custom=NULL;
	thisParent->firstChild=NULL;
}

/**
	create a new parent, function
	
	@param ptype
		the type to use
	@param topParent
		the toplevel parent to use
	@param extras
		the extras to use
	@return
		returns the new eksparent
*/
EksParent *eks_parent_new_base(EksParentType ptype, EksParent *topParent, EksParent *extras)
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
	//eks_parent_set_string(thisParent,name);
	
	thisParent->nextChild=thisParent;
	thisParent->prevChild=thisParent;
	
	return thisParent;
}

/**
	create a new parent with a string as its name
	
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
EksParent *eks_parent_new_string(const char *name, EksParentType ptype, EksParent *topParent, EksParent *extras)
{
	EksParent *retParent=eks_parent_new_base(ptype, topParent, extras);
	eks_parent_set_string(retParent,name);
	return retParent;
}

/**
	create a new parent with an int as its name
	
	@param iname
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
EksParent *eks_parent_new_int(intptr_t iname, EksParentType ptype, EksParent *topParent, EksParent *extras)
{
	EksParent *retParent=eks_parent_new_base(ptype, topParent, extras);
	eks_parent_set_int(retParent,iname);
	return retParent;
}

/**
	create a new parent with a double as its name
	
	@param dname
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
EksParent *eks_parent_new_double(double dname, EksParentType ptype, EksParent *topParent, EksParent *extras)
{
	EksParent *retParent=eks_parent_new_base(ptype, topParent, extras);
	eks_parent_set_double(retParent,dname);
	return retParent;
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
			/*
			char t_dtostr[G_ASCII_DTOSTR_BUF_SIZE];
		
			g_ascii_dtostr(t_dtostr,G_ASCII_DTOSTR_BUF_SIZE,thisParent->dname);
			
			size_t dtostrlen=strlen(t_dtostr);
			
			char *outstr=malloc(sizeof(char)*(dtostrlen+1));
			
			if(outstr)
			{
				memcpy(outstr,t_dtostr,dtostrlen+1);
				
				return outstr;
			}
			eks_error_message("Could not allocate memory!");
			return NULL;
			*/
		
			return g_strdup_printf("%g", thisParent->dname);
		}
		
		eks_error_message("Unknown type/value!");
		return NULL;
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

	eks_parent_foreach_child_start(thisParent,loopUnit)
	{	
		typefunction(loopUnit,inparam);

	}eks_parent_foreach_child_end(thisParent,loopUnit);
	
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
		eks_error_message("No first child? Parent %p",thisParent);
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
	
	eks_error_message("Could not set string! Parent %p, Name %s",thisParent,name);
	return 0;
}

/**
	Set the name as a int.
	
	@param thisParent
		the input parent
	@param name
		the input integer
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
	
	eks_error_message("Could not set int! Parent %p, Name %ld",thisParent,(long)name);
	return 0;
}

/**
	Set the name as a double.
	
	@param thisParent
		the input parent
	@param name
		the input double
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
	
	eks_error_message("Could not set double! Parent %p, Name %f",thisParent,name);
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
	
	@param topParent
		The EksParent as reference
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
	if(!thisParent)
	{
		eks_error_message("Got NULL as input!");
		return NULL;
	}

	EksParent *firstUnit=thisParent->firstChild;
	
	if(!firstUnit)
	{
		eks_error_message("No first child? Parent %p Pos %d",thisParent,pos);
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
	
	eks_error_message("Your position is probably wrong! Parent %p Pos %d",thisParent,pos);
	
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
	
	eks_error_message("Parent is NULL!");// There is [%d] children!",thisParent->amount);
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
	
	eks_error_message("Parent or the first child is NULL!");// There is [%d] children!",thisParent->amount);
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
				EksParent *firstUnit=thisParent->firstChild->prevChild;
				EksParent *loopUnit=firstUnit;

				do
				{
					if(loopUnit->structure>=0)
					{
						thisParent=loopUnit;
						goto next_add;
					}
		
					loopUnit=loopUnit->prevChild;
				}while(loopUnit!=firstUnit);
			}
			
			thisParent=eks_parent_add_child(thisParent,NULL,EKS_PARENT_TYPE_VALUE,NULL);
			
			next_add: ;
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
	if(!thisParent)
	{
		eks_error_message("Got NULL as input!");
		return NULL;
	}

	if(!thisParent->firstChild)
	{
		eks_error_message("No first child? Parent %p, Name-to-find %s",thisParent,tempName);
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
	Add a child to an existing eksparent, static function
	
	@param thisParent
		the parent to add some children into
	@param ptype
		the type of the new child
	@param extras
		extras to add
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child_base(EksParent *thisParent, EksParentType ptype, EksParent *extras)
{
	EksParent *newParent;

	if(thisParent)
	{
		EksParent *firstParent=thisParent->firstChild;
	
		if(!firstParent)
		{
			newParent=eks_parent_new_base(ptype,thisParent,extras);
			thisParent->firstChild=newParent;
			return newParent;
		}
		else
		{
			newParent=malloc(sizeof(EksParent));
		
			newParent->upperEksParent=thisParent;
		
			newParent->firstExtras=extras;
		
			eks_parent_set_base(newParent,ptype);

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
	Add a child to an existing eksparent, using a string as its name
	
	@param thisParent
		the parent to add some children into
	@param name
		the name of the new child
	@param ptype
		the type of the new child
	@param extras
		extras to add
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child_string(EksParent *thisParent, const char *name, EksParentType ptype, EksParent *extras)
{
	EksParent *retParent=eks_parent_add_child_base(thisParent,ptype,extras);
	eks_parent_set_string(retParent,name);
	return retParent;
}

/**
	Add a child to an existing eksparent, using an int as its name
	
	@param thisParent
		the parent to add some children into
	@param iname
		the name of the new child
	@param ptype
		the type of the new child
	@param extras
		extras to add
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child_int(EksParent *thisParent, intptr_t iname, EksParentType ptype, EksParent *extras)
{
	EksParent *retParent=eks_parent_add_child_base(thisParent,ptype,extras);
	eks_parent_set_int(retParent,iname);
	return retParent;
}

/**
	Add a child to an existing eksparent, using a double as its name
	
	@param thisParent
		the parent to add some children into
	@param dname
		the name of the new child
	@param ptype
		the type of the new child
	@param extras
		extras to add
	@return
		returns the newly created parent or NULL if fail
*/
EksParent *eks_parent_add_child_double(EksParent *thisParent, double dname, EksParentType ptype, EksParent *extras)
{
	EksParent *retParent=eks_parent_add_child_base(thisParent,ptype,extras);
	eks_parent_set_double(retParent,dname);
	return retParent;
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
		the parent to set the content into
	@param content
		content, the content to insert.
	@param pos
		the position (like a vector) where you want to set your value
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
		vector=realloc(vector,sizeof(void*)*(len+2));
		
		vector[len]=content;
		vector[len+1]=NULL;
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
	
	if((pos+1)>len)
	{
		eks_error_message("That custom value is not initialized! len: %d, pos: %d",len,pos);
		return NULL;
	}
	
	return ((void**)(thisParent->custom))[pos];
}

#ifndef CC_COMPILING_EKS
	#include "./eksparser.c"
#endif
