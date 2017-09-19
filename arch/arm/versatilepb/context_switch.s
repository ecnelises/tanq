.global irq_entry
irq_entry:
	msr CPSR_c, #0xDF

	push {r7}
	ldr r7, PIC
	ldr r7, [r7]
	PIC: .word 0x10140000
	clz r7, r7
	sub r7, r7, #31

	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov r0, sp

	msr CPSR_c, #0xD2
	mrs ip, SPSR
	sub lr, lr, #0x4
	stmfd r0!, {ip,lr}

	msr CPSR_c, #0xD3

	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.global svc_entry
svc_entry:
    msr CPSR_c, #0xDF
    push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    mov r0, sp
    msr CPSR_c, #0xD3

    mrs ip, SPSR
    stmfd r0!, {ip,lr}
	pop {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
	mov sp, ip
	bx lr

.global activate
activate:
    mov ip, sp
    push {r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

    /* 以 r0 的内容为基址，依次写入寄存器列表，将末尾地址写回 r0 */
    ldmfd r0!, {ip,lr}
	msr SPSR, ip

	msr CPSR_c, #0xDF
	mov sp, r0
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
    pop {r7}
	msr CPSR_c, #0xD3

	movs pc, lr
