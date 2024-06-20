#ifndef ARCH_CORE_HANDLER_H
#define ARCH_CORE_HANDLER_H

#include "common/common.h"

/**
 * @brief      Handler for unused interrupts
 */
void default_handler(void);

/**
 * @brief      CPU hard fault handler
 */
void h_fault_handler(void);

/**
 * @brief      systick interrupt
 */
void systick_handler(void);

/**
 * @brief      pendSV exception handler. Performs context switch routine
 */
void pend_sv_handler(void);

#endif
