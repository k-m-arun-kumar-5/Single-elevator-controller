/**************************************************************************
   FILE          :    dev_chs_map.h
 
   PURPOSE       :   
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   If enter or backspace SW is not a part of keypad, then set IO ch of that enter or backspace SW = IO_CH_INVALID.
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _DEV_CHS_MAP_H
 #define _DEV_CHS_MAP_H
 
 #ifdef KEYBOARD_MOD_ENABLE
 //                                                 IO_CH_ROWA,             IO_CH_COL1,       IO_CH_ENTER,               IO_CH_BS,            ACCESS,   ENABLE,                                           
keyboard_ctrl_t keyboard_ctrl[]  = {  
	                                     { IO_CH_KEYPAD_CH_00_ROWA, IO_CH_KEYPAD_CH_00_COL1, IO_CH_KEYPAD_CH_00_ENTER, IO_CH_KEYPAD_CH_00_BS, STATE_NO, STATE_NO,	
										 
//                                KEYS_ENABLE,   , KEYPAD_PRESSED, ENTER_PRESSED, BS_PRESSED 										 
									 STATE_NO,	    STATE_LOW,         STATE_LOW,  STATE_LOW}											 
                                   };
#endif

#ifdef SEG7_MOD_ENABLE
//                            IO_CH_A_LED,           IO_CH_UNIT_DIGIT,             ACCESS, ENABLE,       TYPE      ,      NUM_DIGITS,    DOT_DISP, DIGIT_PLACE_ACTIVE_STATE        											 
seg7_ctrl_t seg7_ctrl[] = {
                            { IO_CH_SEG7_CH_00_A_LED, IO_CH_SEG7_CH_00_UNIT_DIGIT, STATE_NO, STATE_NO, SEG7_COMMON_CATHODE, SEG7_2_DIGIT, STATE_NO, STATE_LOW  }
                          };
#endif
						  
#ifdef LED_MATRIX_MOD_ENABLE
//                                         IO_CH_ROWA,                       IO_CH_COL1,               ACCESS, ENABLE,           TYPE         , FONT_TYPE,   SEQ_WRITE_TYPE
 led_matrix_ctrl_t led_matrix_ctrl[] = {
	                                       {IO_CH_LED_MATRIX_CH_00_ROWA, IO_CH_LED_MATRIX_CH_00_COL1, STATE_NO, STATE_NO,  ROW_CATHODE_TYPE, FONT_SIZE_5_7, SEQ_WRITE_COL }
                                       };
#endif
 
#ifdef  SW_MOD_ENABLE

  sw_para_t sw_para[ NUM_INPUT_DEV_ID_SW_CHS] = {
  //                                                 IO_CH,       SW_ACCESS,  SW_ENABLE, PRESSED_STATE, SW_TYPE
                                                    {IO_CH_SW_CH_00, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //OVERLOAD_SENSOR
													{IO_CH_SW_CH_01, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //USER_BLOCKS_DOOR_SENSOR
													{IO_CH_SW_CH_02, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOWN_FLOOR_0
													{IO_CH_SW_CH_03, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOWN_FLOOR_1
													{IO_CH_SW_CH_04, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOWN_FLOOR_2
										            {IO_CH_SW_CH_05, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOWN_FLOOR_3
												    {IO_CH_SW_CH_06, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE }, //CAR_START
													{IO_CH_SW_CH_07, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //DOORS_ALIGNED_SENSOR
													{IO_CH_SW_CH_08, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE }, //MANUAL_DOOR_OPEN_SW
													{IO_CH_SW_CH_09, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE }, //MANUAL_DOOR_CLOSE_SW
											        {IO_CH_SW_CH_10, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOOR_CLOSE
													{IO_CH_SW_CH_11, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_DOOR_OPEN
	                                                {IO_CH_SW_CH_12, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_UP_FLOOR_1
													{IO_CH_SW_CH_13, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_UP_FLOOR_2
											        {IO_CH_SW_CH_14, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_UP_FLOOR_3
											        {IO_CH_SW_CH_15, STATE_NO, STATE_NO, STATE_LOW,    TOGGLE_SW_TYPE },      //LIMIT_SW_UP_FLOOR_4
										            {IO_CH_SW_CH_16, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //IN_CAR_CALL_FLOOR_0 
													{IO_CH_SW_CH_17, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //IN_CAR_CALL_FLOOR_1 
											        {IO_CH_SW_CH_18, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //IN_CAR_CALL_FLOOR_2 
											        {IO_CH_SW_CH_19, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //IN_CAR_CALL_FLOOR_3 
											        {IO_CH_SW_CH_20, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //IN_CAR_CALL_FLOOR_4 
										            {IO_CH_SW_CH_21, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_UP_FLOOR_0_CALL
											        {IO_CH_SW_CH_22, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_UP_FLOOR_1_CALL
											        {IO_CH_SW_CH_23, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_DOWN_FLOOR_1_CALL
											        {IO_CH_SW_CH_24, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_UP_FLOOR_2_CALL
                                                    {IO_CH_SW_CH_25, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_DOWN_FLOOR_2_CALL
											        {IO_CH_SW_CH_26, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_UP_FLOOR_3_CALL
											        {IO_CH_SW_CH_27, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE},  //HALL_DOWN_FLOOR_3_CALL	
													{IO_CH_SW_CH_28, STATE_NO, STATE_NO, STATE_LOW,    PUSH_BUTTON_SW_TYPE}   //HALL_DOWN_FLOOR_4_CALL	
                                             											
                                                } ; 
#endif

#ifdef LCD_MOD_ENABLE
//                                  IO_CH_CMD,           IO_CH_DATA,      LCD_ACCESS,  LCD_ENABLE,   INTERFACE,               FONT,         BUSY_FLAG
   lcd_ctrl_t lcd_ctrl[] = 
                              { 
                                  {IO_CH_LCD_CH_00_CMD, IO_CH_LCD_CH_00_DATA, STATE_NO, STATE_NO, DATA_INTERFACE_4_BITS, CHAR_FONT_5_8_DOTS, NO_CHECK_BUSY_FLAG,
								  
//                                MAX_AVAIL_LINES - 1, MAX_CONFIG_LINES - 1,    MAX_AVAIL_COLS - 1,     MAX_CONFIG_COLS - 1 
                                     (4 - 1) ,                 (4 - 1),          (20 - 1),           (20 - 1)                                                                                      
	                              }
                              };    										 
#endif

//                              TIMEOUT_IN_MS,  FLOW_CTRL,         MAX_NUM_RETRANSMIT,        
gen_comm_ctrl_t gen_comm_ctrl = {2000,         COMM_SW_FLOW_CTRL,    10 };

#ifdef UART_MOD_ENABLE
//                              BAUD_RATE, ACCESS_FLAG,  ENABLE_FLAG, TX_ENABLE, RCV_ENABLE,  ECHO_RECEIVE_CHAR, STOP_SELECT,  DATA_BIT_LEN - 1,   PARITY_CHECK  
uart_ctrl_t uart_ctrl[] =  
                           {   
							   {
								   9600, STATE_NO,      STATE_NO,  STATE_NO,       STATE_NO,  STATE_NO,   STOP_BIT_1_5_OR_2,  (8 - 1)   ,  NO_ERROR_CHECK  
							   }
						   };
#endif

#ifdef INTERRUPT_MOD_ENABLE
//                             EXT_INTERRUPT_SRC,    EXT_INTERRUPT_SENSE,   EXT_INTERRUPT_FUNC_MODE
ext_interrupt_ctrl_t  ext_interrupt_ctrl[MAX_NUM_EXT_INTERRUPTS] = 
                             {
								  {CH_ID_00,           EXT_INTERRUPT_SENSE_NA,    IO_FUNC_MODE_NA },
								  {CH_ID_00,           EXT_INTERRUPT_SENSE_NA,    IO_FUNC_MODE_NA },
								  {CH_ID_02,           EXT_INTERRUPT_SENSE_NA,    IO_FUNC_MODE_NA }, 
								  {CH_ID_03,           EXT_INTERRUPT_FALLING_EDGE, IO_FUNC_MODE_03 }//EMERGENCY_CAR_STOP
							 };
#endif

#endif
																					
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
																		
