/* Reset code for Freescale TWR-K60N512
 *
 * Version: Sourcery G++ 2010.09-66
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
#if defined(__thumb2__) || defined(__ARM_ARCH_6M__)
#define THUMB 1
	.thumb
	.thumb_func
#else
	.arm
#endif
	.section .cs3.reset,"ax",%progbits
	.globl __cs3_reset_twr_k60n512_new
	.type	__cs3_reset_twr_k60n512_new, %function
__cs3_reset_twr_k60n512_new:
	/* jump to common start code */
#if THUMB
	ldr	r0,=_start2
	bx	r0
#else
	b	_start2
#endif
	.pool
	.size	__cs3_reset_twr_k60n512_new,.-__cs3_reset_twr_k60n512_new
#if !defined(__ARM_ARCH_6M__)
#if defined(__thumb2__)
	.thumb
#else
	.arm
#endif
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#define M_PROFILE
	/* FIXME: Marking this object as using VFP is wrong because we do
	   runtime detection.  */
	.fpu	fpv4-sp-d16
#else
	.fpu	vfp
#endif
	.syntax	unified
	.text
	.type __vfp_init, %function
__vfp_init:
/* Initialize VFP.  */
#if !defined(__thumb2__)
/* See what sort of CPU we have.  */
	mrc	p15, 0, r0, c0, c0, 0
	tst	r0, #0x80000
	bne	2f /* armv7 or later */
	ands	r1, r0, #0xf000
	beq	1f /* pre-arm7 */
	cmp	r1, #0x7000
	beq	1f /* arm7 */
	and	r1, r0, #0xf0000
	cmp	r1, #0x70000
	blt	1f
2:
#endif /* !thumb2 */
#if defined(M_PROFILE)
	movw	r1, #0xed88
	movt	r1, #0xe000
	ldr	r0, [r1]
	orr	r0, r0, #0x00f00000
	str	r0, [r1]
	/* Load the value back and check that the enable bits stuck.  */
	ldr	r0, [r1]
	and	r0, r0, #0x00f00000
	cmp	r0, #0x00f00000
	beq	1f
	bx	lr
1:
	/* Set the FPACTV bit to ensure VFP state is saved during
	   exceptions.  */
	mrs	r0, control
	orr	r0, r0, #0x4
	msr	control, r0
#else /* A/R profile */
/* ARMv6 or later.  Enable coprocessor access.  */
	mrc	p15, 0, r0, c1, c0, 2
/* Allow access to cp10 and cp11 */
	orr	r0, r0, #0x00f00000
	mcr	p15, 0, r0, c1, c0, 2
/* Flush prefetch buffer.  */
	mov	r0, #0
	mcr	p15, 0, r1, c7, c5, 4
1:
/* Enable the VFP unit.  */
	mov	r0, #0x40000000
	fmxr	fpexc, r0
	fmrx	r0, fpsid
	and	r1, r0, #0xff000000
	cmp	r1, #0x41000000
	bne	1f
	and	r1, r0, #0xf0
	cmp	r1, #0xb0
	bgt	1f
/* Put ARM VFP9/10/11 units into runfast mode so that support code
	is not needed.  The Cortex VFP unit handles everything in hardware
	so can be left alone.  */
	mov	r0, #0x03000000
	fmxr	fpscr, r0
1:
#endif
	bx	lr
	.size __vfp_init, .-__vfp_init

	.section .preinit_array
	.align 2
	.word __vfp_init(target1)
#endif /* !ARMv6-M */
/* checksum: 7650d194e76059b39c733bbb17 */
