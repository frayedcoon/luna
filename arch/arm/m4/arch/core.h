/**
 * @file core.h
 * @author Yevheniy Verzun
 * @date August 2017
 * @brief ARM Cortex-m4 core peripherals defines
 *
 * Defines memory addresses of core peripherals and registers: systick (STK),
 * system control block (SCB), nested vectored interrupt controller (NVIC),
 * floationg point unit (FPU)
 */
#ifndef ARCH_CORE_H
#define ARCH_CORE_H

#include "common/common.h"

//! Macros to enable/disable global interrupts
#define enable_interrupts()                 asm volatile("cpsie i")
#define disable_interrupts()                asm volatile("cpsid i")

//! Core peripherals
#define STK_BASE                            0xe000e010
#define SCB_BASE                            0xe000ed00
#define NVIC_BASE                           0xe000e100
#define SCB_BASE2                           0xe000e008
#define NVIC_BASE2                          0xe000ef00

//! STK
#define STK_CSR                             REGISTER_32(STK_BASE + 0)
#define STK_RVR                             REGISTER_32(STK_BASE + 4)
#define STK_CVR                             REGISTER_32(STK_BASE + 8)
#define STK_CALIB                           REGISTER_32(STK_BASE + 0x0c)

//! SCB_BASE
#define CPUID                               REGISTER_32(SCB_BASE + 0)
#define ICSR                                REGISTER_32(SCB_BASE + 4)
#define AIRCR                               REGISTER_32(SCB_BASE + 0x0c)
#define SCR                                 REGISTER_32(SCB_BASE + 0x10)
#define CCR                                 REGISTER_32(SCB_BASE + 0x14)
#define SHPR1                               REGISTER_32(SCB_BASE + 0x18)
#define SHPR2                               REGISTER_32(SCB_BASE + 0x1c)
#define SHPR3                               REGISTER_32(SCB_BASE + 0x20)
#define CFSR                                REGISTER_32(SCB_BASE + 0x28)
#define HFSR                                REGISTER_32(SCB_BASE + 0x2C)
#define BFAR                                REGISTER_32(SCB_BASE + 0x38)

//! NVIC
#define ISER_BASE                           NVIC_BASE + 0x000
#define ICER_BASE                           NVIC_BASE + 0x080
#define ISPR_BASE                           NVIC_BASE + 0x100
#define ICPR_BASE                           NVIC_BASE + 0x180
#define IABR_BASE                           NVIC_BASE + 0x200
#define IPR_BASE                            NVIC_BASE + 0x300

#endif
