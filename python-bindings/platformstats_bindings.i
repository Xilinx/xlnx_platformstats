/* File : platformstats.h */

%module pbindings
%include "typemaps.i"

%{
extern void skip_lines(FILE *fp, int numlines);
extern void get_sys_abs_path(char *filename, int id, char *filepath);
extern long movingAvg(long *arrNums, long *sum, int pos, int len, long nextNum);

extern struct cpustat {
        unsigned long user;
        unsigned long nice;
        unsigned long system;
        unsigned long idle;
        unsigned long iowait;
        unsigned long irq;
        unsigned long softirq;
        double total_util;
};

extern void print_all_stats(int verbose_flag, int rate, int duration);
extern int print_cpu_utilization(int verbose_flag);
extern double calculate_load(struct cpustat *prev, struct cpustat *curr);
extern int print_cpu_stats(struct cpustat *st, int cpu_id);
extern int get_stats(struct cpustat *st, int cpunum);
extern int print_ram_memory_utilization(int verbose_flag);
extern int print_swap_memory_utilization(int verbose_flag);
extern int print_power_utilization(int verbose_flag, int rate, int duration);
extern int print_ina260_power_info(int verbose_flag, int rate, int duration);
extern int count_hwmon_reg_devices();
extern int get_device_hwmon_id(int verbose_flag, char* name);
extern int print_cma_utilization(int verbose_flag);
extern int print_cpu_frequency(int verbose_flag);
%}

extern void skip_lines(FILE *fp, int numlines);
extern void get_sys_abs_path(char *filename, int id, char *filepath);
extern long movingAvg(long *arrNums, long *sum, int pos, int len, long nextNum);

extern struct cpustat {
        unsigned long user;
        unsigned long nice;
        unsigned long system;
        unsigned long idle;
        unsigned long iowait;
        unsigned long irq;
        unsigned long softirq;
        double total_util;
};

extern void print_all_stats(int verbose_flag, int rate, int duration);
extern int print_cpu_utilization(int verbose_flag);
extern double calculate_load(struct cpustat *prev, struct cpustat *curr);
extern int print_cpu_stats(struct cpustat *st, int cpu_id);
extern int get_stats(struct cpustat *st, int cpunum);
extern int print_ram_memory_utilization(int verbose_flag);
extern int print_swap_memory_utilization(int verbose_flag);
extern int print_power_utilization(int verbose_flag, int rate, int duration);
extern int print_ina260_power_info(int verbose_flag, int rate, int duration);
extern int count_hwmon_reg_devices();
extern int get_device_hwmon_id(int verbose_flag, char* name);
extern int print_cma_utilization(int verbose_flag);
extern int print_cpu_frequency(int verbose_flag);

/* Apply typemaps to make get_* functions usable in python */
%apply unsigned long *OUTPUT { unsigned long* MemTotal, unsigned long* MemFree, unsigned long* MemAvailable };
%apply unsigned long *OUTPUT { unsigned long* SwapTotal, unsigned long* SwapFree };
%apply unsigned long *OUTPUT { unsigned long* CmaTotal, unsigned long* CmaFree };
%apply float *OUTPUT { float* cpu_freq };

%inline %{
extern int get_ram_memory_utilization(unsigned long* MemTotal, unsigned long* MemFree, unsigned long* MemAvailable);
extern int get_swap_memory_utilization(unsigned long* SwapTotal, unsigned long* SwapFree);
extern int get_cma_utilization(unsigned long* CmaTotal, unsigned long* CmaFree);
extern int get_cpu_frequency(int cpu_id, float* cpu_freq);
%}
