/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdio.h>
#include <string.h>

#define MAX_STR_ID_LEN 50

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
