/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files *********************************/
#include <stdio.h>

/************************** Function Definitions *****************************/
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


