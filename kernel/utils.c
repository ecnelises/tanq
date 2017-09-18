#include "utils.h"
#include "versatilepb.h"

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
