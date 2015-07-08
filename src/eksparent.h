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

	The main header for the eksparent, or the eks language.
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>

#include <glib.h>

/** Display a critical message with nice looking colors */
#define eks_critical_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;37m\e[41mCRITICAL\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display an error message with nice looking colors */
#define eks_error_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;31mERROR\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display a warning message with nice looking colors */
#define eks_warning_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;33mWARNING\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display a debug message with nice looking colors */
#define eks_debug_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;36mDEBUG\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Used with rt */
/** if its a \# value. */
#define EKS_PARENT_IS_TAGVALUE 0
/** if its not a \# value. */
#define EKS_PARENT_IS_UNTAGGED 1

/** Used with span */
/** if its not a multiline comment, or just inside {} */
#define EKS_PARENT_IS_NOSPAN 0
/** if its a multiline comment, or just inside {} */
#define EKS_PARENT_IS_SPAN 1

/**
	different types of subobjects
*/
typedef enum EksParentType
{
	EKS_PARENT_TYPE_VALUE,
	EKS_PARENT_TYPE_VARIABLE,
	EKS_PARENT_TYPE_COMMENT
}EksParentType;

/**
	a boolean, maybe not so necessary
*/
typedef enum EksBool
{
	EKS_FALSE=0,
	EKS_TRUE=1
}EksBool;

/**
	Type, this can be or:ed
*/
typedef enum EksParentValue
{
	EKS_PARENT_VALUE_STRING,
	EKS_PARENT_VALUE_INT,
	EKS_PARENT_VALUE_DOUBLE
}EksParentValue;

/**
	the most important object/structure in this library.
	Handles kinda everything
*/
typedef struct EksParent
{
	union
	{
		char *name; ///< The name of the parent.
		intptr_t iname; ///< The int version
		double dname; ///< The double version
	};
	unsigned int type : 4; ///< the type used above
	unsigned int rt : 1; ///< rt= if 'value' then its says if its # or not.
	unsigned int span : 1; ///< span= if 'value' then its says if its {} or not, if 'comment' then it says if its // or /**/
	signed int structure : 16; ///< The significant structure describing what the object 'is', it can be one of these: 1++++ = parent,-1=text,-1=value,-2=comment,0=topmost parent
	unsigned int : 0; //< fill it up
	
	void *custom; ///< for custom usage for an application, This is a vector with NULL termination
	
	struct EksParent *firstExtras; ///< in development, not used at the moment
	
	struct EksParent *upperEksParent; ///< The eks-parent above this object, NULL if nothing.

	struct EksParent *firstChild; ///< The first child below this object.
	
	struct EksParent *nextChild; ///< The next object in this linked list.
	struct EksParent *prevChild; ///< The previous object in this linked list.
	
}EksParent;

/**
	States for the eksparentparser state mashine
*/
typedef enum EksParseStates
{
	EKS_PARENT_STATE_NOTHING,
	EKS_PARENT_STATE_AND_COUNT,
	EKS_PARENT_STATE_AND_NAME,
	EKS_PARENT_STATE_VAR_COUNT,
	EKS_PARENT_STATE_VARIABLE,
	EKS_PARENT_STATE_AND_AFTER
}EksParseStates;

/**
	States for the eksparentparser state mashine only for comments
*/
typedef enum EksParseStateComment
{
	EKS_PARENT_STATE_COMMENT_NONE,
	EKS_PARENT_STATE_COMMENT,
	EKS_PARENT_STATE_COMMENT_SINGLE_LINE,
	EKS_PARENT_STATE_COMMENT_MULTILINE,
	EKS_PARENT_STATE_COMMENT_MULTILINE_END_CHECK,
	EKS_PARENT_STATE_COMMENT_MULTILINE_NESTLE_CHECK
}EksParseStateComment;

/**
	The main structure for the parsing.
*/
typedef struct EksParseType
{
	//for current states
	EksParseStates state; ///< Current state of the statemashine
	EksParseStateComment stateComment; ///< If it is a comment, we are currently looking into 
	uint8_t stateVector; ///< Saves if it is in vector mode or not, eg \#obj ← vector mode, or \#obj{\#obj{}} ← not vector mode
	uint8_t stateDelegate; ///< Saves if its in colon mode eg, \#vect:aoe\n etc
	
	//the return parent
	EksParent *parent; ///< The parent structure we will return
	EksParent *currentParent; ///< The current parent in the parsing. It will jump up and down, so its the most frequently used in the parsing process

	//for handeling the current amount of #s
	size_t currentParentLevel; ///< Saving the current amount of \# (levels)
	size_t prevParentLevel; ///< Saving the previous amount of \# (levels, used when changing from \#\#\# to \#) 

	//for handeling the input of the normal input string
	char *word; ///< The word, or the string. It will collect in here if it is in a string, like \#something or just a plain word
	size_t wordSize; ///< The currently allocated word size
	size_t currentWordSize; ///< How big the current word is at this moment

	//for handeling the input of the comment string
	char *commentWord; ///< The main string for saving chars in a comment.
	size_t commentWordSize; ///< The allocated size for the comment string
	size_t currentCommentWordSize; ///< The current size for the comment-word.
	
	int commentNestleSize; ///< for calculating where in the nestling process we are

	//special string cases
	uint8_t stateBackslash; ///< If it noticed a backslash
	uint8_t stateIsString; ///< If it noticed a \" (quotation mark)

	//for the level ("#value{"s) of the parsing
	int *ladder; ///< If the levels are freaked out we kinda need to wierdly save the levels.
	size_t ladderAmount; ///< how many elements is allocated for the ladder
	int ladderCurrentAmount; ///< The current amount of elements in the ladder
}EksParseType;

