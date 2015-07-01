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

	Main file for handeling the parsing of eks.
	
	This file also uses printf-s to test if it works, this will show that this project is in its start-development.
*/

/* need to clean up the climb */
/*
#(+1)
##(+1)
###(+1)
#(-2)
###(+3)
#####(should work)(undefined 4, when definig 4, then it should be defined with its name)


###(3){(1)####(4)}(-5)@#(-3)

do i need previous parent level?

*/

#include "eksparent.h"

/**
	Default function to initiate and parse the data
	
	@param name
		name of the toplevel parent
	@return
		returns the a new parsing object, ready to be used.
*/
EksParseType *eks_parent_init_parse(const char *name)
{
	EksParent *par=eks_parent_new(name,EKS_PARENT_TYPE_VALUE,NULL,NULL);
	
	EksParseType *parser=calloc(1,sizeof(EksParseType));
	
	//set all defaults
	parser->parent=par;
	parser->currentParent=par;
	
	parser->state=EKS_PARENT_STATE_NOTHING;
	parser->stateComment=EKS_PARENT_STATE_COMMENT_NONE;
	//parser->currentParentLevel=0;
	
	return parser;
}

/**
	Default function to reset and get the toplevel parent.
	
	@param parser
		the input parsing object
	@return 
		returns the toplevel parent
*/
EksParent *eks_parent_exit_parse(EksParseType *parser)
{
	//for the return variable
	EksParent *par=parser->parent;

	free(parser->word);
	free(parser->commentWord);
	free(parser->ladder);
		
	free(parser);
	
	//return the variable
	return par;
}

/**
	Add char into the global variable of a string stream.
	
	@param word
		string stream
	@param wordSize
		max size of the string stream, used to increase itself
	@param currentWordSize
		size of the current string
	@param c
		input char to insert into word.
*/
static void eks_parse_add_char_std(char **word,size_t *wordSize,size_t *currentWordSize,char c)
{
	(*currentWordSize)++;
			
	if(*currentWordSize>*wordSize)
	{
		(*wordSize)=(*currentWordSize);
		(*word)=realloc((*word),sizeof(char)*(*wordSize));
	}
	
	(*word)[(*currentWordSize)-1]=c;
}

/**Add a char to the string in the parsing structure */
#define eks_parse_add_char(parser,char) eks_parse_add_char_std(&parser->word,&parser->wordSize,&parser->currentWordSize,char)
/**Add a char to the comment in the parsing structure */
#define eks_parse_add_char_comment(parser,char) eks_parse_add_char_std(&parser->commentWord,&parser->commentWordSize,&parser->currentCommentWordSize,char)

/**
	Gets the text from a string stream

	@param word
		the string stream
	@param wordSize
		max size of the string stream
	@param currentWordSize
		the current word size of the string (very comfortable)
	@return
		returns a std C string
*/
static char *eks_parse_get_text_std(char *word,size_t wordSize,size_t currentWordSize)
{
	char *str=NULL;
	size_t strlength=0;
	char c;
	
	uint8_t backslash=0;
	size_t rightTrim=0;
	
	for(int i=0;i<currentWordSize;i++)
	{
		c=word[i];
		
		if(backslash)
		{
			backslash=0;
		}
		else
		{
			if(c=='"')
			{
				//shrink string size also (remove "s)
				continue;
			}
			
			if((c!='\t' && c!=' ' && c!='\n') && str==NULL)
			{
				str=malloc(sizeof(char)*(currentWordSize-i+1));
			}
			
			if(c=='\\')
			{
				backslash=1;
				continue;
			}
		}
		
		if(str)
		{
			str[strlength]=c;
			strlength++;
		}
	}
	
	if(str)
	{
		for(int i=0;i<currentWordSize;i++)
		{
			c=word[currentWordSize-i-1];
			if((c!='\t' && c!=' ' && c!='\n'))
			{
				rightTrim=i;
				break;
			}
		}

		str=realloc(str,sizeof(char)*(strlength-rightTrim+1));
		str[strlength-rightTrim]='\0';
		
		return str;
	}

	//on fail
	return NULL;
}

/** Gets the text from the word */
#define eks_parse_get_text(parser) eks_parse_get_text_std(parser->word,parser->wordSize,parser->currentWordSize)
/** Gets the text from the comment */
#define eks_parse_get_text_comment(parser) eks_parse_get_text_std(parser->commentWord,parser->commentWordSize,parser->currentCommentWordSize)

