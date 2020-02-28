/*
 * Utils.h
 *
 *  Created on: 11 θώνÿ 2015 γ.
 *      Author: hudienko_a
 */

#pragma once

int _mitoa(int number, char* out, int base);


int _deletechar(char* data, char simbol);

int indexOf(char* value, char data ,unsigned int len);

int indexOf_(char* value,char data, int start,unsigned int len);

int preSplit(char* source ,char data);

bool split_field(char* source, int sourcelength, char* field, char* value );

bool _strcmp(char* str1, char* str2);
