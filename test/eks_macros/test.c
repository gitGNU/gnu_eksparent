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

int main(void)
{
	EksParent *theParent,*curChildParent;
	char *dumptext;
	
	//create the toplevel parent, or initiate the entire structure
	theParent=eks_parent_new("TopLevelParent",EKS_PARENT_TYPE_VALUE,NULL,NULL);
	
	//insert a #child with the value text
	curChildParent=eks_parent_add_child(theParent,"text",EKS_PARENT_TYPE_VALUE,NULL);
	
	//we just want to insert text here
	eks_parent_add_child(curChildParent,"blah blah",EKS_PARENT_TYPE_VALUE,NULL);
	
	/* Lets try another method */
	
	EksParent *childToInsert;
	
	//we will insert this to the #text tag
	childToInsert=eks_parent_new("even more text...",EKS_PARENT_TYPE_VALUE,NULL,NULL);
	
	//insert it
	eks_parent_insert(curChildParent,childToInsert);
	
	/* Lets make a "new" parent to insert...*/
	
	childToInsert=eks_parent_new("another_text_tag",EKS_PARENT_TYPE_VALUE,NULL,NULL);
	
	eks_parent_add_child(childToInsert,"this should contain interesting text...",EKS_PARENT_TYPE_VALUE,NULL);
	//add a double...
	eks_parent_add_child(childToInsert,(double)5/3,EKS_PARENT_TYPE_VALUE,NULL);
	
	eks_parent_insert(curChildParent,childToInsert);

	//lets see if it was inserted...
	dumptext=eks_parent_dump_text(theParent);
	printf("%s\n",dumptext);
	
	free(dumptext);
	
	//start the macro testing

	#ifdef ASM_OUT
		asm("START OF FIRST LOOP");
	#endif

	eks_parent_foreach_child_start_test(theParent->firstChild,EksParent *loopParent)
	{
		printf("name: %s\n",loopParent->name);
		if(loopParent->firstChild!=NULL)
		{
			eks_parent_foreach_child_start_test(loopParent,EksParent *loopParent2)
			{
				printf("name-in: %s\n",eks_parent_get_string(loopParent2));
			}eks_parent_foreach_child_end_test;
		}
	}eks_parent_foreach_child_end_test;
	
	#ifdef ASM_OUT
		asm("END OF FIRST LOOP");
	#endif
	
	printf("test other version of the same macro\n");
	
	#ifdef ASM_OUT
		asm("START OF SECOND LOOP");
	#endif
	
	eks_parent_foreach_child_start(theParent->firstChild,loopParent)
	{
		printf("name: %s\n",loopParent->name);
		if(loopParent->firstChild!=NULL)
		{
			eks_parent_foreach_child_start(loopParent,loopParent2)
			{
				printf("name-in: %s\n",eks_parent_get_string(loopParent2));
			}eks_parent_foreach_child_end(loopParent,loopParent2);
		}
	}eks_parent_foreach_child_end(theParent->firstChild,loopParent);
	
	#ifdef ASM_OUT
		asm("END OF SECOND LOOP");
	#endif
	
	/* Clean everything up */
	
	//to make valgrind happy
	eks_parent_destroy(theParent,EKS_TRUE);
	
	//lets print that it was successful
	printf("SUCCESS!!!\n");
	
	return 0;
}
