/**
	@file
	@author Florian Evaldsson
	
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
#ifdef EKS_SHOW_DEBUG
#define eks_debug_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;36mDEBUG\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)
#else
#define eks_debug_message(msg, ...) 
#endif

/** Used with rt */
/** if its a \# value. */
#define EKS_PARENT_IS_TAGVALUE 1
/** if its not a \# value. */
#define EKS_PARENT_IS_UNTAGGED 0

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
	unsigned int camount : 8; ///< Amount of customs (use this?)
	unsigned int : 0; ///< fill it up
	
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
	This type is used in the parsing. It contains information regarding the char-buffers.
*/
typedef struct EksParseString
{
	char *word; ///< The word, or the string. It will collect in here if it is in a string, like \#something or just a plain word
	size_t wordSize; ///< The currently allocated word size
	size_t currentWordSize; ///< How big the current word is at this moment
}EksParseString;

/**
	The main structure for the parsing.
*/
typedef struct EksParseType
{
	//for current states
	EksParseStates state; ///< Current state of the statemashine
	EksParseStateComment stateComment; ///< If it is a comment, we are currently looking into 
	unsigned int currentLineAmount; ///< current line number read
	
	//the return parent
	EksParent *parent; ///< The parent structure we will return
	EksParent *currentParent; ///< The current parent in the parsing. It will jump up and down, so its the most frequently used in the parsing process

	//for handeling the current amount of #s
	size_t currentParentLevel; ///< Saving the current amount of \# (levels)
	size_t prevParentLevel; ///< Saving the previous amount of \# (levels, used when changing from \#\#\# to \#) 
	
	EksParseString *commonWord; ///< String used for common stuff such as text
	EksParseString *commentWord; ///< String used for comments
	
	int commentNestleSize; ///< for calculating where in the nestling process we are

	//special string cases
	unsigned char stateBackslash:1; ///< If it noticed a backslash
	unsigned char stateIsString:1; ///< If it noticed a \" (quotation mark)
	unsigned char stateIsDollarvalue:1; ///< If it is # or $
	unsigned char stateIsInternalValueParent:2; ///< If it is [\#par:value]
	unsigned char stateVariableclimb:2; ///< different temp states for variable

	char *storedWord; ///< used with the internal parser.

	union
	{
		struct EksParseType *internalValueParser; ///< the internal parser used for [].
		EksParent *internalValueParserResult; /// where the result is stored (temporarily)
	};
	
	struct EksParseType *upperInternalValueParser; ///< the the upper parser for the '[]'-handeling. If its null, then its the absolute top.

	//for the level ("#value{"s) of the parsing
	int *ladder; ///< If the levels are freaked out we kinda need to wierdly save the levels.
	size_t ladderAmount; ///< how many elements is allocated for the ladder
	int ladderCurrentAmount; ///< The current amount of elements in the ladder
}EksParseType;

/*********** FUNCTIONS START HERE! ***********/

EksParent *eks_parent_new_base(EksParentType ptype, EksParent *topParent, EksParent *extras);
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

