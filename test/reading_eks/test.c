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

/**
	Example-function that demonstrates the foreach function. If you want to construct it urself, then its quite simple. 
	Check the source code to see. However the foreach function is "safe" so you can destroy the parent if you like.
	
	@param theParent
		The parent you will get from the foreach function.
	@param inparam
		An inparam that can be user-defined. However we dont need it in this demo, so its set to "NULL" below. 
*/
void search_for_cuteness(EksParent *theParent,void *inparam)
{
	//if we found the cuteness factor <3
	if(strcmp(theParent->name,"cuteness")==0)
	{
		char *animal=eks_parent_get_name(theParent->upperEksParent);
		char *cutenessFactor=eks_parent_get_name(eks_parent_get_first_child(theParent));
	
		printf("The animal:  %s,\nhas the cuteness factor:  %s!!! <3 <3 <3\n",animal,cutenessFactor);
		
		free(animal);
		free(cutenessFactor);
	}
	
	if(theParent->firstChild!=NULL)
	{
		eks_parent_foreach_child(theParent,search_for_cuteness,NULL);
	}
}

int main(void)
{
	//read the data from the file...
	EksParent *topParentFromRead=eks_parent_parse_file("./good_example.eks");
	
	printf("\n\nReading was successful!!!\n\n");
	
	
	/* Lets read the data */
	
	eks_parent_foreach_child(topParentFromRead,search_for_cuteness,NULL);
	
	/* Clean everything up */
	
	//to make valgrind happy
	eks_parent_destroy(topParentFromRead,EKS_TRUE);
	
	//lets print that it was successful
	printf("SUCCESS!!!\n");
	
	return 0;
}
