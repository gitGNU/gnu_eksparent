/**
	@file
	@author Florian Evaldsson
	@version 0.1
	
	@section LICENSE
	
	Copyright (c) 2015 Florian Evaldsson <florian.evaldsson@telia.com>


	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <eksparent.h>

//the pointer for the file which we will generate (output file)
FILE * pFile;

/**
	Convert the parameters to something readable, this is the internal part, which only needs to be read by the #to_attribute function.
	Note this function is very simple, and might generate wrong.
	
	@param theParent
		the parent to convert to xml parameters
	@param inparam
		fetched from the foreach function
*/
static void to_attribute_internal(EksParent *theParent,void *inparam)
{
	char *text=eks_parent_get_string(theParent);
	
	if(theParent->structure>=0)
	{
		if(theParent->firstChild!=NULL)
		{
			fprintf(pFile,"%s=",text);
			eks_parent_foreach_child(theParent,to_attribute_internal,NULL);
		}
		else
		{
			if(theParent->rt==EKS_PARENT_IS_TAGVALUE)
			{
				fprintf(pFile,"%s=\"\"",text);
			}
			else
			{
				//if it is the last object in internal list
				if(theParent==theParent->upperEksParent->upperEksParent->firstChild->prevChild->firstChild)
				{
					fprintf(pFile,"\"%s\"",text);
				}
				else
				{
					fprintf(pFile,"\"%s\" ",text);
				}
			}
		}
	}
	else
	{
		fprintf(pFile,"<!--%s-->",text);
	}
	
	free(text);
}

/**
	Convert the parameters to something readable
	Note this function is very simple, and might generate wrong.
	
	@param theParent
		the parent to convert to xml parameters
*/
void to_attribute(EksParent *theParent,char *tagname,char *after)
{
	EksParent *extras=eks_get_extras(theParent);

	if(extras)
	{
		fprintf(pFile,"<%s ",tagname);
		eks_parent_foreach_child(extras,to_attribute_internal,NULL);
	}
	else
	{
		fprintf(pFile,"<%s",tagname);
	}
	
	fprintf(pFile,"%s",after);
}

/**
	Example-function that demonstrates the foreach function. If you want to construct it urself, then its quite simple. 
	Check the source code to see. However the foreach function is "safe" so you can destroy the parent if you like.
	
	@param theParent
		The parent you will get from the foreach function.
	@param inparam
		An inparam that can be user-defined. However we dont need it in this demo, so its set to "NULL" below. 
*/
void to_xml(EksParent *theParent,void *inparam)
{
	char *text=eks_parent_get_string(theParent);
	
	if(theParent->structure>=0)
	{
		if(theParent->firstChild!=NULL)
		{
			to_attribute(theParent,text,">");
	
			eks_parent_foreach_child(theParent,to_xml,NULL);
		
			fprintf(pFile,"</%s>",text);
		}
		else
		{
			if(theParent->rt==EKS_PARENT_IS_TAGVALUE)
			{
				if(text[0]=='!')
				{
					to_attribute(theParent,text,">");
				}
				else
				{
					to_attribute(theParent,text," />");
				}
			}
			else
			{
				fprintf(pFile,"%s",text);
			}
		}
	}
	else
	{
		fprintf(pFile,"<!--%s-->",text);
	}
	
	free(text);
}

/**
	The main function
*/
int main(int argv,char *argc[])
{
	char *fileTo="index.html",*fileFrom="./index.eks";

	if(argv>=3)
		fileTo=argc[2];
	if(argv>=2)
		fileFrom=argc[1];
		
	printf("READING: %s, WRITING TO: %s\n",fileFrom,fileTo);
	
	pFile=fopen(fileTo,"w");
		
	//read the data from the file...
	EksParent *topParentFromRead=eks_parent_parse_file(fileFrom);
	
	printf("\n\nReading was successful!!! %ld\n\n",sizeof(EksParent));
	
	char *dumptext=eks_parent_dump_text(topParentFromRead);
	
	printf("%s\n",dumptext);
	
	free(dumptext);
	
	/* Lets read the data */
	
	eks_parent_foreach_child(topParentFromRead,to_xml,NULL);
	
	/* Clean everything up */
	
	//to make valgrind happy
	eks_parent_destroy(topParentFromRead,EKS_TRUE);
	
	fclose(pFile);
	
	//lets print that it was successful
	printf("\nSUCCESS!!!\n");
	
	return 0;
}