/**
	The main foreach loop function in eksparent. (remember to close with: eks_parent_foreach_child_end)
	Also the parameters should be the same for eks_parent_foreach_child_start and eks_parent_foreach_child_end
	
	@param thisParent 
		this parent object is the parameter for the parent you want to run the foreach function for.
	@param loopUnit
		This is the parameter you should use as the return value for each loop.
*/
#define eks_parent_foreach_child_start(thisParent,loopUnit) do{ \
	if(!thisParent){ \
		eks_error_message("thisParent (from eks_parent_foreach_child_start) is NULL!"); \
		break; \
	} \
	EksParent *loopUnit ## _eks_firstUnit=thisParent->firstChild; \
	if(!loopUnit ## _eks_firstUnit){ \
		eks_error_message("No first child? Parent %p",thisParent); \
		break; \
	} \
	EksParent *loopUnit=loopUnit ## _eks_firstUnit; \
	EksParent *loopUnit ## _eks_sloopUnit; \
	do{ \
		loopUnit ## _eks_sloopUnit=loopUnit->nextChild;

/** used with eks_parent_foreach_child_start_test */
#define EKS_FIRST_UNIT_COUNTER EKS_MACRO_CONCAT(eks_firstUnit_ , __LINE__)
/** used with eks_parent_foreach_child_start_test */
#define EKS_SLOOP_UNIT_COUNTER EKS_MACRO_CONCAT(eks_sloopUnit_ , __LINE__)
/** used with eks_parent_foreach_child_start_test */
#define EKS_FIX_UNIT_COUNTER EKS_MACRO_CONCAT(eks_fixUnit_ , __LINE__)
/** used with eks_parent_foreach_child_start_test */
#define EKS_GOTO_COUNTER EKS_MACRO_CONCAT(eks_sloopUnit_ , __LINE__)

/** used with eks_parent_foreach_child_start_test */
#define EKS_CONCAT_TWO(x, y) x ## y
/** used with eks_parent_foreach_child_start_test */
#define EKS_MACRO_CONCAT(x, y) EKS_CONCAT_TWO(x, y)

/** used with eks_parent_foreach_child_start_test */
#define eks_parent_foreach_child_start_test_count(thisParent,loopUnit,EKS_FIRST_UNIT,EKS_SLOOP_UNIT,EKS_FIX_UNIT,EKS_GOTO) \
	do{ \
	if(!thisParent){ \
		eks_error_message("eks_parent_foreach_child_start: thisParent is NULL!"); \
		break; \
	} \
	EksParent *EKS_FIRST_UNIT=thisParent->firstChild; \
	if(!EKS_FIRST_UNIT){ \
		eks_error_message("eks_parent_foreach_child_start: No first child? Parent %p",thisParent); \
		break; \
	} \
	EksParent *EKS_FIX_UNIT=EKS_FIRST_UNIT; \
	EksParent *EKS_SLOOP_UNIT; \
	goto EKS_GOTO; \
	while((EKS_FIX_UNIT=EKS_SLOOP_UNIT)!=EKS_FIRST_UNIT){ \
		EKS_GOTO: \
		EKS_SLOOP_UNIT=EKS_FIX_UNIT->nextChild; \
		loopUnit=EKS_FIX_UNIT;

/** used with eks_parent_foreach_child_start_test */
#define eks_parent_foreach_child_start_test(thisParent,loopUnit) \
	eks_parent_foreach_child_start_test_count(thisParent,loopUnit,EKS_FIRST_UNIT_COUNTER,EKS_SLOOP_UNIT_COUNTER,EKS_FIX_UNIT_COUNTER,EKS_GOTO_COUNTER)


/**
	The closing function to the foreach loop in eksparent. (remember to open with: eks_parent_foreach_child_start).
	Also the parameters should be the same for eks_parent_foreach_child_start and eks_parent_foreach_child_end
	
	@param thisParent 
		this parent object is the parameter for the parent you want to run the foreach function for.
	@param loopUnit
		This is the parameter you should use as the return value for each loop.
*/


#define eks_parent_foreach_child_end(thisParent,loopUnit) \
	loopUnit=loopUnit ## _eks_sloopUnit; \
	}while(loopUnit!=loopUnit ## _eks_firstUnit); \
	}while(0)

/** used with eks_parent_foreach_child_start_test */
#define eks_parent_foreach_child_end_test \
	} \
	}while(0)

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

EksParent *eks_parent_add_child_base(EksParent *thisParent, EksParentType ptype, EksParent *extras);
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

EksParent *eks_get_extras(EksParent *thisParent);

/*****FOR OUTPUT*****/

char *eks_parent_dump_text_with_settings(EksParent *thisParent,int oneLine,int paranthesis);

char *eks_parent_dump_text(EksParent *thisParent);

/*****FOR PARSING*****/

void eks_parent_parse_char(EksParseType *parser, char c);

EksParent *eks_parent_parse_text(const char *buffer,const char *name);

EksParent *eks_parent_parse_file(const char *filename);
