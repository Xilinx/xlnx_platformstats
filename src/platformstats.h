/******************************************************************************
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

struct cpustat {
        unsigned long user;
        unsigned long nice;
        unsigned long system;
        unsigned long idle;
        unsigned long iowait;
        unsigned long irq;
        unsigned long softirq;
        double total_util;
};

/************************** Function Prototypes  *****************************/
void print_all_stats(int verbose_flag,char *filename, int interval);
int print_cpu_utilization(int verbose_flag, char*filename);
double calculate_load(struct cpustat *prev, struct cpustat *curr);
int print_cpu_stats(char* filename,struct cpustat *st, int cpu_id);
int get_stats(struct cpustat *st, int cpunum);

int print_ram_memory_utilization(int verbose_flag, char* filename);
int get_ram_memory_utilization(unsigned long* MemTotal, unsigned long* MemFree, unsigned long* MemAvailable);

int print_swap_memory_utilization(int verbose_flag, char* filename);
int get_swap_memory_utilization(unsigned long* SwapTotal, unsigned long* SwapFree);

int print_power_utilization(int verbose_flag, char* filename);
int print_ina260_power_info(int verbose_flag);
int count_hwmon_reg_devices();
int get_device_hwmon_id(int verbose_flag, char* name);
int read_sysfs_entry(char* filename, char* entry);
