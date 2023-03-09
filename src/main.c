/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    : 5 floor elevator controller.
                         Other name for elevator's cabin is car. 
                         Consideration of 5 floor elevator controller.
                        1:  elevator's in cabin floor call for each floor and hall floor call up and down for each intermediate floors, are considered. 
                            For floor 0 ie lowest floor, only hall floor call up and for floor MAX_NUM_FLOORS - 1 ie uppermost floor, only hall floor call down are considered.
                        2: two limit switches(up and down) for each intermediate floors are considered. 
						   For floor 0 ie lowest floor, only one limit switch(down) and for floor MAX_NUM_FLOORS - 1 ie uppermost floor, only one limit switch(up) are considered.
                        3: forward and reverse of the car pulley motor are considered.
	                    4: At startup, no default stop floor and handles floor call request from startup valid car floor position. 
						   If car lies in between floors, then it either moves up or down to next floor depending on conf.
 						   After valid car floor position is reached, then it handles floor call request.  
						5: cabin door and floor door alignment, car door open and close sequence are considered.  
						6: user blocks the cabin's closing door is considered. 
                     	7: users overload the cabin when the cabin is at stationary is considered. 					
						
						  Features of 5 floor elevator controller are :
						1: easy scableable to max floors, with little and easy code modification.
                        2: provision for emergency stop.
                        3: proper sequence of floor detection.
                        4: timeout if proper event does not occur.
						5: handles multiple different floor call request.
						6:  Elevator control is based on two basic principles.
                          A: Continue to travel in the current cabin movement direction(up or down) while there are still remaining floor call requests in that same cabin movement direction
·                         B: If there are no further floor call requests in that direction, then stop and become idle, or change direction if there are floor call requests in the opposite direction.
                 		7: cabin door which is open closes the door, if timeout on cabin door to remain open or manual door close is triggered.
						8: secure the elevator from improper event.
						9: if the user blocks the cabin closing door, then cabin's door is opened.
					   10: cabin door opens while closing the door, when manual door open is triggered or user blocks the closing door.
						
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
					 					
CAUTION               :  

               LCD    :  Only display least numeric digits and from left to right display is implemented.
			   UART   :  Only display least numeric digits and from left to right display is implemented.
			          :  Only 9600 baud rate is supported.
	     LED MATRIX   :  Does not support data type in str. 
                      :  Disp map ie led_dot_disp_map[] is coded for only seq write type = SEQ_WRITE_COL.
           KEYBOARD   :  If Keyboard is used, then u must hold CPU by using While(read_sw_pressed_state) by defining macro SW_PRESS_HOLD_CPU.
			          :  If ENTER_SW or BACKSPACE_SW is not a part of keyboard, then set IO channnel of that ENTER_SW or BACKSPACE_SW must set to IO_CH_INVALID.
					  :  Keypad's each row are activated in a sequential (serial) order and the columns of that row are sequentially checked for the pressed state. 
		        SW    :  If only independent SW's are used, then no need to hold CPU by using sw read level transition by not defining macro SW_PRESS_HOLD_CPU. 
				      :  NUM_INPUT_DEV_ID_SW_CHS must be appropiately set to accommodate independent SW's(if any) and (if any) Keyboard's required SW's. 
					     Keyboard's required SW's are KEYPAD_NUM_COLS, and ENTER_SW or/and BACKSPACE_SW(if ENTER_SW or/and BACKSPACE_SW are part of keyboard).						 			  
  SW & KEYBOARD INIT  :  First initialise independent SW if any, then only initialize Keyboard, if any.
  
	
DATA & DEV CTRL CONF  :  Configuration of data id and device control must be correct and appropriate. 
    	ERROR PROC    :  Process to do when a error or warning has occured is not implemented, but communicates with error status in LCD(ch: 0) or/and UART(ch: 0).
		  CTRL CHARS  :  Only Backspace ctrl char is supported and enter ctrl char can be used as terminator char. 
		  DATA STREAM :  Data Stream is not supported.
		STDOUT OPER   :  For Put_Char(), Put_Str() and Print() operations, macro UART_MOD_ENABLE must be defined 
		                 and TRACE_UART_CH_ID is used for STDOUT operations.
        STDIN OPER    :  For Get_Char(), Get_Str() and Scan() operations, macros UART_MOD_ENABLE and KEYBOARD_MOD_ENABLE must be defined. 
		                 Keyboard dev must be one of the input data source in cur_data_id and IO Channel of ENTER_SW must not be IO_CH_INVALID(ie must be valid IO channel).
						 and TRACE_UART_CH_ID is used for STDIN operations(for UART, to display read char(s)).		
		FLOW CONTROL  :  Only Stop and Wait ARQ is to be supported. 
	     	CHAR CODE :  Only ASCII 7 bit character code is supported. 
		    DEV INIT  :  All data ids dev source must be initialized and make sure that same dev source(dev id and dev ch id) must not initialized more than once.
          DEV DEINIT  :  All data ids dev source can be deinitialized and make sure that same dev source(dev id and dev ch id) must not deinitialized more than once.
  GENERAL INTERRUPT   :  FIQ interrupt type is not supported and interrupt handlers(ISRs) are non Reentrant interrupt handlers due to irq keyword.
                                    
CHANGE LOGS           :  Base from timer_03

FILE ID               : 01

*****************************************************************************/
#include "main.h"
#include "dev_chs_map.h"
#include "misc_ids_map.h"

/* ------------------------------ macro defination ------------------------------ */


