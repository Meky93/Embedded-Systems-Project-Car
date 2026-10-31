/*
 * dac.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

#include  "functions.h"
#include  "msp430.h"
#include  "macros.h"
#include  "ports.h"

// Global Variables:
volatile unsigned int DAC_data;

void Init_DAC(void){

    DAC_data = DAC_Begin;

    SAC3DAC  = DACSREF_0;
    SAC3DAC |= DACLSEL_0;
    SAC3DAC |=  DACEN;

    SAC3OA   = NMUXEN;
    SAC3OA  |= PMUXEN;
    SAC3OA  |= PSEL_1;
    SAC3OA  |= NSEL_1;
    SAC3OA  |= OAPM;
    SAC3OA  |= SACEN;
    SAC3OA  |= OAEN;

    SAC3PGA  = MSEL_1;
    SAC3DAT  = DAC_data;

    P2OUT &= ~DAC_ENB;                          // Low - external path disabled, assuming DAC_ENB is active-high

    // ---- To activate correct Timer B0 Overflow timing, we must set the TBIE bit. ----
    // ---- Setting a bit high enables the interrupt (Active-High). --------------------

    TB0CTL &= ~TBIE;                            // Start with the overflow interrupt DISABLED UNTIL Line_Run_Start() enables it!
    P1OUT |= RED_LED;
}

// The interrupt is not used
#pragma vector = SAC1_SAC3_VECTOR
__interrupt void SAC3_ISR(void){

  switch(__even_in_range(SAC0IV,SACIV_4)){
  case SACIV_0: break;
  case SACIV_2: break;
  case SACIV_4:
//   DAC_data++;
//   DAC_data &= 0xFFF;
//   SAC3DAT = DAC_data;                 // DAC12 output positive ramp
    break;
  default: break;
  }
}
