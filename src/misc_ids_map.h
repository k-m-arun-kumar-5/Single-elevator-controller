/**************************************************************************
   FILE          :    misc_ids_map.h
 
   PURPOSE       :   Data IDs, Timer IDs and general interrupt request sources configuration header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   If data terminator char = NULL_CHAR, then newly read chars are taken till max nums of chars is reached.
                 :   If data terminator char != NULL_CHAR, then if data's max data allocated char < num entered chars, then new entered chars are taken, till terminator char is read.
			            else, then newly read chars are discarded, till terminator char is read. 
  
  CHANGE LOGS    : 
	   
 **************************************************************************/
 
 #ifndef _MISC_IDS_MAP_H
 #define _MISC_IDS_MAP_H 

             
 const data_id_ctrl_para_t data_id_ctrl_para[]  = {  
//                                            DATA_ID,                    NUM_CHAR,     SMALL_ALPHA, BIG_ALPHA, SPECIAL_CHAR, CTRL_CHAR, I/P_STREAM,             DISP_FORMAT,  
	                                         { DATA_ID_ELEVATOR_CTRL_NORMAL, STATE_NO,  STATE_NO,   STATE_NO,   STATE_NO,       STATE_NO, DATA_NO_STREAM, RCVD_CHAR_PLAIN_DISP_FORMAT,
											 
//											  O/P_DATA_TYPE,              I/P_DATA_TYPE,     O/P_STREAM,  TX_STREAM,         RCV_STREAM,  TERMINATOR, TRY, REQ_MAX_CHARS
											 DATA_TYPE_IN_APPL_DEFINED ,     DATA_TYPE_NO_DATA, DATA_NO_STREAM, DATA_NO_STREAM,   DATA_NO_STREAM,  NULL_CHAR,  0,       0 }	
                                           };

//                                                 DATA_ID,               INPUT_DEV,       OUTPUT_DEV,     	                              RECEIVE_DEV,      TRANSMIT_DEV        									   
 const data_id_dev_src_t  data_ids_dev_srcs[] = {	                                             
                                                  { DATA_ID_ELEVATOR_CTRL_NORMAL, {DEV_ID_INVALID}, {OUTPUT_DEV_ID_SEG7, CH_ID_00, DEV_ID_INVALID},  {DEV_ID_INVALID}, {DEV_ID_INVALID}}												
		                                       };	

#ifdef TIMER_MOD_ENABLE

timer_or_counter_ctrl_t timer_or_counter_ctrl[] =                                         
                             {
//                               TIMER_ID,                        TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,   CAPTURE_FALLING,   								 
                              { TIMER_ID_MOTOR_STOP_TO_DOOR_OPEN, TIMER_MODE,              CH_ID_01,               STATE_YES,                         STATE_NO,
							  
//                             CAPTURE_INTERRUPT,         STATE,                   TIMEOUT_IN_MS_OR_COUNT,               MAX_NUM_TIMEOUTS      
                                STATE_NO,        TMR_OR_COUNTER_IDLE_STATE,   TIMEOUT_MOTOR_STOP_TO_DOOR_OPEN_IN_MS,         1       },
                              
//                                TIMER_ID,            TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,             CAPTURE_FALLING,   								 
                              { TIMER_ID_DOOR_OPENED,   TIMER_MODE,               CH_ID_01,               STATE_YES,                                  STATE_NO,   
							  
//                              CAPTURE_INTERRUPT,        STATE,                TIMEOUT_IN_MS_OR_COUNT,        MAX_NUM_TIMEOUTS               
                                  STATE_NO,        TMR_OR_COUNTER_IDLE_STATE,   TIMEOUT_DOOR_OPENED_IN_MS,         1 },
								
//                                TIMER_ID,           TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,             CAPTURE_FALLING,   								 
                              { TIMER_ID_NEXT_FLOOR,         TIMER_MODE,      CH_ID_01,               STATE_YES,                                    STATE_NO,
							  
//                               CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,        MAX_NUM_TIMEOUTS    
                                    STATE_NO,         TMR_OR_COUNTER_IDLE_STATE,   TIMEOUT_NEXT_FLOOR_IN_MS,           1      },
                               
//                               TIMER_ID,                       TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,   CAPTURE_FALLING,  								 
                              { TIMER_ID_DOOR_OPENED_MOTOR,         TIMER_MODE,      CH_ID_01,               STATE_YES,                               STATE_NO,
							  
//                               CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,          MAX_NUM_TIMEOUTS    
                                       STATE_NO,      TMR_OR_COUNTER_IDLE_STATE,    TIMEOUT_DOOR_OPENED_MOTOR_IN_MS,     1           },
                              
//                               TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING,  								 
                              { TIMER_ID_DOOR_CLOSED_MOTOR,         TIMER_MODE,      CH_ID_01,               STATE_YES,                                STATE_NO,
							  
//                               CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,            MAX_NUM_TIMEOUTS 
                                       STATE_NO,     TMR_OR_COUNTER_IDLE_STATE,     TIMEOUT_DOOR_CLOSED_MOTOR_IN_MS,     1            },
								
//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING,								
                              { TIMER_ID_DOORS_ALIGNED,         TIMER_MODE,           CH_ID_01,               STATE_YES,                              STATE_NO,
							  
//                             CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,            MAX_NUM_TIMEOUTS   
                                  STATE_NO,           TMR_OR_COUNTER_IDLE_STATE,  TIMEOUT_DOORS_ALIGNED_IN_MS,          1             },
								
//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING, 								
                              { TIMER_ID_STARTUP_CUR_FLOOR,     TIMER_MODE,           CH_ID_01,               STATE_YES,                            STATE_NO,
							  
//                             CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,            MAX_NUM_TIMEOUTS   
                                   STATE_NO,        TMR_OR_COUNTER_IDLE_STATE,   TIMEOUT_STARTUP_CUR_FLOOR_IN_MS,       1             },
																
//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING, 								
                              { TIMER_ID_DETECT_DOOR_OPEN,     TIMER_MODE,           CH_ID_01,               STATE_YES,                             STATE_NO,
							  
//                             CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,            MAX_NUM_TIMEOUTS   
                                   STATE_NO,      TMR_OR_COUNTER_IDLE_STATE,     TIMEOUT_DETECT_DOOR_OPEN_IN_MS,        1             },

//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,   CAPTURE_FALLING,  								
                              { TIMER_ID_DETECT_DOOR_CLOSE,     TIMER_MODE,           CH_ID_01,               STATE_YES,                           STATE_NO,
							  
//                             CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,            MAX_NUM_TIMEOUTS 
                                    STATE_NO,         TMR_OR_COUNTER_IDLE_STATE,   TIMEOUT_DETECT_DOOR_CLOSE_IN_MS,        1          },

//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING, 								
                              { TIMER_ID_DOORS_UNALIGNED,     TIMER_MODE,           CH_ID_01,               STATE_YES,                              STATE_NO,
							  
//                             CAPTURE_INTERRUPT,          STATE,                TIMEOUT_IN_MS_OR_COUNT,       MAX_NUM_TIMEOUTS   
                                 STATE_NO,          TMR_OR_COUNTER_IDLE_STATE, TIMEOUT_DOORS_UNALIGNED_IN_MS,      1                   }, 

//                              TIMER_ID,                      TIMER_OR_COUNTER_MODE,  MATCH_OR_CAPTURE_ID, MATCH_INTERRUPT_OR_CAPTURE_RISING,    CAPTURE_FALLING, 								
                              { TIMER_ID_DOOR_CLOSE_TO_START_OPEN,     TIMER_MODE,           CH_ID_01,               STATE_YES,                      STATE_NO, 
							  
//                             CAPTURE_INTERRUPT,         STATE,                TIMEOUT_IN_MS_OR_COUNT,               MAX_NUM_TIMEOUTS   
                                       STATE_NO,   TMR_OR_COUNTER_IDLE_STATE, TIMEOUT_DOOR_CLOSE_TO_START_OPEN_IN_MS,     1               } 								
							 };
 
