/**
	@file
	@author Florian Evaldsson
	
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

/// this is the starting buffer for the common words or strings
#define EKS_PARSER_COMMON_START_BUFFER 500
///this is the starting buffer for the comments
#define EKS_PARSER_COMMENT_START_BUFFER 500

static void eks_parse_set_common_nothing(EksParseType *parser);
static EksParent *eks_parse_set_common_and_list(EksParseType *parser);
static unsigned int eks_parse_get_current_line(EksParseType *parser);

/**
	Default function to initiate and parse the data
	
	@param name
		name of the toplevel parent
	@param upperLevelParser
		if its a parser inside a [] notation and require its parent. This value needs to be NULL when initializing the first parser.
	@return
		returns the a new parsing object, ready to be used.
*/
EksParseType *eks_parent_init_parse(const char *name, EksParseType *upperLevelParser)
{
	EksParent *par=eks_parent_new(name,EKS_PARENT_TYPE_VALUE,NULL,NULL);
	
	EksParseType *parser=calloc(1,sizeof(EksParseType));
	
	//set all defaults
	parser->parent=par;
	parser->currentParent=par;
	
	parser->state=EKS_PARENT_STATE_NOTHING;
	parser->stateComment=EKS_PARENT_STATE_COMMENT_NONE;
	//parser->currentParentLevel=0;
	
	if(upperLevelParser)
	{
		parser->upperInternalValueParser=upperLevelParser;
		
		parser->commonWord=upperLevelParser->commonWord;
		parser->commentWord=upperLevelParser->commentWord;
	}
	else
	{
		EksParseString *common=calloc(1,sizeof(EksParseString));
		EksParseString *comment=calloc(1,sizeof(EksParseString));
	
		parser->commonWord=common;
		parser->commentWord=comment;
		
		common->word=malloc(EKS_PARSER_COMMON_START_BUFFER*sizeof(char));
		comment->word=malloc(EKS_PARSER_COMMENT_START_BUFFER*sizeof(char));
		
		common->wordSize=EKS_PARSER_COMMON_START_BUFFER;
		comment->wordSize=EKS_PARSER_COMMENT_START_BUFFER;
	}
	
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
	//temporary to force all text to end
	if(parser->state==EKS_PARENT_STATE_NOTHING)
		eks_parse_set_common_nothing(parser);
	else if(parser->state==EKS_PARENT_STATE_AND_NAME || parser->state==EKS_PARENT_STATE_AND_AFTER)
		eks_parse_set_common_and_list(parser);
	else
	{
		eks_error_message("syntax error! at line %d",eks_parse_get_current_line(parser));
	}

	//for the return variable
	EksParent *par=parser->parent;

	if(parser->upperInternalValueParser==NULL)
	{
		free(parser->commonWord->word);
		free(parser->commentWord->word);
		free(parser->commonWord);
		free(parser->commentWord);
	}
	
	free(parser->ladder);
		
	free(parser);
	
	//return the variable
	return par;
}

/**
	Gets the current line number in the parsing
	
	@param parser
		The input parser
	@return
		Returns the current line number
*/
static unsigned int eks_parse_get_current_line(EksParseType *parser)
{
	EksParseType *tempParser=parser;
	
	while(tempParser->upperInternalValueParser!=NULL)
	{
		tempParser=tempParser->upperInternalValueParser;
	}
	
	return tempParser->currentLineAmount;
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
static void eks_parse_add_char_std(EksParseString *string, char c)
{
	string->currentWordSize++;
	
	size_t newWordSize=string->currentWordSize;
			
	if(newWordSize>string->wordSize)
	{
		string->wordSize=newWordSize;
		string->word=realloc(string->word,sizeof(char)*newWordSize);
	}
	
	string->word[newWordSize-1]=c;
}

/**Add a char to the string in the parsing structure */
#define eks_parse_add_char(parser,char) eks_parse_add_char_std(parser->commonWord,char)
/**Add a char to the comment in the parsing structure */
void eks_parse_add_char_comment(EksParseType *parser,char c)
{
	if(parser->stateIsInternalValueParent==0)
	{
		eks_parse_add_char_std(parser->commentWord,c);
	}
}

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
static char *eks_parse_get_text_std(EksParseString *string)
{
	char *word=string->word;
	//size_t wordSize=string->wordSize;
	size_t currentWordSize=string->currentWordSize;
	
	if(currentWordSize>0)
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
		
			string->currentWordSize=0;
		
			return str;
		}
	}
	//on fail
	string->currentWordSize=0;

	return NULL;
}