/**
	Some sort of glue to eks_parent_set, but for the parsing. Will set something for a new child.
	See eks_parent_set

	@param parser
		the parsing object
	@param text
		text to add
	@param ptype
		type of the structure
*/
static void eks_parse_set_current_parent_child(EksParseType *parser, char *text, EksParentType ptype)
{
	EksParent *tparent;
			
	tparent=parser->currentParent;
	
	eks_parent_add_children(tparent,1);
	tparent=eks_parent_get_last_child(tparent);
	eks_parent_set(tparent,text,ptype);
}

/**
	Happens when the "and name" (\#\#name) is finnished and wants to do something with the name.
	
	@param parser
		the parser to be changed.
*/
static EksParent *eks_parse_set_common_and_list(EksParseType *parser)
{
	char *str=eks_parse_get_text(parser);

	printf("AND LIST<%d>[%s]\n",(int)parser->currentParentLevel,str);
	
	eks_parent_add_children(parser->currentParent,1);
	EksParent *tempParent=eks_parent_get_last_child(parser->currentParent);
	eks_parent_set(tempParent,str,EKS_PARENT_TYPE_VALUE);
	
	if(str)
		free(str);
			
	parser->currentWordSize=0;
	
	return tempParent;
}

/**
	Happens when a string, vector value or something similar is set.
	
	@param parser
		the parser to be changed
*/
static void eks_parse_set_common_nothing(EksParseType *parser)
{
	if(parser->currentWordSize>0)
	{
		char *str=eks_parse_get_text(parser);
		if(str)
		{
			printf("<%d>[%s]\n",(int)parser->currentParentLevel,str);
			
			eks_parse_set_current_parent_child(parser,str,EKS_PARENT_TYPE_TEXT);
			
			free(str);
		}
		parser->currentWordSize=0;
	}
}

