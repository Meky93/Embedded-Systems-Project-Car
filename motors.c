/*
 * motors.c
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */

#include <msp430.h>
#include "macros.h"
#include "functions.h"
#include "ports.h"
#include <string.h>

// Global Variables:
static unsigned char line_detected = 0;
extern volatile unsigned int ADC_Left_Detect;
extern volatile unsigned int ADC_Right_Detect;
unsigned int cycle_time = 0;
unsigned int limit;
unsigned int turning;
extern volatile unsigned int five_hun_ms_counter;
extern volatile unsigned int five_ms_counter;
static unsigned char demo_started = 0;
static unsigned int last_demo_count = 0xFFFF;
extern volatile unsigned int ADC_global;
extern unsigned int now;

// -------------------------------- Display Updating for States ---------------------------
static void set_line_status(const char *message){
    if (strcmp(display_line[0], message) != 0){       // Only update display when a change to the message has been made rather than continuously.
        strcpy(display_line[0], message);
        display_changed = TRUE;
        Display_Update(0, 0, 0, 0);
    }
}

// --------------------------------- Straight Event ----------------------------------------
void Run_Straight(void){
    switch(state){
        case WAIT:
            wait_case();
            break;
        case START:
            start_case();
            break;
        case RUN:
            run_case_straight();
            break;
        case END:
            end_case();
            break;
        default: break;
    }
}

void Forward_On(void)
{
    Stop();
    TB3CCTL2 = OUT;  // Right forward continuously ON.
    TB3CCTL4 = OUT;  // Left forward continuously ON.
}

void Turn_Right_Forward(void)
{
    Stop();
    TB3CCTL4 = OUT;  // Left forward ON; Right unpowered.
}


void Turn_Left_Forward(void)
{
    Stop();
    TB3CCTL2 = OUT;  // Right forward ON; Left unpowered.
}


void Spin_Clockwise(void)
{
    Stop();
    TB3CCTL4 = OUT;  // Left forward continuously ON.
    TB3CCTL3 = OUT;  // Right reverse continuously ON.
}


// ------------------ Case STOP --------------------------
void Stop(void){
    TB3CCTL2 = 0;
    TB3CCTL3 = 0;
    TB3CCTL4 = 0;
    TB3CCTL5 = 0;

    RIGHT_FORWARD_SPEED = 0;
    RIGHT_REVERSE_SPEED = 0;
    LEFT_FORWARD_SPEED = 0;
    LEFT_REVERSE_SPEED = 0;
}

// ------------------ Case WAIT --------------------------
void wait_case(void){
    if(!time_change){
        return;
    }
    time_change = 0;
    delay_start++;
    if(delay_start >= WAITING2START){
        delay_start = 0;
        state = START;
    }
}

// ------------------ Case START -------------------------
void start_case(void){
    cycle_time = 0;
    right_motor_count = 0;
    left_motor_count = 0;
    segment_count = 0;
    shape_phase = 0;
    Forward_On();
    state = RUN;
}

// ------------------ Case RUN (straight) ---------------
void run_case_straight(void){
    if(!demo_started){
        demo_started = 1;
        set_line_status(" STRAIGHT ");
    }
    if(!time_change){
        return;
    }
    time_change = 0;
    if(cycle_time == 0){
        Forward_On();
    }

    cycle_time++;
    if(cycle_time >= WHEEL_COUNT_TIME){
        cycle_time = 0;
        segment_count++;
    }
    if(segment_count >= TRAVEL_DISTANCE){
        Stop();
        state = END;
    }
}

// ------------------ Case SAFETY_CHECK ------------------
void Safety_Check(void){

    // If output is detected for both left forward and left reverse, immediately stop everything.
    if((TB3CCTL4 & OUT) && (TB3CCTL5 & OUT)){
        Stop();
        state = END;
        P1OUT |= RED_LED;
    }

    // If output is detected for both right forward and right reverse, immediately stop everything.
    if((TB3CCTL2 & OUT) && (TB3CCTL3 & OUT)){
        Stop();
        state = END;
        P1OUT |= RED_LED;
    }
}

