#include "utils.h"
#include "versatilepb.h"
#include "interrupt.h"
#include <stdbool.h>

unsigned ms_since_init = 0;

void chbg(struct terminal *term, unsigned color)
{
    unsigned previous_bg = term->bg_color;
    struct screen_point pt;
    for (unsigned i = 0; i < term->width; ++i) {
        pt.x = (term->lt).x + i;
        for (unsigned j = 0; j < term->height; ++j) {
            pt.y = (term->lt).y + j;
            if (screen_get_point(term->board, pt) == previous_bg) {
                screen_draw_point(term->board, pt, color);
            }
        }
    }
    term->bg_color = color;
}

void chfg(struct terminal *term, unsigned color)
{
    unsigned previous_fg = term->fg_color;
    struct screen_point pt;
    for (unsigned i = 0; i < term->width; ++i) {
        pt.x = (term->lt).x + i;
        for (unsigned j = 0; j < term->height; ++j) {
            pt.y = (term->lt).y + j;
            if (screen_get_point(term->board, pt) == previous_fg) {
                screen_draw_point(term->board, pt, color);
            }
        }
    }
    term->fg_color = color;
}

void bwputs(const char *str)
{
	while(*str) {
        /* 如果串口传输已满就继续等待 */
		while(*(UART0 + UARTFR) & UARTFR_TXFF);

        /* 真正写入字符 */
		*UART0 = *str;
		str++;
	}
}

static void reverse(char *str, int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        char tmp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = tmp;
        start++;
        end--;
    }
}

char *itoa(int num, char *str)
{
    int i = 0;
    bool negative = false;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    if (num < 0) {
        negative = true;
        num = -num;
    }
    while (num != 0) {
        int rem = num % 10;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / 10;
    }
    if (negative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    reverse(str, i);
    return str;
}

void sleep(unsigned ms)
{
    PIC_ENSET |= 0x10;
    TIMER0_DATA = ms * 1000;
    TIMER0_CTRL = 0xA3;
    for (;;) {
        if (PIC_INTERRUPT_STATUS & 0x10) {
            TIMER0_CLR = 1;
            break;
        }
    }
}

void bwputi(int val)
{
    char buf[12];
    itoa(val, buf);
    bwputs(buf);
}

const char *next_nonblank(const char *str)
{
    while (*str && *str == ' ') {
        ++str;
    }
    return str;
}

int atoi(const char *buf)
{
    int value = 0;
    int sign = 1;
    if (*buf == '+' || *buf == '-') {
        if (*buf == '-') {
            sign = -1;
        }
        ++buf;
    }
    while (*buf <= '9' && *buf >= '0') {
        value *= 10;
        value += (int)(*buf - '0');
        ++buf;
    }
    return (value * sign);
}
