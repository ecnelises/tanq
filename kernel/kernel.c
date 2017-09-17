#include <stddef.h>
#include <stdint.h>

#include "versatilepb.h"
#include "asm.h"
#include "string.h"
#include "display.h"
#include "keyboard.h"

void *memcpy(void *dest, const void *src, size_t n) {
	char *d = dest;
	const char *s = src;
	size_t i;
	for(i = 0; i < n; i++) {
		d[i] = s[i];
	}
	return d;
}

int strcmp(const char* a, const char* b) {
	int r = 0;
	while(!r && *a && *b) r = (*a++) - (*b++);
	return (*a) - (*b);
}

size_t strlen(const char *s) {
	size_t r = 0;
	while(*s++) r++;
	return r;
}

void bwputs(char *s) {
	while(*s) {
        /* 如果串口传输已满就继续等待 */
		while(*(UART0 + UARTFR) & UARTFR_TXFF);

        /* 真正写入字符 */
		*UART0 = *s;
		s++;
	}
}

struct screen scr;
struct terminal term;

#define char_visible(c) ((c) >= 32 && (c) <= 127)

void handler(unsigned k)
{
    char realchar = k & 0xFF;
    if (k & LSHIFT_MASK || k & RSHIFT_MASK) {
        if (realchar >= 'a' && realchar <= 'z') {
            realchar += ('A' - 'a');
        } else {
            switch (realchar) {
            case '`': realchar = '~'; break;
            case '1': realchar = '!'; break;
            case '2': realchar = '@'; break;
            case '3': realchar = '#'; break;
            case '4': realchar = '$'; break;
            case '5': realchar = '%'; break;
            case '6': realchar = '^'; break;
            case '7': realchar = '&'; break;
            case '8': realchar = '*'; break;
            case '9': realchar = '('; break;
            case '0': realchar = ')'; break;
            case '-': realchar = '_'; break;
            case '=': realchar = '+'; break;
            case '[': realchar = '{'; break;
            case ']': realchar = '}'; break;
            case ';': realchar = ';'; break;
            case '\'': realchar = '\"'; break;
            case ',': realchar = '<'; break;
            case '.': realchar = '>'; break;
            case '/': realchar = '?'; break;
            case '\\': realchar = '|'; break;
            default: break;
            }
        }
    }
    term_putchar(&term, realchar);
}

int main(void)
{
    *(volatile unsigned int *)(0x10006000) = 0x00000013;
    unsigned int k = *(volatile unsigned int *)(0x10006004);
    char buf[] = {0, 0};
    buf[0] = ((k & 0x20) >> 6) + '0';
    bwputs(buf);
    *(volatile unsigned int *)(0x10003008) = 0x08;

    screen_init(&scr, SCR_SVGA);
    term_full(&scr, &term);
    for (unsigned i = 32; i < 128; ++i) {
        term_putchar(&term, i);
    }
    for (unsigned i = 32; i < 128; ++i) {
        term_putchar(&term, i);
    }

    /* Set up MMC connection */
    *(volatile unsigned *)(0x10005000) = 0x03;

    for (;;) {
        if (*(volatile unsigned int *)(0x10003000) & 0x08) {
            while (*(volatile unsigned int *)(0x10006004) & 0x8);
            if (*(volatile unsigned int *)(0x10006004) & 0x10) {
                k = *(volatile unsigned int *)(0x10006008) & 0xFF;
                handle_key(k, handler);
                *(volatile unsigned int *)(0x1000300C) = 0x08;
                *(volatile unsigned int *)(0x10003008) = 0x08;
            }
        }
    }

	return 0;
}
