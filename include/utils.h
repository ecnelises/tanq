#ifndef UTILS_H
#define UTILS_H

#include "display.h"

void chbg(struct terminal *term, unsigned color);
void chfg(struct terminal *term, unsigned color);
/* 从串口输出调试信息 */
void bwputs(const char *str);

#endif /* UTILS_H */
