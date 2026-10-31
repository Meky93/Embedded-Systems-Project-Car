/*
 *  main.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

// Includes:
#include  "msp430.h"
#include  <string.h>
#include  "functions.h"
#include  "LCD.h"
#include  "ports.h"
#include  "macros.h"

// Global Variables:
extern volatile unsigned int test_flag_sw1;
extern volatile unsigned int test_flag_sw2;
extern volatile unsigned int ADC_Left_Detect;
extern volatile unsigned int ADC_Right_Detect;
extern volatile unsigned int ADC_Channel;
extern volatile unsigned int ADC_global;
extern volatile unsigned int ADC_value;
extern char thousands;
extern char hundreds;
extern char tens;
extern char ones;
volatile unsigned char adc_new_data = 0;
volatile unsigned int time_change = 0;
volatile unsigned int event = NONE;
volatile unsigned int Time_Sequence = 0;
extern volatile char one_time = 0;
unsigned int Last_Time_Sequence = 0;
unsigned int state = WAIT;
unsigned int delay_start = 0;
unsigned int left_motor_count = 0;
unsigned int right_motor_count = 0;
unsigned int segment_count = 0;
unsigned int shape_phase = 0;
volatile unsigned int five_hun_ms_counter = 0;
volatile unsigned int five_ms_counter = 0;
extern unsigned int wheel_move;
extern char forward;



// ---------------------------------------------- MAIN PROGRAM ------------------------------------------------------

void main(void){

    // Local Variables:
    unsigned int now, last_adc = 0, last_display = 0;
    unsigned int left = 0, right = 0, center = 0;
    unsigned char fresh, adc_active = 0, have_sample = 0;
    unsigned char start_request, stop_request;
    int adjusted;
    unsigned int timer_start = 0;
    unsigned int elapsed_ticks = 0;
    unsigned char timer_running = 0;

    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;


    // -------- Initialization and Configuration --------
    Init_Ports();
    Init_Clocks();
    Init_Conditions();
    Init_Timers();
    Init_LCD();
    Init_ADC();
    Init_DAC();

    // ----------- Display Initial ---------------------
    strcpy(display_line[0], "  JAKE'S  ");
    strcpy(display_line[1], "  CAR :)  ");
    strcpy(display_line[2], "----------");
    strcpy(display_line[3], "          ");
    display_changed = TRUE;
    Display_Update(0, 0, 0, 0);

    // ------------- State Initial ---------------------
    state = WAIT;
    event = NONE;
    Last_Time_Sequence = Time_Sequence;
    time_change = 0;


    // -------------- Primary continuously running WHILE LOOP of the program. ----------------------

    while(ALWAYS){

      // Only main starts conversions. Do not start another sequence until
      // all three results from the previous sequence have been consumed.
      fresh = 0;
      __disable_interrupt();

      now = line_ticks;                             // Reset timer to be used 0 in later processes if they should be ran.

      // Set start_reqest high if test_flag_sw1 is high indicating active-low switch 1 was pressed.
      // This indicates we want to startup the DAC and call the Line_Run_Start() sequence.
      start_request = test_flag_sw1;

      // Set stop_reqest high if test_flag_sw2 is high indicating active-low switch 2 was pressed.
      // This indicates we want to call Line_Run_Stop(), cancel any current sequence, shut the motors off, and shut the DAC off. Essentially resetting the state machine.
      stop_request = test_flag_sw2;

      // Reset switch flags
      test_flag_sw1 = 0;
      test_flag_sw2 = 0;

      // Update ADC data when ADC interrupt has completed
      if (adc_new_data){

          // Reset new data flag
          adc_new_data = 0;

          // Manually adjust the left detector value and assign to "left." Offset defined in macros.h.
          adjusted = (int)ADC_Left_Detect + LINE_LEFT_OFFSET;
          left = adjusted < 0 ? 0 : (unsigned int)adjusted;

          // Manually adjust the right detector value and assign to "right." Offset defined in macros.h.
          adjusted = (int)ADC_Right_Detect + LINE_RIGHT_OFFSET;
          right = adjusted < 0 ? 0 : (unsigned int)adjusted;

          // Assign ADC_Global value as center.
          center = ADC_global;

          // Indicates we have a new reading from our detectors, assigned to "left," "right," and "center."
          // This provides simple and adjusted assignments for processes to work with.
          fresh = 1;
          have_sample = 1;

          // adc is done for now
          adc_active = 0;
      }

      __enable_interrupt();

      // Run Line_Run_Stop() if switch 2 has been pressed.
      if (stop_request){
          Line_Run_Stop();
      }
      // Run Line_Run_Start() and start DAC if switch 1 has been pressed.
      else if (start_request){
          timer_start = now;       // Remember when switch 1 was pressed.
          timer_running = 1;
          Line_Run_Start();
          // Require a brand new sample obtained after this start request is initiated.
          fresh = 0;
      }

      Safety_Check();
      Line_Run_Process(now, fresh, left, right, center);

      if (!adc_active && (unsigned int)(now - last_adc) >= 2u){
          // This condition checks if (now - last_adc) >= 2u. Since line_ticks (now) updates
          // every 5 ms within Timer0_B0_ISR, 2u is a 10ms interval.
          // This equates to fresh IR data 100 times per second.
          last_adc = now;
          adc_active = 1;
          ADCCTL0 &= ~ADCENC;
          ADC_Channel = 0;
          ADCMCTL0 = ADCSREF_0 | ADCINCH_2;
          ADCCTL0 |= ADCENC;
          ADCCTL0 |= ADCSC;
      }

      if (have_sample && (unsigned int)(now - last_display) >= 40u) {
          // Update display every 200ms. (40 * 5 ms)
          last_display = now;

          // Update ADC readings on the Display, HEX_to_BCD function is in IRBoard.c
          HEX_to_BCD(left);

          strcpy(display_line[1], "ADCL:     ");
          display_line[1][6] = thousands;
          display_line[1][7] = hundreds;
          display_line[1][8] = tens;
          display_line[1][9] = ones;
          HEX_to_BCD(right);
          strcpy(display_line[2], "ADCR:     ");
          display_line[2][6] = thousands;
          display_line[2][7] = hundreds;
          display_line[2][8] = tens;
          display_line[2][9] = ones;

          // Update 200ms counter ticks on display.
          if (timer_running){
              elapsed_ticks = (unsigned int)(now - timer_start);
          }

          HEX_to_BCD(elapsed_ticks);
          strcpy(display_line[3], "Timer:    ");
          display_line[3][6] = thousands;
          display_line[3][7] = hundreds;
          display_line[3][8] = tens;
          display_line[3][9] = ones;
      }
    }
}
