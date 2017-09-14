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

void handler(unsigned k)
{
    // if (k & LCTRL_MASK || k & RCTRL_MASK) {
    //     term_putchar(&term, 'C');
    // }
    // if (k & LALT_MASK || k & RALT_MASK) {
    //     term_putchar(&term, 'A');
    // }
    // if (k & LSHIFT_MASK || k & RSHIFT_MASK) {
    //     term_putchar(&term, 'S');
    // }
    // if (k & LGUI_MASK || k & RGUI_MASK) {
    //     term_putchar(&term, 'G');
    // }
    // term_putchar(&term, k & 0xFF);
    term_up(&term);
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
