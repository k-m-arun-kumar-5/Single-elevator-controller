/**************************************************************************
   FILE          :    appl_conf.h
 
   PURPOSE       :   main application configuration Header
 
   AUTHOR        :   K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _APPL_CONF_H
 #define _APPL_CONF_H 
 
/* -------------------------------- Debug Conf -----------------------------------------*/
//#define TRACE                                 (1U)
#define TRACE_ERROR                             (2U)
#define TRACE_REQ                               (3U)
#define TRACE_INFO                              (4U)
//#define TRACE_DATA                            (5U)
//#define TRACE_FLOW                            (6U)

#define TRACE_LCD_CH_ID                             (CH_ID_00)
#define TRACE_UART_CH_ID                            (CH_ID_00) 

/* -------------------------------  Macro Conf ------------------------------------------*/
//#define SHOULD_REMOVE                 (7)
#define SHOULD_CALL                     (8)
//#define TEST                          (9)                                                  
//#define SW_PRESS_HOLD_CPU             (10)
//#define DMA_DATA_ID_STR               (11)
//#define STR_TO_NUM_FROM_RIGHT_TO_LEFT (12)
 
//#define BUG_PRINT                       (13)

/* ------------------------------ Appl Data Types --------------------------------------- */

#define MAX_DATA_INPUT_DEVS                 (2)
#define MAX_DATA_OUTPUT_DEVS                (2)
#define MAX_DATA_RECEIVE_DEVS               (1) 
#define MAX_DATA_TRANSMIT_DEVS              (1)

typedef enum 
{
   DATA_ID_ELEVATOR_CTRL_NORMAL, NUM_DATA_IDS    	
} appl_data_id_t;

/*------------------------------- LCD Disp Conf ------------------------------------------*/
#ifdef LCD_MOD_ENABLE

#endif

/* -------------------------------Timer State Conf ---------------------------------------*/
#ifdef TIMER_MOD_ENABLE

  typedef enum
  { 
     TIMER_ID_MOTOR_STOP_TO_DOOR_OPEN, TIMER_ID_DOOR_OPENED, TIMER_ID_NEXT_FLOOR, TIMER_ID_DOOR_OPENED_MOTOR, 
     TIMER_ID_DOOR_CLOSED_MOTOR, TIMER_ID_DOORS_ALIGNED, TIMER_ID_STARTUP_CUR_FLOOR, TIMER_ID_DETECT_DOOR_OPEN, 
	 TIMER_ID_DETECT_DOOR_CLOSE, TIMER_ID_DOORS_UNALIGNED, TIMER_ID_DOOR_CLOSE_TO_START_OPEN, NUM_TIMER_AND_COUNTER_IDS
  } timer_and_counter_id_t;

 #define TIMEOUT_MOTOR_STOP_TO_DOOR_OPEN_IN_MS        (500)
 #define TIMEOUT_DOOR_OPENED_IN_MS                    (5000)
 #define TIMEOUT_NEXT_FLOOR_IN_MS                     (6000) 
 #define TIMEOUT_DOOR_OPENED_MOTOR_IN_MS              (5000) 
 #define TIMEOUT_DOOR_CLOSED_MOTOR_IN_MS              (5000) 
 #define TIMEOUT_DOORS_ALIGNED_IN_MS                  (5000) 
 #define TIMEOUT_STARTUP_CUR_FLOOR_IN_MS              (5000)
 #define TIMEOUT_DETECT_DOOR_OPEN_IN_MS               (5000)
 #define TIMEOUT_DETECT_DOOR_CLOSE_IN_MS              (5000)
 #define TIMEOUT_DOORS_UNALIGNED_IN_MS                (5000)
 #define TIMEOUT_DOOR_CLOSE_TO_START_OPEN_IN_MS       (500)
#endif

#define REQ_DELAY_COUNT_SCAN_LIMIT_SW_FLOORS          (200)
#define REQ_DELAY_COUNT_LIMIT_SW_DOORS                (600)

/*------------------------------- Ext Interrupt Conf ------------------------------------------*/
#ifdef INTERRUPT_MOD_ENABLE

  #define EMER_CAR_STOP_EXT_INTP_CH_ID   (CH_ID_03)
  
#endif

/* ------------------------------ ADC input signal val Conf ------------------------------*/



/* ------------------------------- Application Conf --------------------------------------*/

#define MAX_NUM_FLOORS                      (5)
#define STARTUP_CAR_IN_BETWEEN_FLOORS      (MOVE_DOWN)


#define HIDDEN_KEY_DISP_CHAR                      ('X') 
#define DATA_MAX_NUM_ALLOCATED_CHARS              (11) 
#define HIDDEN_CHAR_LCD_DISP_TIME_IN_MS          (1000)
#define MAX_ICOUNT_HIDDEN_CHAR_DISP_1500MS       (1500)
#define MAX_JCOUNT_HIDDEN_CHAR_DISP_1500MS       (1500)

#define  NUM_CHARS_INPUTDATA        GIVEN_CHARS_MAX_CONFIG_LINES_AND_COLS

#define MAX_COUNT_LONG_PRESSKEY_TIMEOUT   (3U)  

#define MAX_TIMEOUT_NO_KEY_PRESS         (10)  
#define MAX_TIMEOUT_LONG_PRESS_KEY       (5)

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
