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
void print_all_stats(int verbose_flag, int sample_interval, int sample_window);
int print_cpu_utilization(int verbose_flag);
double calculate_load(struct cpustat *prev, struct cpustat *curr);
int print_cpu_stats(struct cpustat *st, int cpu_id);
int get_stats(struct cpustat *st, int cpunum);
int get_cpu_stats(struct cpustat *cpu_stat);
void init();
void deinit();
double* get_cpu_utilization(size_t *len);

int print_ram_memory_utilization(int verbose_flag);
int get_ram_memory_utilization(unsigned long* MemTotal, unsigned long* MemFree, unsigned long* MemAvailable);

int print_swap_memory_utilization(int verbose_flag);
int get_swap_memory_utilization(unsigned long* SwapTotal, unsigned long* SwapFree);

int print_power_utilization(int verbose_flag, int sample_interval, int sample_window);
int print_ina260_power_info(int verbose_flag, int sample_interval, int sample_window, int pos, int len);
int print_sysmon_power_info(int verbose_flag, int sample_interval, int sample_window, int pos, int len);
int count_hwmon_reg_devices();
int get_device_hwmon_id(int verbose_flag, char* name);

int print_cma_utilization(int verbose_flag);
int get_cma_utilization(unsigned long* CmaTotal, unsigned long* CmaFree);

int print_cpu_frequency(int verbose_flag);
int get_cpu_frequency(int cpu_id, float* cpu_freq);

int get_temperatures(long* LPD_TEMP, long* FPD_TEMP, long* PL_TEMP);
int get_voltages(long* VCC_PSPLL, long* PL_VCCINT, long* VOLT_DDRS, long* VCC_PSINTFP, long* VCC_PS_FPD, long* PS_IO_BANK_500, long* VCC_PS_GTR, long* VTT_PS_GTR, long* total_voltage);
int get_current(long* total_current);
int get_power(long* total_power);
