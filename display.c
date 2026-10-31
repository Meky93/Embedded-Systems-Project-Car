/*
 * display.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

// Global Variables: display_line, display_changed, and display.


#include "msp430.h"
#include "macros.h"
#include "functions.h"
#include "LCD.h"
#include "ports.h"
#include <string.h>







void Display_Process(void){
  if(update_display){
    update_display = 0;
    if(display_changed){
      display_changed = 0;
      Display_Update(0,0,0,0);
    }
  }
}


void blink_lcd(void){
    if(P6OUT & LCD_BACKLITE){
        P6OUT &= ~LCD_BACKLITE;
    }
    else{
        P6OUT |= LCD_BACKLITE;
    }
}

void switch_display(void) {
    if(P4IFG & SW1){
        strcpy(display_line[2], "IR is ON  ");
        display_changed = TRUE;
        Display_Update(0,0,0,0);
    }
    else{
        strcpy(display_line[2], "IR is OFF ");
        display_changed = TRUE;
        Display_Update(0,0,0,0);

    }
}




