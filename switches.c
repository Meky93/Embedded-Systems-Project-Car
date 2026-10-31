/*
 * switches.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

#include "msp430.h"
#include "macros.h"
#include "functions.h"
#include "ports.h"
#include "LCD.h"
#include <string.h>

// Global Variables:
volatile unsigned int test_flag_sw1 = 0;
volatile unsigned int test_flag_sw2 = 0;

// -------------------------- Switch ISRs -------------------------------------


// -------- Switch 1 (SW1) - Interrupt Service Routine -----------
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void){

    if (P4IFG & SW1){
        P4IFG &= ~SW1;

        test_flag_sw1 = 1;

    }
}

// -------- Switch 2 (SW2) - Interrupt Service Routine ----------
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void){

    if (P2IFG & SW2){
        P2IFG &= ~SW2;

        test_flag_sw1 = 0;
        test_flag_sw2 = 1;
    }
}

