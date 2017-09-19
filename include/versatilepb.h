/*
 * versatilepb.h
 */

#ifndef VERSATILEPB_H
#define VERSATILEPB_H

/*
 * 这里的 UART 可以看作是 ARM 的串口设备
 * 根据 Versatile 开发板的文档，UART0 寄存器的地址即 0x101f1000
 * 多数 ARM 模拟器都会将串口映射到命令行终端上
 * 此处将串口地址作为输出目的地，仅是为了调试，实际设备上并无意义
 */
#define UART0 ((volatile unsigned int*)0x101F1000)
#define UART1 ((volatile unsigned int*)0x101F2000)
#define UART2 ((volatile unsigned int*)0x101F3000)
#define UART3 ((volatile unsigned int*)0x10009000)

/*
 * UARTFR 的意思是 Flags Register，也就是状态标志寄存器
 * 根据文档，此寄存器的偏移是 0x18
 */
#define UARTFR        0x06 /* 0x18 bytes */
#define UARTIMSC      0x0E /* 0x38 bytes */
#define UARTICR       0x11 /* 0x44 bytes */
#define UARTFR_RXFE   0x10
#define UARTFR_TXFF   0x20
#define UARTIMSC_RXIM 0x10
#define UARTIMSC_TXIM 0x20
#define UARTICR_RXIC  0x10
#define UARTICR_TXIC  0x20

#endif /* VERSATILEPB_H */
