/*
 * IRBoard.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

#include <msp430.h>
#include "macros.h"
#include "functions.h"
#include "ports.h"
#include "LCD.h"
#include <string.h>

// Global Variables:
volatile unsigned int ADC_global;
volatile unsigned int ADC_Left_Detect;
volatile unsigned int ADC_Right_Detect;
volatile unsigned int ADC_Channel = 0;
volatile unsigned int ADC_Thumb;
char thousands;
char hundreds;
char tens;
char ones;

// ------------------------ ADC Init -------------------------
void Init_ADC(void){

    ADCCTL0 = 0;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCON;

    ADCCTL1 = 0;
    ADCCTL1 |= ADCSHS_0;
    ADCCTL1 |= ADCSHP;
    ADCCTL1 |= ADCCONSEQ_0;

    ADCCTL2 = 0;
    ADCCTL2 |= ADCPDIV0;
    ADCCTL2 |= ADCRES_2;
    ADCCTL2 &= ~ADCDF;

    ADCMCTL0 = 0;
    ADCMCTL0 |= ADCSREF_0;
    ADCMCTL0 |= ADCINCH_2;

    ADCIE |= ADCIE0;
    ADCCTL0 |= ADCENC;
}

// ------------------------ ADC ISR ------------------------
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void){

    switch (__even_in_range(ADCIV, ADCIV_ADCIFG)){
        case ADCIV_ADCIFG:
            ADCCTL0 &= ~ADCENC;

            switch (ADC_Channel++){
                case 0:
                    ADC_Left_Detect = ADCMEM0 >> 2;
                    ADCMCTL0 = ADCSREF_0;
                    ADCMCTL0 |= ADCINCH_3;
                    ADC_Channel = 1;
                    ADCCTL0 |= ADCENC;
                    ADCCTL0 |= ADCSC;
                    break;

                case 1:
                    ADC_Right_Detect = ADCMEM0 >> 2;
                    ADCMCTL0 = ADCSREF_0;
                    ADCMCTL0 |= ADCINCH_4;
                    ADC_Channel = 2;
                    ADCCTL0 |= ADCENC;
                    ADCCTL0 |= ADCSC;
                    break;

                case 2:
                    ADC_global = ADCMEM0 >> 2;
                    ADCMCTL0 = ADCSREF_0;
                    ADCMCTL0 |= ADCINCH_2;
                    ADC_Channel = 0;
                    adc_new_data = 1;

                    ADCCTL0 |= ADCENC;
                    break;

                default:
                    ADC_Channel = 0;
                    ADCMCTL0 = ADCSREF_0;
                    ADCMCTL0 |= ADCINCH_2;
                    ADCCTL0 |= ADCENC;
                    break;
            }
            break;

        default:
            break;
    }
}


// --------- Hex (from ADC) to Decimal Converter --------
void HEX_to_BCD(unsigned int value)
{
    thousands = (value / 1000) + '0';
    value %= 1000;

    hundreds = (value / 100) + '0';
    value %= 100;

    tens = (value / 10) + '0';
    ones = (value % 10) + '0';
}

