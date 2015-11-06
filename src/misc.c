/**
	@file
	@author Florian Evaldsson
	
	@section LICENSE
	
	Copyright (c) 2015 Florian Evaldsson <florian.evaldsson@telia.com>


	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
	@section DESCRIPTION

	The c-file for other useful functions
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
//#include <locale.h>
#include "eksparent.h"
#include "misc.h"

/**
	Fast (int)log(num)/log(10)
	
	@param num
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
	
	@param num
		The value to convert
	@return
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

/**
	Converts a double to a string (this was meant to be done in a better way)
	
	@param dnum
		the input double
	@return
		the output string
*/
char *eks_double_to_string(double dnum)
{
/*	intptr_t num=dnum;*/
/*	double restnum=dnum-(double)num;*/

/*	int isNegative=0;*/
/*	if(dnum<0)*/
/*	{*/
/*		isNegative=1;*/
/*		//make it positive*/
/*		num=-num;*/
/*		restnum=-restnum;*/
/*	}*/
/*	*/
/*	int lengthint=log10s(num);*/
/*	int lengthintn=lengthint;*/
/*	int lengthdec=15;*/
/*	int length=lengthint+lengthdec;*/
/*	*/
/*	char* retString=malloc(sizeof(char)*(length+3+isNegative));*/
/*	*/
/*	retString[length+2+isNegative]='\0';*/
/*	retString[lengthint+1+isNegative]='.';*/
/*	*/
/*	if(isNegative)*/
/*	{*/
/*		retString[0]='-';*/
/*	}*/
/*	*/
/*	while(0<=lengthintn)*/
/*	{*/
/*		retString[lengthintn+isNegative]='0'+num%10;*/
/*		num=(intptr_t)(num/10);*/
/*		lengthintn--;*/
/*	}*/
/*	*/
/*	for(int restcount=0;restcount<=lengthdec;restcount++)*/
/*	{*/
/*		restnum=(restnum*10);*/
/*		retString[lengthint+isNegative+2+restcount]='0'+((int)restnum)%10;*/
/*		restnum=restnum-(int)restnum;*/
/*		*/
/*	}*/
/*	*/
/*	return retString;*/
	
	//char *ourlocale=setlocale(LC_NUMERIC, NULL);
	//setlocale(LC_NUMERIC, "C");
	
	char convtostr[G_ASCII_DTOSTR_BUF_SIZE];
	
	sprintf(convtostr, "%.17g", dnum);
	
	//setlocale(LC_NUMERIC, ourlocale);
	
	char *newstr=g_strdup(convtostr);
	
	char *newstrcheck=newstr;
	
	//if it is another locale ... (faster than setlocale i guess ...)
	while(*newstrcheck)
	{
		char c=*newstrcheck;
		if(!((c>='0' && c<='9') || (c=='-')))
		{
			*newstrcheck='.';
			break;
		}
		
		newstrcheck++;
	}
	
	return newstr;
}

/**
	get the numerical value of a char ('0'=>0,'A'=>10)
	
	@param c
		the input char
	@return
		the numerical value
*/
static int eks_get_char_numerical_value(char c)
{
	if(c>='0' && c<='9')
		return c-'0';
	else if(tolower(c)>='a' && tolower(c)<='z')
		return tolower(c)-'a'+10;
	else
		return -1;
}

/**
	This function will convert a string to an intptr_t or double.

	String to doublefunction.
	Should be able to convert from these formats:
	123
	123.123
	.123
	0xfff.fff
	0b101.101
	1.3e-4.
	should also support minus, and underscores '_'.
	
	@param str
		input string to read from, the string requires to be at least size 1 or higher to work.
	@param outd
		the double-pointer to the output, in which we will write to
	@param outi
		the int-pointer to the output, in which we will write to
	@return
		will return which type it will be outputted as
*/
EksParentValue eks_string_to_double(const char *str,double *outd,intptr_t *outi)
{
	char *strchange=(char*)str;
	char c;
	int cnum;
			
	int isminus=1;
	uint8_t decimalpoint=0;
	uint8_t type=10; //0x or 0b
	
	intptr_t maindata=0;
	intptr_t remainder=0;
	intptr_t isfloat=0;
	int remaindercount=0;
	double preexponent=0;
	uint8_t exponenton=0;
	
	if(str[0]=='-')
	{
		isminus=-1;
		strchange++;
	}
		
	if(str[0]=='0')
	{
		if(str[1]=='x' || str[1]=='X')
		{
			type=16;
			strchange+=2;
		}
		else if(str[1]=='b' || str[1]=='B')
		{
			type=2;
			strchange+=2;
		}
		else if(str[1]=='o' || str[1]=='O')
		{
			type=8;
			strchange+=2;
		}
	}
	
	while(*strchange)
	{
		c=*strchange;
		cnum=eks_get_char_numerical_value(c);
		
		if(c=='.')
		{
			isfloat=1;
			decimalpoint=1;
			goto nextchar;
		}
		else if(c=='e' || c=='E')
		{
			exponenton=1;
			preexponent=(double)isminus*((double)maindata+(remainder/pow(type,remaindercount)));
			
			maindata=0;
			remainder=0;
			remaindercount=0;
			decimalpoint=0;
			
			if(strchange[1]=='-')
			{
				isminus=-1;
				strchange++;
			}
			else
				isminus=1;
				
			goto nextchar;
		}
		
		if(cnum < 0 || cnum >= type)
			return EKS_PARENT_VALUE_STRING;
		else
		{
			if(cnum<type)
			{
				if(!decimalpoint)
				{
					maindata=maindata*type+cnum;
				}
				else
				{
					remainder=remainder*type+cnum;
					remaindercount++;
				}
			}
		}
			
		nextchar:
			
		strchange++;
	}
	
	if(isfloat)
	{
		if(exponenton)
		{
			//printf("exp: %f %d %f\n",preexponent,type,(double)isminus*((double)maindata+(remainder/pow(type,remaindercount))));
			*outd=preexponent*pow(type,(double)isminus*((double)maindata+(remainder/pow(type,remaindercount))));
		}
		else
			*outd=(double)isminus*((double)maindata+(remainder/pow(type,remaindercount)));
			
		return EKS_PARENT_VALUE_DOUBLE;
	}
	else
	{
		*outi=isminus*(maindata);
		return EKS_PARENT_VALUE_INT;
	}
}
