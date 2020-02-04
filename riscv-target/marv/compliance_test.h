// RISC-V Compliance Test Header File

#ifndef _COMPLIANCE_TEST_H
#define _COMPLIANCE_TEST_H

#include "riscv_test.h"

//-----------------------------------------------------------------------
// RV Compliance Macros
//-----------------------------------------------------------------------

#define MARV_TEST_PASS_FAIL_ADDR (0x20000000)

#define RV_COMPLIANCE_HALT                                                    \
        li t0, 0; \
        li t1, MARV_TEST_PASS_FAIL_ADDR; \
        sb t0, 0(t1); \
        /*RVTEST_PASS*/                                                       \

#define RV_COMPLIANCE_RV32M                                                   \
        RVTEST_RV32M                                                          \

#define RV_COMPLIANCE_CODE_BEGIN                                              \
        .section .entry; \
        .globl _entry; \
_entry: \
        j _stub; \
        .section .text.stub; \
_stub: \
        /* Copy data sections from ROM into RAM, as they need to be writable */ \
        lui t0, %hi(__data_start); \
        addi t0, t0, %lo(__data_start); \
        lui t1, %hi(__data_end); \
        addi t1, t1, %lo(__data_end); \
        lui t2, %hi(__data_va); \
        addi t2, t2, %lo(__data_va); \
        j copy_data_loop_end; \
copy_data_loop: \
            lw t3, 0(t0); \
            sw t3, 0(t2); \
            addi t0, t0, 4; \
            addi t2, t2, 4; \
copy_data_loop_end: \
        blt t0, t1, copy_data_loop; \
\
        /* Clear bss sections */ \
        lui t0, %hi(__bss_start); \
        addi t0, t0, %lo(__bss_start); \
        lui t1, %hi(__bss_end); \
        addi t1, t1, %lo(__bss_end); \
        j clear_bss_loop_end; \
clear_bss_loop: \
            sw zero, 0(t0); \
            addi t0, t0, 4; \
clear_bss_loop_end: \
        blt t0, t1, clear_bss_loop; \
\
        /* Set up env registers */ \
        lui sp, %hi(__stack_va); \
        addi sp, sp, %lo(__stack_va); \
        lui tp, %hi(__tp); \
        addi tp, tp, %lo(__tp); \
        lui gp, %hi(__gp); \
        addi gp, gp, %lo(__gp); \
\
        /* Let's GOOOOO */ \
        j begin_testcode; \
        .section .text; \
begin_testcode: \
        /*RVTEST_CODE_BEGIN*/                                                 \

#define RV_COMPLIANCE_CODE_END                                                \
        RVTEST_CODE_END                                                       \

#define RV_COMPLIANCE_DATA_BEGIN                                              \
        RVTEST_DATA_BEGIN                                                     \

#define RV_COMPLIANCE_DATA_END                                                \
        RVTEST_DATA_END                                                       \

#endif
