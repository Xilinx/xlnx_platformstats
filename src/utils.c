/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <sys/stat.h>
#include <freeipmi/freeipmi.h>
#include "utils.h"

#define MAX_STR_ID_LEN 50

ipmi_ctx_t ipmi_ctx;
ipmi_fru_ctx_t fru_ctx;

/************************** Function Definitions *****************************/
/*****************************************************************************/
/*
 *
 * This API computes moving average for a stream of data
 *
 * @param	arrNums: contains stream of data sum
 * @param	sum: total sum of stream of data
 * @param	pos: count of total data in arrNums
 * @param	len: total length of stream of data
 * @param	nextNum: next data in stream
 *
 * @return	moving average of arrNums
 *
 * @note		Internal API only.
 *
 ******************************************************************************/
long movingAvg(long *arrNums, long *sum, int pos, int len, long nextNum)
{
	*sum = *sum - arrNums[pos] + nextNum;
	arrNums[pos] = nextNum;

	return (*sum)/len;

}
/*****************************************************************************/
/*
 *
 * This API skips N number of lines for a file
 *
 * @param	fp: File pointer of the file
 * @param	numlines: number of lines to skip
 *
 * @return	None.
 *
 * @note		Internal API only.
 *
 ******************************************************************************/
void skip_lines(FILE *fp, int numlines)
{
	int cnt;
	char ch;

	cnt = 0;

	while((cnt < numlines) && ((ch = getc(fp)) != EOF))
	{
		if (ch == '\n')
			cnt++;
	}
}

//Function to compute sysfys file entry and return the final file to read from
void get_sys_abs_path(char *filename, int id, char *filepath)
{
	char str_id[MAX_STR_ID_LEN];

	sprintf(str_id,"%d",id);
	strcat(filename,str_id);
	strcat(filename,filepath);
}

// Function to init fru context
void fru_init()
{
	ipmi_ctx_t ipmi_ctx = NULL;
	fru_ctx = ipmi_fru_ctx_create(ipmi_ctx);
}

// Function to cleanup fru context
void fru_deinit()
{
	ipmi_fru_ctx_destroy(fru_ctx);
	ipmi_ctx_destroy(ipmi_ctx);
}

// Function to read from fru file
void fru_read_file(const char *fru_file_pattern, fruDataType *fru_data)
{
	int fd = -1;
	glob_t globlist;
	glob(fru_file_pattern, GLOB_NOCHECK, 0, &globlist);

	// Check if FRU file was found, glob returns original pattern if not found
	if(strcmp(fru_file_pattern, globlist.gl_pathv[0]) == 0)
	{
		return;
	}

	struct stat sbuf;
	stat(globlist.gl_pathv[0], &sbuf);
	fd = open(globlist.gl_pathv[0], O_RDONLY);
	fru_data->frubuflen = read(fd, fru_data->frubuf, sbuf.st_size);
	close(fd);
	ipmi_fru_open_device_id_with_buffer(fru_ctx, fru_data->frubuf, fru_data->frubuflen);

	fru_data->area_type = 1;
	fru_data->area_length = 0;
	memset(fru_data->areabuf, '\0', IPMI_FRU_AREA_SIZE_MAX + 1);
	ipmi_fru_read_data_area(fru_ctx,
				&(fru_data->area_type),
				&(fru_data->area_length),
				fru_data->areabuf,
				IPMI_FRU_AREA_SIZE_MAX);
}

// Function to get the Board Product Name field
void fru_product_name(fruDataType *fru_data, char *product_name)
{
	unsigned int product_name_len = IPMI_FRU_AREA_STRING_MAX;

	ipmi_fru_board_info_area(fru_ctx,
				 fru_data->areabuf,
				 fru_data->area_length,
				 &(fru_data->language_code),
				 &(fru_data->mfg_date_time),
				 &(fru_data->board_manufacturer),
				 &(fru_data->board_product_name),
				 &(fru_data->board_serial_number),
				 &(fru_data->board_part_number),
				 &(fru_data->board_fru_file_id),
				 fru_data->board_custom_fields,
				 64);

	memset(product_name, '\0', IPMI_FRU_AREA_STRING_MAX + 1);
	ipmi_fru_type_length_field_to_string(fru_ctx,
					     fru_data->board_product_name.type_length_field,
					     fru_data->board_product_name.type_length_field_length,
					     fru_data->language_code,
					     product_name,
					     &product_name_len);
}

// Function to check if the carrier card is a KV, KR, or KD
int is_kria_cc(fruDataType *fru_data)
{
	char product_name[IPMI_FRU_AREA_STRING_MAX + 1];
	fru_product_name(fru_data, product_name);

	// Check for KV/KR/KD
	if(strcmp(product_name, "") != 0)
	{
		char *carrier_card;
		carrier_card = strtok(product_name, "-");
		carrier_card = strtok(NULL, "-");
		if(carrier_card != NULL)
		{
			if((strcmp(carrier_card, "KV") == 0) ||
			   (strcmp(carrier_card, "KR") == 0) ||
			   (strcmp(carrier_card, "KD") == 0))
			{
				return 1;
			}
		}
	}
	return 0;
}
