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
	printf(" Usage: platformstats [options] [stats]\n\n");
	printf(" Options \n");
	printf(" 	-i --interval		Specify the decimal value for polling in ms. The default is 1000ms.  \n");
	printf(" 	-v --verbose		Print verbose messages  \n");
	printf(" 	-l --logfile		Print output to logfile  \n");
	printf(" 	-s --stop   		Stop any running instances of platformstats  \n");
	printf("	-h --help		Show this usuage.\n\n");
	printf(" List of stats to print\n");
	printf("	-a --all		Print all supported stats.\n");
	printf("	-c --cpu-util		Print CPU Utilization.\n");
	printf("	-r --ram-util		Print RAM Utilization.\n");
	printf("	-s --swap-util		Print Swap Mem Utilization.\n");
	printf("	-p --power-util		Print Power Utilization.\n");
	printf("	-m --cma-util		Print CMA Mem Utilization.\n");
	printf("	-f --cpu-freq		Print CPU frequency.\n");

}

int main(int argc, char *argv[])
{
	int opt,options_index = 0;
	static struct option long_options[] =
	{
		/* These options set a flag; */
		{"verbose", no_argument, &verbose_flag, 1},
		{"brief", no_argument, &verbose_flag, 0},
		{"all", no_argument, 0, 'a'},
		/* These options dont set a flag; */
		{"interval", required_argument, 0, 'i'},
		{"logfile", required_argument, 0, 'l'},
		{"stop", required_argument, 0, 's'},
		{"help", no_argument, 0, 'h'},
		{"cpu-util", no_argument, 0, 'c'},
		{"ram-util", no_argument, 0, 'r'},
		{"swap-util", no_argument, 0, 's'},
		{"power-util", no_argument, 0, 'p'},
		{"cma-util", no_argument, 0, 'm'},
		{"cpu-freq", no_argument, 0, 'f'},
		{0,0,0,0}
	};

	while(1)
	{
		/* Parse arguments */
		opt = getopt_long(argc, argv, "voacrspmfi:l:s:h",long_options, &options_index);
		if (opt == -1)
		{
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
			case 'a':
				print_all_stats(verbose_flag,filename,interval);
				break;
			case 'c':
				print_cpu_utilization(verbose_flag,filename);
				break;
			case 'r':
				print_ram_memory_utilization(verbose_flag,filename);
				break;
			case 's':
				print_swap_memory_utilization(verbose_flag,filename);
				break;
			case 'p':
				print_power_utilization(verbose_flag,filename);
				break;
			case 'm':
				print_cma_utilization(verbose_flag,filename);
				break;
			case 'f':
				print_cpu_frequency(verbose_flag,filename);
				break;
			default:
				printf("Incorrect options passed, please see usage");
				print_usage();
				return(0);
		}
	}
	return(0);
}