/** Gets the text from the word */
#define eks_parse_get_text(parser) eks_parse_get_text_std(parser->commonWord)
/** Gets the text from the comment */
#define eks_parse_get_text_comment(parser) eks_parse_get_text_std(parser->commentWord)

/**
	Happens when the "and name" (\#\#name) is finnished and wants to do something with the name.
	
	@param parser
		the parser to be changed.
*/
static EksParent *eks_parse_set_common_and_list(EksParseType *parser)
{
	char *str=eks_parse_get_text(parser);

	eks_debug_message("AND LIST<%d>[%s]\n",(int)parser->currentParentLevel,str);
	
	EksParent *tempParent=eks_parent_add_child(parser->currentParent,str,EKS_PARENT_TYPE_VALUE,NULL);
	tempParent->rt=EKS_PARENT_IS_TAGVALUE;
	
	if(parser->internalValueParserResult)
	{
		tempParent->firstExtras=parser->internalValueParserResult;
		parser->internalValueParserResult=NULL;
	}
	
	if(str)
		free(str);
			
	//parser->currentWordSize=0;
	
	return tempParent;
}

/**
	Happens when a string, vector value or something similar is set.
	
	@param parser
		the parser to be changed
*/
static void eks_parse_set_common_nothing(EksParseType *parser)
{
	char *str=eks_parse_get_text(parser);
	
	if(str)
	{
		eks_debug_message("<%d>[%s]\n",(int)parser->currentParentLevel,str);
		
		EksParent *tempParent=eks_parent_add_child(parser->currentParent,str,EKS_PARENT_TYPE_VALUE,NULL);
		tempParent->rt=EKS_PARENT_IS_UNTAGGED;
		
		free(str);
	}
}

