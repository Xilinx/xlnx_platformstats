/******************************************************************************
 * Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

/************************** Function Prototypes  *****************************/
void skip_lines(FILE *fp, int numlines);
void get_sys_abs_path(char *filename, int id, char *filepath);
long movingAvg(long *arrNums, long *sum, int pos, int len, long nextNum);
