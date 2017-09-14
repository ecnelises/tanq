#ifndef DISPLAY_H
#define DISPLAY_H

/* 预设显示设置 */
#define SCR_QVGAP 0x00
#define SCR_QVGAL 0x01
#define SCR_QCIFP 0x02
#define SCR_VGA   0x03
#define SCR_SVGA  0x04
#define SCR_EQCIF 0x05
#define SCR_SQVGA 0x06

struct screen {
    volatile unsigned * base;
    unsigned width;
    unsigned height;
};

struct screen_point {
    unsigned x : 16;
    unsigned y : 16;
};

/* 颜色现在的格式是 0x00BBGGRR，平台默认小端序 */
#define COLOR_RGB(r, g, b) (((b) << 16) | ((g) << 8) | r)
#define COLOR_RED   0x000000FF
#define COLOR_BLUE  0x00FF0000
#define COLOR_GREEN 0x0000FF00
#define COLOR_WHITE 0x00FFFFFF
#define COLOR_BLACK 0x00000000

/* 初始化显示设置 */
void screen_init(struct screen * scr, int type);

/* 绘制像素点 */
#define screen_draw_point(scr, pt, clr) \
    (*((scr)->base + (pt).y * (scr)->width + (pt).x) = (clr))
#define screen_get_point(scr, pt) \
    (*((scr)->base + (pt).y * (scr)->width + (pt).x))

struct terminal {
    unsigned id;
    struct screen * board;
    struct screen_point lt;
    struct screen_point cursor;
    unsigned width;
    unsigned height;
    unsigned fg_color;
    unsigned bg_color;
};

void term_full(struct screen * scr, struct terminal * term);
void term_putchar(struct terminal * term, char ch);
void term_clear(struct terminal * term);

#endif /* DISPLAY_H */
