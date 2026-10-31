/*
 * ports.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */
// Global Variables: n/a

// In this file, ports.c, a primary function (called by main.c) calls each one of the Init_Port_i functions
// that describe each (usually 8) connections to each port individually.

#include "msp430.h"
#include "macros.h"
#include "ports.h"
#include "functions.h"

volatile unsigned char smclk_mode = USE_GPIO;


// This is the main (parent) ports function that calls the 6 individual Init_port_<i> functions.
void Init_Ports(void)
{
    Init_Port_1();
    Init_Port_2();
    Init_Port_3(smclk_mode);
    Init_Port_4();
    Init_Port_5();
    Init_Port_6();
}

// These Init_Port_i functions describe the configuration of each pin of each port.
// Pins and their corresponding signal names (defined in ports.h) are configured as
// either GPI/Os (General Purpose Input/Outputs) or Functions.
// GPI/O connections are configured as either an INPUT or an OUTPUT (Default SEL1=0 SEL0=0).
// For FUNCTION connections, selector bits SEL1 and SEL0 are configured.

void Init_Port_1(void){

    // Port 1 defines:
    // P1.0 - RED_LED - GPI/O, OUT.
    // P1.1 - A1_SEED - FUNCTION, SEL1=1, SEL0=1. (A1)
    // P1.2 - V_DETECT_L - FUNCTION, SEL1=1, SEL0=1. (A2)
    // P1.3 - V_DETECT_R - FUNCTION, SEL1=1, SEL0=1. (A3)
    // P1.4 - A4_SEED - FUNCTION, SEL1=1, SEL0=1. (A4)
    // P1.5 - V_THUMB - FUNCTION, SEL1=1, SEL0=1. (A5)
    // P1.6 - UCA0RXD - FUNCTION, SEL1=0, SEL0=1.
    // P1.7 - UCA0TXD - FUNCTION, SEL1=0, SEL0=1.

    P1SEL1 = A1_SEED;
    P1SEL1 |= V_DETECT_L;
    P1SEL1 |= V_DETECT_R;
    P1SEL1 |= A4_SEED;
    P1SEL1 |= V_THUMB;

    P1SEL0 = A1_SEED;
    P1SEL0 |= V_DETECT_L;
    P1SEL0 |= V_DETECT_R;
    P1SEL0 |= A4_SEED;
    P1SEL0 |= UCA0TXD;
    P1SEL0 |= UCA0RXD;
    P1SEL0 |= V_THUMB;

    P1OUT = RESET_STATE;    // Initialize OUTPUT latches LOW.

    P1REN &= ~V_THUMB;

    P1DIR = RED_LED;        // OUTPUT
    P1DIR &= ~V_THUMB;
}

void Init_Port_2(void){

    // Port 2 defines:
    // P2.0 - SLOW_CLK - GPI/O, OUT.
    // P2.1 - CHECK_BAT - GPI/O, OUT.
    // P2.2 - IR_LED - GPI/O, OUT.
    // P2.3 - SW2 - GPI/O, IN.
    // P2.4 - IOT_RUN_CPU - GPI/O, OUT.
    // P2.5 - DAC_ENB - GPI/O, OUT.
    // P2.6 - LFXOUT - FUNCTION, SEL1=1, SEL0=0.
    // P2.7 - LFXIN - FUNCTION, SEL1=1, SEL0=0.

    P2SEL0 = RESET_STATE;
    P2SEL1 = RESET_STATE;

    P2SEL1 = LFXOUT;
    P2SEL1 |= LFXIN;



    P2OUT = RESET_STATE;    // Initialize OUTPUT latches LOW.

    P2DIR = SLOW_CLK;       // OUTPUT
    P2DIR |= CHECK_BAT;     // OUTPUT
    P2DIR |= IR_LED;        // OUTPUT
    P2DIR |= IOT_RUN_CPU;   // OUTPUT
    P2DIR |= DAC_ENB;       // OUTPUT
    P2DIR &= ~SW2;          // INPUT

    P2REN = SW2;            // ENABLE resistor on P2.3
    P2OUT |= SW2;           // Configure resistor as PULL-UP.

    P2IES |= SW2;           // Interrupt on high-to-low transition
    P2IFG &= ~SW2;          // Clear any pending interrupt flag
    P2IE  |= SW2;           // Enable interrupt for SW2
}

void Init_Port_3(unsigned char smclk_mode){

    // Port 3 defines:
    // P3.0 - TEST_PROBE - GPI/O, OUT.
    // P3.1 - DAC_CTRL_2 - FUNCTION, SEL1=1, SEL0=1.
    // P3.2 - OA2N - FUNCTION, SEL1=1, SEL0=1. - (NOTE: Photo Diode Circuit Negative-).
    // P3.3 - OA2P - FUNCTION, SEL1=1, SEL0=1. - (NOTE: Photo Diode Circuit Positive+).
    // P3.4 - SMCLK - GPI/O, OUT.
    // P3.5 - DAC_CTRL_3 - FUNCTION, SEL1=1, SEL0=1.
    // P3.6 - IOT_LINK_CPU - GPI/O, OUT. - (NOTE: IOT_LINK_GRN).
    // P3.7 - IOT_RN_CPU - GPI/O, OUT. - (NOTE: IOT_EN).

    P3SEL1 = DAC_CTRL_2;
    P3SEL1 |= OA2N;
    P3SEL1 |= OA2P;
    P3SEL1 &= ~DAC_CTRL_3;

    P3SEL0 = DAC_CTRL_2;
    P3SEL0 |= OA2N;
    P3SEL0 |= OA2P;
    P3SEL0 &= ~DAC_CTRL_3;

    P3SELC  |=  DAC_CTRL_3;

    P3DIR = TEST_PROBE;     // OUTPUT
    P3DIR |= IOT_LINK_CPU;  // OUTPUT
    P3DIR |= IOT_RN_CPU ;   // OUTPUT
    P3DIR |= SMCLK;
    P3DIR &= ~DAC_CTRL_3;

    P3OUT &= ~DAC_CTRL_3;



    // SMCLK configure as a standard GPI/O pin. Selector bits 00.
    if (smclk_mode == USE_GPIO) {
        P3SEL1 &= ~SMCLK;  // Select GPIO function
        P3SEL0 &= ~SMCLK;
        P3OUT &= ~SMCLK;  // Initial GPIO output level
    }

    // SMCLK configured to output the pin, select bits 01 selected.
    else if (smclk_mode == USE_SMCLK) {
        P3SEL1 &= ~SMCLK;
        P3SEL0 |= SMCLK;
    }
}

