#ifndef INTERRUPT_H
#define INTERRUPT_H

/* 中断状态 */
#define PIC_INTERRUPT_STATUS *(volatile unsigned *)(0x10140000)
#define SEC_INTERRUPT_STATUS *(volatile unsigned *)(0x10003000)

/* 中断控制 */
#define PIC_ENSET *(volatile unsigned *)(0x10140010)
#define PIC_ENCLR *(volatile unsigned *)(0x10140014)
#define SIC_ENSET *(volatile unsigned *)(0x10003008)
#define SIC_ENCLR *(volatile unsigned *)(0x1000300C)
#define TIMER0_DATA *(volatile unsigned *)(0x101E2000)
#define TIMER0_CTRL *(volatile unsigned *)(0x101E2008)
#define TIMER0_CLR  *(volatile unsigned *)(0x101E200C)
#define TIMER2_DATA *(volatile unsigned *)(0x101E3000)
#define TIMER2_CTRL *(volatile unsigned *)(0x101E3008)
#define TIMER2_CLR  *(volatile unsigned *)(0x101E300C)

#endif
