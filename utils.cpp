/*
 * utils.cpp
 *
 *  Created on: 28 февр. 2020 г.
 *      Author: hudienko_a
 */

#include"Utils.h"
#include<string.h>

#define ALPHANUMS "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

int _mitoa(int number, char* out, int base)
{
	int t, count;
	char *p, *q;
	char c;

	p = q = out;
	if (base < 2 || base > 36)
		base = 10;

	do
	{
		t = number;
		number /= base;
		if (out)
			*p = ALPHANUMS[t + 35 - number * base];
		p++;
	} while (number);

	if (t < 0)
	{
		if (out)
			*p = '-';
		p++;
	}
	count = p - out;
	if (out)
	{
		*p-- = '\0';
		while (q < p)
		{
			c = *p;
			*p-- = *q;
			*q++ = c;
		}
	}
	return count;
}

int _deletechar(char* data, char simbol)
{
	char tmp[256];
	int length = strlen(data);
	if (length <= 0)
		return 0;

	int count = 0;
	for (int i = 0; i < length; i++) {
		if (data[i] == simbol)
			count++;
	}

	if (count > 0) {
		memcpy(tmp, data,  length);
		int l = strlen(data);
		memset(data,0,l);

		int index = 0;
		for (int i = 0; i < length; i++) {
			if (tmp[i] != simbol)
			{
				data[index++] = tmp[i];
			}
		}

		length = index ;
		data[length]=0;
		return length;
	}
	return 0;
}

int indexOf(char* value, char data ,unsigned int len)
{
	for (unsigned int i = 0; i < len; i++)
	{
		if (value[i] == data)return i;
	}
	return 0;
}

int indexOf_(char* value,char data, int start,unsigned int len)
{
	for (unsigned int i = start; i < len; i++)
	{
		if (value[i] == data)return i;
	}
	return 0;
}

int preSplit(char* source ,char data)
{
	unsigned int length_src = strlen(source);
	int length =0;
	for (unsigned int i = 0; i < length_src; i++)
	{
		if (source[i] == data)length++;
	}
	length++;
	return length;
}

bool split_field(char* source, int sourcelength, char* field, char* value )
{
	if(sourcelength<=0) return false;
	int split = indexOf(source, ':',sourcelength);

	if(split>0)
	{
		memcpy(field,source+1,split-2);
		if(source[split+1]=='"')
		{
			memcpy(value,source+split+2,sourcelength-2);
			int st = strlen(value);
			value[st-1]=0;
		}
		else
		{
			if(source[sourcelength-1]=='}')
			{
				memcpy(value,source+split+1,sourcelength-1);
				int st = strlen(value);
				value[st-1]=0;
			}
			else
			{
				memcpy(value,source+split+1,sourcelength);
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

bool _strcmp(char* str1, char* str2)
{
	if (strlen(str1) != strlen(str2)) return false;
	for (int i = 0; i < (signed)strlen(str1); i++) {
		if (str1[i] != str2[i]) {
			return false;
		}
	}
	return true;
}




