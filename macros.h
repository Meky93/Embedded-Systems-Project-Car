/*
 * macros.h
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */



// This file, macros.h, was created to contain the many #define statements of the program.
// This "cleans up" other files and provides one central place for all define statements.

#include "msp430.h"

// Global Variables:
extern volatile unsigned int event;
extern volatile unsigned int time_change;
extern volatile unsigned int Time_Sequence;
extern unsigned int Last_Time_Sequence;
extern unsigned int cycle_time;
extern unsigned int state;
extern unsigned int delay_start;
extern unsigned int left_motor_count;
extern unsigned int right_motor_count;
extern unsigned int segment_count;
extern unsigned int shape_phase;

// General #define statements
#define ALWAYS                      (0x01)
#define RESET_STATE                 (0x00)
#define TRUE                        (0x01)
#define THUMB_ADC                   ADC12INCH_5  // ADC input A5

//These define statements were made using both the MSP-EXP430FR2355-Schematic and the given control board schematic.
#define FALSE                       (0x00)
#define TRUE                        (0x01)
#define MOTOR                       (0x00)
#define SMCLK_OFF                   (0x00)
#define SMCLK_ON                    (0x01)
#define PORTS                       (0x00)
#define PWM_MODE                    (0x01)
#define WHEEL_OFF                   (0x00)
#define WHEEL_PERIOD                (10000)
#define PWM_PERIOD                  (TB3CCR0)
#define RIGHT_FORWARD_SPEED         (TB3CCR2)                                           // P6.1 - Right Forward
#define RIGHT_REVERSE_SPEED         (TB3CCR3)                                           // P6.2 - Right Reverse
#define LEFT_FORWARD_SPEED          (TB3CCR4)                                           // P6.3 - Left Forward
#define LEFT_REVERSE_SPEED          (TB3CCR5)                                           // P6.4 - Left Reverse
//#define LCD_BACKLITE_DIMMING        (TB3CCR5)
#define FORWARD                     (0x00)
#define REVERSE                     (0x01)

//------------------------------ Clocks ----------------------------------------------
#define USE_GPIO                    (0x00)
#define USE_SMCLK                   (0x01)
#define TB0CCR0_INTERVAL            (2500)                                              // 8,000,000 / 2 / 8 / (1 / 5ms) = 2500
#define DAC_INTERVAL                (2500)                                              // 2500 = 5ms intervals for the DAC also.
#define TB0CCR1_INTERVAL            (10000)                                             // 8,000,000 / 2 / 8 / (1 / 5)
#define TB0CCR2_INTERVAL            (10000)                                             // 8,000,000 / 2 / 8 / (1 / 5)
#define DEBOUNCE_THRESHOLD          (100)

//------------------------------- Motor States ---------------------------------------
#define NONE                        ('N')
#define STRAIGHT                    ('L')
#define CIRCLE                      ('C')
#define FIGURE8                     ('F')
#define TRIANGLE                    ('T')
#define WAIT                        ('W')
#define START                       ('S')
#define RUN                         ('R')
#define END                         ('E')
#define PROJFIVEDEMO                ('D')
#define PROJ6DEMO                   ('P')
#define BLINK_LCD                   ('B')

//--------------------------- Wheel Specs ---------------------------------------------
#define WHEEL_COUNT_TIME            (20)
#define RIGHT_COUNT_TIME            (20)
#define LEFT_COUNT_TIME             (19)
#define TRAVEL_DISTANCE             (100)
#define WAITING2START               (150)

// ---------------------------- DAC Values --------------------------------------------
#define DAC_STEP                    (50)
#define DAC_TARGET                  DAC_Limit

#define DAC_Begin                   (2725)      // 2v
#define DAC_Limit                   (1700)      // 4.02v
#define DAC_Adjust                  (1700)      // 4.00v
//#define DAC_Limit                 (850)       // 6.08v
//#define DAC_Adjust                (875)       // 6.00v
//#define DAC_Limit                 (1400)      // 4.29v
//#define DAC_Adjust                (1405)      // 4.32v
//#define DAC_Limit                 (1300)      // 4.66v
//#define DAC_Adjust                (1300)      // 4.66v
//#define DAC_Limit                 (1200)      // 4.98v
//#define DAC_Adjust                (1190)      // 5.00v
//#define DAC_Limit                 (1000)      // 5.61v
//#define DAC_Adjust                (1033)      // 5.50v
//#define DAC_Limit                 (715)       // 6.5v
//#define DAC_Adjust                (712)       // 6.5v


// ---------------------- Line Run State Machine Timing -------------------------------
#define LINE_TURN_TICKS                 65u
#define LINE_PAUSE_TICKS                20u
#define LINE_STEER_THRESHOLD            600
#define LINE_DETECT_THRESHOLD           320u
#define LINE_LEFT_OFFSET                50
#define LINE_RIGHT_OFFSET               130

#define LINE_FOLLOW_TICKS               5800u
#define LINE_EXIT_FORWARD_TICKS         400u
