/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include "platformstats.h"
#include "utils.h"

/************************** Function Definitions *****************************/
/*****************************************************************************/
/*
*
* This API opens /proc/stat file to read information for each CPU and store it in struct
* /proc/stat displays the following columns of information for any CPU
* user: time spent on processes executing in user mode with normal priority
* nice: time spent on processes executing in user mode with "niced" priority
* system: time spent on processes executing in kernel mode
* idle: time spent idling (no CPU instructions) while there were no disk I/O requests
* outstanding.
* iowait: time spent idling while there were outstanding disk I/O requests.
* irq: time spent servicing interrupt requests.
* softirq: time spent servicing softirq.
*
* @param	cpu_stat: store CPU stats
* @param	cpu_id: CPU id for which the details must be caputred.
*
* @return	None.
*
* @note		Internal API only.
*
******************************************************************************/
int get_stats(struct cpustat *cpu_stat, int cpu_id)
{
	FILE *fp;

	fp = fopen("/proc/stat", "r");

	if(fp == NULL)
	{
		printf("Unable to open /proc/stat. Returned errono: %d", errno);
		return(errno);
	}
	else
	{
		int lskip;
		char cpun[255];

		lskip = cpu_id+1;
		skip_lines(fp, lskip);

		fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld", cpun,
			&(cpu_stat->user), &(cpu_stat->nice), &(cpu_stat->system),
			&(cpu_stat->idle), &(cpu_stat->iowait), &(cpu_stat->irq),
			&(cpu_stat->softirq));

		fclose(fp);
	}

	return(0);
}

/*****************************************************************************/
/*
*
* This API prints CPU stats stored in given structure for particular CPU id 
*
* @param	filename: Print details to specified logfile
* @param	cpu_stat: struct that stores CPU stats
* @param	cpu_id: CPU id for which the details must be caputred.
*
* @return	None.
*
* @note		Internal API only.
*
******************************************************************************/
int print_cpu_stats(char *filename,struct cpustat *st, int cpu_id)
{
	printf("CPU%d: %ld %ld %ld %ld %ld %ld %ld\n", cpu_id, (st->user), (st->nice), 
        (st->system), (st->idle), (st->iowait), (st->irq),
        (st->softirq));
	
	return(0);
}

/*****************************************************************************/
/*
*
* This API calculates CPU util in real time, by computing delta at two time instances.
* By default the interval between two time instances is 1s if not specified. 
*
* @param	prev: CPU stats at T0
* @param	curr: CPU stats at T1
*
* @return	cpu_util.
*
* @note		Internal API only.
*
******************************************************************************/
double calculate_load(struct cpustat *prev, struct cpustat *curr)
{
	unsigned long idle_prev, idle_curr, nidle_prev, nidle_curr;
	unsigned long total_prev, total_curr;
	double total_delta, idle_delta, cpu_util; 

	idle_prev=(prev->idle)+(prev->iowait);
	idle_curr=(curr->idle)+(curr->iowait);

	nidle_prev = (prev->user) + (prev->nice) + (prev->system) + (prev->irq) + (prev->softirq);
    	nidle_curr = (curr->user) + (curr->nice) + (curr->system) + (curr->irq) + (curr->softirq);

    	total_prev = idle_prev + nidle_prev;
    	total_curr = idle_curr + nidle_curr;

    	total_delta = (double) total_curr - (double) total_prev;
    	idle_delta = (double) idle_curr - (double) idle_prev;

	cpu_util = (1000 * (total_delta - idle_delta) / total_delta + 1) / 10;
	
	return (cpu_util);
}

/*****************************************************************************/
/*
*
* This API identifies the number of configured CPUs in the system. For each
* active CPU it reads the CPU stats by calling get_stats and then calculates
* load.
*
* @param	verbose_flag: Enable verbose prints on stdout
* @param	filename: Print to specified logfile
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
int print_cpu_utilization(int verbose_flag, char*filename)
{
	int num_cpus_conf, cpu_id;
	struct cpustat *st0_0, *st0_1;

	num_cpus_conf= get_nprocs_conf();
	cpu_id=0;

	st0_0 = malloc(num_cpus_conf * sizeof (struct cpustat));
	st0_1 = malloc(num_cpus_conf * sizeof (struct cpustat));
	
	if(!st0_0 || !st0_1)
	{
		printf("Unable to allocate memory, malloc failed");
		return(errno);
	}

	for(; cpu_id < num_cpus_conf; cpu_id++)
	{
		st0_0[cpu_id].total_util = 0;
		st0_1[cpu_id].total_util = 0;

		get_stats(&st0_0[cpu_id],cpu_id);
	        sleep(1);
		get_stats(&st0_1[cpu_id],cpu_id);
		st0_1[cpu_id].total_util = calculate_load(&st0_0[cpu_id],&st0_1[cpu_id]);
	
		if(verbose_flag)
		{
			filename = NULL;
			printf("cpu_id=%d\nStats at t0\n",cpu_id);
			print_cpu_stats(filename,&st0_0[cpu_id],cpu_id);
			printf("Stats at t1 after 1s\n");
			print_cpu_stats(filename,&st0_1[cpu_id],cpu_id);
		}
		printf("CPU%d utilization: %lf%%\n",cpu_id,st0_1[cpu_id].total_util);
	}

	free(st0_0);
	free(st0_1);

	return(0);
}

/*****************************************************************************/
/*
*
* This API calls all other APIs that read, compute and print different platform
* stats
*
* @param        verbose_flag: Enable verbose prints on stdout
* @param        filename: Print to specified logfile 
* @param        interval: The interval these different stats should be queried 
* and printed
*
* @return       None.
*
* @note         None.
*
******************************************************************************/
void print_all_stats(int verbose_flag, char*filename, int interval)
{
	print_cpu_utilization(verbose_flag, filename);
}
