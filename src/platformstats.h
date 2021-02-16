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