// ------------------ Case END ---------------------------
void end_case(void){
    Stop();
    state = WAIT;
    demo_started = 0;
}


// ----------------------------------------------- Line Run State Machine -----------------------------------------------------------------------------

// enum as in assign a number in sequence to the listed variables in order.
// These represents each consecutive state in this Line_Run_Process state machine.
enum {LR_IDLE, LR_DAC, LR_APPROACH, LR_PAUSE, LR_TURN, LR_AFTER_TURN, LR_FOLLOW, LR_EXIT_PAUSE, LR_EXIT_TURN, LR_EXIT_AFTER_TURN, LR_EXIT_FORWARD};

static unsigned char line_state = LR_IDLE;
static unsigned char line_motion = 0;
static unsigned int line_started;

void Line_Run_Stop(void){
    Stop();
    line_state = LR_IDLE;
    line_motion = 0;
    event = NONE;
    TB0CTL &= ~TBIE;
    DAC_data = DAC_Begin;
    SAC3DAT = DAC_data;
    P2OUT &= ~(DAC_ENB | IR_LED);
    SAC3DAC &= ~DACEN;
}

void Line_Run_Start(void){
    if (line_state != LR_IDLE) return;
    Stop();
    line_motion = 0;
    state = WAIT;
    event = STRAIGHT;
    TB0CTL &= ~TBIE;
    DAC_data = DAC_Begin;
    SAC3DAT = DAC_data;
    SAC3DAC |= DACEN;
    P2OUT |= DAC_ENB | IR_LED;
    TB0CTL &= ~TBIFG;
    TB0CTL |= TBIE;
    line_state = LR_DAC;
}

