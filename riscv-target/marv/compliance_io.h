// RISC-V Compliance IO Test Header File

/*
 * Copyright (c) 2005-2018 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


//
// In general the following registers are reserved
// ra, a0, t0, t1
// x1, x10 x5, x6
// new reserve x31
//

#ifndef _COMPLIANCE_IO_H
#define _COMPLIANCE_IO_H

//#define RVTEST_IO_QUIET

//-----------------------------------------------------------------------
// RV IO Macros (Character transfer by custom MMIO write)
//-----------------------------------------------------------------------

#ifdef RVTEST_IO_QUIET

#define RVTEST_IO_INIT
#define RVTEST_IO_WRITE_STR(_SP, _STR)
#define RVTEST_IO_CHECK()
#define RVTEST_IO_ASSERT_GPR_EQ(_SP, _R, _I)
#define RVTEST_IO_ASSERT_SFPR_EQ(_F, _R, _I)
#define RVTEST_IO_ASSERT_DFPR_EQ(_D, _R, _I)

#else

#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)

#define RSIZE 4
// _SP = (volatile register)
#define LOCAL_IO_PUSH(_SP)                                              \
    la      _SP,  begin_regstate;                                       \
    sw      x1,   (1*RSIZE)(_SP);                                       \
    sw      x5,   (5*RSIZE)(_SP);                                       \
    sw      x6,   (6*RSIZE)(_SP);                                       \
    sw      x8,   (8*RSIZE)(_SP);                                       \
    sw      x10,  (10*RSIZE)(_SP);                                      \
    sw      x28,  (28*RSIZE)(_SP);                                      \

// _SP = (volatile register)
#define LOCAL_IO_POP(_SP)                                               \
    la      _SP,   begin_regstate;                                      \
    lw      x1,   (1*RSIZE)(_SP);                                       \
    lw      x5,   (5*RSIZE)(_SP);                                       \
    lw      x6,   (6*RSIZE)(_SP);                                       \
    lw      x8,   (8*RSIZE)(_SP);                                       \
    lw      x10,  (10*RSIZE)(_SP);                                      \
    lw      x28,  (28*RSIZE)(_SP);                                      \

#define LOCAL_IO_WRITE_GPR(_R)                                          \
    mv          a0, _R;                                                 \
    jal         FN_WriteA0;

#define MARV_SERIAL_WRITE (0x21000000)

#define LOCAL_IO_PUTC(_R)                                               \
    li t1, MARV_SERIAL_WRITE; \
    sb _R, (t1); \

#define RVTEST_IO_INIT

// _SP = (volatile register)
#define LOCAL_IO_WRITE_STR(_STR) RVTEST_IO_WRITE_STR(x31, _STR)
#define RVTEST_IO_WRITE_STR(_SP, _STR)                                  \
    LOCAL_IO_PUSH(_SP)                                                  \
    .section .data.string;                                              \
10000:                                                                  \
    .string _STR;                                                       \
    .section .text;                                                     \
    la a0, 10000b;                                                      \
    jal FN_WriteStr;                                                    \
    LOCAL_IO_POP(_SP)

#define RVTEST_IO_CHECK()

#define MARV_TEST_PASS_FAIL_ADDR (0x20000000)

// Assertion violation: file file.c, line 1234: (expr)
// _SP = (volatile register)
// _R = GPR
// _I = Immediate
#define RVTEST_IO_ASSERT_GPR_EQ(_SP, _R, _I)                            \
    LOCAL_IO_PUSH(_SP)                                                  \
    mv          s0, _R;                                                 \
    li          t0, _I;                                                 \
    beq         s0, t0, 20000f;                                         \
    LOCAL_IO_WRITE_STR("Assertion violation: file ");                   \
    LOCAL_IO_WRITE_STR(__FILE__);                                       \
    LOCAL_IO_WRITE_STR(", line ");                                      \
    LOCAL_IO_WRITE_STR(TOSTRING(__LINE__));                             \
    LOCAL_IO_WRITE_STR(": ");                                           \
    LOCAL_IO_WRITE_STR(# _R);                                           \
    LOCAL_IO_WRITE_STR("(");                                            \
    LOCAL_IO_WRITE_GPR(s0);                                             \
    LOCAL_IO_WRITE_STR(") != ");                                        \
    LOCAL_IO_WRITE_STR(# _I);                                           \
    LOCAL_IO_WRITE_STR("\n");                                           \
    li t0, 1; \
    li t1, MARV_TEST_PASS_FAIL_ADDR; \
    sb t0, 0(t1); \
    /*li TESTNUM, 100*/;                                                \
    /*RVTEST_FAIL*/;                                                    \
20000:                                                                  \
    LOCAL_IO_POP(_SP)

#define RVTEST_IO_ASSERT_SFPR_EQ(_F, _R, _I)
#define RVTEST_IO_ASSERT_DFPR_EQ(_D, _R, _I)

//
// FN_WriteStr: Uses a0, t0, t1
//
FN_WriteStr:
    mv          t0, a0;
10000:
    lbu         a0, (t0);
    addi        t0, t0, 1;
    beq         a0, zero, 10000f;
    LOCAL_IO_PUTC(a0);
    j           10000b;
10000:
    ret;

//
// FN_WriteA0: write register a0(x10) (destroys a0(x10), t0-t2(x5-x7), t3(x28))
//
FN_WriteA0:
        mv          t0, a0
FN_WriteA0_32:
        // reverse register when xlen is 32
        li          t3, 8
10000:  slli        t2, t2, 4
        andi        a0, t0, 0xf
        srli        t0, t0, 4
        or          t2, t2, a0
        addi        t3, t3, -1
        bnez        t3, 10000b
        li          t3, 8
        // write reversed characters
        li          t0, 10
10000:  andi        a0, t2, 0xf
        blt         a0, t0, 10001f
        addi        a0, a0, 'a'-10
        j           10002f
10001:  addi        a0, a0, '0'
10002:  LOCAL_IO_PUTC(a0)
        srli        t2, t2, 4
        addi        t3, t3, -1
        bnez        t3, 10000b
        ret

#endif // RVTEST_IO_QUIET

#endif // _COMPLIANCE_IO_H
