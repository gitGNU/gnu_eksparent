/**
	@file
	@author Florian Evaldsson
	
	@section LICENSE
	
	Copyright (c) 2015 Florian Evaldsson <florian.evaldsson@telia.com>


	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	@section DESCRIPTION

	This file contains all functions related to dump the text. It also contains functions to print correct information.
*/

#include "eksparent.h"
#include "misc.h"

/**
	Converts a name like a"b to "a\"b", something that will be parsed to a"b.
	
	@param thisParent
		the parent to convert its name from
	@return
		the string which will be used. 
*/
static char *eks_parent_convert_name_to_dumpable_text(EksParent *thisParent)
{
	//find forbidden characters
	int NOFANCY=0;
	char *thetext=eks_parent_get_string(thisParent);
	char *looptext=thetext;
	char c;
	
	if(thetext && !eks_parent_compare_type(thisParent,EKS_PARENT_TYPE_COMMENT))
	{
		while(*looptext)
		{
			c=*looptext;
		
			if(c==',' || c=='#' || c=='@' || c=='$' || c=='{' || c=='}' || c=='"' || c=='\n' || c==':')
			{
				NOFANCY=1;
			}
			
			if(NOFANCY && c=='"')
			{
				*looptext='\0';
				
				intptr_t currentpos=(intptr_t)looptext-(intptr_t)thetext;
				
				printf("current pos %ld\n",currentpos);
				
				char *tmp=thetext;
				thetext=g_strconcat(thetext,"\\\"",(looptext+1),NULL);
				free(tmp);
				
				looptext=thetext+currentpos+1;
			}
		
			looptext++;
		}
	}
	
	if(NOFANCY)
	{
		char *tmp=thetext;
		thetext=g_strconcat("\"",thetext,"\"",NULL);
		free(tmp);
	}
	
	return thetext;
}