void Line_Run_Process(unsigned int now, unsigned char fresh, unsigned int left, unsigned int right, unsigned int center){

    unsigned char requested;

    if (state == END && line_state != LR_IDLE){
        Line_Run_Stop();
        return;
    }

    switch (line_state){

        case LR_DAC:

            set_line_status("Starting..");

            if (DAC_data == DAC_Limit && fresh){
                Forward_On();
                line_state = LR_APPROACH;
            }
            break;

        case LR_APPROACH:

            set_line_status("Intrcpting");

            if (fresh && center > LINE_DETECT_THRESHOLD){

                // -- Continue approaching until center detect goes past threshold, indicating line has been detected.
                // -- Reset timer, continue to LR_PAUSE.

                Stop();
                line_started = now;
                line_state = LR_PAUSE;
            }
            break;

        case LR_PAUSE:

            set_line_status("Waiting...");

            // -- Drop the DAC value early so it actively steps down/settles during the pause ---
            if ((unsigned int)(now - line_started) == 0){
                DAC_data = DAC_Begin;                                           // Force voltage to 2V ASAP!
                SAC3DAT = DAC_data;                                             // Write to hardware register.
            }

            if ((unsigned int)(now - line_started) >= LINE_PAUSE_TICKS){

                // -- Now that we know the DAC out voltage has physically dropped to 2V, we can safely engage spin_clockwise! --
                // -- This entire sequence does not need to be repeated for Turn_Right_Forward calls for example, --------------
                // -- since the forward momentum can continue where it left off and switch dynamically, unlike fully -----------
                // -- stopping and reversing a motor like here: ----------------------------------------------------------------

                Spin_Clockwise();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_TURN;
            }
            break;

        case LR_TURN:

            set_line_status(" Turning  ");

            if ((unsigned int)(now - line_started) >= LINE_TURN_TICKS){

                // -- The line_state stays in LR_Turn until the number of --
                // -- of 5ms ticks defined by LINE_TURN_TICKS in macros.h --
                // -- then resets timer and continunes to LR_AFTER_TURN. ---

                Stop();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_AFTER_TURN;
            }
            break;

        case LR_AFTER_TURN:

            set_line_status("Intrcpting");

            if ((unsigned int)(now - line_started) >= LINE_PAUSE_TICKS){

                // -- The line_state stays in LR_AFTYER_Turn until the number --
                // -- of 5ms ticks defined by LINE_PAUSE_TICKS in macros.h -----
                // -- then resets timer and continunes to LR_FOLLOW. -----------

                line_motion = 0;
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_FOLLOW;
            }
            break;

        case LR_FOLLOW:

            set_line_status(" Circling ");

            if ((unsigned int)(now - line_started) >= LINE_FOLLOW_TICKS){

                // -- The line_state stays in LR_FOLLOW until the number -----
                // -- of 5ms ticks defined by LINE_FOLLOW_TICKS in macros.h --
                // -- which is the the total time needed to make the entire --
                // -- 2 Circles in Project 7 which was ~29.6s ----------------

                Stop();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_EXIT_PAUSE;
                break;
            }

            if (!fresh) break;

            // If neither left detector drops below the threshold or both do,
            // indicating either the line is "currently captured" or "no line has been captured yet," respectively,
            // requested is set = to 1 by default, which in turn simply calls Forward_On().
            requested = 1;

            // If the left detector drops below the threshold and the right detector has not,
            // indicating a drift to the left of the black line,
            // requested is set = to 2, which in turn calls Turn_Right_Forward() in the next "if" statement.
            if (left < LINE_STEER_THRESHOLD && right >= LINE_STEER_THRESHOLD)
                requested = 2;

            // If the right detector drops below the threshold and the right detector has not,
            // indicating a drift to the right of the black line,
            // requested is set = to 3, which in turn calls Turn_Left_Forward() in the next "if" statement.
            else if (right < LINE_STEER_THRESHOLD && left >= LINE_STEER_THRESHOLD)
                requested = 3;

            // If the right detector and left detector suddenly both lose the line and drops below threshold,
            // indicating a sharp drift off of the line, which since the car is going clockwise, most likwly means
            // a sharp drift to the left, we set requested = to 4, which also calls Turn_Right_Forward() in the next "if" statement.
            // **For the time being**
            else if (left < LINE_STEER_THRESHOLD && right < LINE_STEER_THRESHOLD)
                requested = 4;

            if (requested != line_motion){
                if (requested == 1){                                    // Explicitly handle return to center case from non center previous case.
                    Forward_On();
                    set_line_status(" Circling ");
                }
                else if (requested == 2){
                    Turn_Right_Forward();
                    set_line_status("Searching!");
                }
                else if (requested == 3){
                    Turn_Left_Forward();
                    set_line_status("Searching!");
                }
                else if (requested == 4){
                    Turn_Right_Forward();
                    set_line_status("Searching!");
                }
                else{
                    Forward_On();
                    set_line_status(" Circling ");
                }
            }
            line_motion = requested;                                    // "requested" is either a 1, 2, 3, or 4, based on IR Detector Threshold values
            break;

        case LR_EXIT_PAUSE:

            set_line_status("Waiting...");

            // -- Drop the DAC value early so it actively steps down/settles during the pause --
            if ((unsigned int)(now - line_started) == 0){
                DAC_data = DAC_Begin;                                           // Force voltage to 2V ASAP!
                SAC3DAT = DAC_data;                                             // Write to hardware register.
            }

            if ((unsigned int)(now - line_started) >= LINE_PAUSE_TICKS){

                // -- Now that we know the DAC out voltage has physically dropped to 2V, we can safely engage spin_clockwise! --
                // -- This entire sequence does not need to be repeated for Turn_Right_Forward calls for example, --------------
                // -- since the forward momentum can continue where it left off, unlike fully stopping and ---------------------
                // -- reversing a motor like we're about to do here: -----------------------------------------------------------

                Spin_Clockwise();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_EXIT_TURN;
            }
            break;

        case LR_EXIT_TURN:

            set_line_status(" Turning  ");

            if ((unsigned int)(now - line_started) >= LINE_TURN_TICKS){

                // -- The line_state stays in LR_EXIT_TURN until the number of --
                // -- of 5ms ticks defined by LINE_TURN_TICKS in macros.h -------
                // -- then resets timer and continunes to LR_EXIT_AFTER_TURN. ---

                Stop();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_EXIT_AFTER_TURN;
            }
            break;

        case LR_EXIT_AFTER_TURN:

            set_line_status("Waiting...");

            if ((unsigned int)(now - line_started) >= LINE_PAUSE_TICKS){
                Forward_On();
                line_started = now;                                             // Update line_started with the current time!
                line_state = LR_EXIT_FORWARD;
            }
            break;

        case LR_EXIT_FORWARD:

            set_line_status("Finishing!");

            if ((unsigned int)(now - line_started) >= LINE_EXIT_FORWARD_TICKS){
                Line_Run_Stop();
                P6OUT &= ~LCD_BACKLITE;
                set_line_status(" Stopped  ");
            }
            break;

        default:
            break;
        }                       // switch-case statement closed
}                               // Line_Run_Process closed





