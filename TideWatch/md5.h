/*   
*   Copyright (c) 2008, ƮƮ����(kesalin@gmail.com)   
*   All rights reserved.   
*     
*   �ļ����ƣ�md5.h   
*   ժ    Ҫ��md5У��ͷ�ļ�
*     
*   ��ǰ�汾��1.1   
*   ��    �ߣ�ƮƮ����   
*   ������ڣ�2008/11/30
*/

#pragma once

void MD5String(char *dst, const char *src, int dataSize);
int MD5File(const char *szFileName);
