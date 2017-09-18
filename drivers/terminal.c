#include "display.h"

/* 字体从空格开始 */
#define FONTTBL_OFFSET 32u
#define FONT_WIDTH 11u
#define FONT_HEIGHT 20u
#define FONT_REAL_HEIGHT 16u
#define FONT_HEIGHT_PADDING (FONT_HEIGHT - FONT_REAL_HEIGHT)

static unsigned short font11x16[][11] = {
    { 0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x003E, 0x33FF, 0x33FF, 0x003E, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0007, 0x0007, 0x0000, 0x0000, 0x0007, 0x0007, 0x0000, 0x0000, 0 },
    { 0, 0x0120, 0x1FFE, 0x1FFE, 0x0120, 0x0120, 0x1FFE, 0x1FFE, 0x0120, 0x0000, 0 },
    { 0, 0x0638, 0x0E3C, 0x0864, 0x3FFF, 0x3FFF, 0x0984, 0x0F1C, 0x0718, 0x0000, 0 },
    { 0, 0x301E, 0x1C33, 0x071E, 0x01C0, 0x0070, 0x1E1C, 0x3307, 0x1E01, 0x0000, 0 },
    { 0, 0x1F1E, 0x31B3, 0x20E1, 0x20E1, 0x31B3, 0x1F1E, 0x1B00, 0x3180, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0x000B, 0x0007, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x07F0, 0x3FFE, 0x780F, 0x4001, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x4001, 0x780F, 0x3FFE, 0x07F0, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0080, 0x02A0, 0x03E0, 0x01C0, 0x01C0, 0x03E0, 0x02A0, 0x0080, 0x0000, 0 },
    { 0, 0x0080, 0x0080, 0x0080, 0x03E0, 0x03E0, 0x0080, 0x0080, 0x0080, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0xB000, 0x7000, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x3000, 0x3C00, 0x0F00, 0x03C0, 0x00F0, 0x003C, 0x000F, 0x0003, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3603, 0x2181, 0x2061, 0x301B, 0x1FFE, 0x0FFC, 0x0000, 0 },
    { 0, 0x2004, 0x2004, 0x2006, 0x3FFF, 0x3FFF, 0x2000, 0x2000, 0x2000, 0x0000, 0 },
    { 0, 0x380C, 0x3C0E, 0x2603, 0x2301, 0x2181, 0x20C3, 0x207E, 0x203C, 0x0000, 0 },
    { 0, 0x0C0C, 0x1C0E, 0x3043, 0x2041, 0x2041, 0x3043, 0x1FFE, 0x0FBC, 0x0000, 0 },
    { 0, 0x0F00, 0x0FE0, 0x08FC, 0x081F, 0x3F83, 0x3F80, 0x0800, 0x0800, 0x0000, 0 },
    { 0, 0x0C3F, 0x1C3F, 0x3021, 0x2021, 0x2021, 0x3061, 0x1FC1, 0x0F81, 0x0000, 0 },
    { 0, 0x0FE0, 0x1FF8, 0x305C, 0x2046, 0x2043, 0x30C1, 0x1F81, 0x0F01, 0x0000, 0 },
    { 0, 0x0001, 0x0001, 0x0001, 0x3C01, 0x3F81, 0x03F1, 0x007F, 0x000F, 0x0000, 0 },
    { 0, 0x0F1C, 0x1FBE, 0x30E3, 0x2041, 0x2041, 0x30E3, 0x1FBE, 0x0F1C, 0x0000, 0 },
    { 0, 0x203C, 0x207E, 0x20C3, 0x3081, 0x1881, 0x0E83, 0x07FE, 0x01FC, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0x3060, 0x3060, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0xB060, 0x7060, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0080, 0x01C0, 0x0360, 0x0630, 0x0C18, 0x180C, 0x1004, 0x0000, 0x0000, 0 },
    { 0, 0x0120, 0x0120, 0x0120, 0x0120, 0x0120, 0x0120, 0x0120, 0x0120, 0x0000, 0 },
    { 0, 0x1004, 0x180C, 0x0C18, 0x0630, 0x0360, 0x01C0, 0x0080, 0x0000, 0x0000, 0 },
    { 0, 0x0006, 0x0007, 0x0003, 0x3301, 0x33C1, 0x00F3, 0x003F, 0x001E, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3003, 0x21E1, 0x23F1, 0x2211, 0x2113, 0x33FE, 0x13FC, 0 },
    { 0, 0x3FFC, 0x3FFE, 0x0103, 0x0101, 0x0101, 0x0103, 0x3FFE, 0x3FFC, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x2041, 0x2041, 0x2041, 0x30E3, 0x1FBE, 0x0F1C, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3003, 0x2001, 0x2001, 0x3003, 0x1C0E, 0x0C0C, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x2001, 0x2001, 0x2001, 0x3807, 0x1FFE, 0x07F8, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x2041, 0x2041, 0x2041, 0x2041, 0x2041, 0x2001, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0001, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3003, 0x2001, 0x2081, 0x3083, 0x1F8E, 0x0F8C, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0040, 0x0040, 0x0040, 0x0040, 0x3FFF, 0x3FFF, 0x0000, 0 },
    { 0, 0x2001, 0x2001, 0x2001, 0x3FFF, 0x3FFF, 0x2001, 0x2001, 0x2001, 0x0000, 0 },
    { 0, 0x0C00, 0x1C00, 0x3000, 0x2000, 0x2000, 0x3000, 0x1FFF, 0x0FFF, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x00C0, 0x01E0, 0x0738, 0x0E1C, 0x3C0F, 0x3003, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0078, 0x03C0, 0x03C0, 0x0078, 0x3FFF, 0x3FFF, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x001C, 0x0070, 0x01C0, 0x0700, 0x3FFF, 0x3FFF, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3003, 0x2001, 0x2001, 0x3003, 0x1FFE, 0x0FFC, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0081, 0x0081, 0x0081, 0x00C3, 0x007E, 0x003C, 0x0000, 0 },
    { 0, 0x0FFC, 0x1FFE, 0x3003, 0x2001, 0x6001, 0xF003, 0x9FFE, 0x0FFC, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0081, 0x0081, 0x0181, 0x1FC3, 0x3E7E, 0x203C, 0x0000, 0 },
    { 0, 0x0C3C, 0x1C7E, 0x3063, 0x20E1, 0x21C1, 0x3183, 0x1F8E, 0x0F0C, 0x0000, 0 },
    { 0, 0x0001, 0x0001, 0x0001, 0x3FFF, 0x3FFF, 0x0001, 0x0001, 0x0001, 0x0000, 0 },
    { 0, 0x0FFF, 0x1FFF, 0x3000, 0x2000, 0x2000, 0x3000, 0x1FFF, 0x0FFF, 0x0000, 0 },
    { 0, 0x003F, 0x01FF, 0x0FC0, 0x3C00, 0x3C00, 0x0FC0, 0x01FF, 0x003F, 0x0000, 0 },
    { 0, 0x01FF, 0x3FFF, 0x3E00, 0x01C0, 0x01C0, 0x3E00, 0x3FFF, 0x01FF, 0x0000, 0 },
    { 0, 0x3C0F, 0x3F3F, 0x03F0, 0x00C0, 0x00C0, 0x03F0, 0x3F3F, 0x3C0F, 0x0000, 0 },
    { 0, 0x003F, 0x00FF, 0x01C0, 0x3F80, 0x3F80, 0x01C0, 0x00FF, 0x003F, 0x0000, 0 },
    { 0, 0x3801, 0x3E01, 0x2781, 0x21C1, 0x2061, 0x2039, 0x201F, 0x2007, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x3FFF, 0x3FFF, 0x2001, 0x2001, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0003, 0x000F, 0x003C, 0x00F0, 0x03C0, 0x0F00, 0x3C00, 0x3000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x2001, 0x2001, 0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0004, 0x0004, 0x0006, 0x0003, 0x0003, 0x0006, 0x0006, 0x0004, 0x0000, 0 },
    { 0, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0x0007, 0x000B, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x1E00, 0x3F40, 0x2120, 0x2120, 0x2120, 0x1120, 0x3FE0, 0x3FC0, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x1040, 0x2020, 0x2020, 0x3060, 0x1FC0, 0x0F80, 0x0000, 0 },
    { 0, 0x0F80, 0x1FC0, 0x3060, 0x2020, 0x2020, 0x2020, 0x3060, 0x1040, 0x0000, 0 },
    { 0, 0x0F80, 0x1FC0, 0x3060, 0x2020, 0x2020, 0x1040, 0x3FFF, 0x3FFF, 0x0000, 0 },
    { 0, 0x0F80, 0x1FC0, 0x3260, 0x2220, 0x2220, 0x2260, 0x33C0, 0x1380, 0x0000, 0 },
    { 0, 0x0020, 0x0020, 0x3FFE, 0x3FFF, 0x0021, 0x0021, 0x0023, 0x0002, 0x0000, 0 },
    { 0, 0x4780, 0x4FC0, 0xD860, 0x9020, 0x9020, 0xC840, 0x7FE0, 0x3FE0, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0040, 0x0020, 0x0020, 0x0060, 0x3FC0, 0x3F80, 0x0000, 0 },
    { 0, 0x0000, 0x2020, 0x2020, 0x3FE3, 0x3FE3, 0x2000, 0x2000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x4000, 0xC020, 0x8020, 0xFFE3, 0x7FE3, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x3FFF, 0x3FFF, 0x0600, 0x0700, 0x0D80, 0x18C0, 0x3060, 0x2020, 0x0000, 0 },
    { 0, 0x0000, 0x2001, 0x2001, 0x3FFF, 0x3FFF, 0x2000, 0x2000, 0x0000, 0x0000, 0 },
    { 0, 0x3FE0, 0x3FE0, 0x0060, 0x1FC0, 0x1FC0, 0x0060, 0x3FE0, 0x3FC0, 0x0000, 0 },
    { 0, 0x3FE0, 0x3FE0, 0x0040, 0x0020, 0x0020, 0x0060, 0x3FC0, 0x3F80, 0x0000, 0 },
    { 0, 0x0F80, 0x1FC0, 0x3060, 0x2020, 0x2020, 0x3060, 0x1FC0, 0x0F80, 0x0000, 0 },
    { 0, 0xFFE0, 0xFFE0, 0x1040, 0x2020, 0x2020, 0x3060, 0x1FC0, 0x0F80, 0x0000, 0 },
    { 0, 0x0F80, 0x1FC0, 0x2020, 0x2020, 0x2020, 0x1040, 0xFFE0, 0xFFE0, 0x0000, 0 },
    { 0, 0x3FE0, 0x3FE0, 0x00C0, 0x0040, 0x0060, 0x0060, 0x0060, 0x0040, 0x0000, 0 },
    { 0, 0x10C0, 0x31E0, 0x2120, 0x2320, 0x2620, 0x2420, 0x3C60, 0x1840, 0x0000, 0 },
    { 0, 0x0000, 0x0020, 0x0020, 0x1FFE, 0x3FFE, 0x2020, 0x2020, 0x0000, 0x0000, 0 },
    { 0, 0x1FE0, 0x3FE0, 0x2000, 0x2000, 0x2000, 0x1000, 0x3FE0, 0x3FE0, 0x0000, 0 },
    { 0, 0x03E0, 0x07E0, 0x1C00, 0x3000, 0x3000, 0x1C00, 0x07E0, 0x03E0, 0x0000, 0 },
    { 0, 0x07E0, 0x3FE0, 0x3800, 0x07C0, 0x07C0, 0x3800, 0x3FE0, 0x07E0, 0x0000, 0 },
    { 0, 0x3060, 0x3DE0, 0x0D80, 0x0700, 0x0700, 0x0D80, 0x3DE0, 0x3060, 0x0000, 0 },
    { 0, 0x83E0, 0x8FE0, 0xDC00, 0x7000, 0x3000, 0x1C00, 0x0FE0, 0x03E0, 0x0000, 0 },
    { 0, 0x3020, 0x3820, 0x2C20, 0x2620, 0x2320, 0x21A0, 0x20E0, 0x2060, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0080, 0x3FFE, 0x7F7F, 0x4001, 0x4001, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x0000, 0x0000, 0xFF7F, 0xFF7F, 0x0000, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0000, 0x4001, 0x4001, 0x7F7F, 0x3FFE, 0x0080, 0x0000, 0x0000, 0x0000, 0 },
    { 0, 0x0002, 0x0002, 0x0001, 0x0001, 0x0002, 0x0002, 0x0001, 0x0001, 0x0000, 0 },
};

