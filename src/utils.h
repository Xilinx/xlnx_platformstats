/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdint.h>
#include <freeipmi/freeipmi.h>

typedef struct fruData {
	uint8_t frubuf[IPMI_FRU_AREA_SIZE_MAX];
	ssize_t frubuflen;
	uint8_t areabuf[IPMI_FRU_AREA_SIZE_MAX+1];
	unsigned int area_type;
	unsigned int area_length;
	ipmi_fru_field_t board_manufacturer;
	ipmi_fru_field_t board_product_name;
	ipmi_fru_field_t board_serial_number;
	ipmi_fru_field_t board_part_number;
	ipmi_fru_field_t board_fru_file_id;
	ipmi_fru_field_t board_custom_fields[64];
	uint8_t language_code;
	uint32_t mfg_date_time;
} fruDataType;

/************************** Function Prototypes  *****************************/
void skip_lines(FILE *fp, int numlines);
void get_sys_abs_path(char *filename, int id, char *filepath);
long movingAvg(long *arrNums, long *sum, int pos, int len, long nextNum);
void fru_init();
void fru_deinit();
void fru_read_file(const char *cc_fru_file_pattern, fruDataType *fru_data);
void fru_product_name(fruDataType *fru_data, char *product_name);
int is_kria_cc(fruDataType *fru_data);