/**
	Will dump the contents of a parent structure.
	
	@param thisParent
		The parent to dump the information from
	@param oneLine
		this will set if it will do everything on one line or not
	@param paranthesis
		set this one to 1 if you want everything in paranthesis instead of ##:s
	@return
		The output text
*/
char *eks_parent_dump_text_with_settings(EksParent *thisParent,int oneLine,int paranthesis)
{
	//FIX FIX FIX (works now but not as i want)
	char *returnString=calloc(1,sizeof(char));
	char *StructureString=NULL;
	
	char *tabString=NULL;
	
	char *thisname=eks_parent_convert_name_to_dumpable_text(thisParent);
	
	char newLineStr[2];
	char newLineTagStr[2];
	char paranthesisStr[2];
	
	char *firstExtrasText;
	
	char doNothingString[1]={0};
	
	if(oneLine)
	{
		//dont print for the last value!
		int lastValue=(thisParent->upperEksParent->firstChild==thisParent->nextChild);
	
		if(!lastValue)
			newLineStr[0]='|';
		else
			newLineStr[0]='\0';
	}
	else
		newLineStr[0]='\n';
	
	newLineStr[1]='\0';
	
	if(oneLine)
		newLineTagStr[0]=':';
	else
		newLineTagStr[0]='\n';
	
	newLineTagStr[1]='\0';
	
	if(paranthesis)
	{
		paranthesisStr[0]='}';
		paranthesisStr[1]='\0';
	}
	else
		paranthesisStr[0]='\0';
	
	if(thisParent->firstExtras)
	{
		char *dumpedOutText=eks_parent_dump_text_with_settings(thisParent->firstExtras,1,0);
		firstExtrasText=g_strconcat("[",dumpedOutText,"]",NULL);
		free(dumpedOutText);
	}
	else
	{
		firstExtrasText=doNothingString;
	}
	
	if(thisParent->structure>=0)
	{
		//if its #obj
		//				obj
		//			text //(should be #text)
		int notstructedchildbelow=!(thisParent->upperEksParent->firstChild!=thisParent && thisParent->prevChild->firstChild!=NULL && thisParent->firstChild==NULL);
		
		if(thisParent->firstChild==NULL)
		{
			int strlen=thisParent->upperEksParent->structure+thisParent->rt;
			
			if(!oneLine)
			{
				tabString=malloc(sizeof(char)*(strlen+1));
				memset(tabString,'\t',thisParent->upperEksParent->structure);
				tabString[thisParent->upperEksParent->structure]='\0';
			}
			else
				tabString=doNothingString;
		
			void *temp=returnString;
			if(thisParent->rt)
				returnString=g_strconcat(returnString,tabString,"$",thisname,firstExtrasText,newLineTagStr,NULL);
			else
				returnString=g_strconcat(returnString,tabString,thisname,newLineStr,NULL);
			free(temp);
		}
		else
		{
			//add the hashtag signs, for the structure level.
			if(thisParent->structure>0)
			{
				if(!paranthesis)
				{
					if((StructureString=malloc(sizeof(char)*(thisParent->structure+1)))==NULL)
					{
						eks_error_message("Failed to allocate space for the returning string!");
						return NULL;
					}
		
					memset(StructureString,'#',thisParent->structure);
					StructureString[thisParent->structure]='\0';
			
					if(!oneLine)
					{
						if((tabString=malloc(sizeof(char)*(thisParent->structure)))==NULL)
						{
							eks_error_message("Failed to allocate space for the tabs!");
							return NULL;
						}
						memset(tabString,'\t',thisParent->structure-1);
						tabString[thisParent->structure-1]='\0';
					}
					else
						tabString=doNothingString;
			
					void *temp=returnString;
					if(thisname)
						returnString=g_strconcat(returnString,tabString,StructureString,thisname,firstExtrasText,newLineTagStr,NULL);
					else
						returnString=g_strconcat(returnString,tabString,StructureString,firstExtrasText,newLineTagStr,NULL);
					free(temp);
				}
				else
				{
					void *temp=returnString;
					returnString=g_strconcat(returnString,"#",thisname,"{",newLineTagStr,NULL);
					free(temp);
				}
			}
			/*else
			{
				//if it is a comment (should be improved, this will also include the variables!)
				void *temp=returnString;
				returnString=g_strconcat(returnString,"//",thisname,"\n",NULL);
				free(temp);
			}*/
		
			//do it for all the sub-children
			EksParent *firstUnit=thisParent->firstChild;
	
			if(firstUnit)
			{
				EksParent *loopUnit=firstUnit;

				do
				{
					char *temp=eks_parent_dump_text_with_settings(loopUnit,oneLine,paranthesis);
					void *temp2=returnString;
					returnString=g_strconcat(returnString,temp,NULL);
					free(temp2);
					free(temp);
			
					loopUnit=loopUnit->nextChild;
				}while(loopUnit!=firstUnit);
				
				if(paranthesis)
				{
					void *temp=returnString;
					returnString=g_strconcat(returnString,"}",NULL);
					free(temp);
				}
			}
			
		}
	}
	else if(thisParent->structure==-1)
	{
		if(!oneLine)
		{
			tabString=malloc(sizeof(char)*(thisParent->upperEksParent->structure+1));
			memset(tabString,'\t',thisParent->upperEksParent->structure);
			tabString[thisParent->upperEksParent->structure]='\0';
		}
		else
			tabString=doNothingString;
		
		void *temp=returnString;
		returnString=g_strconcat(returnString,tabString,"/*",thisname,"*/",newLineStr,NULL);
		free(temp);
	}
	
	free(StructureString);
	
	if(tabString!=doNothingString)
	free(tabString);
	free(thisname);
	
	if(firstExtrasText!=doNothingString)
		free(firstExtrasText);
	
	return returnString;
}

/**
	The standard version of the dump text see #eks_parent_dump_text_with_settings, but with the parameters set to default.
	
	@param thisParent
		the parent to extract its content from
	@return
		the string of everything
*/
inline char *eks_parent_dump_text(EksParent *thisParent)
{
	return eks_parent_dump_text_with_settings(thisParent,0,0);
}
