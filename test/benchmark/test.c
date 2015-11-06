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
#include <statistik.h>
#include <sys/time.h>

#define SAMPLE_SIZE 10000

/**
	Get the current time-stamp.
	
	@returns
		the current time-stamp
*/
long long current_timestamp()
{
	struct timeval te; 
	//get the current time
	gettimeofday(&te, NULL);

	//calculate the microseconds
	long long microseconds = te.tv_sec * 1000000LL + te.tv_usec;

	return microseconds;
}

int main(void)
{
	stati_t *myStats=stati_init(SAMPLE_SIZE);

	const char *filename="./../functionality/ex.eks";

	for(int i=0;i<SAMPLE_SIZE;i++)
	{
		long long startTime=current_time_microseconds();

		EksParent *test=eks_parent_parse_file(filename);
	
		long long finishTime=current_time_microseconds();
	
		stati_add(myStats,finishTime-startTime);
	
/*		char *result=eks_parent_dump_text(test);*/
	
		eks_parent_destroy(test,EKS_TRUE);

/*		free(result);*/
	}
	
	stati_sort(myStats);
	
	char *out=stati_print_all(myStats,0);
	printf("%s\n",out);
	stati_free(out);
	
	stati_destroy(myStats);
	
/*	double mean     = gsl_stats_mean(data, 1, SAMPLE_SIZE);*/
/*	double variance = gsl_stats_variance(data, 1, SAMPLE_SIZE);*/
/*	double largest  = gsl_stats_max(data, 1, SAMPLE_SIZE);*/
/*	double smallest = gsl_stats_min(data, 1, SAMPLE_SIZE);*/
/*	*/
/*	printf("Mean: %g, Variance: %g, Largest: %g, Smallest: %g\n", mean,variance,largest,smallest);*/
	
	return 0;
}
