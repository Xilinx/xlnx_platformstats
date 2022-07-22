/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <time.h>

#include "xlnx_platformstats.h"
#include "utils.h"

#define MAX_FILENAME_LEN 500

FILE *fp_out;

struct cpustat* stat0;
struct cpustat* stat1;
double* util_arr;

long* powerarr;
long* currarr;
long* volarr;

long* LPD_TEMP_arr;
long* FPD_TEMP_arr;
long* PL_TEMP_arr;
long* VCC_PSPLL_arr;
long* PL_VCCINT_arr;
long* VOLT_DDRS_arr;
long* VCC_PSINTFP_arr;
long* VCC_PS_FPD_arr;
long* PS_IO_BANK_500_arr;
long* VCC_PS_GTR_arr;
long* VTT_PS_GTR_arr;

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
		fprintf(fp_out, "Unable to open /proc/stat. Returned errono: %d", errno);
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
 * @param       cpu_stat: store CPU stats
 *
 * @return      None.
 *
 * @note        Internal API only.
 *
 ******************************************************************************/
int get_cpu_stats(struct cpustat *cpu_stat)
{
        FILE *fp;
        int num_cpus = get_nprocs_conf();
        int cpu_id = 0;

        fp = fopen("/proc/stat", "r");

        if(fp == NULL)
        {
                fprintf(fp_out, "Unable to open /proc/stat. Returned errono: %d", errno);
                return(errno);
        }
        else
        {
                skip_lines(fp, 1);
                char cpun[255];
                for(; cpu_id < num_cpus; cpu_id++)
                {
                        fscanf(fp,"%s %ld %ld %ld %ld %ld %ld %ld", cpun,
                                &(cpu_stat[cpu_id].user), &(cpu_stat[cpu_id].nice),
                                &(cpu_stat[cpu_id].system), &(cpu_stat[cpu_id].idle),
                                &(cpu_stat[cpu_id].iowait), &(cpu_stat[cpu_id].irq),
                                &(cpu_stat[cpu_id].softirq));

                        skip_lines(fp, 1);

		}
                fclose(fp);
        }

        return(0);
}

/*****************************************************************************/
/*
 *
 * This API allocates memory for an array of cpustat structs based on the
 * number of cpus. stat0, stat1, util_arr are global pointers.
 *
 * @param       None
 *
 * @return      None
 *
 * @note        None
 *
 ******************************************************************************/
void init()
{
	int num_cpus = get_nprocs_conf();

	stat0 = malloc(num_cpus * sizeof (struct cpustat));
	get_cpu_stats(stat0);

	stat1 = malloc(num_cpus * sizeof (struct cpustat));
	get_cpu_stats(stat1);

	util_arr = malloc(num_cpus * sizeof (double));

	return;
}

/*****************************************************************************/
/*
 *
 * This API frees memory used by array of cpustat structs. stat0, stat1,
 * util_arr are global pointers.
 *
 * @param       None
 *
 * @return      None
 *
 * @note        None
 *
 ******************************************************************************/
void deinit()
{
	free(stat0);
	free(stat1);
	free(util_arr);

	return;
}

/*****************************************************************************/
/*
 *
 * This API reads the CPU stats by calling get_cpu_stats and then calculates
 * cpu utilization for each CPU based on current and previous reading.
 *
 * @param       prev: pointer to the cpustat array for previous reading
 * @param       curr: pointer to the cpustat array for current reading
 * @param	util: pointer to the utilization array
 * @param	len: length of util array (not required in python binding)
 *
 * @return      util: pointer to the utilization array
 *
 * @note        None.
 *
 ******************************************************************************/
double* get_cpu_utilization(size_t *len)
{
	int cpu_id = 0;
	int num_cpus = get_nprocs_conf();
	*len = num_cpus;

	memcpy(stat0, stat1, num_cpus * sizeof (struct cpustat));

	get_cpu_stats(stat1);

	for(; cpu_id < num_cpus; cpu_id++)
	{
		util_arr[cpu_id] = calculate_load(&stat0[cpu_id], &stat1[cpu_id]);
	}

	return(util_arr);
}

/*****************************************************************************/
/*
 *
 * This API prints CPU stats stored in given structure for particular CPU id 
 *
 * @param	cpu_stat: struct that stores CPU stats
 * @param	cpu_id: CPU id for which the details must be caputred.
 *
 * @return	None.
 *
 * @note		Internal API only.
 *
 ******************************************************************************/
