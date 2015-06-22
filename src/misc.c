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

	The c-file for other useful functions
*/

#include <stdint.h>
#include <stdlib.h>
#include "misc.h"

/**
	Fast (int)log(num)/log(10)
	
	@num .
		the value to calculate
	@return
		the successful value
*/
static int log10s(intptr_t num)
{
	int res=0;
	while(num>=10)
	{
		num=(num/10);
		res++;
	}
	return res;
}

/**
	Converts int to string
	
	@num .
		The value to convert
	@return NEW
		returns pointer to the new string
*/
char *eks_int_to_string(intptr_t num)
{
	int isNegative=0;
	if(num<0)
	{
		isNegative=1;
		//make it positive
		num=-num;
	}
	
	int length=log10s(num);
	
	char* retString=malloc(sizeof(char)*(length+2+isNegative));
	
	retString[length+1+isNegative]='\0';
	
	if(isNegative)
	{
		retString[0]='-';
	}
	
	while(0<=length)
	{
		retString[length+isNegative]='0'+num%10;
		num=(intptr_t)(num/10);
		length--;
	}
	return retString;
}