/**
	The function used to end the optimization of the ']', since it needs to be defined on multiple spots.
	
	@param parser
		the input parser
*/
static void eks_parse_optimize_square_bracket_end(EksParseType *parser)
{
	EksParseType *upperParser=parser->upperInternalValueParser;
	upperParser->internalValueParserResult=eks_parent_exit_parse(parser);
	upperParser->stateIsInternalValueParent=0;
	
	EksParseString *currword=upperParser->commonWord;
	
	eks_debug_message("storedword %s\n",upperParser->storedWord);
	
	if(upperParser->storedWord!=NULL && currword->currentWordSize==0)
	{
		size_t storedWordSize=strlen(upperParser->storedWord);
	
		memcpy(currword->word,upperParser->storedWord,storedWordSize);
		currword->currentWordSize=storedWordSize;
		free(upperParser->storedWord);
		upperParser->storedWord=NULL;
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
	//if it is [#internal value:text]
	if(parser->stateIsInternalValueParent>=1)
	{
		//run only once
		if(parser->stateIsInternalValueParent==1)
		{
			parser->internalValueParser=eks_parent_init_parse(NULL,parser);
			parser->stateIsInternalValueParent=2;
		}
	
		eks_parent_parse_char(parser->internalValueParser,c);
		return;
	}

	//count for each new line
	if(c=='\n' && parser->upperInternalValueParser==NULL)
	{
		parser->currentLineAmount++;
	}

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
			
			char *str=eks_parse_get_text_comment(parser);
			if(str)
			{
				eks_debug_message("COMMENT<%ld>[%s]\n",parser->currentParentLevel,str);
				
				EksParent *theChild=eks_parent_add_child(parser->currentParent,str,EKS_PARENT_TYPE_COMMENT,NULL);
				theChild->rt=EKS_PARENT_IS_NOSPAN;
				
				free(str);
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

				char *str=eks_parse_get_text_comment(parser);
				if(str)
				{
					eks_debug_message("ML COMMENT<%ld>[%s] %c\n",parser->currentParentLevel,str,c);
				
					EksParent *theChild=eks_parent_add_child(parser->currentParent,str,EKS_PARENT_TYPE_COMMENT,NULL);
					theChild->rt=EKS_PARENT_IS_SPAN;
					
					free(str);
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
				parser->stateIsDollarvalue=0;
			}
			else
			{
				parser->state=EKS_PARENT_STATE_NOTHING;
				
				//it should be like this
				//#->##
				//if(parser->stateIsDollarvalue==0)
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel-1));
				
				EksParent *theChild=eks_parse_set_common_and_list(parser);
				
				if(parser->stateIsDollarvalue==0)
				{
					parser->currentParent=theChild;
				}
				else
				{
					parser->currentParentLevel--;
				}
			}
		}
		
		//for optimization
		int stateIsNotString=(parser->stateIsString==0);
		
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
			else if(c==']' && stateIsNotString && parser->upperInternalValueParser)
			{
				eks_parse_optimize_square_bracket_end(parser);
				
				return;
			}
			else if(c=='#' && stateIsNotString)
			{
				parser->state=EKS_PARENT_STATE_AND_COUNT;
				
				eks_parse_set_common_nothing(parser);
				
				parser->prevParentLevel=parser->currentParentLevel;
				parser->currentParentLevel=0;
			}
			else if(c=='$' && stateIsNotString)
			{
				parser->state=EKS_PARENT_STATE_AND_NAME;
				parser->stateIsDollarvalue=1;
				
				eks_parse_set_common_nothing(parser);
				
				parser->prevParentLevel=parser->currentParentLevel;
				parser->currentParentLevel++;
				
				return;
			}
			else if((c=='\n' || c=='|') && stateIsNotString)
			{
				//for and lists
				eks_parse_set_common_nothing(parser);
				
				return;
			}
			else if(c=='{' && stateIsNotString)
			{
				eks_parse_set_common_nothing(parser);
				
				//add #
				parser->currentParentLevel++;
				
				//at {
				
				parser->ladderCurrentAmount++;
				
				if(parser->ladderCurrentAmount>parser->ladderAmount)
				{
					parser->ladderAmount=parser->ladderCurrentAmount;
					parser->ladder=realloc(parser->ladder,sizeof(int)*parser->ladderAmount);
				}
				
				parser->ladder[parser->ladderCurrentAmount-1]=parser->currentParentLevel;
				
				parser->currentParent=eks_parse_set_common_and_list(parser);
				parser->currentParent->span=EKS_PARENT_IS_SPAN;
				
				//relative itself
				parser->currentParentLevel=0;
				parser->prevParentLevel=0;
				
				return;
			}
			
			//needs more testing
			else if(c=='}' && stateIsNotString)
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
			else if(c=='/' && stateIsNotString)
			{
				//for comment
				//eks_parse_set_common_nothing(parser);
				parser->stateComment=EKS_PARENT_STATE_COMMENT;
				return;
			}
			else if(c=='@' && stateIsNotString)
			{
				//for variable
				parser->state=EKS_PARENT_STATE_VAR_COUNT;
				eks_parse_set_common_nothing(parser);
				
				return;
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
				parser->stateIsDollarvalue=0;
			}
		}
		else if(parser->state==EKS_PARENT_STATE_VAR_COUNT)
		{
			if(c=='#')
			{
				if(parser->stateVariableclimb==0)
				{
					parser->prevParentLevel=parser->currentParentLevel;
					parser->currentParentLevel=1;
					
					parser->stateVariableclimb=1;
				}
				else if(parser->stateVariableclimb==1)
					parser->currentParentLevel++;
				
				return;
			}
			else if(c=='!')
			{
				if(parser->stateVariableclimb==0)
				{
					parser->prevParentLevel=parser->currentParentLevel;
					parser->currentParentLevel=0;
					
					parser->stateVariableclimb=3;
				}
				return;
			}
			else if(c=='>')
			{
				if(parser->stateVariableclimb==0)
				{
					parser->prevParentLevel=parser->currentParentLevel;
					parser->currentParentLevel++;
					
					parser->stateVariableclimb=2;
				}
				return;
			}
			else if(c=='<')
			{
				if(parser->stateVariableclimb==0)
				{
					parser->prevParentLevel=parser->currentParentLevel;
					parser->currentParentLevel--;
					
					parser->stateVariableclimb=2;
				}
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
			else if(c==']' && stateIsNotString && parser->upperInternalValueParser)
			{
				eks_parse_optimize_square_bracket_end(parser);
				
				return;
			}
			else if((c==':' || c=='\n') && stateIsNotString && parser->stateIsDollarvalue==0)
			{	
				parser->state=EKS_PARENT_STATE_AND_AFTER;
			}
			else if((c==' ' || c=='\n' || c=='|') && stateIsNotString && parser->stateIsDollarvalue==1)
			{	
				parser->state=EKS_PARENT_STATE_AND_AFTER;
			}
			//if something is written: #hello#hello:hai
			else if(c=='#' && stateIsNotString)
			{
				parser->state=EKS_PARENT_STATE_AND_COUNT;
				
				//it should be like this
				//#->##
				parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel-1));
				
				EksParent *theChild=eks_parse_set_common_and_list(parser);
				
				if(parser->stateIsDollarvalue==0)
				{
					parser->currentParent=theChild;
				}
				
				parser->prevParentLevel=parser->currentParentLevel;
				parser->currentParentLevel=0;
			}
			else if(c=='/' && stateIsNotString)
			{
				parser->stateComment=EKS_PARENT_STATE_COMMENT;
				
				return;
			}
			else if(c=='[' && stateIsNotString)
			{
				parser->storedWord=eks_parse_get_text(parser);
			
				parser->stateIsInternalValueParent=1;
				return;
			}
			else if(c=='{' && stateIsNotString)
			{
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
			else if((c==' ' || c=='\t' || c=='\n' || c=='|') && stateIsNotString)
			{
				parser->state=EKS_PARENT_STATE_NOTHING;
				
				//####(4)->@(1->0)
				//printf("POS: %ld %ld\n",parser->currentParentLevel,parser->prevParentLevel);
				if(parser->stateVariableclimb)
				{
					parser->currentParent=eks_parent_climb_parent(parser->currentParent,-(parser->currentParentLevel-parser->prevParentLevel));
					parser->stateVariableclimb=0;
				}
				
				char *str=eks_parse_get_text(parser);
				if(str)
				{
					eks_debug_message("VARIABLE<%ld %ld>[%s]\n",parser->currentParentLevel,-(parser->currentParentLevel-parser->prevParentLevel),str);
					
					free(str);
				}
				else
					eks_debug_message("VARIABLE<%ld %ld>\n",parser->currentParentLevel,-(parser->currentParentLevel-parser->prevParentLevel));
				
				//parser->currentParentLevel--;
				//parser->currentParent=eks_parent_climb_parent(parser->currentParent,1);
				
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
EksParent *eks_parent_parse_text(const char *buffer,const char *name)
{
	EksParseType *parser=eks_parent_init_parse(name,NULL);
	
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
EksParent *eks_parent_parse_file(const char *filename)
{
	char c;
	FILE *filepointer;
	
	
	EksParseType *parser=eks_parent_init_parse(filename,NULL);
	
	filepointer=fopen(filename, "r");
	
	if(filepointer) 
	{ 
		//goes through the entire file, even the last character
		while((c=fgetc(filepointer)) != EOF)
		{
			eks_parent_parse_char(parser,c);
		}
	}
	else
	{
		//if its not possible to open the file
		eks_error_message("Could not open \"%s\"\n",filename);
		fclose(filepointer);
		eks_parent_exit_parse(parser);
		return 0;
	}
	
	fclose(filepointer);
	
	return eks_parent_exit_parse(parser);
}