// ------------------------------------------------------------ Unused for Now -----------------------------------------------------------------------------------------


/*

// ----------------- Case Forward ---------------------------------
void Forward_PWM(unsigned int left, unsigned int right)
{
    if ((left == 0) || (left >= WHEEL_PERIOD) ||
        (right == 0) || (right >= WHEEL_PERIOD)) {
        Stop();
        return;
    }

    // Disable reverse outputs before enabling forward.
    TB3CCTL3 = 0;
    TB3CCTL5 = 0;
    RIGHT_REVERSE_SPEED = 0;
    LEFT_REVERSE_SPEED = 0;

    LEFT_FORWARD_SPEED = left;
    RIGHT_FORWARD_SPEED = right;

    TB3CCTL4 = OUTMOD_7;
    TB3CCTL2 = OUTMOD_7;
}

void Turn_Right_Forward(void)
{
    Forward_PWM(10000, 3000);  // Left 60%, right 30%.
}

// ------------------ Case REVERSE -----------------------

void Reverse_On(void){
    Stop();

    RIGHT_REVERSE_SPEED = WHEEL_PERIOD;
    LEFT_REVERSE_SPEED = WHEEL_PERIOD;
}


// ------------------ Case CLOCKWISE ---------------------
// Call after stopping and allowing a direction-change pause.
void Spin_Clockwise(void)
{
    Stop();

    LEFT_FORWARD_SPEED = 5000;
    RIGHT_REVERSE_SPEED = 5000;

    TB3CCTL4 = OUTMOD_7;  // Left forward.
    TB3CCTL3 = OUTMOD_7;  // Right reverse.
}

// ------------------ Case COUNTER-CLOCKWISE -------------
void Spin_CounterClockwise(void) {
    Stop();

    LEFT_FORWARD_SPEED = 0;
    RIGHT_FORWARD_SPEED = WHEEL_PERIOD / 2;

    RIGHT_REVERSE_SPEED = 0;
    LEFT_REVERSE_SPEED = WHEEL_PERIOD / 2;

    Safety_Check();

}

// ------------------ Case Black Line Detected -----------
void black_detect(void){
    strcpy(display_line[0], "Line Found");
    display_changed = TRUE;
    Display_Update(0,0,0,0);

    switch(five_hun_ms_counter){

        case 0:
            Stop();
            break;
        case 15:
            strcpy(display_line[0], "Searching!");
            display_changed = TRUE;
            Display_Update(0, 0, 0, 0);
            Stop();

            Spin_Clockwise();
            break;
        case 17:
            Stop();
            Safety_Check();

        case 24:
            strcpy(display_line[0], "Line Found");
            display_changed = TRUE;
            Display_Update(0,0,0,0);

            end_case();
    }
}

// ------------------------ Forward Right -------------------------
//void Forward_Right(void){

//        TB3CCTL2 = OUT;       // P6.1 right forward
//        TB3CCTL4 = OUT;       // P6.3 left forward
//    }
//}

/*
void run_proj6(void){
    switch(state){
        case WAIT:
            wait_case();
            break;
        case START:
            start_case();
            break;
        case RUN:
            proj_6_demo();
            break;
        case END:
            end_case();
            break;
        default: break;
    }
}
*/


/*
void Run_Circle(void){
    switch(state){
        case WAIT:
            wait_case();
            break;
        case START:
            start_case();
            break;
        case RUN:
            run_case_circle();
            break;
        case END:
            event = TRIANGLE;
            end_case();
            break;
        default: break;
    }
}
*/