/* ----------------------------- global variable defination --------------------- */
num_value_t disp_trace_num;
disp_num_fmt_t disp_trace_num_fmt;
uint32_t system_status_flag = NO_ERROR, uint32_temp_disp_data;
data_id_status_para_t data_id_status_para[NUM_DATA_IDS];
uint8_t cur_data_id = DATA_ID_INVALID;

/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */
static uint8_t System_Init(void); 
static uint8_t HW_Init(const void *const get_init_ptr);
static uint8_t PLL_Init(void);
extern uint16_t Appl_Proc(void);
extern uint16_t Appl_Init(const void *const data_ptr);
extern uint16_t Appl_HW_Init(void);
extern uint16_t Appl_Reset(const uint8_t reset_type);

/*------------------------------------------------------------*
FUNCTION NAME  : main

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.01  

BUGS           :              
-*------------------------------------------------------------*/
int main(void)
{
	uint16_t ret_status;
	  	
	if((ret_status = System_Init()) != SUCCESS)
	{
       return FAILURE;
	}	
    if((ret_status = Appl_Init(NULL_PTR)) != SUCCESS)
	{
		return FAILURE;  
    }
	while(1)
	{
	    if((ret_status = Appl_Proc()) != SUCCESS)
	    {
			return FAILURE;
	    }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Process

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.02

BUGS           :    
-*------------------------------------------------------------*/
uint8_t Reset_Process(const uint8_t reset_type)
{
	uint8_t ret_status;
  
	switch(reset_type)
	{
		case RESET_WHOLE:          	
		  #ifdef SW_MOD_ENABLE
	         Disable_All_SWs();
	      #endif   
          #ifdef KEYBOARD_MOD_ENABLE	
            Disable_All_Keyboards();
	      #endif
	      #ifdef SEG7_MOD_ENABLE
	        Disable_All_Seg7s();
	      #endif
	      #ifdef LED_MATRIX_MOD_ENABLE
	         Disable_All_LED_Matrixs();
	      #endif
	      #ifdef LCD_MOD_ENABLE
	         Disable_All_LCDs();
	      #endif 
		  #ifdef UART_MOD_ENABLE
		    Disable_All_UARTs();
		  #endif
		  Reset_Src_Interrupt_Data_Arr(MAX_NUM_INTP_PROCESS_TO_EXEC);
		   //memset(lcd_const_disp_flag, STATE_NO_IN_CHAR, sizeof(lcd_const_disp_flag)/ sizeof(char));		   
		case RESET_DATA_IDS_AND_APPL: 
		    system_status_flag = NO_ERROR;
    //SHOULD_CALL
    /*
		    if((ret_status = IO_Channel_Write(INTERNAL_ERROR_LED_IO_CH, STATE_LOW))!= SUCCESS)
	        {
		         system_status_flag = ERR_IO_CH_WRITE;		 
		         return system_status_flag;
	        }	
     */
		    Reset_Data_IDs_Status();		   
		case RESET_APPL:		    
            if((ret_status = Appl_Reset(RESET_APPL)) != SUCCESS)
		    {
			   system_status_flag = ERR_RESET_OPER;
		       return system_status_flag;
		    }
        break;
        default:
           system_status_flag = ERR_FORMAT_INVALID;		  
		   return system_status_flag;		
	}
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : System_Init

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.03  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t System_Init(void)
{
	 uint8_t ret_status; 
	 
	 if((ret_status = HW_Init(NULL_PTR)) != SUCCESS)
	 {
		 system_status_flag = ERR_HW_INIT;
		 return system_status_flag; 
	 }
	 if((ret_status = Reset_Process(RESET_WHOLE)) != SUCCESS)
	 {
		 system_status_flag = ERR_RESET_OPER;
		 return system_status_flag;
	 }	 
	 if((ret_status = Data_IDs_Set_Para()) != SUCCESS)
	 {
         system_status_flag = ERR_DATA_ID_CONF;
		 return system_status_flag; 
	 }
     return SUCCESS;	 
}

/*------------------------------------------------------------*
FUNCTION NAME  : HW_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : First initialise independent SW if any, then initialize Keyboard, if any.

Func ID        : 01.04  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t HW_Init(const void *const get_init_ptr)
{
	uint8_t ret_status = SUCCESS ;
	
	PLL_Init();
	Reset_IO_Chs();	
    if((ret_status = Appl_HW_Init()) != SUCCESS)
	{
		 system_status_flag = ERR_HW_INIT;
		 Error_or_Warning_Proc("01.04.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}    
    return SUCCESS;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : PLL_Init

DESCRIPTION    : PLL IS CONFIGURED. 
								
INPUT          : 

OUTPUT         : 

NOTE           : calculated for crystal oscillitor of 12MHz 
                 and PLL IS CONFIGURED for CCLK(processor clock) = 60MHz
				 and PCLK(APB clock) = 15MHz.

Func ID        : 01.05  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t PLL_Init(void)
{
	PLLCFG = 0x24; // SET (divider)PSEL = 2 AND (Multiplier) MSEL=5 
    PLLCON = 0x01; //Enable PLL, PLL IS ACTIVE BUT NOT YET CONNECT   
    PLLFEED = 0xAA; //Feed sequence
    PLLFEED = 0x55; //Feed sequence 
    while(!(PLLSTAT & 0x00000400)); //WAIT FOR FEED SEQUENCE TO BE INSERTED AND PLL to be locked 
    PLLCON = 0x03; //PLL HAS BEEN ACTIVE, LOCKED and Connect PLL
    PLLFEED = 0xAA; //Feed sequence
    PLLFEED = 0x55; //Feed sequence
  	VPBDIV=0X00;   // or APBDIV, SET APBDIV, PCLK(APB clock) = CCLK(processor clock) / 4
   	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
