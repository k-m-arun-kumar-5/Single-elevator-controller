/**************************************************************************
   FILE          :    io_conf.h
 
   PURPOSE       :   main peripherial configuration Header
 
   AUTHOR        :  K.M. Arun Kumar alias Arunkumar Murugeswaran  
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _IO_CONF_H
 #define _IO_CONF_H
 

/* -------------------------------- OSC Freq conf -------------------------------------*/
 #define _XTAL_FREQ                              (12000000)
 
 /* ------------------------------ Number of Device Channels ----------------------------- */
#define NUM_COMM_DEV_ID_UART_CHS                  (2)
#define NUM_COMM_DEV_ID_I2C_CHS                   (1)
#define NUM_COMM_DEV_ID_SPI_CHS                   (1)
#define NUM_COMM_DEV_ID_MISC_CHS                  (1)  
#define NUM_INPUT_DEV_ID_MISC_CHS            (1)          
#define NUM_INPUT_DEV_ID_ADC_CHS             (1) 
#define NUM_INPUT_DEV_ID_KEYBOARD_CHS        (1)
#define NUM_INPUT_DEV_ID_SW_CHS              (29)
#define NUM_OUTPUT_DEV_ID_LED_MATRIX_CHS     (1)
#define NUM_OUTPUT_DEV_ID_DAC_CHS            (1)
#define NUM_OUTPUT_DEV_ID_PWM_CHS            (1)
#define NUM_OUTPUT_DEV_ID_LCD_CHS            (1)
#define NUM_OUTPUT_DEV_ID_SEG7_CHS           (1)
#define NUM_TIMER_DEV_ID_CHS                 (2)

/* --------------------------------- module enable conf ---------------------------------*/ 
 #define TIMER_MOD_ENABLE                      (1)
 #define INTERRUPT_MOD_ENABLE                  (2)
// #define LCD_MOD_ENABLE                        (4)
// #define KEYBOARD_MOD_ENABLE                   (5)
// #define CAPTURE_MOD_ENABLE                     (6)
// #define COMPARE_MOD_ENABLE                     (7)
// #define PWM_MOD_ENABLE                         (8)
 #define UART_MOD_ENABLE                          (9)
// #define ADC_MOD_ENABLE                         (10)
// #define GSM_MOD_ENABLE                         (11)
// #define COMPARATOR1_MOD_ENABLE                 (12)
// #define COMPARATOR2_MOD_ENABLE                 (13)
// #define SPI_MOD_ENABLE                         (14)
// #define I2C_MOD_ENABLE                         (15) 
// #define WATCHDOG_TIMER_MOD_ENABLE              (16)
 #define SW_MOD_ENABLE                          (17)
 #define SEG7_MOD_ENABLE                        (18)
// #define DAC_MOD_ENABLE                         (19)
// #define OUTPUT_MISC_MOD_ENABLE                 (20)
// #define INPUT_MISC_MOD_ENABLE                  (21)
// #define MISC_MOD_ENABLE                        (22)
// #define LED_MATRIX_MOD_ENABLE                  (23)

#ifdef SW_MOD_ENABLE 
   #include "sw_lib.h"
#endif

#ifdef KEYBOARD_MOD_ENABLE 
  #ifndef SW_MOD_ENABLE
    #define SW_MOD_ENABLE                          (17) 
    #include "sw_lib.h"
  #endif
  #include "keyboard.h"    
#endif

#ifdef SEG7_MOD_ENABLE
  #include "seg7_lib.h"
#endif
 
#ifdef LED_MATRIX_MOD_ENABLE
  #include "led_matrix.h"
#endif

#ifdef LCD_MOD_ENABLE
   #include "lcd.h"
#endif
 
#ifdef UART_MOD_ENABLE
  #include "uart.h"
#endif
 
#ifdef TIMER_MOD_ENABLE
  #include "timer.h"
#endif
 
#ifdef INTERRUPT_MOD_ENABLE
  #include "isr.h"
#endif
  
/* ------------------------------- LCD oper conf ---------------------------------------*/ 
#ifdef LCD_MOD_ENABLE

 #define MAX_LCD_DIGITS                    (10) 
 
#define MAX_ICOUNT_LCD_1500MS              (2000)
#define MAX_JCOUNT_LCD_1500MS              (2000)
 
#define MAX_ICOUNT_LCD_10MS              (400)
#define MAX_JCOUNT_LCD_10MS              (400)

#define MAX_ICOUNT_LCD_1MS               (100) 
#define MAX_JCOUNT_LCD_1MS               (100)

// for JHD162A series LCD parameters
#define LCD_WRITE_ENABLE_PULSE_IN_USEC       (1)
#define LCD_ADDR_SETUP_IN_USEC               (1)
#define LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC (1)
#define LCD_DATA_SETUP_IN_USEC               (1)
#define LCD_WRITE_ENABLE_PULSE_DELAY_IN_USEC (1)
#define LCD_CLEAR_EXEC_IN_USEC             (1650)
#define LCD_CURSOR_RETURN_EXEC_IN_USEC     (1650)
#define LCD_OTHER_INST_EXEC_IN_USEC          (40) 

#endif

/* -------------------------------- timer oper conf ------------------------------------*/
#ifdef TIMER_MOD_ENABLE
 
#define TIMER_SET_TIME_DELAY_IN_MULTIPLE                       TIMER_PRESET_TIME_DELAY_IN_MULTIPLE
#define TIMER_0_TICK_FOR_1_MILLI_SEC                                (15U)  
//in depends on Osc freq
#define TIMER_1_TICK_FOR_1_MILLI_SEC                               (15U)  
//in depends on Osc freq
#define TIMER_0_MIN_TIMEOUT_IN_MS                                    (50)
#define TIMER_1_MIN_TIMEOUT_IN_MS                                  (500)
#endif
  
/* ------------------------------- uart oper conf -------------------------------------- */
#ifdef UART_MOD_ENABLE

#define MAX_UART_DIGITS                      (31) 
#define LCD_BEGIN_LOC_ECHO_UART_RCVD_STR      BEGIN_LOC_LINE2

#endif

/* ------------------------------- 7 segment LED oper conf ------------------------------*/
#ifdef  SEG7_MOD_ENABLE

#define MAX_SEG7_DIGITS                   SEG7_6_DIGIT 
#define REQ_CNT_SEG7_REPEAT_NUM                  (1)
#define MAX_ICOUNT_7SEG_SW_DELAY                 (100)
#define MAX_JCOUNT_7SEG_SW_DELAY                 (100)

#endif

/* ------------------------------- LED DOT MATRIX oper conf -----------------------------*/
#ifdef LED_MATRIX_MOD_ENABLE

#define REG_CNT_LED_MATRIX_CHAR                   (10) 
#define REQ_SCAN_DELAY_IN_MS                      (1)
#define MAX_ICOUNT_LED_MATRIX_SCAN_DELAY          (12) 
#define MAX_JCOUNT_LED_MATRIX_SCAN_DELAY          (12)

#endif

/* ------------------------------------ SW oper conf ------------------------------------*/
#ifdef SW_MOD_ENABLE

#define REQ_TIME_SW_DEBOUNCE_IN_MS                 (3)

#define MAX_ICOUNT_SW_DEBOUNCE_SW_DELAY           (100) 
#define MAX_JCOUNT_SW_DEBOUNCE_SW_DELAY           (100)

#endif

/* ------------------------------------ INTERRUPT Conf ----------------------------------*/
#ifdef INTERRUPT_MOD_ENABLE

#define MAX_NUM_INTP_PROCESS_TO_EXEC                  (10) 

#endif


#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
