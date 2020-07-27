/*
 *   Copyright (c) 2008, 飘飘白云(kesalin@gmail.com)
 *   All rights reserved.
 *
 *   文件名称：md5.h
 *   摘    要：md5校验头文件
 *
 *   当前版本：1.1
 *   作    者：飘飘白云
 *   完成日期：2008/11/30
 */

#pragma once

void MD5String(char *dst, const char *src, int dataSize);
int MD5File(const char *szFileName);