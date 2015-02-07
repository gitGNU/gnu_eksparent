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
#define b_critical_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;37m\e[41mCRITICAL\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display an error message with nice looking colors */
#define b_error_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;31mERROR\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display a warning message with nice looking colors */
#define b_warning_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;33mWARNING\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/** Display a debug message with nice looking colors */
#define b_debug_message(msg, ...) printf("\e[0m\e[1m" __FILE__ ":%d: \e[1;36mDEBUG\e[0m\e[1m: (%s)\n\e[32m→\e[0m\e[1m " msg "\e[0m\n\n",__LINE__,__func__, ##__VA_ARGS__)

/**
	different types of subobjects
*/
typedef enum EksParentType
{
	EKS_PARENT_TYPE_VALUE,
	EKS_PARENT_TYPE_TEXT,
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
	States for the eksparentparser state mashine
*/
typedef enum EksParseStates
{
	EKS_PARENT_STATE_NOTHING,
	EKS_PARENT_STATE_AND_COUNT,
	EKS_PARENT_STATE_AND_NAME,
	EKS_PARENT_STATE_VARIABLE
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
	EKS_PARENT_STATE_COMMENT_MULTILINE_END_CHECK
}EksParseStateComment;

/** 
	Destruction method for the eks_parent_destroy 
*/
typedef enum EksParentDestroyMethod
{
	EKS_DESTROY_ZERO=0,
	EKS_DESTROY_RECURSIVE=1,
	EKS_DESTROY_UPPER=2
}EksParentDestroyMethod;

/**
	This structure is meant to be used with the extra parameters for a structure.
	
	eg \#struct[obj1=something;obj2=something]{}
	
	this structure is not the 'final one' so it will probably change.
*/
typedef struct EksParentExtras
{
	char *name; ///< the name for an element, void if nothing.
	char *info; ///< the 'readable information' for an element.
	
	struct EksParentExtras *nextChild; ///< next child in linked list
	struct EksParentExtras *prevChild; ///< previous child in linked list
}EksParentExtras;

/**
	the most important object/structure in this library.
	Handles kinda everything
*/
typedef struct EksParent
{
	char *name; ///< The name of the parent.
	int structure; ///< The significant structure describing what the object 'is', it can be one of these: 1++++ = parent,-1=text,-1=value,-2=comment,0=topmost parent
	
	EksParentExtras *firstExtras; ///< in development, not used at the moment
	
	struct EksParent *upperEksParent; ///< The eks-parent above this object, NULL if nothing.

	struct EksParent *firstChild; ///< The first child below this object.
	
	struct EksParent *nextChild; ///< The next object in this linked list.
	struct EksParent *prevChild; ///< The previous object in this linked list.
	
}EksParent;

/**
	The main structure for the parsing.
*/
typedef struct EksParseType
{
	//for current states
	EksParseStates state; ///< Current state of the statemashine
	EksParseStateComment stateComment; ///< If it is a comment, we are currently looking into 
	uint8_t stateVector; ///< Saves if it is in vector mode or not, eg \#obj ← vector mode, or \#obj{\#obj{}} ← not vector mode

	//the return parent
	EksParent *parent; ///< The parent structure we will return
	EksParent *currentParent; ///< The current parent in the parsing. It will jump up and down, so its the most frequently used in the parsing process

	//for handeling the current amount of #s
	size_t currentParentLevel; ///< Saving the current amount of \# (levels)
	size_t previousParentLevel; ///< Saving the previous amount of \# (levels)

	//for handeling the input of the normal input string
	char *word; ///< The word, or the string. It will collect in here if it is in a string, like \#something or just a plain word
	size_t wordSize; ///< The currently allocated word size
	size_t currentWordSize; ///< How big the current word is at this moment

	//for handeling the input of the comment string
	char *commentWord; ///< The main string for saving chars in a comment.
	size_t commentWordSize; ///< The allocated size for the comment string
	size_t currentCommentWordSize; ///< The current size for the comment-word.

	//special string cases
	uint8_t stateBackslash; ///< If it noticed a backslash
	uint8_t stateIsString; ///< If it noticed a \" (quotation mark)

	//for the level ("#value{"s) of the parsing
	int *ladder; ///< If the levels are freaked out we kinda need to wierdly save the levels.
	size_t ladderAmount; ///< how many elements is allocated for the ladder
	int ladderCurrentAmount; ///< The current amount of elements in the ladder
}EksParseType;

/***********FUNCTIONS START HERE!******/

char *eks_parent_get_name(EksParent *tempEksParent);

size_t eks_parent_get_child_amount(EksParent *tempEksParent);

int eks_parent_set(EksParent *tempEksParent, char *name,EksParentType ptype);

void eks_parent_fix_structure(EksParent *parentToFix);

int eks_parent_insert(EksParent *thisEksParent,int childNum,EksParent *inEksParent);

EksParent *eks_parent_clone(EksParent *thisEksParent);

uint8_t eks_parent_check_child(EksParent *tempEksParent,int pos);

EksParent* eks_parent_get_child(EksParent *tempEksParent,int pos);

EksParent* eks_parent_get_first_child(EksParent *tempEksParent);

EksParent* eks_parent_get_last_child(EksParent *tempEksParent);

EksParent *eks_parent_climb_parent(EksParent *tempEksParent,int amount);

EksParent* eks_parent_get_child_from_name(EksParent *tempEksParent,const char *tempName);

/** if you only want to check if a child exists*/
#define eks_parent_check_child_from_name(par,name) eks_parent_get_child_from_name(par,name)

uint8_t eks_parent_compare_type(EksParent *tempEksParent, EksParentType ptype);

int eks_parent_get_amount_from_type(EksParent *tempEksParent, EksParentType ptype);

/** simplified version of eks_parent_get_amount_from_type.*/
#define eks_parent_get_amount_from_value_t(tempEksParent) eks_parent_get_amount_from_type(tempEksParent,EKS_PARENT_TYPE_VALUE)

/** simplified version of eks_parent_get_amount_from_type.*/
#define eks_parent_get_amount_from_text_t(tempEksParent) eks_parent_get_amount_from_type(tempEksParent,EKS_PARENT_TYPE_TEXT)

EksParent *eks_parent_get_child_from_type(EksParent *tempEksParent,int pos, EksParentType ptype);

/** simplified version of eks_parent_get_child_from_type.*/
#define eks_parent_get_child_from_value_t(tempEksParent,pos) eks_parent_get_child_from_type(tempEksParent,pos,EKS_PARENT_TYPE_VALUE)

/** simplified version of eks_parent_get_child_from_type.*/
#define eks_parent_get_child_from_text_t(tempEksParent,pos) eks_parent_get_child_from_type(tempEksParent,pos,EKS_PARENT_TYPE_TEXT)

char *eks_parent_get_information_from_type(EksParent *tempEksParent,int pos, EksParentType ptype);

/** simplified version of eks_parent_get_information_from_type.*/
#define eks_parent_get_information_from_value_t(tempEksParent,pos) eks_parent_get_information_from_type(tempEksParent,pos,EKS_PARENT_TYPE_VALUE)

/** simplified version of eks_parent_get_information_from_type.*/
#define eks_parent_get_information_from_text_t(tempEksParent,pos) eks_parent_get_information_from_type(tempEksParent,pos,EKS_PARENT_TYPE_TEXT)

void eks_parent_add_children(EksParent *tempEksParent,int num);

EksParent *eks_parent_add_child_from_type(EksParent *tempParent,char *name, EksParentType ptype);

void eks_parent_destroy(EksParent *tempEksParent,EksParentDestroyMethod recursive);

char *eks_parent_dump_text(EksParent *topLevelEksParent);

/*****FOR PARSING*****/

void eks_parent_parse_char(EksParseType *parser, char c);

EksParent *eks_parent_parse_text(unsigned char *buffer,const char *name);

EksParent *eks_parent_parse_file(char *filename);
