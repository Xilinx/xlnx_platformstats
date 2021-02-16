/******************************************************************************
*
* Copyright (C) 2018-2019 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMANGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
******************************************************************************/

/***************************** Include Files *********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <platformstats.h>

/************************** Variable Definitions *****************************/
static int verbose_flag=0;
int interval=1;
char *filename;

/************************** Function Definitions *****************************/
/*****************************************************************************/
/**
*
* This function prints command usage for platformstats utility.
*
* @param    None
*
* @return   None
*
* @note     None
*
*******************************************************************************/
static void print_usage()
{
	printf(" XILINX PLATFORM STATS UTILITY \n");
	printf(" Monitor the following platform stats: \n");
	printf(" CPU utilization: Print CPU utilization for all configured CPUs on system \n");
	printf(" Usage: ./platformstats --interval 1000\n");
	printf(" 	--interval	Specify the decimal value for polling in ms. The default is 1000ms.  \n");
	printf(" 	--verbose	Print verbose messages  \n");
	printf(" 	--logfile	Print output to logfile  \n");
	printf(" 	--stop   	Stop any running instances of platformstats  \n");
	printf("	--help		Show this usuage.\n");

}

int main(int argc, char *argv[])
{
	int opt,options_index = 0;
	static struct option long_options[] =
	{
		/* These options set a flag; */
		{"verbose", no_argument, &verbose_flag, 1},
		{"brief", no_argument, &verbose_flag, 0},
		/* These options dont set a flag; */
		{"interval", required_argument, 0, 'i'},
		{"logfile", required_argument, 0, 'l'},
		{"stop", required_argument, 0, 's'},
		{"help", no_argument, 0, 'h'},
		{0,0,0,0}
	};

	while(1)
	{
		/* Parse arguments */
		opt = getopt_long(argc, argv, "vi:l:s:h",long_options, &options_index);
		if (opt == -1)
		{
			print_all_stats(verbose_flag, filename, interval);
			break;
		}
		switch(opt)
		{
			case 'v':
				verbose_flag = 1;
				break;
			case 'i':
				interval = atoi(optarg);
				break;
			case 'l':
				filename = optarg;
				break;
			case 'h':
				print_usage();
				break;
			default:
				printf("Incorrect options passed, please see usage");
				print_usage();
				return(0);
		}
	}
	return(0);
}