/***********FUNCTIONS START HERE!******/

EksParent *eks_parent_new_string(const char *name, EksParentType ptype, EksParent *topParent, EksParent *extras);
EksParent *eks_parent_new_int(intptr_t iname, EksParentType ptype, EksParent *topParent, EksParent *extras);
EksParent *eks_parent_new_double(double dname, EksParentType ptype, EksParent *topParent, EksParent *extras);

/**
	Create a new parent with a name
	This is a generic functon which will just change over depending on the input you give.
	
	@param name
		the name you want to set it to, it can be an int or double or char*
	@param ptype
		the type to use eg value or comment
	@param topParent
		the parent to have as its upper parent
	@param extras
		A parent containing extras information
*/
#define eks_parent_new(name, ptype, topParent, extras) _Generic((name),	double: eks_parent_new_double, \
																					float: eks_parent_new_double, \
																					short: eks_parent_new_int, \
																					int: eks_parent_new_int, \
																					intptr_t: eks_parent_new_int, \
																					default: eks_parent_new_string)(name, ptype, topParent, extras)

char *eks_parent_get_string(EksParent *thisParent);
intptr_t eks_parent_get_int(EksParent *thisParent);
double eks_parent_get_double(EksParent *thisParent);

void eks_parent_foreach_child(EksParent *theParent,void *func,void *inparam);

size_t eks_parent_get_child_amount(EksParent *thisParent);

int eks_parent_set_string(EksParent *thisParent, const char *name);
int eks_parent_set_int(EksParent *thisParent, intptr_t name);
int eks_parent_set_double(EksParent *thisParent, double name);

/**
	Set the name of a parent
	This is a generic functon which will just change over depending on the input you give.
	
	@param parent
		the parent you want to set
	@param name
		the name you want to set it to, it can be an int or double or char*
*/
#define eks_parent_set(parent, name) _Generic((name),	double: eks_parent_set_double, \
																					float: eks_parent_set_double, \
																					short: eks_parent_set_int, \
																					int: eks_parent_set_int, \
																					intptr_t: eks_parent_set_int, \
																					default: eks_parent_set_string)(parent, name)

EksParent *eks_parent_add_child_string(EksParent *thisParent,const char *name, EksParentType ptype, EksParent *extras);
EksParent *eks_parent_add_child_int(EksParent *thisParent,intptr_t iname, EksParentType ptype, EksParent *extras);
EksParent *eks_parent_add_child_double(EksParent *thisParent,double dname, EksParentType ptype, EksParent *extras);

/**
	Add a child to a parent with a name
	This is a generic functon which will just change over depending on the input you give.
	
	@param parent
		the parent you want to set
	@param name
		the name you want to set it to, it can be an int or double or char*
	@param type
		the type to use eg value or comment
	@param extras
		A parent containing extras information
*/
#define eks_parent_add_child(parent, name, type, extras) _Generic((name),	double: eks_parent_add_child_double, \
																					float: eks_parent_add_child_double, \
																					short: eks_parent_add_child_int, \
																					int: eks_parent_add_child_int, \
																					intptr_t: eks_parent_add_child_int, \
																					default: eks_parent_add_child_string)(parent, name, type, extras)

void eks_parent_fix_structure(EksParent *thisParent);

int eks_parent_from_child_insert_prev(EksParent *child,EksParent *inEksParent);

int eks_parent_insert(EksParent *thisParent,EksParent *inEksParent);

EksParent *eks_parent_clone(EksParent *thisParent);

EksParent* eks_parent_get_child(EksParent *thisParent,int pos);

EksParent* eks_parent_get_first_child(EksParent *thisParent);

EksParent* eks_parent_get_last_child(EksParent *thisParent);

EksParent *eks_parent_climb_parent(EksParent *thisParent,int amount);

EksParent* eks_parent_get_child_from_name(EksParent *thisParent,const char *tempName);

int eks_parent_compare_type(EksParent *thisParent, EksParentType ptype);

int eks_parent_get_amount_from_type(EksParent *thisParent, EksParentType ptype);

EksParent *eks_parent_get_child_from_type(EksParent *thisParent,int pos, EksParentType ptype);

void eks_parent_destroy(EksParent *thisParent,EksBool recursive);

void eks_parent_custom_set(EksParent *thisParent,void *content,int pos);
void *eks_parent_custom_get(EksParent *thisParent,int pos);

char *eks_parent_dump_text(EksParent *thisParent);

/*****FOR PARSING*****/

void eks_parent_parse_char(EksParseType *parser, char c);

EksParent *eks_parent_parse_text(unsigned char *buffer,const char *name);

EksParent *eks_parent_parse_file(char *filename);
