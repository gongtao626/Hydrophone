/*
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
	.section .text.cs3.init,"ax",%progbits
#if defined(__thumb2__) || defined(__ARM_ARCH_6M__)
#define THUMB 1
	.code 16
	.thumb_func
#else
	.code 32
#endif
	.globl _start2
	.type _start2, %function
_start2:
#if THUMB
	ldr	r1,=__cs3_stack
	mov	sp,r1
	ldr	r1,=__cs3_start_c2  //b06862
	bx	r1
#else
	ldr	sp,=__cs3_stack
	b	__cs3_start_c2      //b06862
#endif
	.pool
