#include "libc/string.h"

size_t strlen(const char *str)
{
    size_t len = 0;
    while (*str++) {
        ++len;
    }
    return len;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;
	size_t i;
	for(i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return d;
}

int strcmp(const char *str1, const char *str2)
{
    for (unsigned i = 0; ; i++) {
        if (str1[i] != str2[i]) {
            return str1[i] < str2[i] ? -1 : 1;
        }
        if (str1[i] == '\0') {
            return 0;
        }
    }
}

int strncmp(const char *str1, const char *str2, size_t n)
{
    for (unsigned i = 0; i < n; ++i) {
        if (str1[i] != str2[i]) {
            return str1[i] < str2[i] ? -1 : 1;
        }
        if (str1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    for (unsigned i = 0; i < n; ++i) {
        dest[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    return dest;
}