void Init_Port_4(void){

    // Port 4 defines:
    // P4.0 - RESET_LCD - GPI/O, OUT.
    // P4.1 - SW1 - GPI/O, IN.
    // P4.2 - Back Channel UCA1RXD - FUNCTION, SEL1=0, SEL0=1.
    // P4.3 - Back Channel UCA1TXD - FUNCTION, SEL1=0, SEL0=1.
    // P4.4 - UCB1_CS_LCD (Chip select) - GPI/O, OUT.
    // P4.5 - SPI mode - clock output—UCB1CLK - FUNCTION, SEL1=0, SEL0=1.
    // P4.6 - UCB1SIMO - FUNCTION, SEL1=0, SEL0=1.
    // P4.7 - UCB1SOMI - FUNCTION, SEL1=0, SEL0=1.

    P4SEL1 = RESET_STATE;

    P4SEL0 = UCA1RXD;
    P4SEL0 |= UCA1TXD;
    P4SEL0 |= UCB1CLK;
    P4SEL0 |= UCB1SIMO;
    P4SEL0 |= UCB1SOMI;

    P4OUT = RESET_STATE;    // Initialize OUTPUT latches LOW.

    P4DIR = RESET_LCD;      // OUTPUT
    P4DIR |= UCB1_CS_LCD;   // OUTPUT
    P4DIR &= ~SW1;          // INPUT

    P4REN |= SW1;           // ENABLE Resistor on P4.1.
    P4OUT |= SW1;           // Configure resistor as PULL-UP.

    P4IES |= SW1;           // Interrupt on high-to-low transition
    P4IFG &= ~SW1;          // Clear any pending interrupt flag
    P4IE  |= SW1;           // Enable interrupt for SW1
}


void Init_Port_5(void){

    // Port 5 defines:
    // P5.0 - V_BAT - FUNCTION, SEL1=1, SEL0=1.
    // P5.1 - V_5 - FUNCTION, SEL1=1, SEL0=1.
    // P5.2 - V_DAC - FUNCTION, SEL1=1, SEL0=1.
    // P5.3 - V3_3 - FUNCTION, SEL1=1, SEL0=1.
    // P5.4 - IOT_BOOT_CPU - GPI/O, IN, SEL1=0, SEL0=0.

    P5SEL1 = V_BAT;
    P5SEL1 |= V_5;
    P5SEL1 |= V_DAC;
    P5SEL1 |= V3_3;

    P5SEL0 = V_BAT;
    P5SEL0 |= V_5;
    P5SEL0 |= V_DAC;
    P5SEL0 |= V3_3;

    P5OUT = RESET_STATE;        // Initialize OUTPUT latches LOW.
    P5DIR = RESET_STATE;
    P5DIR &= ~IOT_BOOT_CPU;     // INPUT

    P5REN |= IOT_BOOT_CPU;      // ENABLE resistor at P5.4.
    P5OUT |= IOT_BOOT_CPU;      // Configure resistor as PULL-UP.
}

void Init_Port_6(void){

    // Port 6 defines:
    // P6.0 - LCD Backlight
    // P6.1 - R_FORWARD
    // P6.2 - R_REVERSE
    // P6.3 - L_FORWARD
    // P6.4 - L_REVERSE
    // P6.5 - P6_5
    // P6.6 - GRN_LED
    // All Port 6 pins are - GPI/O, OUT.
    // EXCEPT P6.5 - P6_5, which is set to input but unused as of now.

    P6SEL0 |= R_FORWARD;
    P6SEL0 |= R_REVERSE;
    P6SEL0 |= L_FORWARD;
    P6SEL0 |= L_REVERSE;

    P6SEL1 &= ~R_FORWARD;
    P6SEL1 &= ~R_REVERSE;
    P6SEL1 &= ~L_FORWARD;
    P6SEL1 &= ~L_REVERSE;


    P6OUT = RESET_STATE;        // Initialize OUTPUT latches LOW.

    P6DIR = LCD_BACKLITE;
    P6DIR |= R_FORWARD;
    P6DIR |= R_REVERSE;
    P6DIR |= L_FORWARD;
    P6DIR |= L_REVERSE;
    P6DIR |= GRN_LED;

    P6OUT &= ~R_REVERSE;        // Ensure right reverse is turned OFF initially.
    P6OUT &= ~L_REVERSE;        // Ensure left reverse is turned OFF initially.
    P6OUT |= LCD_BACKLITE;      // Set LCD_BACKLIGHT high (aka: turn on the LCD).
}