/*
void Run_Figure8(void){
    switch(state){
        case WAIT:
            wait_case();
            break;
        case START:
            start_case();
            break;
        case RUN:
            run_case_figure8();
            break;
        case END:
            end_case();
            break;
        default: break;
    }
}
*/


/*
void Run_Triangle(void){
    switch(state){
        case WAIT:
            wait_case();
            break;
        case START:
            start_case();
            break;
        case RUN:
            run_case_triangle();
            break;
        case END:
            event = FIGURE8;
            end_case();
            break;
        default: break;
    }
}
*/




/*
void run_case_circle(void){
    if(!demo_started){

        demo_started = 1;

        strcpy(display_line[0], "  CIRCLE  ");
        display_changed = TRUE;
        Display_Update(0,0,0,0);
    }

    if(!time_change){
        return;
    }

    time_change = 0;

    if(cycle_time == 0){
        Forward_Move();
    }

    // Right motor runs for 60 ms
    if(cycle_time == RIGHT_COUNT_TIME){
        RIGHT_FORWARD_SPEED = 0;
    }

    // Left motor runs for 25 ms, causing lefdt turn
    if(cycle_time == CIRCLE_LEFT_TIME){
        LEFT_FORWARD_SPEED = 0;
    }

    cycle_time++;

    // Complete one motor cycle, reset cycle_time, increment segment_count.
    if(cycle_time >= WHEEL_COUNT_TIME){

        cycle_time = 0;
        segment_count++;

        if(segment_count >= CIRCLE_CYCLES){

            segment_count = 0;
            shape_phase++;

            if(shape_phase >= 2){
                state = END;
            }
        }
    }

    // Stop after 50 cycles (approximately 5 seconds)
    if(segment_count >= TRAVEL_DISTANCE){
        Stop();
        state = END;
    }
}
*/

/*
void run_case_figure8(void){

    if(!demo_started){

        demo_started = 1;

        strcpy(display_line[0], "  FIGURE8 ");
        display_changed = TRUE;
        Display_Update(0,0,0,0);
    }

    if(!time_change){
        return;
    }

    time_change = 0;

    if(cycle_time == 0){
        Forward_Move();
    }

    // Alternate left and right circles
    if(shape_phase % 2 == 0){

        // Left circle: right wheel faster
        if(cycle_time == RIGHT_COUNT_TIME-4){
            RIGHT_FORWARD_SPEED = 0;
        }

        if(cycle_time == (CIRCLE_LEFT_TIME-3)){
            LEFT_FORWARD_SPEED = 0;
        }

    } else {

        // Right circle: left wheel faster
        if(cycle_time == (CIRCLE_LEFT_TIME-3)){
            RIGHT_FORWARD_SPEED = 0;

        }

        if(cycle_time == RIGHT_COUNT_TIME-4){
            LEFT_FORWARD_SPEED = 0;
        }
    }

    cycle_time++;

    if(cycle_time >= WHEEL_COUNT_TIME-4){

        cycle_time = 0;
        segment_count++;

        if(segment_count >= CIRCLE_CYCLES-7){

            segment_count = 0;
            shape_phase++;

            if(shape_phase >= 4){
                state = END;
            }
        }
    }
}
*/