static unsigned term_id_counter = 0;

void term_full(struct screen * scr, struct terminal * term)
{
    term->id = ++term_id_counter;
    term->board = scr;
    term->lt = term->cursor = (struct screen_point) {0, 0};
    term->width = scr->width;
    term->height = scr->height;
    term->fg_color = COLOR_RGB(14, 14, 14);
    term->bg_color = COLOR_WHITE;
    struct screen_point pt;
    for (unsigned i = 0; i < term->width; ++i) {
        pt.x = i + (term->lt).x;
        for (unsigned j = 0; j < term->height; ++j) {
            pt.y = j + (term->lt).y;
            screen_draw_point(term->board, pt, term->bg_color);
        }
    }
}

void term_putchar(struct terminal * term, char ch)
{
    unsigned rm = (term->lt).x + term->width;
    if (ch == '\n') {
        if ((term->cursor).y + 2 * FONT_HEIGHT >
                (term->lt).y + term->height) {
            term_up(term);
        } else {
            (term->cursor).y += FONT_HEIGHT;
        }
        (term->cursor).x = (term->lt).x;
        return;
    } else if (ch == '\b') {
        term_unput(term);
        return;
    }
    if ((term->cursor).x + FONT_WIDTH > rm) {
        if ((term->cursor).y + 2 * FONT_HEIGHT >
                (term->lt).y + term->height) {
            term_up(term);
        } else {
            (term->cursor).y += FONT_HEIGHT;
        }
        (term->cursor).x = (term->lt).x;
    }
    struct screen_point pt;
    for (unsigned j = 0; j < FONT_WIDTH; ++j) {
        pt.x = j + (term->cursor).x + (term->lt).x;
        for (unsigned i = 0; i < FONT_REAL_HEIGHT; ++i) {
            pt.y = FONT_HEIGHT_PADDING + i + (term->cursor).y + (term->lt).y;
            if (font11x16[ch - FONTTBL_OFFSET][j] & (1 << i)) {
                screen_draw_point(term->board, pt, term->fg_color);
            }
        }
    }
    (term->cursor).x += FONT_WIDTH;
}

