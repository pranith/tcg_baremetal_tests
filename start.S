/*
 * Copyright (C) 2015 Virtual Open Systems SAS
 * Author: Alexander Spyridakis <a.spyridakis@virtualopensystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Before jumping to C code we need to setup a stack.
 * This macro gets the CPU ID from MPIDR and offsets the stack 
 * for each CPU by 1 << 18 (256 KiB), starting after the text section.
 */
#ifdef ARCH_ARM
.macro setup_stack
    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #15
    add r0, r0, #1

    lsl r0, r0, #20
    ldr r1, =text_end
    add r0, r0, r1
    mov sp, r0
.endm
#elif ARCH_AARCH64
.macro setup_stack
    mrs x0, MPIDR_EL1
    and x0, x0, #15
    add x0, x0, #1

    lsl x0, x0, #18
    ldr x1, =text_end
    add x0, x0, x1
    mov sp, x0
.endm
#endif

/* Entry point */
.section .text
.global _start
_start:
    setup_stack
    bl main
    wfi