/*
void run_case_triangle(void){

    if(!demo_started){
        demo_started = 1;

        strcpy(display_line[0], " TRIANGLE ");
        display_changed = TRUE;
        Display_Update(0,0,0,0);
    }

    if(!time_change){
        return;
    }

    time_change = 0;




    if(shape_phase % 2 == 0){

        // ---------------- STRAIGHT ----------------

        if(cycle_time == 0){
            Forward_Move();
        }

        // Your current straight settings
        if(cycle_time == RIGHT_COUNT_TIME){
            RIGHT_FORWARD_SPEED = 0;
        }

        if(cycle_time == LEFT_COUNT_TIME){
            LEFT_FORWARD_SPEED = 0;
        }

    }

    else{

        // ---------------- TURN ----------------

        if(cycle_time == 0){
            Forward_Move();
        }


         // Left turn:

         // Right wheel = outside wheel = faster
         // Left = inside wheel = slower


        if(cycle_time == TRIANGLE_INNER_TIME){
            LEFT_FORWARD_SPEED = 0;
        }

        if(cycle_time == TRIANGLE_OUTER_TIME){
            RIGHT_FORWARD_SPEED = 0;
        }
    }


    // Advance PWM cycle
    cycle_time++;


    // One complete wheel-control cycle
    if(cycle_time >= WHEEL_COUNT_TIME){

        cycle_time = 0;
        segment_count++;


        // STRAIGHT phase completed
        if((shape_phase % 2 == 0) &&
           (segment_count >= TRIANGLE_STRAIGHT_CYCLES)){

            segment_count = 0;
            shape_phase++;
        }


        // TURN phase completed
        else if((shape_phase % 2 == 1) &&
                (segment_count >= TRIANGLE_TURN_CYCLES)){

            segment_count = 0;
            shape_phase++;
        }


        // 12 phases = TWO complete triangles
        if(shape_phase >= 12){

            state = END;
            end_case();
        }
    }
}
*/


/*
void proj_five_demo(void) {

      if(!demo_started){
          five_hun_ms_counter = 0;
          five_ms_counter = 0;

          last_demo_count = 0xFFFF;
          demo_started = 1;

          strcpy(display_line[0], "Proj5 Demo");
          display_changed = TRUE;
          Display_Update(0,0,0,0);
      }

      // Do nothing unless the 1s count changed
      if(five_hun_ms_counter == last_demo_count){
          return;
      }

      last_demo_count = five_hun_ms_counter;

  switch(five_hun_ms_counter){

      // After 3s, move forward for 1s.
      case 5:

         Stop();
         Forward_On();
         Safety_Check();

         strcpy(display_line[1], "1-Forward ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

         break;

      // Pause for 1s.
      case 10:

         Stop();

         strcpy(display_line[1], "1-Pause   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);

         break;

      // Go Reverse for 2s.
      case 15:

         Stop();
         Reverse_On();
         Safety_Check();

         strcpy(display_line[1], "2-Reverse ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

          break;

      // Pause 1s.
      case 25:
         Stop();

         strcpy(display_line[1], "2-Pause   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

          break;

      // Forward 1s.
      case 30:
         Forward_On();
         Safety_Check();

         strcpy(display_line[1], "3-Forward ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

          break;

      // Pause 1s.
      case 35:
         Stop();

         strcpy(display_line[1], "3-Pause   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

          break;

      // Rotate counter-clockwise for 3s.
      case 40:
         Stop();
         Spin_Clockwise();
         Safety_Check();

         strcpy(display_line[1], "4-RotateR ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

         break;

      // Pause for 2s.
      case 55:

         Stop();

         strcpy(display_line[1], "4-Pause   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

         break;

      // Rotate Counter-Clockwise for 3s.
      case 65:
         Stop();
         Spin_CounterClockwise();
         Safety_Check();

         strcpy(display_line[1], "5-RotateL ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

          break;

      // Pause for 2s.
      case 75:

         Stop();

         strcpy(display_line[1], "5-Pause   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;
         break;

      // Finished - prints "DONE" to screen then turns it off.
      case 85:
         Stop();

         strcpy(display_line[1], "   DONE   ");
         display_changed = TRUE;
         Display_Update(0,0,0,0);;

         P6OUT &= ~LCD_BACKLITE;

         demo_started = 0;
         end_case();
         break;

      default:
         break;


  }
}
*/


/*
void proj_6_demo(void)
{
    if (!demo_started) {
        five_hun_ms_counter = 0;
        five_ms_counter = 0;
        last_demo_count = 0xFFFF;
        demo_started = 1;
        line_detected = 0;

        strcpy(display_line[1], "Proj6 Demo");
        strcpy(display_line[0], "Searching!");
        display_changed = TRUE;
        Display_Update(0, 0, 0, 0);
    }

    if (!line_detected) {
        if (ADC_Left_Detect > 500) {
            line_detected = 1;
            five_hun_ms_counter = 0;
            Stop();
        } else {
            Forward_On();
        }
    } else {
        black_detect();
    }
}
*/
