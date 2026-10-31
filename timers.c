/*
 * timers.c
 *
 *  Created on: Oct 5, 2026
 *      Author: Jake
 */

#include "functions.h"
#include "macros.h"
#include "ports.h"
#include "LCD.h"

// Global Variables:
extern volatile unsigned int DAC_data;
extern volatile unsigned int five_ms_counter;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int five_hun_ms_counter;
volatile unsigned int line_ticks = 0;

void Init_Timers(void){
    Init_Timer_B0();
    Init_Timer_B3();
}

//------------------------- Timer B0 - Vector B0 - CCR0 dedicated Interrupt Service Routine! ----------------------
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void){

    line_ticks++;
    five_ms_counter++;
    Time_Sequence++;

    TB0CCR0 += TB0CCR0_INTERVAL;
}

//----------- Timer B0 - Vector B1 - CCR1, CCR2, & Overflow chosen by TB0IV - Interrupt Service Routine! ----------
#pragma vector = TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR(void){

    switch (TB0IV){

        case TB0IV_TBCCR1:
            break;

        case TB0IV_TBCCR2:
            break;

        case TB0IV_TBIFG:
            if (DAC_data > DAC_TARGET){
                if (DAC_data <= DAC_TARGET + DAC_STEP){
                    DAC_data = DAC_TARGET;
                }
                else{
                    DAC_data -= DAC_STEP;
                }
                SAC3DAT = DAC_data;
            }
            break;

        default:
            break;
    }
}

//------------------------------ Timer B0 Configuration --------------------------------------
void Init_Timer_B0(void){

    TB0CTL = TBSSEL__SMCLK;             // SMCLK source
    TB0CTL |= ID_1;                     // Divide clock by 2
    TB0CTL |= MC__CONTINUOUS;           // Continuous up
    TB0CTL |= TBCLR;

    TB0EX0 = TBIDEX_7;                  // Divide clock by an additional 8

    TB0CCR0 = TB0R + TB0CCR0_INTERVAL;  // CCR0 initial interval
    TB0CCTL0 = CCIE;                    // CCR0 enable interrupt

    TB0CCR1 = TB0R + TB0CCR1_INTERVAL;  // CCR1 initial interval
    TB0CCTL1 = 0;                       // CCR1 interrupt disabled initially

    TB0CCR2 = TB0R + TB0CCR2_INTERVAL;  // CCR2 initial interval
    TB0CCTL2 = 0;                       // CCR2 interrupt disabled initially

    TB0CTL &= ~TBIE;                    // Disable Overflow Interrupt
    TB0CTL &= ~TBIFG;                   // Clear Overflow Interrupt flag
}

//------------------------------ Timer B3 Configuration --------------------------------------
void Init_Timer_B3(void){

    TB3CTL = TBSSEL__SMCLK;             // SMCLK source
    TB3CTL |= MC__UP;                   // MC_UP
    TB3CTL |= TBCLR;

    TB3CCR0 = WHEEL_PERIOD;

    TB3CCTL2 = 0;                       // P6.1 right forward inactive
    TB3CCTL3 = 0;                       // P6.2 right reverse inactive
    TB3CCTL4 = 0;                       // P6.3 left forward inactive
    TB3CCTL5 = 0;                       // P6.4 left reverse inactive

    RIGHT_FORWARD_SPEED = 0;
    RIGHT_REVERSE_SPEED = 0;
    LEFT_FORWARD_SPEED = 0;
    LEFT_REVERSE_SPEED = 0;
}


