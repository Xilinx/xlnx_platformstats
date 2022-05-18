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
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <platformstats.h>


#define SLEEP_MIN_TIME 1

/************************** Variable Definitions *****************************/
static int verbose_flag=0;
char *filename;
FILE *fp_out;
static int sample_interval=0;
static int sample_window=1;
char *temp;
char *token;
int vals[2];
int i=0;

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
	printf("	-h --help		Show this usuage.\n\n");
	printf("	can combine multiple options at the same time.\n\n");
	printf("	Example: platformstats -va \n\n");
	printf("	         platformstats -v -a \n\n");
	printf("	         platformstats -l \"log.txt\" -p\"1 10\" \n\n");
	printf("	         platformstats -p\"1 10\" \n\n");
	printf(" List of stats to print\n");
	printf("	-a --all		Print all supported stats.\n");
	printf("	-c --cpu-util		Print CPU Utilization.\n");
	printf("	-p --power-util		Print Power and Temperature Info.\n");
	printf("	          		This option takes two optional arguments.\n");
	printf("	          		sample_interval: Frequency of readings. Ex 1 sec.\n");
	printf("	          		sample_window: Number of samples in moving average. Ex 10\n");
	printf("	-m --mem-util		Print all Mem Utilization.\n");
	printf("	-f --cpu-freq		Print CPU frequency.\n");
	printf("	-v --verbose		Enable verbose prints.\n");
	printf("	-l --log		Print to log file.\n");

}

int main(int argc, char *argv[])
{
	fp_out = stdout;
	int opt,options_index = 0;
	static struct option long_options[] =
	{
		/* These options set a flag; */
		{"verbose", no_argument, 0, 'v'},
		{"log", required_argument, 0, 'l'},
		{"all", no_argument, 0, 'a'},
		/* These options dont set a flag; */
		{"help", no_argument, 0, 'h'},
		{"cpu-util", no_argument, 0, 'c'},
		{"power-util", no_argument, 0, 'p'},
		{"mem-util", no_argument, 0, 'm'},
		{"cpu-freq", no_argument, 0, 'f'},
		{0,0,0,0}
	};

	if(argc == 1)
	{
		print_all_stats(verbose_flag, sample_interval, sample_window);
	}
	else
	{
		/* Process -v and -l options first */
		int i;
		for(i=0; i<argc; i++)
		{
			if(strcmp(argv[i],"-v")==0)
			{
				verbose_flag = 1;
			}
			if(strcmp(argv[i],"-l")==0)
			{
				if(i >= (argc-1))
				{
					printf("-l option requires an argument to be passed.\n");
					return(0);
				}
				filename = argv[i+1];
				fp_out = fopen(filename, "w");
				time_t rawtime;
				time(&rawtime);
				fprintf(fp_out, "Log file created on: %s\n", ctime(&rawtime));
			}
		}

		/* Parse arguments */
		while((opt = getopt_long(argc, argv, "voacmfp::l:s:h",long_options, &options_index))!=-1)
		{
			switch(opt)
			{
				case 'v':
					break;
				case 'l':
					break;
				case 'h':
					print_usage();
					break;
				case 'a':
					print_all_stats(verbose_flag, sample_interval, sample_window);
					break;
				case 'c':
					print_cpu_utilization(verbose_flag);
					break;
				case 'p':
					if(optarg)
					{
						temp = optarg;
						token = strtok(temp," ");
						i = 0;
						while(token != NULL)
						{
							vals[i++]=atoi(token);
							token = strtok(NULL," ");
						}
						sample_interval = vals[0];
						sample_window = vals[1];
					}
					else{
						sample_interval = 0;
						sample_window = 1;
					}
					print_power_utilization(verbose_flag,sample_interval,sample_window);
					break;
				case 'm':
					print_cma_utilization(verbose_flag);
					print_swap_memory_utilization(verbose_flag);
					print_ram_memory_utilization(verbose_flag);
					break;
				case 'f':
					print_cpu_frequency(verbose_flag);
					break;
				case ':':
					printf("Option requires an argument to be passed.\n");
					break;
				case '?':
					printf("Option requires an argument to be passed.\n");
					break;
				default:
					return(0);
			}
		}
	}

	return(0);
}

