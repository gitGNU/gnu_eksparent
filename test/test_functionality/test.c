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

	This is the main in this simple test.
	This also demonstrates how the parser works.
*/

#include <stdio.h>
#include <eksparent.h>

int main(void)
{

	EksParent *test=eks_parent_parse_file("./ex.eks");
	char *result=eks_parent_dump_text(test);
	
	printf("\nDONE!\n\n");
	
	printf("%s",result);
	
	//SaveFile_tl("result.eks", result,strlen(result));
	
	size_t flen;
	
	char *fileContents;
	g_file_get_contents("result.eks",&fileContents,&flen,NULL);
	
	printf("the test should fail, since im testing :P\n");
	
	printf("\n\n%s\n",strcmp(fileContents,result)==0?"Test succesfully passed!":"The test failed!");
	
	//test internal destroy
	eks_parent_destroy(test->firstChild,EKS_TRUE);
	
	//eks_parent_destroy(test->firstChild,EKS_TRUE);
	
	//eks_parent_destroy(test->firstChild,EKS_TRUE);
	
	eks_parent_destroy(test,EKS_TRUE);
	
	free(fileContents);
	free(result);
	
	return 0;
}
