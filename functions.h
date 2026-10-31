/*
 * functions.h
 *
 *  Created on: Sep 10, 2026
 *  Author: Jake Lewis
 *  Compiler: Code Composer 12.8.1
 */
// Global Variables: display_changed, update_display, update_display_count, Time_Sequence, one_time.

// Main
void main(void);

// Initialization
void Init_Conditions(void);


// Interrupts
void enable_interrupts(void);
__interrupt void Timer0_B0_ISR(void);
__interrupt void switch_interrupt(void);

// Analog to Digital Converter

// Clocks
void Init_Clocks(void);

// LED Configurations
void Init_LEDs(void);
void IR_LED_control(char selection);
void Backlite_control(char selection);

  // LCD
void Display_Process(void);
void Display_Update(char p_L1,char p_L2,char p_L3,char p_L4);
void enable_display_update(void);
void update_string(char *string_data, int string);
void lcd_clear(void);
void lcd_putc(char c);
void lcd_puts(char *s);

void lcd_power_on(void);
void lcd_write_line1(void);
void lcd_write_line2(void);
//void lcd_draw_time_page(void);
//void lcd_power_off(void);
void lcd_enter_sleep(void);
void lcd_exit_sleep(void);
//void lcd_write(unsigned char c);
//void out_lcd(unsigned char c);

void Write_LCD_Ins(char instruction);
void Write_LCD_Data(char data);
void ClrDisplay(void);
void ClrDisplay_Buffer_0(void);
void ClrDisplay_Buffer_1(void);
void ClrDisplay_Buffer_2(void);
void ClrDisplay_Buffer_3(void);

void SetPostion(char pos);
void DisplayOnOff(char data);
void lcd_BIG_mid(void);
void lcd_BIG_bot(void);
void lcd_120(void);

void lcd_4line(void);
void lcd_out(char *s, char line, char position);
void lcd_rotate(char view);

void update_adc_display(void);


//void lcd_write(char data, char command);
void lcd_write(unsigned char c);
void lcd_write_line1(void);
void lcd_write_line2(void);
void lcd_write_line3(void);

void lcd_command( char data);
void LCD_test(void);
void LCD_iot_meassage_print(int nema_index);

// Menu
void Menu_Process(void);

// Ports
void Init_Ports(void);
void Init_Port1(void);
void Init_Port2(void);
//void Init_Port3(char smclk);
void Init_Port3(void);
void Init_Port4(void);
void Init_Port5(void);
void Init_Port6(void);

// Motors
void Stop(void);
void Forward_On(void);
void Reverse_On(void);
void Forward_Move(void);
void wait_case(void);
void start_case(void);
void end_case(void);


//Shapes
void Run_Straight(void);
void run_case_straight(void);
void Run_Circle(void);
void run_case_circle(void);
void Run_Triangle(void);
void run_case_triangle(void);
void Run_Figure8(void);
void run_case_figure8(void);

// SPI
void Init_SPI_B1(void);
void SPI_B1_write(char byte);
void spi_rs_data(void);
void spi_rs_command(void);
void spi_LCD_idle(void);
void spi_LCD_active(void);
void SPI_test(void);
void WriteIns(char instruction);
void WriteData(char data);

// Switches
void Init_Switches(void);
void switch_control(void);
void enable_switch_SW1(void);
void enable_switch_SW2(void);
void disable_switch_SW1(void);
void disable_switch_SW2(void);
void Switches_Process(void);
void Init_Switch(void);
void Switch_Process(void);
void Switch1_Process(void);
void Switch2_Process(void);
void menu_act(void);
void menu_select(void);
void Port_2_ISR(void);
void Port_4_ISR(void);

// Timers
void Init_Timers(void);
void Init_Timer_B0(void);
void Init_Timer_B1(void);
void Init_Timer_B2(void);
void Init_Timer_B3(void);

void usleep(unsigned int usec);
void usleep10(unsigned int usec);
void five_msec_sleep(unsigned int msec);
void measure_delay(void);
void out_control_words(void);

void proj_five_demo(void);


// IR Board Functions
extern volatile unsigned int adc_value;

void Init_ADC(void);
unsigned int ADC_Read_Thumb(void);
void HEX_to_BCD(unsigned int value);

// Function Prototypes
void main(void);
void Carlson_StateMachine(void);

void Init_Clocks(void);
void Switches_Process(void);
void Safety_Check(void);

void switch_display(void);

// Global Variables
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
extern volatile unsigned char smclk_mode;
extern volatile unsigned int five_hun_ms_counter;
extern volatile unsigned int five_ms_counter;
extern volatile unsigned int debounce_count_sw1;
extern volatile unsigned int debounce_count_sw2;
extern volatile unsigned char debouncing_currently_sw1;
extern volatile unsigned char debouncing_currently_sw2;
//extern static unsigned char demo_started;
//extern static unsigned int last_demo_count;



extern char display_line[4][11];
extern char *display[4];


extern volatile unsigned int ADC_Thumb;
extern volatile unsigned char adc_new_data;

void Init_ADC(void);
void HEX_to_BCD(unsigned int value);

void proj6demo(void);
void proj_6_demo(void);
void black_detect(void);
void run_proj6(void);



void Init_Timer_B3(void);
void Init_DAC(void);
extern volatile unsigned int DAC_data;
void Turn_Right_Forward(void);
void Turn_Left_Forward(void);
void Spin_Clockwise(void);
void Spin_CounterClockwise(void);



extern volatile unsigned int line_ticks;
void Line_Run_Start(void);
void Line_Run_Stop(void);
void Line_Run_Process(unsigned int now, unsigned char fresh,
                      unsigned int left, unsigned int right, unsigned int center);



