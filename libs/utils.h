#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "io.h"

#define IP_DELIM "."

int UTILS_compare(const char *str1, const char *str2, int size);
int UTILS_compareCaseInsensitive(const char *str1, const char* str2);
char** UTILS_str_split(char* a_str, const char a_delim);
int UTILS_valid_digit(char *ip_str);
int UTILS_ip_checker(char *ip_str) ;

#endif 
