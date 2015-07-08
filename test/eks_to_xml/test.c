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

FILE * pFile;

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
	
	//printf("struct: %s %d\n",text,theParent->structure);
	
	if(theParent->structure>=0)
	{
		if(theParent->firstChild!=NULL)
		{
			fprintf(pFile,"<%s>",text);
	
			eks_parent_foreach_child(theParent,to_xml,NULL);
		
			fprintf(pFile,"</%s>",text);
		}
		else
		{
			if(theParent->rt==EKS_PARENT_IS_TAGVALUE)
			{
				if(text[0]=='!')
					fprintf(pFile,"<%s>",text);
				else
					fprintf(pFile,"<%s />",text);
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
	
	printf("%s\n",eks_parent_dump_text(topParentFromRead));
	/* Lets read the data */
	
	eks_parent_foreach_child(topParentFromRead,to_xml,NULL);
	
	/* Clean everything up */
	
	//to make valgrind happy
	eks_parent_destroy(topParentFromRead,EKS_TRUE);
	
	//lets print that it was successful
	printf("\nSUCCESS!!!\n");
	
	return 0;
}