/**
	For each char to parse. This is the main function of the parsing. Make sure you have initiated it first and that you read in char after char.

	@param parser
		the parser object to be changed
	@param c
		the input char
*/
void eks_parent_parse_char(EksParseType *parser, char c)
{
	//check comment type
	if(parser->stateComment==EKS_PARENT_STATE_COMMENT)
	{
		if(c=='/')
		{
			parser->stateComment=EKS_PARENT_STATE_COMMENT_SINGLE_LINE;
			return;
		}
		else if(c=='*')
		{
			parser->stateComment=EKS_PARENT_STATE_COMMENT_MULTILINE;
			return;
		}
		else
		{
			eks_parse_add_char(parser,'/');
			parser->stateComment=EKS_PARENT_STATE_COMMENT_NONE;
		}
	}
	
	/*****
	For comments
	*****/
	if(parser->stateComment==EKS_PARENT_STATE_COMMENT_SINGLE_LINE)
	{
		if(c=='\n')
		{
			parser->stateComment=EKS_PARENT_STATE_COMMENT_NONE;
			
			if(parser->currentCommentWordSize>0)
			{
				char *str=eks_parse_get_text_comment(parser);
				if(str)
				{
					printf("COMMENT<%ld>[%s]\n",parser->currentParentLevel,str);
					
					eks_parse_set_current_parent_child(parser,str,EKS_PARENT_TYPE_COMMENT);
					
					free(str);
				}
				parser->currentCommentWordSize=0;
			}
		}
		else
		{
			eks_parse_add_char_comment(parser,c);
		}
	}
	else if(parser->stateComment==EKS_PARENT_STATE_COMMENT_MULTILINE_END_CHECK)
	{
		if(c=='/')
		{
			if(parser->commentNestleSize<=0)
			{
				parser->stateComment=EKS_PARENT_STATE_COMMENT_NONE;
			
				if(parser->currentCommentWordSize>0)
				{
					char *str=eks_parse_get_text_comment(parser);
					if(str)
					{
						printf("ML COMMENT<%ld>[%s] %c\n",parser->currentParentLevel,str,c);
					
						eks_parse_set_current_parent_child(parser,str,EKS_PARENT_TYPE_COMMENT);
					
						free(str);
					}
					parser->currentCommentWordSize=0;
				}
			
				return;
			}
			else
			{
				eks_parse_add_char_comment(parser,'*');
				eks_parse_add_char_comment(parser,'/');
				parser->commentNestleSize--;
			}
		}
		else
		{
			//eks_parse_add_char_comment(parser,'*');
			//see below
			parser->stateComment=EKS_PARENT_STATE_COMMENT_MULTILINE;
		}
	}
	else if(parser->stateComment==EKS_PARENT_STATE_COMMENT_MULTILINE_NESTLE_CHECK)
	{
		eks_parse_add_char_comment(parser,'/');
	
		if(c=='*')
		{
			eks_parse_add_char_comment(parser,'*');
			parser->commentNestleSize++;
		}
		
		parser->stateComment=EKS_PARENT_STATE_COMMENT_MULTILINE;
	}
	
	if(parser->stateComment==EKS_PARENT_STATE_COMMENT_MULTILINE)
	{
		if(c=='*')
		{
			parser->stateComment=EKS_PARENT_STATE_COMMENT_MULTILINE_END_CHECK;
		}
		else if(c=='/')
		{
			parser->stateComment=EKS_PARENT_STATE_COMMENT_MULTILINE_NESTLE_CHECK;
		}
		else
		{
			eks_parse_add_char_comment(parser,c);
		}
		
		return;
	}
	/******
	For non comments
	******/
	if(parser->stateComment==EKS_PARENT_STATE_COMMENT_NONE)
	{
		if(parser->stateBackslash)
		{
			eks_parse_add_char(parser,c);
			parser->stateBackslash=0;
			
			return;
		}
		
		if(parser->state==EKS_PARENT_STATE_AND_AFTER)
		{
			if(c==' ' || c=='\t' || c=='\n')
				return;
			else if(c=='{')
			{
				//go down to state name
				parser->state=EKS_PARENT_STATE_AND_NAME;
			}
			else
			{
				parser->stateVector=1;
				parser->state=EKS_PARENT_STATE_NOTHING;
				
				//it should be like this
				//#->##
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel-1));
				
				parser->currentParent=eks_parse_set_common_and_list(parser);
			}
		}
		
		if(parser->state==EKS_PARENT_STATE_NOTHING)
		{
			if(c=='\\')
			{
				parser->stateBackslash=1;
				eks_parse_add_char(parser,c);
				return;
			}
			else if(c=='"')
			{
				parser->stateIsString^=1;
				eks_parse_add_char(parser,c);
			}
			else if(c=='#' && parser->stateIsString==0)
			{
				parser->state=EKS_PARENT_STATE_AND_COUNT;
				
				eks_parse_set_common_nothing(parser);
				
				parser->prevParentLevel=parser->currentParentLevel;
				parser->currentParentLevel=0;
			}
			else if(((c=='\n' && parser->stateVector==1) || c==',') && parser->stateIsString==0)
			{
				//for and lists
				eks_parse_set_common_nothing(parser);
				
				return;
			}
			
			else if(c=='{' && parser->stateIsString==0)
			{
				eks_parse_set_common_nothing(parser);
				
				//add #
				parser->currentParentLevel++;
				
				//at {
				
				parser->stateVector=0;
				
				parser->ladderCurrentAmount++;
				
				if(parser->ladderCurrentAmount>parser->ladderAmount)
				{
					parser->ladderAmount=parser->ladderCurrentAmount;
					parser->ladder=realloc(parser->ladder,sizeof(int)*parser->ladderAmount);
				}
				
				parser->ladder[parser->ladderCurrentAmount-1]=parser->currentParentLevel;
				
				parser->currentParent=eks_parse_set_common_and_list(parser);
				
				//relative itself
				parser->currentParentLevel=0;
				parser->prevParentLevel=0;
				
				return;
			}
			
			//needs more testing
			else if(c=='}' && parser->stateIsString==0)
			{
				eks_parse_set_common_nothing(parser);
			
				parser->ladderCurrentAmount--;
				
				if(parser->ladderCurrentAmount<0)
				{
					eks_error_message("Should '}' be there?");
				}
				
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,parser->currentParentLevel+1);
				
				parser->currentParentLevel=parser->ladder[parser->ladderCurrentAmount]-1;
				parser->prevParentLevel=parser->currentParentLevel;

				parser->ladder[parser->ladderCurrentAmount]=0;
					
				return;
			}
			else if(c==';' && parser->stateIsString==0)
			{
				eks_parse_set_common_nothing(parser);
			
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,1);
				parser->currentParentLevel--;
				parser->prevParentLevel=parser->currentParentLevel;
			}
			else if(c=='/' && parser->stateIsString==0)
			{
				//for comment
				//eks_parse_set_common_nothing(parser);
				parser->stateComment=EKS_PARENT_STATE_COMMENT;
				return;
			}
			else if(c=='@' && parser->stateIsString==0)
			{
				//for variable
				parser->state=EKS_PARENT_STATE_VAR_COUNT;
				eks_parse_set_common_nothing(parser);
				
				parser->prevParentLevel=parser->currentParentLevel;
				parser->currentParentLevel=0;
			}
			else
			{
				eks_parse_add_char(parser,c);
			}
		}
		
		if(parser->state==EKS_PARENT_STATE_AND_COUNT)
		{
			if(c=='#')
			{
				parser->currentParentLevel++;
				return;
			}
			else
			{
				parser->state=EKS_PARENT_STATE_AND_NAME;
			}
		}
		else if(parser->state==EKS_PARENT_STATE_VAR_COUNT)
		{
			if(c=='@')
			{
				parser->currentParentLevel++;
				return;
			}
			else
			{
				parser->state=EKS_PARENT_STATE_VARIABLE;
			}
		}
		
		if(parser->state==EKS_PARENT_STATE_AND_NAME)
		{
			if(c=='\\')
			{
				parser->stateBackslash=1;
				
				if(parser->stateIsString)
				{
					eks_parse_add_char(parser,c);
				}
				
				return;
			}
			else if(c=='"')
			{
				parser->stateIsString^=1;
				
				eks_parse_add_char(parser,c);
			}
			else if((c==':' || c=='\n') && parser->stateIsString==0)
			{	
				parser->state=EKS_PARENT_STATE_AND_AFTER;
			}
			else if(c=='/' && parser->stateIsString==0)
			{
				//eks_parse_set_common_and_list(parser);
				parser->stateComment=EKS_PARENT_STATE_COMMENT;
				
				return;
			}
			else if(c=='{' && parser->stateIsString==0)
			{
				parser->stateVector=0;
				parser->state=EKS_PARENT_STATE_NOTHING;
				
				parser->ladderCurrentAmount++;
				
				if(parser->ladderCurrentAmount>parser->ladderAmount)
				{
					parser->ladderAmount=parser->ladderCurrentAmount;
					parser->ladder=realloc(parser->ladder,sizeof(int)*parser->ladderAmount);
				}
				
				parser->ladder[parser->ladderCurrentAmount-1]=parser->currentParentLevel;
				
				//##(2)->####{(4+1)}
				//printf("DIS:%d\n",-(parser->currentParentLevel-parser->prevParentLevel-1));
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel-1));
				parser->currentParent=eks_parse_set_common_and_list(parser);
				
				parser->prevParentLevel=0;
				parser->currentParentLevel=0;
				
				return;
			}
			else
			{
				eks_parse_add_char(parser,c);
			}
		}
		
		if(parser->state==EKS_PARENT_STATE_VARIABLE)
		{
			if(c=='\\')
			{
				parser->stateBackslash=1;
				
				if(parser->stateIsString)
				{
					eks_parse_add_char(parser,c);
				}
				
				return;
			}
			else if(c=='"')
			{
				parser->stateIsString^=1;
				
				eks_parse_add_char(parser,c);
			}
			else if((c==' ' || c=='\t' || c=='\n') && parser->stateIsString==0)
			{
				parser->state=EKS_PARENT_STATE_NOTHING;
				
				//####(4)->@(1->0)
				printf("POS: %ld %ld\n",parser->currentParentLevel,parser->prevParentLevel);
				parser->currentParentLevel--;
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel));
				
				if(parser->currentWordSize>0)
				{
					char *str=eks_parse_get_text(parser);
					if(str)
					{
						printf("VARIABLE<%ld>[%s]\n",parser->currentParentLevel,str);
						
						if(strcmp(str,"#")==0)
						{
							parser->stateVector^=1;
						}
						free(str);
					}
					parser->currentWordSize=0;
				}
				
				return;
			}
			else
			{
				eks_parse_add_char(parser,c);
			}
		}
	}
}

