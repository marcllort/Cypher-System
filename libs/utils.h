#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <sys/wait.h>

#include "io.h"
#include "config.h"

#define IP_DELIM "."

int UTILS_compare(const char *str1, const char *str2, int size);
int UTILS_compareCaseInsensitive(const char *str1, const char *str2);
char **UTILS_str_split(char *a_str, const char a_delim);
int UTILS_valid_digit(char *ip_str);
int UTILS_ip_checker(char *ip_str);
int UTILS_sizeOf(const char *str);
int UTILS_printName(char *config);
void UTILS_removeChar(char *p, int ch);
int UTILS_isEmpty(const char *s);
char *UTILS_readKB();
int UTILS_fileExists(char *filename);
char *UTILS_md5(char *filename);
void UTILS_freeMemory();

#endif