#endif		
				
#ifdef INTERRUPT_MOD_ENABLE
//                               VECTOR_IRQ_ISR,            INTERRUPT_REQUEST_SRC,          INTERRUPT_TYPE,      NEXT_TRIGGER_IRQS,    VECTOR_SLOT,   SAME_TRIGGER_IRQS                                     	
interrupt_request_ctrl_t  interrupt_request_ctrl[NUM_INTP_REQ_SRCS] = 
                              {
								                   {0,                                INTP_REQ_WDT,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },
																	 {0,                           INTP_REQ_RESERVED_SW_INTP,    INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },  
																	 {0,                                INTP_REQ_ARM_CORE0,      INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },
																	 {0,                                INTP_REQ_ARM_CORE1,      INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {(uint32_t)ISR_Timer_0,            INTP_REQ_TMR0,           INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },
                                                                     {(uint32_t)ISR_Timer_1,            INTP_REQ_TMR1,           INTERRUPT_REQ_VECTOR_IRQ,   STATE_NO,    VECTOR_SLOT_06,   STATE_NO },
																	 {0,                                INTP_REQ_UART0,          INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },  
																	 {0,                                INTP_REQ_UART1,          INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {0,                                INTP_REQ_PWM0,           INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {0,                                INTP_REQ_I2C0,           INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {0,                                INTP_REQ_SPI0 ,          INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },
																	 {0,                                INTP_REQ_SPI1_OR_SSP,    INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {0,                                INTP_REQ_PLL,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },   
																	 {0,                                INTP_REQ_RTC,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {(uint32_t)ISR_Ext_Interrupt_0,    INTP_REQ_EINT0,          INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {(uint32_t)ISR_Ext_Interrupt_1,    INTP_REQ_EINT1,          INTERRUPT_REQ_NA,   STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }, 
																	 {(uint32_t)ISR_Ext_Interrupt_2,    INTP_REQ_EINT2,          INTERRUPT_REQ_VECTOR_IRQ,   STATE_NO,    VECTOR_SLOT_04,   STATE_NO },  
																	 {(uint32_t)ISR_Ext_Interrupt_3,    INTP_REQ_EINT3,          INTERRUPT_REQ_VECTOR_IRQ,   STATE_NO,    VECTOR_SLOT_05,   STATE_NO },  
																	 {0,                                INTP_REQ_AD0,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },  
																	 {0,                                INTP_REQ_I2C1,           INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },  
																	 {0,                                INTP_REQ_BOD,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO },   
																	 {0,                                INTP_REQ_AD1,            INTERRUPT_REQ_NA,           STATE_NO,    VECTOR_SLOT_NA,   STATE_NO }  
							  }; 
							  
#endif	

						  
#endif
																						 
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
																						 
