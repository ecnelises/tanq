/*
 * string.h
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
size_t strlen(const char *s);

#endif