int print_cpu_stats(struct cpustat *st, int cpu_id)
{
	fprintf(fp_out, "CPU%d: %ld %ld %ld %ld %ld %ld %ld\n", cpu_id, (st->user), (st->nice), 
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

	cpu_util = (1000 * (total_delta - idle_delta) / (total_delta + 1)) / 10;

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
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
int print_cpu_utilization(int verbose_flag)
{
	int num_cpus_conf, cpu_id;
	struct cpustat *st0_0, *st0_1;

	num_cpus_conf= get_nprocs_conf();
	cpu_id=0;

	st0_0 = malloc(num_cpus_conf * sizeof (struct cpustat));
	st0_1 = malloc(num_cpus_conf * sizeof (struct cpustat));

	if(!st0_0 || !st0_1)
	{
		fprintf(fp_out, "Unable to allocate memory, malloc failed");
		return(errno);
	}

	fprintf(fp_out, "CPU Utilization\n");
	for(; cpu_id < num_cpus_conf; cpu_id++)
	{
		st0_0[cpu_id].total_util = 0;
		get_stats(&st0_0[cpu_id],cpu_id);

		if(verbose_flag)
		{
			fprintf(fp_out, "cpu_id=%d\nStats at t0\n",cpu_id);
			print_cpu_stats(&st0_0[cpu_id],cpu_id);
		}
	}

	sleep(1);

	cpu_id = 0;

	for(; cpu_id < num_cpus_conf; cpu_id++)
	{
		st0_1[cpu_id].total_util = 0;
		get_stats(&st0_1[cpu_id],cpu_id);
		st0_1[cpu_id].total_util = calculate_load(&st0_0[cpu_id],&st0_1[cpu_id]);

		if(verbose_flag)
		{
			fprintf(fp_out, "Stats at t1 after 1s\n");
			print_cpu_stats(&st0_1[cpu_id],cpu_id);
		}
		fprintf(fp_out, "CPU%d\t:     %lf%%\n",cpu_id,st0_1[cpu_id].total_util);
	}

	fprintf(fp_out, "\n");
	free(st0_0);
	free(st0_1);

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API reads the sysfs enteries for a given sysfs file
 *
 * @param	filename: sysfs path
 * @param	value: value read from sysfs entry
 *
 * @return       None
 *
 * @note         None.
 *
 ******************************************************************************/
static void read_int_sysfs_entry(char* base_filename, char* filepath, int id, long *val)
{
	FILE *fp;
	char filename[MAX_FILENAME_LEN];

	strcpy(filename,base_filename);

	get_sys_abs_path(filename, id, filepath);

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		fprintf(fp_out, " File open returned with error : %s\n", strerror(errno));
		exit(0);
	}

	fscanf(fp,"%ld",val);
	fclose(fp);

}
/*****************************************************************************/
/*
 *
 * This API reads the sysfs enteries for a given sysfs file
 *
 * @param	filename: sysfs path
 * @param	value: value read from sysfs entry
 *
 * @return       None
 *
 * @note         None.
 *
 ******************************************************************************/
static void read_float_sysfs_entry(char* base_filename, char* filepath, int id, float *val)
{
	FILE *fp;
	char filename[MAX_FILENAME_LEN];

	strcpy(filename,base_filename);

	get_sys_abs_path(filename, id, filepath);

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		fprintf(fp_out, " File open returned with error : %s\n", strerror(errno));
		exit(0);
	}

	fscanf(fp,"%f",val);
	fclose(fp);

}

/*****************************************************************************/
/*
 *
 * This API reads the sysfs enteries for a given sysfs file
 *
 * @param	filename: sysfs path
 * @param	value: value read from sysfs entry
 *
 * @return       None
 *
 * @note         None.
 *
 ******************************************************************************/
static int read_char_sysfs_entry(char* base_filename, char* filepath, int id, char* value)
{

	FILE *fp;
	char filename[MAX_FILENAME_LEN];

	strcpy(filename,base_filename);

	get_sys_abs_path(filename, id, filepath);

	fp = fopen(filename,"r");

	if(fp == NULL)
	{
		fprintf(fp_out, "Unable to open %s\n",filename);
		exit(0);
	}

	fscanf(fp,"%s",value);
	fclose(fp);

	return(0);

}

/*****************************************************************************/
/*
 *
 * This API identifies the number of configured CPUs in the system. For each
 * active CPU it reads the CPU frequency by opening
 * /sys/devices/system/cpu/<cpuid>/cpufreq/cpuinfo_cur_freq.
 *
 * @param	verbose_flag: Enable verbose prints on stdout
 *
 * @return	cpu_freq.
 *
 * @note		Internal API.
 *
 ******************************************************************************/

int get_cpu_frequency(int cpu_id, float* cpu_freq)
{
	char base_filename[MAX_FILENAME_LEN] = "/sys/devices/system/cpu/cpu";
	char filepath[MAX_FILENAME_LEN] = "/cpufreq/cpuinfo_cur_freq";

     	read_float_sysfs_entry(base_filename,filepath,cpu_id,cpu_freq);

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API identifies the number of configured CPUs in the system. For each
 * active CPU it reads the CPU frequency by calling get_cpu_freq and prints it.
 *
 * @param	verbose_flag: Enable verbose prints on stdout
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
int print_cpu_frequency(int verbose_flag)
{
	int num_cpus_conf, cpu_id;
	float cpu_freq = 0;

	num_cpus_conf= get_nprocs_conf();
	cpu_id=0;

	fprintf(fp_out, "CPU Frequency\n");
	for(; cpu_id < num_cpus_conf; cpu_id++)
	{
		get_cpu_frequency(cpu_id,&cpu_freq);
		fprintf(fp_out, "CPU%d\t:    %f MHz\n",cpu_id,(cpu_freq)/1000);
	}
	fprintf(fp_out, "\n");

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API scans the following information about physical memory:
 * MemTotal: Total usable physical ram
 * MemFree: The amount of physical ram, in KB, left unused by the system
 * MemAvailable: An estimate on how much memory is available for starting new
 * applications, without swapping.It can be timated from MemFree, Active(file),
 * Inactive(file), and SReclaimable, as well as the "low"
 * watermarks from /proc/zoneinfo.
 *
 * @param        MemTotal: Total usable physical ram size
 * @param        MemFree: amount of RAM left unsused
 * @param        MemAvailable: estimate of amount of memory available to start a new
 * app
 *
 * @return       Error code.
 *
 * @note         Internal API.
 *
 ******************************************************************************/
int get_ram_memory_utilization(unsigned long* MemTotal, unsigned long* MemFree, unsigned long* MemAvailable)
{
	//read first three lines of file
	//print to terminal
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");

	if(fp == NULL)
	{
		fprintf(fp_out, "Unable to open /proc/meminfo. Returned errono: %d", errno);
		return(errno);
	}
	else
	{
		char buff[80];
		unsigned long temp;
		int reads = 3;

		while(reads)
		{
			fscanf(fp," %s %ld",buff,&temp);
			if(strcmp(buff,"MemTotal:") == 0)
			{
				*MemTotal = temp;
				reads--;
			}
			if(strcmp(buff,"MemFree:") == 0)
			{
				*MemFree = temp;
				reads--;
			}
			if(strcmp(buff,"MemAvailable:") == 0)
			{
				*MemAvailable = temp;
				reads--;
			}
			skip_lines(fp,1);
		}

		fclose(fp);
	}

	return(0);

}

/*****************************************************************************/
/*
 *
 * This API prints the following information about physical memory:
 * MemTotal: Total usable physical ram
 * MemFree: The amount of physical ram, in KB, left unused by the system
 * MemAvailable: An estimate on how much memory is available for starting new
 * applications, without swapping.It can be timated from MemFree, Active(file),
 * Inactive(file), and SReclaimable, as well as the "low"
 * watermarks from /proc/zoneinfo.
 *
 * @param        verbose_flag: Enable verbose prints
 * @param        MemAvailable: estimate of amount of memory available to start a new
 * app
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int print_ram_memory_utilization(int verbose_flag)
{
	unsigned long MemTotal=0, MemFree=0, MemAvailable=0;
	int mem_util_ret;

	mem_util_ret = 0;

	mem_util_ret = get_ram_memory_utilization(&MemTotal, &MemFree, &MemAvailable);

	fprintf(fp_out, "RAM Utilization\n");
	fprintf(fp_out, "MemTotal      :     %ld kB\n",MemTotal);
	fprintf(fp_out, "MemFree	      :     %ld kB\n", MemFree);
	fprintf(fp_out, "MemAvailable  :     %ld kB\n\n", MemAvailable);

	return(mem_util_ret);

}

/*****************************************************************************/
/*
 *
 * This API prints the following information about physical memory:
 * CMATotal: Total CMA information
 * CMAFree: The CMA alloc free information
 *
 * @param        verbose_flag: Enable verbose prints
 *
 * @return       Error code.
 *
 * @note         Internal API.
 *
 ******************************************************************************/
int get_cma_utilization(unsigned long* CmaTotal, unsigned long* CmaFree)
{
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");

	if(fp == NULL)
	{
		fprintf(fp_out, "Unable to open /proc/stat. Returned errono: %d", errno);
		return(errno);
	}
	else
	{
		char buff[80];
		unsigned long temp;
		int reads = 2;

		while(reads)
		{
			fscanf(fp," %s %ld",buff,&temp);
			if(strcmp(buff,"CmaTotal:") == 0)
			{
				*CmaTotal = temp;
				reads--;
			}
			if(strcmp(buff,"CmaFree:") == 0)
			{
				*CmaFree = temp;
				reads--;
			}
			skip_lines(fp,1);
		}

		fclose(fp);
	}

	return(0);

}

/*****************************************************************************/
/*
 *
 * This API prints the following information about physical memory:
 * CMATotal: Total CMA information
 * CMAFree: The CMA alloc free information
 *
 * @param        verbose_flag: Enable verbose prints
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int print_cma_utilization(int verbose_flag)
{
	unsigned long CmaTotal=0, CmaFree=0;
	int cma_util_ret;

	cma_util_ret = 0;

	cma_util_ret = get_cma_utilization(&CmaTotal, &CmaFree);

	fprintf(fp_out, "CMA Mem Utilization\n");
	fprintf(fp_out, "CmaTotal   :     %ld kB\n",CmaTotal);
	fprintf(fp_out, "CmaFree    :     %ld kB\n\n", CmaFree);

	return(cma_util_ret);

}

/*****************************************************************************/
/*
 *
 * This API scans the following information about physical swap memory:
 * SwapTotal: Total usable physical swap memory
 * SwapFree: The amount of swap memory free. Memory which has been evicted from RAM, 
 * and is temporarily on the disk.
 *
 * @param        SwapTotal: Total usable physical swap size
 * @param        SwapFree: amount of swap memory free
 *
 * @return       Error code.
 *
 * @note         Internal API.
 *
 ******************************************************************************/
int get_swap_memory_utilization(unsigned long* SwapTotal, unsigned long* SwapFree)
{
	FILE *fp;

	fp = fopen("/proc/meminfo", "r");

	if(fp == NULL)
	{
		fprintf(fp_out, "Unable to open /proc/stat. Returned errono: %d", errno);
		return(errno);
	}
	else
	{
		char buff[80];
		unsigned long temp;
		int reads = 2;

		while(reads)
		{
			fscanf(fp," %s %ld",buff,&temp);
			if(strcmp(buff,"SwapTotal:") == 0)
			{
				*SwapTotal = temp;
				reads--;
			}
			if(strcmp(buff,"SwapFree:") == 0)
			{
				*SwapFree = temp;
				reads--;
			}
			skip_lines(fp,1);
		}

		fclose(fp);
	}

	return(0);

}

/*****************************************************************************/
/*
 *
 * This API prints the following information about swap memory:
 * SwapTotal: Total usable physical swap memory
 * SwapFree: The amount of swap memory free. Memory which has been evicted from RAM, 
 * and is temporarily on the disk.
 *
 * @param        verbose_flag: Enable verbose prints
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int print_swap_memory_utilization(int verbose_flag)
{
	unsigned long SwapTotal=0, SwapFree=0;
	int mem_util_ret;

	mem_util_ret = 0;

	mem_util_ret = get_swap_memory_utilization(&SwapTotal, &SwapFree);

	fprintf(fp_out, "Swap Mem Utilization\n");
	fprintf(fp_out, "SwapTotal    :    %ld kB\n",SwapTotal);
	fprintf(fp_out, "SwapFree     :    %ld kB\n\n",SwapFree);

	return(mem_util_ret);

}

/*****************************************************************************/
/*
 *
 * This API returns the number of hwmon devices registered under /sys/class/hwmon
 *
 * @return       num_hwmon_devices: Number of registered hwmon devices
 *
 * @note         None.
 *
 ******************************************************************************/
int count_hwmon_reg_devices()
{
	//find number of hwmon devices listed under
	int num_hwmon_devices;
	DIR *d;
	struct dirent *dir;

	num_hwmon_devices = 0;
	d = opendir("/sys/class/hwmon");

	if(!d)
	{
		fprintf(fp_out, "Unable to open /sys/class/hwmon path\n");
		return(errno);
	}

	while((dir = readdir(d)) != NULL)
	{
		if(strstr(dir->d_name, "hwmon"))
		{
			num_hwmon_devices++;
		}
	}

	closedir(d);

	return(num_hwmon_devices);
}

/*****************************************************************************/
/*
 *
 * This API returns hwmon_id of the specified device:
 *
 * @param        name: device name for which hwmon_id needs to be identified
 *
 * @return       hwmon_id
 *
 * @note         None.
 *
 ******************************************************************************/
int get_device_hwmon_id(int verbose_flag, char* name)
{
	//find number of hwmon devices listed under
	int num_hwmon_devices,hwmon_id;
	char base_filename[MAX_FILENAME_LEN]="/sys/class/hwmon/hwmon";
	char filename[MAX_FILENAME_LEN];
	char device_name[MAX_FILENAME_LEN];

	hwmon_id=-1;

	num_hwmon_devices = count_hwmon_reg_devices();

	for(hwmon_id = 0; hwmon_id < num_hwmon_devices; hwmon_id++)
	{
		read_char_sysfs_entry(base_filename,"/name", hwmon_id, device_name);

		if(!strcmp(name,device_name))
		{
			return(hwmon_id);
		}

		if(verbose_flag)
		{
			fprintf(fp_out, "filename %s\n",filename);
			fprintf(fp_out, "device_name = %s\n",device_name);
		}
	}

	return(-1);
}

/*****************************************************************************/
/*
 *
 * Signal Handler for SIGINT, runs when user presses Ctrl+C
 *
 ******************************************************************************/
void sigint_handler(int sig_num)
{
	free(powerarr);
	free(currarr);
	free(volarr);

	free(LPD_TEMP_arr);
	free(FPD_TEMP_arr);
	free(PL_TEMP_arr);
	free(VCC_PSPLL_arr);
	free(PL_VCCINT_arr);
	free(VOLT_DDRS_arr);
	free(VCC_PSINTFP_arr);
	free(VCC_PS_FPD_arr);
	free(PS_IO_BANK_500_arr);
	free(VCC_PS_GTR_arr);
	free(VTT_PS_GTR_arr);

	exit(0);
}

/*****************************************************************************/
/*
 *
 * This API prints the following information about power utilization for ina260:
 * in1_input: Voltage input value.
 * curr1_input: Current input value.
 * power1_input: Instantaneous power use
 *
 * @param        verbose_flag: Enable verbose prints
 *
 * @return       Error code.
 *
 * @note         Internal API.
 *
 ******************************************************************************/
int print_ina260_power_info(int verbose_flag, int sample_interval, int sample_window, int pos, int len)
{
	int hwmon_id;
	long total_power, total_current, total_voltage;
	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(verbose_flag,"ina260_u14");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ina260_u14 under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/power1_input", hwmon_id, &total_power);
	read_int_sysfs_entry(base_filename,"/curr1_input", hwmon_id, &total_current);
	read_int_sysfs_entry(base_filename,"/in1_input", hwmon_id, &total_voltage);

	if(sample_interval > 0)
	{
		static long power_avg = 0;
		static long power_sum = 0;

		static long curr_avg = 0;
		static long curr_sum = 0;

		static long vol_avg = 0;
		static long vol_sum = 0;

		power_avg = movingAvg(powerarr, &power_sum, pos, len, (total_power)/1000);
		curr_avg = movingAvg(currarr, &curr_sum, pos, len, total_current);
		vol_avg = movingAvg(volarr, &vol_sum, pos, len, total_voltage);

		if(fp_out == stdout)
		{
			system("clear");
		}
		fprintf(fp_out, "----------------------------------------------------------------------------------------\n");
		time_t rawtime;
		time(&rawtime);
		fprintf(fp_out, "%d samples averaged on %s\n", len, ctime(&rawtime));
		fprintf(fp_out, "                                                           Instantaneous\t Average\n");
		fprintf(fp_out, "Power Utilization\n");
		fprintf(fp_out, "SOM total power                                         :     %ld mW\t\t %ld mW\n",(total_power)/1000, power_avg);
		fprintf(fp_out, "SOM total current                                       :     %ld mA\t\t %ld mA\n",total_current, curr_avg);
		fprintf(fp_out, "SOM total voltage                                       :     %ld mV\t\t %ld mV\n\n",total_voltage,vol_avg);
	}
	else
	{
		fprintf(fp_out, "Power Utilization\n");
		fprintf(fp_out, "SOM total power                                         :     %ld mW\n",(total_power)/1000);
		fprintf(fp_out, "SOM total current                                       :     %ld mA\n",total_current);
		fprintf(fp_out, "SOM total voltage                                       :     %ld mV\n\n",total_voltage);
	}

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API prints power utilization information for the system.
 *
 * @param        verbose_flag: Enable verbose prints
 * @param        sample_interval: Frequency of readings. Ex 1 sec
 * @param        sample_window: Number of samples in moving average. Ex 10
 *
 * @return       Error code.
 *
 * @note         Internal API.
 *
 ******************************************************************************/
int print_sysmon_power_info(int verbose_flag, int sample_interval, int sample_window, int pos, int len)
{
	int hwmon_id;
	long LPD_TEMP, FPD_TEMP, PL_TEMP;
	long VCC_PSPLL, PL_VCCINT, VOLT_DDRS, VCC_PSINTFP, VCC_PS_FPD;
	long PS_IO_BANK_500, VCC_PS_GTR, VTT_PS_GTR;

	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(verbose_flag,"ams");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ams under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/temp1_input", hwmon_id, &LPD_TEMP);
	read_int_sysfs_entry(base_filename,"/temp2_input", hwmon_id, &FPD_TEMP);
	read_int_sysfs_entry(base_filename,"/temp3_input", hwmon_id, &PL_TEMP);
	read_int_sysfs_entry(base_filename,"/in1_input", hwmon_id, &VCC_PSPLL);
	read_int_sysfs_entry(base_filename,"/in3_input", hwmon_id, &PL_VCCINT);
	read_int_sysfs_entry(base_filename,"/in6_input", hwmon_id, &VOLT_DDRS);
	read_int_sysfs_entry(base_filename,"/in7_input", hwmon_id, &VCC_PSINTFP);
	read_int_sysfs_entry(base_filename,"/in9_input", hwmon_id, &VCC_PS_FPD);
	read_int_sysfs_entry(base_filename,"/in13_input", hwmon_id, &PS_IO_BANK_500);
	read_int_sysfs_entry(base_filename,"/in16_input", hwmon_id, &VCC_PS_GTR);
	read_int_sysfs_entry(base_filename,"/in17_input", hwmon_id, &VTT_PS_GTR);

	if(sample_interval > 0)
	{
		static long LPD_TEMP_avg = 0;
		static long LPD_TEMP_sum = 0;

		static long FPD_TEMP_avg = 0;
		static long FPD_TEMP_sum = 0;

		static long PL_TEMP_avg = 0;
		static long PL_TEMP_sum = 0;

		static long VCC_PSPLL_avg = 0;
		static long VCC_PSPLL_sum = 0;

		static long PL_VCCINT_avg = 0;
		static long PL_VCCINT_sum = 0;

		static long VOLT_DDRS_avg = 0;
		static long VOLT_DDRS_sum = 0;

		static long VCC_PSINTFP_avg = 0;
		static long VCC_PSINTFP_sum = 0;

		static long VCC_PS_FPD_avg = 0;
		static long VCC_PS_FPD_sum = 0;

		static long PS_IO_BANK_500_avg = 0;
		static long PS_IO_BANK_500_sum = 0;

		static long VCC_PS_GTR_avg = 0;
		static long VCC_PS_GTR_sum = 0;

		static long VTT_PS_GTR_avg = 0;
		static long VTT_PS_GTR_sum = 0;

		LPD_TEMP_avg = movingAvg(LPD_TEMP_arr, &LPD_TEMP_sum, pos, len, LPD_TEMP);
		FPD_TEMP_avg = movingAvg(FPD_TEMP_arr, &FPD_TEMP_sum, pos, len, FPD_TEMP);
		PL_TEMP_avg = movingAvg(PL_TEMP_arr, &PL_TEMP_sum, pos, len, PL_TEMP);
		VCC_PSPLL_avg = movingAvg(VCC_PSPLL_arr, &VCC_PSPLL_sum, pos, len, VCC_PSPLL);
		PL_VCCINT_avg = movingAvg(PL_VCCINT_arr, &PL_VCCINT_sum, pos, len, PL_VCCINT);
		VOLT_DDRS_avg = movingAvg(VOLT_DDRS_arr, &VOLT_DDRS_sum, pos, len, VOLT_DDRS);
		VCC_PSINTFP_avg = movingAvg(VCC_PSINTFP_arr, &VCC_PSINTFP_sum, pos, len, VCC_PSINTFP);
		VCC_PS_FPD_avg = movingAvg(VCC_PS_FPD_arr, &VCC_PS_FPD_sum, pos, len, VCC_PS_FPD);
		PS_IO_BANK_500_avg = movingAvg(PS_IO_BANK_500_arr, &PS_IO_BANK_500_sum, pos, len, PS_IO_BANK_500);
		VCC_PS_GTR_avg = movingAvg(VCC_PS_GTR_arr, &VCC_PS_GTR_sum, pos, len, VCC_PS_GTR);
		VTT_PS_GTR_avg = movingAvg(VTT_PS_GTR_arr, &VTT_PS_GTR_sum, pos, len, VTT_PS_GTR);

		fprintf(fp_out, "AMS CTRL\n");
		fprintf(fp_out, "System PLLs voltage measurement, VCC_PSLL   		:     %ld mV\t\t %ld mV\n",VCC_PSPLL,VCC_PSPLL_avg);
		fprintf(fp_out, "PL internal voltage measurement, VCC_PSBATT 		:     %ld mV\t\t %ld mV\n",PL_VCCINT,PL_VCCINT_avg);
		fprintf(fp_out, "Voltage measurement for six DDR I/O PLLs, VCC_PSDDR_PLL :     %ld mV\t\t %ld mV\n",VOLT_DDRS,VOLT_DDRS_avg);
		fprintf(fp_out, "VCC_PSINTFP_DDR voltage measurement         		:     %ld mV\t\t %ld mV\n\n",VCC_PSINTFP,VCC_PSINTFP_avg);

		fprintf(fp_out, "PS Sysmon\n");
		fprintf(fp_out, "LPD temperature measurement 		    		:     %ld C\t\t %ld C\n",(LPD_TEMP)/1000,(LPD_TEMP_avg)/1000);
		fprintf(fp_out, "FPD temperature measurement (REMOTE)  		    	:     %ld C\t\t %ld C\n",(FPD_TEMP)/1000,(FPD_TEMP_avg)/1000);
		fprintf(fp_out, "VCC PS FPD voltage measurement (supply 2)   		:     %ld mV\t\t %ld mV\n",VCC_PS_FPD,VCC_PS_FPD_avg);
		fprintf(fp_out, "PS IO Bank 500 voltage measurement (supply 6)		:     %ld mV\t\t %ld mV\n",PS_IO_BANK_500,PS_IO_BANK_500_avg);
		fprintf(fp_out, "VCC PS GTR voltage   					:     %ld mV\t\t %ld mV\n",VCC_PS_GTR,VCC_PS_GTR_avg);
		fprintf(fp_out, "VTT PS GTR voltage    					:     %ld mV\t\t %ld mV\n\n",VTT_PS_GTR,VTT_PS_GTR_avg);

		fprintf(fp_out, "PL Sysmon\n");
		fprintf(fp_out, "PL temperature    					:     %ld C\t\t %ld C\n\n",(PL_TEMP)/1000,(PL_TEMP_avg)/1000);

	}
	else
	{
		fprintf(fp_out, "AMS CTRL\n");
		fprintf(fp_out, "System PLLs voltage measurement, VCC_PSLL   		:     %ld mV\n",VCC_PSPLL);
		fprintf(fp_out, "PL internal voltage measurement, VCC_PSBATT 		:     %ld mV\n",PL_VCCINT);
		fprintf(fp_out, "Voltage measurement for six DDR I/O PLLs, VCC_PSDDR_PLL :     %ld mV\n",VOLT_DDRS);
		fprintf(fp_out, "VCC_PSINTFP_DDR voltage measurement         		:     %ld mV\n\n",VCC_PSINTFP);

		fprintf(fp_out, "PS Sysmon\n");
		fprintf(fp_out, "LPD temperature measurement 		    		:     %ld C\n",(LPD_TEMP)/1000);
		fprintf(fp_out, "FPD temperature measurement (REMOTE)  		    	:     %ld C\n",(FPD_TEMP)/1000);
		fprintf(fp_out, "VCC PS FPD voltage measurement (supply 2)   		:     %ld mV\n",VCC_PS_FPD);
		fprintf(fp_out, "PS IO Bank 500 voltage measurement (supply 6)		:     %ld mV\n",PS_IO_BANK_500);
		fprintf(fp_out, "VCC PS GTR voltage   					:     %ld mV\n",VCC_PS_GTR);
		fprintf(fp_out, "VTT PS GTR voltage    					:     %ld mV\n\n",VTT_PS_GTR);

		fprintf(fp_out, "PL Sysmon\n");
		fprintf(fp_out, "PL temperature    					:     %ld C\n\n",(PL_TEMP)/1000);
	}

	return(0);
}
/*****************************************************************************/
/*
 *
 * This API prints the following information about power utilization for the system:
 *
 * @param        verbose_flag: Enable verbose prints
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int print_power_utilization(int verbose_flag, int sample_interval, int sample_window)
{
	int pos = 0;
	int len = pos+1;

	if(sample_interval > 0)
	{
		powerarr = (long*)calloc(sample_window,sizeof(long));
		currarr = (long*)calloc(sample_window,sizeof(long));
		volarr = (long*)calloc(sample_window,sizeof(long));

		LPD_TEMP_arr = (long*)calloc(sample_window,sizeof(long));
		FPD_TEMP_arr = (long*)calloc(sample_window,sizeof(long));
		PL_TEMP_arr = (long*)calloc(sample_window,sizeof(long));
		VCC_PSPLL_arr = (long*)calloc(sample_window,sizeof(long));
		PL_VCCINT_arr = (long*)calloc(sample_window,sizeof(long));
		VOLT_DDRS_arr = (long*)calloc(sample_window,sizeof(long));
		VCC_PSINTFP_arr = (long*)calloc(sample_window,sizeof(long));
		VCC_PS_FPD_arr = (long*)calloc(sample_window,sizeof(long));
		PS_IO_BANK_500_arr = (long*)calloc(sample_window,sizeof(long));
		VCC_PS_GTR_arr = (long*)calloc(sample_window,sizeof(long));
		VTT_PS_GTR_arr = (long*)calloc(sample_window,sizeof(long));

		signal(SIGINT, sigint_handler);
		fprintf(fp_out, "Calculating moving average of last %d samples taken at %d second intervals.\nPress Ctrl+C to exit.\n",sample_window,sample_interval);

		while(1)
		{
			print_ina260_power_info(verbose_flag, sample_interval, sample_window, pos, len);
			print_sysmon_power_info(verbose_flag, sample_interval, sample_window, pos, len);

			pos++;
			if(pos >= sample_window){
				pos = 0;
			}

			len++;
			if(len > sample_window){
				len = sample_window;
			}

			sleep(sample_interval);
		}
	}
	else
	{
		print_ina260_power_info(verbose_flag, sample_interval, sample_window, pos, len);
		print_sysmon_power_info(verbose_flag, sample_interval, sample_window, pos, len);
	}

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API calls all other APIs that read, compute and print different platform
 * stats
 *
 * @param        verbose_flag: Enable verbose prints on stdout
 * and printed
 *
 * @return       None.
 *
 * @note         None.
 *
 ******************************************************************************/
void print_all_stats(int verbose_flag, int sample_interval, int sample_window)
{

	print_cpu_utilization(verbose_flag);

	print_ram_memory_utilization(verbose_flag);

	print_swap_memory_utilization(verbose_flag);

	print_power_utilization(verbose_flag,sample_interval,sample_window);

	print_cma_utilization(verbose_flag);

	print_cpu_frequency(verbose_flag);
}

/*****************************************************************************/
/*
 *
 * This API gets the temperatures listed below.
 *
 * @param        LPD_TEMP: LPD temperature (millidegrees C).
 * @param        FPD_TEMP: FPD temperature (millidegrees C).
 * @param        PL_TEMP: PL temperature (millidegrees C).
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int get_temperatures(long* LPD_TEMP, long* FPD_TEMP, long* PL_TEMP)
{
	int hwmon_id;
	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(0,"ams");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ams under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/temp1_input", hwmon_id, LPD_TEMP);
	read_int_sysfs_entry(base_filename,"/temp2_input", hwmon_id, FPD_TEMP);
	read_int_sysfs_entry(base_filename,"/temp3_input", hwmon_id, PL_TEMP);

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API gets the voltages listed below.
 *
 * @param        VCC_PSPLL: System PLLs voltage (mV).
 * @param        PL_VCCINT: PL internal voltage (mV).
 * @param        VOLT_DDRS: Voltage measurement for six DDR I/O PLLs (mV).
 * @param        VCC_PSINTFP: VCC_PSINTFP_DDR voltage (mV).
 * @param        VCC_PS_FPD: VCC PS FPD voltage (mV).
 * @param        PS_IO_BANK_500: PS IO Bank 500 voltage (mV).
 * @param        VCC_PS_GTR: VCC PS GTR voltage (mV).
 * @param        VTT_PS_GTR: VTT PS GTR voltage (mV).
 * @param        total_voltage: Total voltage for ina260 (mV).
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int get_voltages(long* VCC_PSPLL, long* PL_VCCINT, long* VOLT_DDRS, long* VCC_PSINTFP, long* VCC_PS_FPD, long* PS_IO_BANK_500, long* VCC_PS_GTR, long* VTT_PS_GTR, long* total_voltage)
{
	int hwmon_id;
	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(0,"ams");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ams under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/in1_input", hwmon_id, VCC_PSPLL);
	read_int_sysfs_entry(base_filename,"/in3_input", hwmon_id, PL_VCCINT);
	read_int_sysfs_entry(base_filename,"/in6_input", hwmon_id, VOLT_DDRS);
	read_int_sysfs_entry(base_filename,"/in7_input", hwmon_id, VCC_PSINTFP);
	read_int_sysfs_entry(base_filename,"/in9_input", hwmon_id, VCC_PS_FPD);
	read_int_sysfs_entry(base_filename,"/in13_input", hwmon_id, PS_IO_BANK_500);
	read_int_sysfs_entry(base_filename,"/in16_input", hwmon_id, VCC_PS_GTR);
	read_int_sysfs_entry(base_filename,"/in17_input", hwmon_id, VTT_PS_GTR);

	hwmon_id = get_device_hwmon_id(0,"ina260_u14");

	if(hwmon_id == -1)
        {
                fprintf(fp_out, "no hwmon device found for ams under /sys/class/hwmon\n");
                return(0);
        }

	read_int_sysfs_entry(base_filename,"/in1_input", hwmon_id, total_voltage);

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API gets the total current for ina260.
 *
 * @param        total_current: Total current for ina260 (mA).
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int get_current(long* total_current)
{
	int hwmon_id;
	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(0,"ina260_u14");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ina260_u14 under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/curr1_input", hwmon_id, total_current);

	return(0);
}

/*****************************************************************************/
/*
 *
 * This API gets the total power for ina260.
 *
 * @param        total_power: Total power for ina260 (microwatts).
 *
 * @return       Error code.
 *
 * @note         None.
 *
 ******************************************************************************/
int get_power(long* total_power)
{
	int hwmon_id;
	char base_filename[MAX_FILENAME_LEN] = "/sys/class/hwmon/hwmon";

	hwmon_id = get_device_hwmon_id(0,"ina260_u14");

	if(hwmon_id == -1)
	{
		fprintf(fp_out, "no hwmon device found for ina260_u14 under /sys/class/hwmon\n");
		return(0);
	}

	read_int_sysfs_entry(base_filename,"/power1_input", hwmon_id, total_power);

	return(0);
}
