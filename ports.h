/*
 * ports.h
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */
// Global Variables: none.

// This file, ports.h, is mainly to declare the function prototypes to be defined in ports.c

#include "msp430.h"

// Port function prototypes
void Init_Ports(void);
void Init_Port_1(void);
void Init_Port_2(void);
void Init_Port_3(unsigned char);
void Init_Port_4(void);
void Init_Port_5(void);
void Init_Port_6(void);

// Port 1 Pins
#define RED_LED         (0x01)          //1.0 RED_LED
#define A1_SEED         (0x02)          //1.1 A1_SEEED
#define V_DETECT_L      (0x04)          //1.2 V_DETECT_L - A2 - BIT2
#define V_DETECT_R      (0x08)          //1.3 V_DETECT_R - A3 - BIT3
#define A4_SEED         (0x10)          //1.4 A4_SEEED
#define V_THUMB         (0x20)          //1.5 V_THUMB
#define UCA0RXD         (0x40)          //1.6 UCA0RXD
#define UCA0TXD         (0x80)          //1.7 UCA0TXD

// Port 2 Pins
#define SLOW_CLK        (0x01) // 2.0 SLOW_CLK
#define CHECK_BAT       (0x02) // 2.1 CHECK_BAT
#define IR_LED          (0x04) // 2.2 IR LED
#define SW2             (0x08) // 2.3 SW2
#define IOT_RUN_CPU     (0x10) // 2.4 IOT_RUN_RED
#define DAC_ENB         (0x20) // 2.5 DAC_ENB
#define LFXOUT          (0x40) // 2.6 XOUTR
#define LFXIN           (0x80) // 2.7 XINR

// Port 3 Pins
#define TEST_PROBE      (0x01) // 3.0 TEST PROBE
#define DAC_CTRL_2      (0x02) // 3.1 OA2O
#define OA2N            (0x04) // 3.2 Photodiode Circuit
#define OA2P            (0x08) // 3.3 Photodiode Circuit
#define SMCLK           (0x10) // 3.4 SMCLK
#define DAC_CTRL_3      (0x20) // 3.5 DAC_CNTL
#define IOT_LINK_CPU    (0x40) // 3.6 IOT_LINK_GRN
#define IOT_RN_CPU      (0x80) // 3.7 IOT_EN

// Port 4 Pins
#define RESET_LCD       (0x01) // 4.0 RESET_LCD
#define SW1             (0x02) // 4.1 SW1
#define UCA1RXD         (0x04) // 4.2 Back Channel UCA1RXD
#define UCA1TXD         (0x08) // 4.3 Back Channel UCA1TXD
#define UCB1_CS_LCD     (0x10) // 4.4 Chip Select
#define UCB1CLK         (0x20) // 4.5 SPI mode - clock output—UCB1CLK
#define UCB1SIMO        (0x40) // 4.6 UCB1SIMO
#define UCB1SOMI        (0x80) // 4.7 UCB1SOMI

// Port 5 Pins
#define V_BAT           (0x01) // 5.0 V_BAT
#define V_5             (0x02) // 5.1 V_5
#define V_DAC           (0x04) // 5.2 V_DAC
#define V3_3            (0x08) // 5.3 V3_3
#define IOT_BOOT_CPU    (0x10) // 5.4 IOT_BOOT_CPU

// Port 6 Pins
#define LCD_BACKLITE    (0x01) // P6.0
#define R_FORWARD       (0x02) // P6.1
#define R_REVERSE       (0x04) // P6.2
#define L_FORWARD       (0x08) // P6.3
#define L_REVERSE       (0x10) // P6.4
#define P6_5            (0x20) // P6.5
#define GRN_LED         (0x40) // P6.6
