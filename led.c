/*
 * led.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */
// Global Variables: none

#include "msp430.h"
#include "macros.h"
#include "functions.h"
#include "ports.h"

void Init_LEDs(void){
//------------------------------------------------------------------------------
// LED Configurations
//------------------------------------------------------------------------------
// Turns on both LEDs
  P1OUT &= ~RED_LED;
  P6OUT &= ~GRN_LED;
//------------------------------------------------------------------------------
}