void term_clear(struct terminal * term)
{
    struct screen_point pt;
    for (unsigned i = 0; i < term->width; ++i) {
        pt.x = i + (term->lt).x;
        for (unsigned j = 0; j < term->height; ++j) {
            pt.y = j + (term->lt).y;
            screen_draw_point(term->board, pt, term->bg_color);
        }
    }
    term->cursor = term->lt;
}

void term_up(struct terminal * term)
{
    if (term->height <= FONT_HEIGHT) {
        term_clear(term);
        return;
    }
    struct screen_point pt, dest;
    for (unsigned i = 0; i < term->width; ++i) {
        pt.x = i + (term->lt).x;
        dest.x = pt.x;
        for (unsigned j = 0; j < term->height - FONT_HEIGHT; ++j) {
            pt.y = j + (term->lt).y;
            dest.y = pt.y + FONT_HEIGHT;
            screen_draw_point(term->board, pt, screen_get_point(term->board, dest));
        }
        for (unsigned j = term->height - FONT_HEIGHT; j < term->height; ++j) {
            pt.y = j + (term->lt).y;
            screen_draw_point(term->board, pt, term->bg_color);
        }
    }
}

void term_unput(struct terminal * term)
{
    struct screen_point pt;
    if ((term->cursor).x <= FONT_WIDTH) {
        if ((term->cursor).y <= (term->lt).y) {
            return;
        } else {
            for (unsigned j = 0; j < FONT_WIDTH; ++j) {
                pt.x = j + (term->lt).x;
                for (unsigned i = 0; i < FONT_HEIGHT; ++i) {
                    pt.y = i + (term->cursor).y + (term->lt).y;
                    screen_draw_point(term->board, pt, term->bg_color);
                }
            }
            (term->cursor).y -= FONT_HEIGHT;
            (term->cursor).x =
                (term->lt).x + term->width - (term->width % FONT_WIDTH);
            return;
        }
    }
    (term->cursor).x -= FONT_WIDTH;
    for (unsigned j = 0; j < FONT_WIDTH; ++j) {
        pt.x = j + (term->lt).x + (term->cursor).x;
        for (unsigned i = 0; i < FONT_HEIGHT; ++i) {
            pt.y = i + (term->cursor).y + (term->lt).y;
            screen_draw_point(term->board, pt, term->bg_color);
        }
    }
}

void term_print(struct terminal * term, const char *str)
{
    while (*str) {
        term_putchar(term, *str++);
    }
}