////////////////////////////////

/**
	Parse ordinary text. input stream

	@param buffer 
		input buffer stream
	@param name
		name of the topmost parent
	@return
		returns the finnished parent, which you can use
*/
EksParent *eks_parent_parse_text(unsigned char *buffer,const char *name)
{
	EksParseType *parser=eks_parent_init_parse(name);
	
	while(*buffer)
	{
		eks_parent_parse_char(parser,*buffer);
		buffer++;
	}
	
	return eks_parent_exit_parse(parser);
}

/**
	Will load a tradstruct file and turn it into a EksParent structure.

	@param filename NO_FREE
		input filename
	@return NEW, FREE_WITH( eks_parent_destroy )
		the output EksParent structure
*/
EksParent *eks_parent_parse_file(char *filename)
{
	char c;
	FILE *filepointer;
	
	
	EksParseType *parser=eks_parent_init_parse(filename);
	
	filepointer=fopen(filename, "r");
	
	if(filepointer) 
	{ 
		//kollar igenom varje bokstav i filen även slutbokstaven
		while((c=fgetc(filepointer)) != EOF)
		{
			eks_parent_parse_char(parser,c);
		}
	}
	else
	{
		//om det inte går att öppna filen.
		eks_error_message("Could not open \"%s\"\n",filename);
		fclose(filepointer);
		eks_parent_exit_parse(parser);
		return 0;
	}
	
	fclose(filepointer);
	
	return eks_parent_exit_parse(parser);
}
