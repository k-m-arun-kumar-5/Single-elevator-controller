/* ********************************************************************
FILE                   : appl.c

PURPOSE                :  Application 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
						
CAUTION               :  
                                    
CHANGE LOGS           :  

FILE ID               : 11

*****************************************************************************/
#include "main.h"
#include "appl.h"
#include "elevator_fsm.h"

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */ 
uint32_t appl_status_flag = NO_ERROR;
			
/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */


/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Reset

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.01

BUGS           :    
-*------------------------------------------------------------*/
 uint16_t Appl_Reset(const uint8_t reset_type)
{
	uint16_t ret_status;
	
	switch(reset_type)
	{
		case RESET_APPL:	
            appl_status_flag = NO_ERROR;	
		    Appl_Reset_Proc(MAX_NUM_ELEVATORS);
		break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_Proc("11.01.04", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
	}
	return SUCCESS;
} 

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.02

BUGS           :    
-*------------------------------------------------------------*/
uint16_t Appl_Proc(void)
{	
   	switch(cur_data_id) 
	{	
	    case DATA_ID_ELEVATOR_CTRL_NORMAL:		    
            if((Elevator_FSM_Proc(CTRL_ELEVATOR_CH_ID)) != SUCCESS)
			{
				 appl_status_flag = ERR_ELEVATOR_FSM_PROC;
		         Error_or_Warning_Proc("11.02.02", ERROR_OCCURED, appl_status_flag);
		         return appl_status_flag;
			}                 			
		break;
        default:
		   appl_status_flag = ERR_CUR_DATA_ID_INVALID;
		   Error_or_Warning_Proc("11.02.02", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
	 } 
     return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.03  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Appl_Init(const void *const data_ptr)
{	
    uint8_t sw_ch_id;
	
	if((Next_Data_Conf_Parameter(DATA_ID_ELEVATOR_CTRL_NORMAL)) != SUCCESS)
	{
		 appl_status_flag = ERR_NEXT_DATA_CONF;
		 Error_or_Warning_Proc("11.03.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}	
	for(sw_ch_id = 0; sw_ch_id < NUM_INPUT_DEV_ID_SW_CHS; ++sw_ch_id )
	{
		if((SW_Allow_Access(sw_ch_id) ) != SUCCESS)
		{
			 appl_status_flag = ERR_DEV_ALLOW_ACCESS_PROC;
		     Error_or_Warning_Proc("11.03.01", ERROR_OCCURED, system_status_flag);
		     return system_status_flag;  
		}
	}
	if((Seg7_Enable(CH_ID_00) ) != SUCCESS)
	{
		 appl_status_flag = ERR_DEV_ENABLE_PROC;
		 Error_or_Warning_Proc("11.03.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_HW_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : First initialise independent SW in order from SW_CH_ID from 0 if any, then initialize Keyboard, if any.

Func ID        : 11.04  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Appl_HW_Init(void)
{
    uint8_t cur_sw_ch_id;
	
	if((UART_Init(TRACE_UART_CH_ID, COMM_TRANSMIT_FUNC)) != SUCCESS)
	{
		return FAILURE;
	}
	if((UART_Init(TRACE_UART_CH_ID, COMM_RECEIVE_FUNC)) != SUCCESS)
	{
		return FAILURE;
	}
	if((UART_Init(TRACE_UART_CH_ID, DEV_INIT_OPER)) != SUCCESS)
	{
		return FAILURE;
	}
  //SHOULD_CALL
  /*
	if((Output_Dev_Init(INTERNAL_ERROR_LED_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	} */
	if((Output_Dev_Init(ALARM_LED_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	}
	if((Output_Dev_Init(CAR_UP_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	}
	if((Output_Dev_Init(CAR_DOWN_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	}
	if((Output_Dev_Init(CAR_DOOR_OPEN_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	}
	if((Output_Dev_Init(CAR_DOOR_CLOSE_IO_CH, 1)) != SUCCESS)
	{
		return FAILURE;
	}
	if((Seg7_Init(CH_ID_00, DATA_NA)) != SUCCESS)
	{
		return FAILURE;
	} 
	for(cur_sw_ch_id = 0; cur_sw_ch_id < NUM_INPUT_DEV_ID_SW_CHS; ++cur_sw_ch_id )
	{
		if((SW_Init(cur_sw_ch_id, DATA_NA)) != SUCCESS)
		{
			   return FAILURE;
		}
	}
	#ifdef TRACE_FLOW
	   Print("TRA: HW Init over\r");
	#endif
	return SUCCESS; 
}

#ifdef TIMER_MOD_ENABLE 
/*------------------------------------------------------------*
FUNCTION NAME  : Timer_0_Timeout_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.05  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Timer_0_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_0_or_counter_data_ptr)
{
	uint16_t ret_status;
	
	if(timer_0_or_counter_data_ptr == NULL_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("11.05.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	if(timer_0_or_counter_data_ptr->timer_or_counter_run_id != TMR_OR_COUNTER_SYSTEM_TICK_STATE)
	{
		appl_status_flag = ERR_TIMER_ID_EXCEEDS;
		Error_or_Warning_Proc("11.05.02", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
    /* --------------------------------------- User Code: Begin ------------------------------------------ */ 
	
	/* --------------------------------------- User Code: End   ------------------------------------------ */ 
	switch(timeout_type)
	{
		 case TMR_NO_MAX_NUM_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_NO_MAX_NUM_TIMEOUT_PROC;
	     break;		
		 case TMR_AT_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_AT_LAST_TIMEOUT_PROC;
		 break;  
		 case TMR_BEFORE_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_BEFORE_LAST_TIMEOUT_PROC;
		 break;
         case TMR_MAX_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_MAX_NUM_TIMEOUT_PROC; 
         break;		
         default:
             appl_status_flag = ERR_TMR_TIMEOUT_TYPE_INVALID;
		     Error_or_Warning_Proc("11.05.03", ERROR_OCCURED, appl_status_flag);
		     return appl_status_flag;
	}
    return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Timer_1_Timeout_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.06  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Timer_1_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_1_or_counter_data_ptr)
{
	 elevator_ctrl_and_status_t *cur_elevator_ptr;
     uint16_t ret_status;
    
    if(timer_1_or_counter_data_ptr == NULL_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("11.06.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	if(timer_1_or_counter_data_ptr->timer_or_counter_run_id >= NUM_TIMER_AND_COUNTER_IDS)
	{
		appl_status_flag = ERR_TIMER_ID_EXCEEDS;
		Error_or_Warning_Proc("11.06.02", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	/* --------------------------------------- User Code: Begin ------------------------------------------ */ 
	
	/*cur_elevator_ptr = elevator_ctrl_and_status + CTRL_ELEVATOR_CH_ID;
	switch(timer_1_or_counter_data_ptr->timer_or_counter_run_id)
	{
		case TIMER_ID_DOOR_CLOSED_MOTOR:
		  if(cur_elevator_ptr->cur_fsm_state != FSM_WAIT_TILL_DOOR_CLOSED) 
		  {
			  appl_status_flag = ERR_ELEVATOR_CUR_FSM_INVALID;
		      Error_or_Warning_Proc("11.06.03", ERROR_OCCURED, appl_status_flag);
		      return appl_status_flag;
		  }
		  #ifdef TRACE_ERROR
		     Print( "WAIT_TILL_DOOR_CLOSE -> ABNORMAL_EVENT \r");
			 Print( "ERR: Event - door closed not detected \r");
		  #endif
		  cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		  cur_elevator_ptr->elevator_status = ELEVATOR_ERROR;		  
		break;		
	}*/
	
	/* --------------------------------------- User Code: End   ------------------------------------------ */ 
	
	 switch(timeout_type)
	 {
		 case TMR_NO_MAX_NUM_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_NO_MAX_NUM_TIMEOUT_PROC;
	     break;		
		 case TMR_AT_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_AT_LAST_TIMEOUT_PROC;
		 break;  
		 case TMR_BEFORE_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_BEFORE_LAST_TIMEOUT_PROC;
		 break;
         case TMR_MAX_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_MAX_NUM_TIMEOUT_PROC; 
         break;		 
         default:
		     appl_status_flag = ERR_TMR_TIMEOUT_TYPE_INVALID;
		     Error_or_Warning_Proc("11.06.05", ERROR_OCCURED, appl_status_flag);
		     return appl_status_flag;
	 }
    return ret_status;	
}

#endif

#ifdef INTERRUPT_MOD_ENABLE
/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_0_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.07  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_0_Proc(void)
{
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_1_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.08  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_1_Proc(void)
{
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_2_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.09  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_2_Proc(void)
{
	uint16_t ret_status;   
	
    return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_3_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.10  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_3_Proc(void)
{
	uint16_t ret_status;	 
	
	/* --------------------------- triggered emerency car stop ---------------------- */	
	if((ret_status = Disable_Ext_Interrupt(EMER_CAR_STOP_EXT_INTP_CH_ID)) != SUCCESS)
	{
		appl_status_flag = ERR_DISABLE_INTERRUPT;
		Error_or_Warning_Proc("11.10.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	ret_status = Delete_Src_Interrupt_Data_Arr_By_Src(INTP_REQ_TMR1);
	switch(ret_status)
	{
		
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SEARCH_INTP_SRC_MORE_THAN_ONE_DATA_ARR:
		case SEARCH_INTP_SRC_ONE_DATA_ARR:
		break;
		default:
		   appl_status_flag = ERR_DELETE_INTP_SRC_DATA_PROC;
	       Error_or_Warning_Proc("11.10.02", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
	}
	return SUCCESS;
}

#endif

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Reset_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.11

BUGS           :    
-*------------------------------------------------------------*/ 
uint16_t Appl_Reset_Proc(const uint8_t ctrl_elevator_ch_id)
{
	uint8_t cur_elevator_ch_id;
	
	if(ctrl_elevator_ch_id > MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.11.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    } 
	if((IO_Channel_Write(ALARM_LED_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("11.11.02", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}		
	if(ctrl_elevator_ch_id == MAX_NUM_ELEVATORS)
	{
		for(cur_elevator_ch_id = 0 ; cur_elevator_ch_id < MAX_NUM_ELEVATORS; ++cur_elevator_ch_id)
		{
			Reset_Elevator_Datas(cur_elevator_ch_id, RESET_WHOLE_DATAS);
		}
		return SUCCESS;
	}
    Reset_Elevator_Datas(ctrl_elevator_ch_id, RESET_WHOLE_DATAS);    
	#ifdef TRACE_FLOW
	  Print("Reset Elevator Datas \r");
	#endif
	return SUCCESS;
}	


/*------------------------------------------------------------*
FUNCTION NAME  : Validate_Floor

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.12  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Validate_Floor(const uint8_t floor)
{
	if(floor < 0 || floor >=  MAX_NUM_FLOORS)
	{
		appl_status_flag = ERR_FLOOR_INVALID;
		Error_or_Warning_Proc("11.12.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Elevator_Datas

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.13  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Reset_Elevator_Datas(const uint8_t ctrl_elevator_ch_id, const uint8_t reset_type)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	
    if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.13.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(reset_type == RESET_WHOLE_DATAS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_IDLE;	
	}
    if(reset_type == RESET_WHOLE_DATAS || reset_type == RESET_ABNORMAL_STATE_DATAS )
	{
		cur_elevator_ptr->before_fsm_state = FSM_IDLE;
        cur_elevator_ptr->elevator_status = STARTUP_STATIONARY;
		cur_elevator_ptr->cur_floor =  FLOOR_ID_INVALID;		
	}
	if(reset_type == RESET_WHOLE_DATAS || reset_type == RESET_ABNORMAL_STATE_DATAS || reset_type == RESET_STOP_DATAS)
	{
		cur_elevator_ptr->next_stop_floor = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_min_floor_call = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_max_floor_call = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_min_floor_call_in_car = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_max_floor_call_in_car = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_min_floor_call_up = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_max_floor_call_up = FLOOR_ID_INVALID;	
	    cur_elevator_ptr->cur_min_floor_call_down = FLOOR_ID_INVALID;
	    cur_elevator_ptr->cur_max_floor_call_down = FLOOR_ID_INVALID;	
	    cur_elevator_ptr->pending_floor_call_bit_field = 0;
	    cur_elevator_ptr->in_car_floor_call_bit_field = 0;
        cur_elevator_ptr->hall_floor_call_up_bit_field = 0;
        cur_elevator_ptr->hall_floor_call_down_bit_field = 0;
	    cur_elevator_ptr->num_pending_calls_in_car = 0;
        cur_elevator_ptr->num_pending_calls_up = 0;
        cur_elevator_ptr->num_pending_calls_down = 0;
	    cur_elevator_ptr->num_pending_calls = 0;
		return SUCCESS;
	}
    appl_status_flag = ERR_FORMAT_INVALID;
	Error_or_Warning_Proc("11.13.02", ERROR_OCCURED, appl_status_flag);
	return appl_status_flag;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Disp_Elevator_Status

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : Seg7 appl defined data str =  <present_floor> <status>, 
                  where status are 'A' - UP, 'B' - DOWN, 'C' - IDLE, 'D' - Error 
				  
Func ID        : 11.14  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Disp_Elevator_Status(const uint8_t ctrl_elevator_ch_id)
{
	 elevator_ctrl_and_status_t *cur_elevator_ptr;
	 dev_write_ctrl_para_t temp_dev_write_ctrl_para; 	 
	 uint8_t disp_elevator_status_code;
	 static uint8_t cur_floor = MAX_NUM_FLOORS;
	 
    if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		  appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	      Error_or_Warning_Proc("11.14.01", ERROR_OCCURED, appl_status_flag);
	      return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(cur_elevator_ptr->elevator_status >= NUM_ELEVATOR_STATUS)
	{
		  appl_status_flag = ERR_DEV_OPER_WRITE_FUNC;
		  Error_or_Warning_Proc("11.04.02", ERROR_OCCURED, appl_status_flag);
		  return appl_status_flag;
	}
	switch(cur_elevator_ptr->elevator_status)
	{
		case STARTUP_STATIONARY:
        case MOVED_UP_STATIONARY:
        case MOVED_DOWN_STATIONARY:
		    disp_elevator_status_code = DISP_STATIONARY;
        break;
        case MOVE_UP:
		case MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN:
		case MOVE_UP_CUR_UP_TO_NEXT_DOWN:
		case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
		case MOVE_UP_NO_CUR_UP_TO_NEXT_UP:
		case MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN:
		case MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP:
		  disp_elevator_status_code = DISP_MOVE_UP;
		break;
        case MOVE_DOWN: 
		case MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP:
		case MOVE_DOWN_CUR_DOWN_TO_NEXT_UP:
		case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:
		case MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN:
		case MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP:
		case MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN:
		   disp_elevator_status_code = DISP_MOVE_DOWN;
		break;
		default:
		   disp_elevator_status_code = DISP_ERROR;
	}	
     if(disp_elevator_status_code >= NUM_ELEVATOR_DISP_STATUS)
	 {
		 appl_status_flag = ERR_ELEVATOR_DISP_STATUS;
	     Error_or_Warning_Proc("11.14.03", ERROR_OCCURED, appl_status_flag);
	     return appl_status_flag;
	 }
	 if(cur_elevator_ptr->cur_floor < MAX_NUM_FLOORS)
	 {
	     temp_dev_write_ctrl_para.seg7_appl_defined_str[0] = cur_elevator_ptr->cur_floor + NUM_0_CHAR;
		 cur_floor = cur_elevator_ptr->cur_floor;
	 }
	 else
	 {
		 if(cur_floor < MAX_NUM_FLOORS)
		 {
		     temp_dev_write_ctrl_para.seg7_appl_defined_str[0] = cur_floor + NUM_0_CHAR;
		 }
		 else
		 {
			 temp_dev_write_ctrl_para.seg7_appl_defined_str[0] = DISP_STATIONARY + SEG7_START_NON_DIGIT_CODE;
		 }
	 }
     temp_dev_write_ctrl_para.seg7_appl_defined_str[1] = disp_elevator_status_code  + SEG7_START_NON_DIGIT_CODE;	
	 temp_dev_write_ctrl_para.seg7_appl_defined_str[2] = NULL_CHAR;	
	 if((Cur_Data_ID_Write_Oper_Func(DEV_OUTPUT_TYPE, &temp_dev_write_ctrl_para)) != SUCCESS)
	 {
	      appl_status_flag = ERR_DEV_OPER_WRITE_FUNC;
		  Error_or_Warning_Proc("11.14.04", ERROR_OCCURED, appl_status_flag);
		  return appl_status_flag;
    }
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Elevator_FSM_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.15  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Elevator_FSM_Proc(const uint8_t ctrl_elevator_ch_id)
{
	 elevator_ctrl_and_status_t *cur_elevator_ptr;
	 	 
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.15.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(cur_elevator_ptr->cur_fsm_state >= NUM_ELEVATOR_FSM_STATES)
	{
		appl_status_flag = ERR_ELEVATOR_FSM_INVALID;
	    Error_or_Warning_Proc("11.15.02", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
	if((Elevator_FSM_Proc_Func_Ptr[cur_elevator_ptr->cur_fsm_state](ctrl_elevator_ch_id)) != SUCCESS)
	{
	  	appl_status_flag = ERR_ELEVATOR_FSM_PROC;
	    Error_or_Warning_Proc("11.15.03", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
	if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
	{
		appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	    Error_or_Warning_Proc("11.15.04", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
 	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Car_Movement_Direction 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.16

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Car_Movement_Direction(const uint8_t ctrl_elevator_ch_id, const uint8_t cur_floor, uint8_t *const elevator_trigger_move_ptr)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	
	if(elevator_trigger_move_ptr == NULL_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
	    Error_or_Warning_Proc("11.16.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
	*elevator_trigger_move_ptr = NUM_ELEVATOR_STATUS;
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.16.02", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;	
	switch(cur_elevator_ptr->before_fsm_state)
	{
		case FSM_PREPARE_USER_ENTRY_AND_EXIT:
          if(cur_floor == cur_elevator_ptr->next_stop_floor)
	      {
	     	 switch(cur_elevator_ptr->elevator_status)
		     {
		    	case STARTUP_STATIONARY:
			       *elevator_trigger_move_ptr = STARTUP_STATIONARY;
			    break;
			    case MOVE_UP:
			    case MOVED_UP_STATIONARY:
			       *elevator_trigger_move_ptr = MOVED_UP_STATIONARY;
			    break;
			    case MOVE_DOWN:
			    case MOVED_DOWN_STATIONARY:
			       *elevator_trigger_move_ptr = MOVED_DOWN_STATIONARY;
			    break;
                default:
                   appl_status_flag = ERR_FORMAT_INVALID;
	               Error_or_Warning_Proc("11.16.03", ERROR_OCCURED, appl_status_flag);
	               return appl_status_flag;
			  }
		  }		    
	     else
	     {
		     if(cur_floor < cur_elevator_ptr->next_stop_floor) 
	         {			
			    *elevator_trigger_move_ptr = MOVE_UP;
		     }
		     else
		     {	
	           *elevator_trigger_move_ptr = MOVE_DOWN;		   
		     }
		 }
       break;
       case FSM_STARTUP:
       break;	   
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_Time_Elevator 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.17

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Delay_Time_Elevator(const uint8_t ctrl_elevator_ch_id, const uint32_t max_delay_count)
{
	uint32_t x, elasped_count;
		
	for(elasped_count = 0; elasped_count < max_delay_count; ++elasped_count)
	{
        for(x = 0; x < 500; x++);			
		if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
	    {
		   appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	       Error_or_Warning_Proc("11.17.01", ERROR_OCCURED, appl_status_flag);
	       return appl_status_flag;
	    }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Check_Move_Condition

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.18

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Check_Move_Condition(const uint8_t ctrl_elevator_ch_id, const uint8_t proc_fsm_state )
{
    elevator_ctrl_and_status_t *cur_elevator_ptr;
	 uint16_t ret_status;
   uint8_t proc_bit_field = 0;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.18.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
    switch(proc_fsm_state)
	{
		case FSM_TRIGGER_MOVE_UP:
		case FSM_TRIGGER_MOVE_DOWN:
		case FSM_MOVING:
		break;
		default: 
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("11.18.02", ERROR_OCCURED, appl_status_flag);
	      return appl_status_flag;
	}
	ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		   switch(proc_fsm_state)
			  {
				  case FSM_TRIGGER_MOVE_UP:
				     #ifdef TRACE_FLOW
	                   Print("TRIGGER_MOVE_UP -> ABNORMAL_EVENT \r");	                  
					 #endif					  
				  break;
				  case FSM_TRIGGER_MOVE_DOWN:
				     #ifdef TRACE_FLOW
	                   Print("TRIGGER_MOVE_DOWN -> ABNORMAL_EVENT \r");	                  
					 #endif					
				  break;
                  case FSM_MOVING:
				      #ifdef TRACE_FLOW
	                   Print("MOVING -> ABNORMAL_EVENT \r");	                  
					 #endif	
                  break;				  
			  }	
			  #ifdef TRACE_ERROR
			   Print("ERR: event - Move but doors aligned \r");
			   #endif
			    cur_elevator_ptr->elevator_status = ERR_DOORS_ALIGNED_FOR_MOVE;
			   appl_status_flag = ERR_SW_IS_ACTIVE;
	           Error_or_Warning_Proc("11.18.03", ERROR_OCCURED, appl_status_flag);			  
		     return appl_status_flag; 	 
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:		    
			proc_bit_field = (1 << 0); 
		 break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.18.04", ERROR_OCCURED, appl_status_flag);
			return appl_status_flag;
		// break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.18.05", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		    switch(proc_fsm_state)
			{
				case FSM_TRIGGER_MOVE_UP:
				   #ifdef TRACE_FLOW
	                  Print("TRIGGER_MOVE_UP -> ABNORMAL_EVENT \r");	                 
	               #endif	             
				break;
				case FSM_TRIGGER_MOVE_DOWN:
				   #ifdef TRACE_FLOW
	                  Print("TRIGGER_MOVE_DOWN -> ABNORMAL_EVENT \r");	                
	               #endif	              
				break;
				case FSM_MOVING:
				   #ifdef TRACE_FLOW
	                   Print("MOVING -> ABNORMAL_EVENT \r");	                  
				   #endif	
                break;
			}
			#ifdef TRACE_ERROR
			    Print("ERR: event - door opened, but ready to move \r");
			 #endif
			  cur_elevator_ptr->elevator_status = ERR_DOOR_OPENED_BUT_READY_MOVE;
			  appl_status_flag = ERR_SW_IS_ACTIVE;
	          Error_or_Warning_Proc("11.18.06", ERROR_OCCURED, appl_status_flag);			
	         return appl_status_flag;    
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:
		    proc_bit_field |= (1 << 1);
		 break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		   appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.18.07", ERROR_OCCURED, appl_status_flag);
			return appl_status_flag;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.18.08", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		    proc_bit_field |= (1 << 2);
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    switch(proc_fsm_state)
			{
				case FSM_TRIGGER_MOVE_UP:
				   #ifdef TRACE_FLOW
	                 Print("TRIGGER_MOVE_UP -> ABNORMAL_EVENT \r");	                
	              #endif
				break;
				case FSM_TRIGGER_MOVE_DOWN:
				   #ifdef TRACE_FLOW
	                 Print("TRIGGER_MOVE_DOWN -> ABNORMAL_EVENT \r");
	              #endif
				break;
				case FSM_MOVING:
				   #ifdef TRACE_FLOW
	                  Print("MOVING -> ABNORMAL_EVENT \r");	                  
				   #endif	
                break;
			}
			appl_status_flag = ERR_SW_IS_INACTIVE;
	         Error_or_Warning_Proc("11.18.09", ERROR_OCCURED, appl_status_flag);
			 #ifdef TRACE_ERROR
                Print("ERR: event - door not closed, but ready to move \r");
			 #endif
             cur_elevator_ptr->elevator_status = ERR_DOOR_NOT_CLOSED_BUT_READY_MOVE;			 
	       return appl_status_flag; 
		// break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		   appl_status_flag = ERR_SW_IS_DISABLED;
	       Error_or_Warning_Proc("11.18.10", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.18.11", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;
	}
  if(proc_bit_field != 0x07)
  {
     return FAILURE;
  }
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Check_Stationary_Door_Opened_State

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.19

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Check_Stationary_Door_Opened_State(const uint8_t ctrl_elevator_ch_id, const uint8_t proc_fsm_state )
{
    elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
    uint8_t proc_bit_field = 0;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.19.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	switch(proc_fsm_state)
	{
		case FSM_PREPARE_USER_ENTRY_AND_EXIT:
		case FSM_USER_ENTRY_AND_EXIT:
		case FSM_PREPARE_FOR_DOOR_CLOSE:
        break;
		default: 
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("11.19.02", ERROR_OCCURED, appl_status_flag);
	       return appl_status_flag;
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
	    Error_or_Warning_Proc("11.19.03", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
	proc_bit_field |= (1 << 0);
    ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		   proc_bit_field |= (1 << 1);
		 break;
		 case SW_OR_KEY_NOT_PRESSED: 
		    switch(proc_fsm_state)
	        {
				case FSM_PREPARE_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
				break;
		        case FSM_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
		        break;
		        case FSM_PREPARE_FOR_DOOR_CLOSE:
              #ifdef TRACE_FLOW
				      Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT\r");
				   #endif
		        break;
	        }
		   #ifdef TRACE_ERROR
		      Print("ERR: Event - doors unaligned at stationary \r");
		   #endif
		   cur_elevator_ptr->elevator_status = ERR_DOORS_UNALIGNED_AT_STATIONARY;
		   appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("11.19.04", ERROR_OCCURED, appl_status_flag);		   
	       return cur_elevator_ptr->elevator_status;
		 //break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.19.05", ERROR_OCCURED, appl_status_flag);
			return appl_status_flag;
		// break; 
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.19.06", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;		 
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:	
             proc_bit_field |= (1 << 2);       
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    switch(proc_fsm_state)
	        {
				case FSM_PREPARE_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
				break;
		        case FSM_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
		        break;
           	case FSM_PREPARE_FOR_DOOR_CLOSE:
              #ifdef TRACE_FLOW
				      Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT\r");
				   #endif
		        break;	        
	        }
		   #ifdef TRACE_ERROR
		      Print("ERR: Event - door not opened at stationary \r");
		   #endif
		   cur_elevator_ptr->elevator_status = ERR_DOOR_NOT_OPENED_AT_STATIONARY;
		   appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("11.19.07", ERROR_OCCURED, appl_status_flag);		     		    
		    return appl_status_flag;
		 //break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.19.08", ERROR_OCCURED, appl_status_flag);
		 break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.19.09", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		     switch(proc_fsm_state)
	        {
				case FSM_PREPARE_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
				break;
		        case FSM_USER_ENTRY_AND_EXIT:
				   #ifdef TRACE_FLOW
				      Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
				   #endif
		        break;
            case FSM_PREPARE_FOR_DOOR_CLOSE:
              #ifdef TRACE_FLOW
				      Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT\r");
				   #endif
		        break;		        
	        }
		   #ifdef TRACE_ERROR
		      Print("ERR: Event - door closed at stationary \r");
		   #endif		   
		    cur_elevator_ptr->elevator_status = ERR_DOORS_CLOSED_AT_STATIONARY; 
			appl_status_flag = ERR_SW_IS_ACTIVE;
	        Error_or_Warning_Proc("11.19.10", ERROR_OCCURED, appl_status_flag);
		    return appl_status_flag;  
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:
		    proc_bit_field |= (1 << 3);
		 break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		   appl_status_flag = ERR_SW_IS_DISABLED;
	       Error_or_Warning_Proc("11.19.11", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.19.12", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;
	}
	if(proc_bit_field != 0x0F)
	{
	  	return FAILURE;
	}
	
    return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Poll_Hall_And_In_Car_Floor_Calls_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.20

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Poll_Hall_And_In_Car_Floor_Calls_Proc(const uint8_t ctrl_elevator_ch_id )
{
    elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
    uint8_t cur_floor_call, min_floor_call, max_floor_call, oper_floor_call_type, proc_bit_field = 0, *floor_call_type_ptr;
	uint16_t Active_Hall_And_In_Car_Calls_Proc(const uint8_t ctrl_elevator_ch_id, const uint8_t floor_call_type, const uint8_t floor_call );
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.20.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	switch(cur_elevator_ptr->before_fsm_state)
	{
		case FSM_PREPARE_USER_ENTRY_AND_EXIT:
		case FSM_WAIT_FOR_DOOR_CLOSE_TO_START_OPEN:
			    proc_bit_field |= (1 << 0 );
		break;
		default:
		  return SUCCESS;
	}
	if((proc_bit_field & (1 << 0)))
	{
	   if((Validate_Floor(cur_elevator_ptr->cur_floor)) != SUCCESS)
	   {
	      appl_status_flag = ERR_FLOOR_INVALID;
	      Error_or_Warning_Proc("11.20.02", ERROR_OCCURED, appl_status_flag);
	      return appl_status_flag;
	   }
	   for(oper_floor_call_type = UP_SW_PROC; oper_floor_call_type <= IN_CAR_SW_PROC; ++oper_floor_call_type)
	   {
	      switch(oper_floor_call_type)
		  {
			  case UP_SW_PROC:
			     min_floor_call = FLOOR_00;
				 max_floor_call = TOP_MOST_FLOOR_ID - 1; 
				 floor_call_type_ptr = hall_call_up_arr;
			  break;
			  case DOWN_SW_PROC:
			     min_floor_call = FLOOR_01;
				 max_floor_call = TOP_MOST_FLOOR_ID;
				 floor_call_type_ptr = hall_call_down_arr;
			  break;
			  case IN_CAR_SW_PROC:
			     min_floor_call = FLOOR_00;
				 max_floor_call = TOP_MOST_FLOOR_ID;
				 floor_call_type_ptr = in_car_call_arr;
			  break;	 
		  }			
          for(cur_floor_call = min_floor_call; cur_floor_call <= max_floor_call; ++cur_floor_call)			  
		  {
			   if(cur_elevator_ptr->elevator_status == STARTUP_STATIONARY || cur_elevator_ptr->elevator_status == MOVED_UP_STATIONARY || cur_elevator_ptr->elevator_status == MOVED_DOWN_STATIONARY)
			   {
			      if(cur_floor_call == cur_elevator_ptr->cur_floor)
			      {
				      continue;
			      }
			   }
			   ret_status = SW_Oper(*(floor_call_type_ptr + cur_floor_call), DEV_READ_OPER);
               switch(ret_status)
	           {
		            case SUCCESS:
			           ret_status = Active_Hall_And_In_Car_Calls_Proc(ctrl_elevator_ch_id, oper_floor_call_type, cur_floor_call);
				       switch(ret_status)
				       {
                           case SUCCESS:
					       break;
						   case WARN_FLOOR_CALL_SAME_CUR_FLOOR:
						   case WARN_FLOOR_CALL_ALREADY_SELECT:
							  appl_status_flag = WARN_FLOORS_CALL_PROC;
	                          Error_or_Warning_Proc("11.20.03", WARNING_OCCURED, appl_status_flag);
		                      return appl_status_flag;
						  //break;
						   default:
						      appl_status_flag = ERR_FORMAT_INVALID;
	                          Error_or_Warning_Proc("11.20.04", ERROR_OCCURED, appl_status_flag);
	                          return appl_status_flag;
						}
					break;
					case SW_OR_KEY_NOT_PRESSED:		    
	                break;
		            case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
					case WARN_CUR_DATA_ID_DEV_DISABLED:
		            break;
                    default:
                        appl_status_flag = ERR_SW_READ_PROC;
	                    Error_or_Warning_Proc("11.20.05", ERROR_OCCURED, appl_status_flag);
	                    return appl_status_flag;
				}
		  }
		  if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
		  {
			  appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	          Error_or_Warning_Proc("11.20.06", ERROR_OCCURED, appl_status_flag);
	          return appl_status_flag;
		  }
	   }
	}
	return SUCCESS;
}		

/*------------------------------------------------------------*
FUNCTION NAME  : Active_Hall_And_In_Car_Calls_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.21

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Active_Hall_And_In_Car_Calls_Proc(const uint8_t ctrl_elevator_ch_id, const uint8_t floor_call_type, const uint8_t floor_call )
{
    elevator_ctrl_and_status_t *cur_elevator_ptr;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.21.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
    switch(floor_call_type)
	{
		case UP_SW_PROC:
		case DOWN_SW_PROC:
		case IN_CAR_SW_PROC:
		  if(cur_elevator_ptr->elevator_status == STARTUP_STATIONARY || cur_elevator_ptr->elevator_status == MOVED_UP_STATIONARY || cur_elevator_ptr->elevator_status == MOVED_DOWN_STATIONARY)
		  {
		      if(floor_call == cur_elevator_ptr->cur_floor)
	          {
	        	#ifdef TRACE_ERROR 
		           uint32_temp_disp_data = floor_call;
		           Print("WARN: floor call: %u = current floor \r", uint32_temp_disp_data);
                #endif 		
	            appl_status_flag = WARN_FLOOR_CALL_SAME_CUR_FLOOR;
	            Error_or_Warning_Proc("11.21.03", WARNING_OCCURED, appl_status_flag);
	            return appl_status_flag; 
	         }
		  }
		break;
		default:
		   appl_status_flag = ERR_FLOOR_INVALID;
	       Error_or_Warning_Proc("11.21.03", ERROR_OCCURED, appl_status_flag);
	       return appl_status_flag;
	}		
	switch(floor_call_type)
	{
		case UP_SW_PROC:
		   if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << floor_call)))
		   {
			   #ifdef TRACE_ERROR
			       uint32_temp_disp_data = floor_call;
			       Print("WARN: hall up floor call : %u is already pending \r", uint32_temp_disp_data);
			   #endif
			   appl_status_flag = WARN_HALL_CALL_UP_FLOOR_IS_ACTIVE ;
	           Error_or_Warning_Proc("11.21.08", WARNING_OCCURED, appl_status_flag);		 
		       return appl_status_flag;
		   }					   
		   cur_elevator_ptr->hall_floor_call_up_bit_field |= (1 << floor_call);
		   #ifdef TRACE_DATA
		       uint32_temp_disp_data = cur_elevator_ptr->hall_floor_call_up_bit_field;
	           Print("hall calls up bit field : 0x%x\r",uint32_temp_disp_data );
		   #endif
		   if(cur_elevator_ptr->cur_max_floor_call_up == FLOOR_ID_INVALID)
	       {
	 	       cur_elevator_ptr->cur_max_floor_call_up = floor_call;
		       cur_elevator_ptr->cur_min_floor_call_up = floor_call;
			   cur_elevator_ptr->num_pending_calls_up = 1;    
	       }
	       else
	       {
			   ++cur_elevator_ptr->num_pending_calls_up;
	           if(cur_elevator_ptr->cur_max_floor_call_up < floor_call)
		       {
	 	          	cur_elevator_ptr->cur_max_floor_call_up = floor_call;
	           }
		       if(cur_elevator_ptr->cur_min_floor_call_up > floor_call)
		       {
		          	cur_elevator_ptr->cur_min_floor_call_up = floor_call;
		       }			  
		      #ifdef TRACE_DATA
		            uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_up;
		            Print("TRA: pending hall up floor calls range[%u , ", uint32_temp_disp_data);
		            uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_up;
		            Print("%u], ", uint32_temp_disp_data); 
					uint32_temp_disp_data = cur_elevator_ptr->num_pending_calls_up;
		            Print("num hall up calls: %u\r", uint32_temp_disp_data);
		      #endif	 
	       }
		break;
		case DOWN_SW_PROC:
		   if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << floor_call)))
		   {
			   #ifdef TRACE_ERROR
			       uint32_temp_disp_data = floor_call;
			       Print("WARN: hall down floor call : %u is already pending \r", uint32_temp_disp_data);
			   #endif
		       appl_status_flag = WARN_HALL_CALL_DOWN_FLOOR_IS_ACTIVE ;
	           Error_or_Warning_Proc("11.21.10", WARNING_OCCURED, appl_status_flag);		 
		       return appl_status_flag;
		   }				   
		   cur_elevator_ptr->hall_floor_call_down_bit_field |= (1 << floor_call);
		   #ifdef TRACE_DATA 
		        uint32_temp_disp_data = cur_elevator_ptr->hall_floor_call_down_bit_field;
		        Print("hall floor calls down bit field   : 0x%x\r", uint32_temp_disp_data);
		   #endif
		   if(cur_elevator_ptr->cur_max_floor_call_down == FLOOR_ID_INVALID)
	       {
	 	        cur_elevator_ptr->cur_max_floor_call_down = floor_call;
		        cur_elevator_ptr->cur_min_floor_call_down = floor_call;
			    cur_elevator_ptr->num_pending_calls_down = 1; 
	       }
	       else
	       {
	          	 ++cur_elevator_ptr->num_pending_calls_down;
	             if(cur_elevator_ptr->cur_max_floor_call_down < floor_call)
		         {
	 	           	cur_elevator_ptr->cur_max_floor_call_down = floor_call;
	             }
		         if(cur_elevator_ptr->cur_min_floor_call_down > floor_call)
		         {
		          	cur_elevator_ptr->cur_min_floor_call_down = floor_call;
		         }						 
		         #ifdef TRACE_DATA
		              uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_down;
		              Print("TRA: pending hall down floor call range[%u , ", uint32_temp_disp_data);
		              uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_down;
		              Print("%u], ", uint32_temp_disp_data); 
				      uint32_temp_disp_data = cur_elevator_ptr->num_pending_calls_down;
		        	  Print("num hall down calls: %u\r", uint32_temp_disp_data);
		         #endif	 
	       }  		
		break;
		case IN_CAR_SW_PROC:
		   if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << floor_call)))
		   {
			   #ifdef TRACE_ERROR
			       uint32_temp_disp_data = floor_call;
			       Print("WARN: in car floor call : %u is already pending \r", uint32_temp_disp_data);
			   #endif
			    appl_status_flag = WARN_IN_CAR_FLOOR_CALL_IS_ACTIVE ;
	            Error_or_Warning_Proc("11.21.06", WARNING_OCCURED, appl_status_flag);		 
		        return appl_status_flag;
		   } 						   					   
		   cur_elevator_ptr->in_car_floor_call_bit_field |= (1 << floor_call);
		   #ifdef TRACE_DATA
		       Print("in car floor calls bit field     : 0x%x\r", cur_elevator_ptr->in_car_floor_call_bit_field);
		   #endif
			if(cur_elevator_ptr->cur_max_floor_call_in_car == FLOOR_ID_INVALID)
	        {
	 	        cur_elevator_ptr->cur_max_floor_call_in_car = floor_call;
		        cur_elevator_ptr->cur_min_floor_call_in_car = floor_call;
			    cur_elevator_ptr->num_pending_calls_in_car = 1; 
	        }
	        else
	        {
				 ++cur_elevator_ptr->num_pending_calls_in_car;
	            if(cur_elevator_ptr->cur_max_floor_call_in_car < floor_call)
		        {
	 	           	cur_elevator_ptr->cur_max_floor_call_in_car = floor_call;
	            }
		        if(cur_elevator_ptr->cur_min_floor_call_in_car > floor_call)
		        {
		           	cur_elevator_ptr->cur_min_floor_call_in_car = floor_call;
		        }			  
		        #ifdef TRACE_DATA
		             uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_in_car;
		             Print("TRA: pending in car floor call range[%u , ", uint32_temp_disp_data);
		             uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_in_car;
		             Print("%u], ", uint32_temp_disp_data); 
				 	uint32_temp_disp_data = cur_elevator_ptr->num_pending_calls_in_car;
		             Print("num in car calls: %u\r", uint32_temp_disp_data);
		        #endif	 
	        }
		break;
		default:
         appl_status_flag = ERR_FORMAT_INVALID;
	     Error_or_Warning_Proc("11.21.11", ERROR_OCCURED, appl_status_flag);
	     return appl_status_flag;
	}
	if((cur_elevator_ptr->pending_floor_call_bit_field & (1 << floor_call)))
	{
		return SUCCESS;
	}
	cur_elevator_ptr->pending_floor_call_bit_field |= (1 << floor_call);
	#ifdef TRACE_INFO
	    uint32_temp_disp_data = floor_call; 
	    Print("INFO: Pending floor call : %u added \r", uint32_temp_disp_data);
    #endif	
	if(cur_elevator_ptr->cur_max_floor_call == FLOOR_ID_INVALID)
	{
	     cur_elevator_ptr->cur_max_floor_call = floor_call;
	     cur_elevator_ptr->cur_min_floor_call = floor_call;
	     cur_elevator_ptr->next_stop_floor = floor_call;
	     cur_elevator_ptr->num_pending_calls = 1;
		 #ifdef TRACE_DATA
		      uint32_temp_disp_data = floor_call;
		      Print("TRA: first pending, next_stop: %u \r", uint32_temp_disp_data);
	     #endif		  
	}
	else
	{
	    ++cur_elevator_ptr->num_pending_calls;
	 	 if(cur_elevator_ptr->cur_max_floor_call < floor_call)
	     {
	       	cur_elevator_ptr->cur_max_floor_call = floor_call;
	     }
	     if(cur_elevator_ptr->cur_min_floor_call > floor_call)
	     {
	       	cur_elevator_ptr->cur_min_floor_call = floor_call;
	     }
	     #ifdef TRACE_DATA
	         uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call;
	         Print("TRA: pending floor call range[%u , ", uint32_temp_disp_data);
	         uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
	      	 Print("%u], ", uint32_temp_disp_data); 
			 uint32_temp_disp_data = cur_elevator_ptr->num_pending_calls;
	       	 Print("num calls: %u\r", uint32_temp_disp_data);
	     #endif	 
	}
	#ifdef TRACE_DATA
	     uint32_temp_disp_data = cur_elevator_ptr->pending_floor_call_bit_field;
	     Print("TRA: pending floor calls bit field : 0x%x \r", uint32_temp_disp_data);
	#endif
	return SUCCESS;				
}

/*------------------------------------------------------------*
FUNCTION NAME  : Compute_Floor_Stop_Datas

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.22

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Compute_Floor_Stop_Datas(const uint8_t ctrl_elevator_ch_id, const uint8_t stage_type, uint8_t *const elevator_status_ptr)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	uint8_t stop_floor_call_in_car_and_move, stop_floor_call_move;
	static int8_t cur_floor;
	
	if(elevator_status_ptr == NULL_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
	    Error_or_Warning_Proc("11.22.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
	}
	switch(stage_type)
	{
		case DOOR_OPENED_STAGE:
	    case DOOR_CLOSED_STAGE:
		break;
		default:
		   	appl_status_flag = ERR_FORMAT_INVALID;
	        Error_or_Warning_Proc("11.22.02", ERROR_OCCURED, appl_status_flag);
	        return appl_status_flag;  		
	}
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.22.03", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(cur_elevator_ptr->pending_floor_call_bit_field == 0)
	{
		//no more floor calls 		
		 *elevator_status_ptr = NO_PENDING_FLOOR_CALLS;
		 return SUCCESS;
	}	
	switch(cur_elevator_ptr->elevator_status)
    { 
      	case MOVED_UP_STATIONARY:
		    if(cur_elevator_ptr->num_pending_calls_in_car != 0) 
 			{
			   if(cur_elevator_ptr->num_pending_calls_up != 0)
			   {
				   stop_floor_call_in_car_and_move = (cur_elevator_ptr->cur_max_floor_call_in_car > cur_elevator_ptr->cur_max_floor_call_up) ? cur_elevator_ptr->cur_max_floor_call_in_car : cur_elevator_ptr->cur_max_floor_call_up;
			   }
			   else
			   {
				   stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_max_floor_call_in_car;
			   }
			}
            else 
			{
                if(cur_elevator_ptr->num_pending_calls_up != 0)
			    {
				     stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_max_floor_call_up;
				}
				else
				{
					 stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_floor;
				}
			}
            if(cur_elevator_ptr->num_pending_calls_down != 0)
			{
				 if((cur_elevator_ptr->cur_max_floor_call_down >= stop_floor_call_in_car_and_move ))
				 {
					 stop_floor_call_move = cur_elevator_ptr->cur_max_floor_call_down;
				 }
                 else
				 {
					  stop_floor_call_move = stop_floor_call_in_car_and_move;
				 }					 
			}
            else
			{
				 stop_floor_call_move = stop_floor_call_in_car_and_move;
			}
			if(stop_floor_call_move != cur_elevator_ptr->cur_floor)
			{ 
		        cur_elevator_ptr->cur_max_floor_call = stop_floor_call_move;
				#ifdef TRACE_DATA
				   uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
			       Print("TRA: 11.22.05: MOVE_UP, max_call: %u \r", uint32_temp_disp_data);
			    #endif
			}
			#ifdef TRACE_DATA
			   uint32_temp_disp_data = stop_floor_call_in_car_and_move;
			   Print("TRA: 11.22.06: MOVE_UP, in_car_and_move: %u, ", uint32_temp_disp_data);
			   uint32_temp_disp_data = stop_floor_call_move;
			   Print("move : %u \r",  uint32_temp_disp_data);
			#endif 			
		    if(stop_floor_call_in_car_and_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_floor <= stop_floor_call_in_car_and_move)
			{
				#ifdef TRACE_DATA
				  uint32_temp_disp_data = cur_elevator_ptr->cur_floor + 1;
			      Print("TRA: 11.22.07: MOVE_UP, cur_floor + 1: %u & ", uint32_temp_disp_data);
				  uint32_temp_disp_data = stop_floor_call_in_car_and_move;
			      Print("in_car_and_move : %u \r", uint32_temp_disp_data);
			    #endif 
				for(cur_floor = cur_elevator_ptr->cur_floor + 1; cur_floor <= stop_floor_call_in_car_and_move; ++cur_floor)
			    {
				    if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)) || (cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor)))
			        {
				        switch(stage_type)
				        {
			             	case DOOR_OPENED_STAGE:					   
			    	           if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor))) 
			    	           {
								    #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_floor;
			                             Print("TRA: 11.22.08: HALL_UP & IN_CAR: MOVE_UP, in_car call : %u \r", uint32_temp_disp_data);
			                        #endif
								   if(cur_elevator_ptr->cur_min_floor_call_in_car == cur_elevator_ptr->cur_floor)
								   {
								      cur_elevator_ptr->cur_min_floor_call_in_car = cur_floor;
									  #ifdef TRACE_DATA
									      uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_in_car;
			                             Print("TRA: 11.22.09: HALL_UP & IN_CAR: MOVE_UP, min_in_car : %u \r", uint32_temp_disp_data);
			                          #endif
								   }
						       }
							   if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor))) 
							   {
								   #ifdef TRACE_DATA
								      uint32_temp_disp_data = cur_floor;
			                          Print("TRA: 11.22.10: HALL_UP & IN CAR MOVE_UP, hall_up call : %u \r", uint32_temp_disp_data);
			                        #endif
								   if(cur_elevator_ptr->cur_min_floor_call_up == cur_elevator_ptr->cur_floor)
								   {
								       cur_elevator_ptr->cur_min_floor_call_up = cur_floor;
                                        #ifdef TRACE_DATA
										   uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_up;
			                               Print("TRA 11.22.11: HALL_UP & IN_CAR: MOVE_UP, min_hall_up : %u \n", uint32_temp_disp_data);
			                            #endif									   
								   }
							   }			   
					        break;
					        case DOOR_CLOSED_STAGE:					   	
					          cur_elevator_ptr->next_stop_floor = cur_floor;
					          #ifdef TRACE_DATA
							     uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
			                     Print("TRA: 11.22.12: HALL_UP & IN_CAR: MOVE_UP, next_stop : %u \r", uint32_temp_disp_data);
			                 #endif	
					        break;
						}
						break;
					}		
			   	 }
			     if(cur_floor > cur_elevator_ptr->cur_max_floor_call)
			     {
						#ifdef TRACE_FLOW
					       Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \r");
						#endif
						#ifdef TRACE_ERROR
					       uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;						
			               Print("ERR: event - max_stop : %u but not in stop list \r", uint32_temp_disp_data);
				        #endif		
					 appl_status_flag = ERR_MAX_STOP_FLOOR_REACH_BUT_NOT_IN_STOP_LIST;
	                 Error_or_Warning_Proc("11.22.13", ERROR_OCCURED, appl_status_flag);					 
	                 return SUCCESS;
			     }
				 #ifdef TRACE_FLOW				     
					  Print("TRA: 11.22.14: HALL_UP & IN_CAR, TRIGGER_MOVE_UP_NO_DIR_CHANGE \r"); 
				 #endif				  
				 *elevator_status_ptr = TRIGGER_MOVE_UP_NO_DIR_CHANGE;
				 return SUCCESS;
			 }
		     if(cur_elevator_ptr->num_pending_calls_down != 0 && cur_elevator_ptr->cur_max_floor_call_down > stop_floor_call_in_car_and_move)
		     {
				  #ifdef TRACE_DATA	
                        uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_down;					 
					    Print("TRA: 11.22.15: HALL_DOWN: MOVE_UP, max_down: %u > ",uint32_temp_disp_data ); 
						uint32_temp_disp_data = stop_floor_call_in_car_and_move;					 
					    Print("in_car_and_move : %u\r",uint32_temp_disp_data); 
				     #endif	
					switch(stage_type)
					{
						case DOOR_OPENED_STAGE:   
					    break;
						case DOOR_CLOSED_STAGE:
						   cur_elevator_ptr->next_stop_floor = cur_elevator_ptr->cur_max_floor_call_down;
					       #ifdef TRACE_DATA
                               uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;  						   
					           Print("TRA: 11.22.16: HALL_DOWN: MOVE_UP, next_stop : %u \r", uint32_temp_disp_data); 
				           #endif	
						break;   
					}					
				    #ifdef TRACE_FLOW				     
					    Print("TRA: 11:22.17: HALL_DOWN: MOVE_UP, TRIGGER_MOVE_UP_NO_DIR_CHANGE \r"); 
				    #endif				  
				    *elevator_status_ptr = TRIGGER_MOVE_UP_NO_DIR_CHANGE;
				   return SUCCESS;
			 }
			//action to move down
			//cur_elevator_ptr->cur_max_floor_call_down == stop_floor_call_in_car_and_move
			 if(cur_elevator_ptr->num_pending_calls_down != 0 )
		     {
			        if(cur_elevator_ptr->num_pending_calls_in_car != 0)
			        {
					    stop_floor_call_in_car_and_move = (cur_elevator_ptr->cur_min_floor_call_in_car < cur_elevator_ptr->cur_min_floor_call_down) ? cur_elevator_ptr->cur_min_floor_call_in_car : cur_elevator_ptr->cur_min_floor_call_down;
			        }
			        else
			        {
				       stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_min_floor_call_down;
				    }
			 }
		     else
		     {
				   if(cur_elevator_ptr->num_pending_calls_in_car != 0)
			       {
				        stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_min_floor_call_in_car;
				   }
				   else
			       {
				     	 stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_floor;
			       }      
		    }
		    if(cur_elevator_ptr->num_pending_calls_up != 0)
		    {
				   if((cur_elevator_ptr->cur_min_floor_call_up < stop_floor_call_in_car_and_move ))
				   {
				  	   stop_floor_call_move = cur_elevator_ptr->cur_min_floor_call_up;
				   }
                   else
				   {
				   	  stop_floor_call_move = stop_floor_call_in_car_and_move;
				   }					 
			}
		    else
		    {
			       	 stop_floor_call_move = stop_floor_call_in_car_and_move;
			}
			#ifdef TRACE_DATA
			     uint32_temp_disp_data = stop_floor_call_in_car_and_move;
			     Print("TRA: 11.22.18: HALL_DOWN & IN_CAR: MOVE_UP, in_car_and_move: %u & ",uint32_temp_disp_data);
				 uint32_temp_disp_data = stop_floor_call_move;
			     Print("move : %u\r", uint32_temp_disp_data);
		    #endif 
			if(stop_floor_call_move != cur_elevator_ptr->cur_floor)
			{
		         cur_elevator_ptr->cur_min_floor_call = stop_floor_call_move;
				 #ifdef TRACE_DATA
				    uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call;
			        Print("TRA: 11.22.19: HALL_DOWN & IN_CAR: MOVE_UP, min_call: %u \r", uint32_temp_disp_data);
		        #endif 
			}
			#ifdef TRACE_DATA
			    uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
			    Print("TRA: 11.22.20: HALL_DOWN & IN_CAR: MOVE_UP, cur_floor: %u & " , uint32_temp_disp_data);
				uint32_temp_disp_data = stop_floor_call_in_car_and_move;
				Print("in_car_and_move: %u\r", uint32_temp_disp_data);
		    #endif
			if(cur_elevator_ptr->num_pending_calls_up != 0)
			{
				 switch(stage_type)
				 {
					case DOOR_OPENED_STAGE:	
					   for(cur_floor = cur_elevator_ptr->cur_max_floor_call_up - 1; cur_floor >= cur_elevator_ptr->cur_min_floor_call_up; --cur_floor) 
					   {
						 	 if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor))) 
						     {
							    cur_elevator_ptr->cur_max_floor_call_up = cur_floor;									
								#ifdef TRACE_DATA
								     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_up;
			                         Print("TRA: 11.22.21: HALL_UP: MOVE_UP, max_up : %u\r", uint32_temp_disp_data);
			                    #endif
						    	break;
							 }	
					   }      
					break;  
					case DOOR_CLOSED_STAGE:	                       					
					break; 
				 }		
			 }
             if(stop_floor_call_in_car_and_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_floor >= stop_floor_call_in_car_and_move)
		     {	
		         for(cur_floor = cur_elevator_ptr->cur_floor - 1; cur_floor >= (int8_t)stop_floor_call_in_car_and_move; --cur_floor)
				 {
			 	    if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)) || (cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor)))
			   	    {
			           switch(stage_type)						
			     	   {
			    	     	case DOOR_OPENED_STAGE:						  
					      
						           if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor)))
						           {
								      cur_elevator_ptr->cur_max_floor_call_down = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_down;
			                             Print("TRA: 11.22.22: HALL_DOWN & IN_CAR: MOVE_UP, max_down : %u \r", uint32_temp_disp_data );
		                              #endif
							       }
							       if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)))
							       {
								      cur_elevator_ptr->cur_max_floor_call_in_car = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_in_car;
			                             Print("TRA: 11.22.23: HALL_DOWN & IN_CAR: MOVE_UP, max_in_car : %u \r", uint32_temp_disp_data);
		                              #endif
							       }
			   	        	       cur_elevator_ptr->cur_max_floor_call = cur_floor;
                                   #ifdef TRACE_DATA
								       uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
			                           Print("TRA: 11.22.24: HALL_DOWN & IN_CAR: MOVE_UP, max_call : %u\r", uint32_temp_disp_data );
		                           #endif 
								    					
						    break;
						    case DOOR_CLOSED_STAGE:		
			                   cur_elevator_ptr->next_stop_floor = cur_floor;
						       #ifdef TRACE_DATA
							      uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
			                     Print("TRA: 11.22.25: HALL_DOWN & IN_CAR: MOVE_UP, next_stop : %u \r", uint32_temp_disp_data );
		                       #endif  
					        break;
					    }
						break;
 					}
				 }			
					 if(cur_floor < cur_elevator_ptr->cur_min_floor_call)
			         {
						  #ifdef TRACE_FLOW  
			                 Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \r");
						  #endif
						  #ifdef TRACE_ERROR
						     uint32_temp_disp_data = cur_floor;	
						     Print("ERR: event - move down cur : %u < ", uint32_temp_disp_data);
						     uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call;
					         Print("min_call : %u \r", uint32_temp_disp_data );
				        #endif
				        appl_status_flag = ERR_MIN_STOP_FLOOR_INVALID;
	                    Error_or_Warning_Proc("11.22.26", ERROR_OCCURED, appl_status_flag);
	                    return SUCCESS; 
			         }
			         #ifdef TRACE_FLOW
			             Print("TRA: 11.22.27: HALL_DOWN & IN_CAR: MOVE_UP, TRIGGER_MOVE_DOWN_DIR_CHANGE \r"); 
			         #endif
			         *elevator_status_ptr = TRIGGER_MOVE_DOWN_DIR_CHANGE;
			         return SUCCESS;	
			  }
			 if(stop_floor_call_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_min_floor_call_up < stop_floor_call_in_car_and_move)
			 {
				 switch(stage_type)
				 {
					case DOOR_OPENED_STAGE:	
					    if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_elevator_ptr->cur_min_floor_call_up)))
						{
							 cur_elevator_ptr->next_stop_floor = cur_elevator_ptr->cur_min_floor_call_up;
							 #ifdef TRACE_DATA
							    uint32_temp_disp_data =  cur_elevator_ptr->next_stop_floor;
			                    Print("TRA: 11.22.28: HALL_UP: MOVE_UP, TRIGGER_MOVE_DOWN_DIR_CHANGE, next_stop : %u\r",uint32_temp_disp_data ); 
			                 #endif		
                             for(cur_floor = cur_elevator_ptr->cur_min_floor_call_up + 1; cur_floor <= cur_elevator_ptr->cur_max_floor_call_up; ++cur_floor) 
							 {
								 if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor))) 
							     {
								    cur_elevator_ptr->cur_min_floor_call_up = cur_floor;									
									#ifdef TRACE_DATA
									    uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_up;
			                            Print("TRA: 11.22.29: HALL_UP: MOVE_UP, min_up : %u\r", uint32_temp_disp_data);
			                        #endif
							    }	
							}								 
						}
					break;  
					case DOOR_CLOSED_STAGE:					  
					break; 
				}
				#ifdef TRACE_FLOW
			       Print("TRA: 11.22.30: HALL_UP: MOVE_UP, UP TRIGGER_MOVE_DOWN_DIR_CHANGE \r"); 
			    #endif
			    *elevator_status_ptr = TRIGGER_MOVE_DOWN_DIR_CHANGE;
			    return SUCCESS;				
			 }			  
		break;
		case MOVED_DOWN_STATIONARY:
		    if(cur_elevator_ptr->num_pending_calls_down != 0 )
		     {
			        if(cur_elevator_ptr->num_pending_calls_in_car != 0)
			        {
					    stop_floor_call_in_car_and_move = (cur_elevator_ptr->cur_min_floor_call_in_car < cur_elevator_ptr->cur_min_floor_call_down) ? cur_elevator_ptr->cur_min_floor_call_in_car : cur_elevator_ptr->cur_min_floor_call_down;
			        }
			        else
			        {
				       stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_min_floor_call_down;
				    }
			 }
		     else
		     {
				   if(cur_elevator_ptr->num_pending_calls_in_car != 0)
			       {
				        stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_min_floor_call_in_car;
				   }
				   else
			       {
				     	 stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_floor;
			       }      
		    }
		    if(cur_elevator_ptr->num_pending_calls_up != 0)
		    {
				   if((cur_elevator_ptr->cur_min_floor_call_up < stop_floor_call_in_car_and_move ))
				   {
				  	   stop_floor_call_move = cur_elevator_ptr->cur_min_floor_call_up;
				   }
                   else
				   {
				   	  stop_floor_call_move = stop_floor_call_in_car_and_move;
				   }					 
			}
		    else
		    {
			       	 stop_floor_call_move = stop_floor_call_in_car_and_move;
			}
			#ifdef TRACE_DATA
			     uint32_temp_disp_data = stop_floor_call_in_car_and_move;
			     Print("TRA: 11.22.31: HALL_DOWN & IN_CAR: MOVE_DOWN, in_car_and_move: %u & ",uint32_temp_disp_data);
				 uint32_temp_disp_data = stop_floor_call_move;
			     Print("move : %u\r",uint32_temp_disp_data);
		    #endif 
			if(stop_floor_call_move != cur_elevator_ptr->cur_floor)
			{
		         cur_elevator_ptr->cur_min_floor_call = stop_floor_call_move;
				 #ifdef TRACE_DATA
				    uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call; 
			        Print("TRA: 11.22.32: HALL_DOWN & IN_CAR: MOVE_DOWN, min_call : %u\r", uint32_temp_disp_data);
		        #endif 
			}
            if(stop_floor_call_in_car_and_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_floor >= stop_floor_call_in_car_and_move)
		    {	                      		   
			         for(cur_floor = cur_elevator_ptr->cur_floor - 1; cur_floor >= (int8_t)stop_floor_call_in_car_and_move; --cur_floor)
					 {
			 	        if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)) || (cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor)))
			   	        {
			   	        	 switch(stage_type)						
			     	         {
			    	         	case DOOR_OPENED_STAGE:
						           if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor)))
						           {
								      cur_elevator_ptr->cur_max_floor_call_down = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_down;
			                             Print("TRA: 11.22.33: HALL_DOWN & IN_CAR: MOVE_DOWN, max_down : %u\r", uint32_temp_disp_data);
		                              #endif
							       }
							       if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)))
							       {
								      cur_elevator_ptr->cur_max_floor_call_in_car = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_in_car ;
			                             Print("TRA: 11.22.34: HALL_DOWN & IN CAR: MOVE_DOWN, max_in_car : %u\r", uint32_temp_disp_data );
		                              #endif
							       }
			   	        	       cur_elevator_ptr->cur_max_floor_call = cur_floor;
                                   #ifdef TRACE_DATA
								       uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
			                           Print("TRA: 11.22.35: HALL_DOWN & IN CAR: MOVE_DOWN, max_call : %u \r", uint32_temp_disp_data);
		                           #endif  								   
						        break;
						        case DOOR_CLOSED_STAGE:		
			            	       cur_elevator_ptr->next_stop_floor = cur_floor;
								   #ifdef TRACE_DATA
								       uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
			                           Print("TRA: 11.22.36: HALL_DOWN & IN CAR: MOVE_DOWN, stop_floor : %u \r", uint32_temp_disp_data );
		                           #endif  
					            break;
					         }
			     	      break;
			            }
			         }
					 if(cur_floor < cur_elevator_ptr->cur_min_floor_call)
			         {
				        #ifdef TRACE_FLOW
			              Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \n");
						 #endif
                         #ifdef TRACE_ERROR						 
						  uint32_temp_disp_data = cur_floor;
					      Print("ERR: event - move down cur : %u < ",uint32_temp_disp_data );
						  uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call;
					      Print("min_call : %u \r", uint32_temp_disp_data );
				        #endif
				        appl_status_flag = ERR_MIN_STOP_FLOOR_INVALID;
	                    Error_or_Warning_Proc("11.22.37", ERROR_OCCURED, appl_status_flag);
	                    return appl_status_flag; 
			         }
			         #ifdef TRACE_FLOW
			             Print("TRA: 11.22.38: HALL_DOWN & IN_CAR: MOVE_DOWN, TRIGGER_MOVE_DOWN_NO_DIR_CHANGE \r"); 
			         #endif
			         *elevator_status_ptr = TRIGGER_MOVE_DOWN_NO_DIR_CHANGE;
			         return SUCCESS;	
			}
			if(cur_elevator_ptr->num_pending_calls_up != 0 && cur_elevator_ptr->cur_min_floor_call_up < stop_floor_call_in_car_and_move)
		    {
				 #ifdef TRACE_DATA	
                        uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_up;					 
					    Print("TRA: 11.22.39: HALL_UP: MOVE_DOWN, min_up: %u < ", uint32_temp_disp_data );
						uint32_temp_disp_data = stop_floor_call_in_car_and_move;					 
					    Print("in_car_and_move : %u\r", uint32_temp_disp_data ); 
				     #endif	
					switch(stage_type)
					{
					    case DOOR_OPENED_STAGE:
					    break;
						case DOOR_CLOSED_STAGE:	  
						     cur_elevator_ptr->next_stop_floor = cur_elevator_ptr->cur_min_floor_call_up;
					         #ifdef TRACE_DATA	
                                 uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;							 
					            Print("TRA: 11.22.40: HALL_UP: MOVE_DOWN, next_stop : %u\r", uint32_temp_disp_data); 
				             #endif	            				
						break;   
					}					
				    #ifdef TRACE_FLOW				     
					    Print("TRA: 11.42.41: HALL_UP: MOVE_DOWN, TRIGGER_MOVE_DOWN_NO_DIR_CHANGE \r"); 
				    #endif				  
				    *elevator_status_ptr = TRIGGER_MOVE_DOWN_NO_DIR_CHANGE;
				   return SUCCESS; 
			}
			//action to move up
			//cur_elevator_ptr->cur_min_floor_call_up == stop_floor_call_in_car_and_move
			if(cur_elevator_ptr->num_pending_calls_in_car != 0) 
 			{
			   if(cur_elevator_ptr->num_pending_calls_up != 0)
			   {
				   stop_floor_call_in_car_and_move = (cur_elevator_ptr->cur_max_floor_call_in_car > cur_elevator_ptr->cur_max_floor_call_up) ? cur_elevator_ptr->cur_max_floor_call_in_car : cur_elevator_ptr->cur_max_floor_call_up;
			   }
			   else
			   {
				   stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_max_floor_call_in_car;
			   }
			}
            else 
			{
                if(cur_elevator_ptr->num_pending_calls_up != 0)
			    {
				     stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_max_floor_call_up;
				}
				else
				{
					 stop_floor_call_in_car_and_move = cur_elevator_ptr->cur_floor;
				}
			}
            if(cur_elevator_ptr->num_pending_calls_down != 0)
			{
				 if((cur_elevator_ptr->cur_max_floor_call_down >= stop_floor_call_in_car_and_move ))
				 {
					 stop_floor_call_move = cur_elevator_ptr->cur_max_floor_call_down;
				 }
                 else
				 {
					  stop_floor_call_move = stop_floor_call_in_car_and_move;
				 }					 
			}
            else
			{
				 stop_floor_call_move = stop_floor_call_in_car_and_move;
			}
			#ifdef TRACE_DATA
			     uint32_temp_disp_data = stop_floor_call_in_car_and_move;	
			     Print("TRA: 11.22.42: HALL_UP & IN_CAR: MOVE_DOWN, in_car_and_move: %u & ", uint32_temp_disp_data);
				 uint32_temp_disp_data = stop_floor_call_move;	
			     Print("move : %u\r", uint32_temp_disp_data);
		    #endif
			if(stop_floor_call_move != cur_elevator_ptr->cur_floor)
			{ 
		        cur_elevator_ptr->cur_max_floor_call = stop_floor_call_move;
				#ifdef TRACE_DATA
				   uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
			       Print("TRA: 11.22.43: MOVE_DOWN, max_call : %u \r", uint32_temp_disp_data);
			    #endif
			}
			if(cur_elevator_ptr->num_pending_calls_down != 0)
			{
				 switch(stage_type)
				 {
					case DOOR_OPENED_STAGE:	
					   for(cur_floor = cur_elevator_ptr->cur_min_floor_call_down + 1; cur_floor <= cur_elevator_ptr->cur_max_floor_call_down; ++cur_floor) 
					   {
						 	 if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor))) 
						     {
							    cur_elevator_ptr->cur_min_floor_call_down = cur_floor;									
								#ifdef TRACE_DATA
								     uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_down;
			                         Print("TRA: 11.22.44: HALL_DOWN: MOVE_DOWN, min_down : %u \r",uint32_temp_disp_data );
			                    #endif
						    	break;
							 }	
					   }      
					break;  
					case DOOR_CLOSED_STAGE:	                       					
					break; 
				 }		
			 }
               if(stop_floor_call_in_car_and_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_floor <= stop_floor_call_in_car_and_move)
		       {	                      		   
			         for(cur_floor = cur_elevator_ptr->cur_floor + 1; cur_floor <= stop_floor_call_in_car_and_move; ++cur_floor)
					 {
			 	        if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)) || (cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor)))
			   	        {
			   	        	 switch(stage_type)						
			     	         {
			    	         	case DOOR_OPENED_STAGE:
						           if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_floor)))
						           {
								      cur_elevator_ptr->cur_min_floor_call_up = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_up;
			                             Print("TRA: 11.22.45: HALL_UP & IN_CAR: MOVE_DOWN, min_up : %u\r", uint32_temp_disp_data );
		                              #endif
							       }
							       if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_floor)))
							       {
								      cur_elevator_ptr->cur_min_floor_call_in_car = cur_floor; 
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call_in_car;
			                             Print("TRA: 11.22.45: HALL_UP & IN_CAR: MOVE_DOWN, min_in_car : %u\r", uint32_temp_disp_data );
		                              #endif
							       }
			   	        	       cur_elevator_ptr->cur_min_floor_call = cur_floor;
                                   #ifdef TRACE_DATA
								       uint32_temp_disp_data = cur_elevator_ptr->cur_min_floor_call; 
			                           Print("TRA: 11.22.46: HALL_UP & IN_CAR: MOVE_DOWN, min_call : %u \r", uint32_temp_disp_data );
		                           #endif  								   
						        break;
						        case DOOR_CLOSED_STAGE:		
			            	       cur_elevator_ptr->next_stop_floor = cur_floor;
								   #ifdef TRACE_DATA
								       uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
			                           Print("TRA: 11.22.47: HALL_UP & IN_CAR: MOVE_DOWN, next_stop : %u\r", uint32_temp_disp_data );
		                           #endif  
					            break;
					         }
			     	        break;
			            }
			         }
					 if(cur_floor > cur_elevator_ptr->cur_max_floor_call)
			         {
				        #ifdef TRACE_FLOW
			              Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \n");
						#endif
                        #ifdef TRACE_ERROR						
 						  uint32_temp_disp_data = cur_floor;
					      Print("ERR: event - move up cur : %u > ",uint32_temp_disp_data );
						  uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call;
					      Print("max_call : %u \r",uint32_temp_disp_data );
				        #endif
				        appl_status_flag = ERR_MIN_STOP_FLOOR_INVALID;
	                    Error_or_Warning_Proc("11.22.48", ERROR_OCCURED, appl_status_flag);
	                    return SUCCESS; 
			         }
			         #ifdef TRACE_FLOW
			             Print("TRA: 11.22.48: HALL_UP & IN_CAR: MOVE_DOWN, TRIGGER_MOVE_UP_DIR_CHANGE\r"); 
			         #endif			           
			         *elevator_status_ptr = TRIGGER_MOVE_UP_DIR_CHANGE;
			         return SUCCESS;	
			  }
			  
			  if(stop_floor_call_move != cur_elevator_ptr->cur_floor && cur_elevator_ptr->cur_max_floor_call_down > stop_floor_call_in_car_and_move)
			  {
				 switch(stage_type)
				 {
					case DOOR_OPENED_STAGE:	
					    if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_elevator_ptr->cur_max_floor_call_down)))
						{
							 cur_elevator_ptr->next_stop_floor = cur_elevator_ptr->cur_max_floor_call_down;
							 #ifdef TRACE_DATA
							    uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
			                    Print("TRA: 11.22.49: HALL_DOWN: MOVE_DOWN, next_stop : %u\r", uint32_temp_disp_data); 
			                 #endif		
							 for(cur_floor = cur_elevator_ptr->cur_max_floor_call_down - 1; cur_floor >= (int8_t)cur_elevator_ptr->cur_min_floor_call_down; --cur_floor )
							 {
								  if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_floor)))
						          {
									  cur_elevator_ptr->cur_max_floor_call_down = cur_floor;
									  #ifdef TRACE_DATA
									     uint32_temp_disp_data = cur_elevator_ptr->cur_max_floor_call_down; 
			                             Print("TRA: 11.22.50: HALL_DOWN: MOVE_DOWN, max_down : %u\r", uint32_temp_disp_data );
		                              #endif
							      }
							 }
						}
					break;  
					case DOOR_CLOSED_STAGE:					  
					break; 
				}
                #ifdef TRACE_FLOW
			       Print("TRA: 11.22.51: HALL_DOWN: MOVE_DOWN, TRIGGER_MOVE_UP_DIR_CHANGE \r"); 
			    #endif
			    *elevator_status_ptr = TRIGGER_MOVE_UP_DIR_CHANGE;
			    return SUCCESS;				
			 }
		break;
	    case STARTUP_STATIONARY:
		  for(cur_floor = cur_elevator_ptr->cur_floor + 1; cur_floor <= cur_elevator_ptr->cur_max_floor_call; ++cur_floor)
		  {
		  	   if((cur_elevator_ptr->pending_floor_call_bit_field & (1 << cur_floor)) )
			   { 
			     ++stop_floor_call_move;
			   }
		  }
		  for(cur_floor = cur_elevator_ptr->cur_floor - 1; cur_floor >= (int8_t)cur_elevator_ptr->cur_min_floor_call; --cur_floor)
		  {
		  	   if((cur_elevator_ptr->pending_floor_call_bit_field & (1 << cur_floor)))
			   { 
			     ++stop_floor_call_in_car_and_move;
			   }
		  }
		  if(stop_floor_call_move >= stop_floor_call_in_car_and_move)
		  {
		  	  cur_elevator_ptr->elevator_status = MOVED_UP_STATIONARY;
		  }
		  else
		  {
		  	   cur_elevator_ptr->elevator_status = MOVED_DOWN_STATIONARY;
		  }
		break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_Proc("11.22.52", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;	
	 }
	return SUCCESS;
}

 /*------------------------------------------------------------*
FUNCTION NAME  : Internal_Error_Elevator_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.23

BUGS           :              
-*------------------------------------------------------------*/  
uint16_t Internal_Error_Elevator_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.23.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }
     cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	 
	 switch(cur_elevator_ptr->cur_fsm_state)	
	{
		case FSM_IDLE:	
          #ifdef TRACE_FLOW		
		   Print("IDLE -> ABNORMAL_EVENT\r");
          #endif		   
		break;
        case FSM_WAIT_FOR_START_OPER:
		  #ifdef TRACE_FLOW
        	Print("WAIT_FOR_START_OPER -> ABNORMAL_EVENT\r");
		   #endif		
        break;
        case FSM_STARTUP:
		  #ifdef TRACE_FLOW
           Print("STARTUP -> ABNORMAL_EVENT\r");
		   #endif	 
        break;
        case FSM_DECIDE_CAR_MOVEMENT:
		  #ifdef TRACE_FLOW
           Print("DECIDE_CAR_MOVEMENT -> ABNORMAL_EVENT\r");
		   #endif	 
        break;
        case FSM_TRIGGER_MOVE_UP:
		  #ifdef TRACE_FLOW
           Print("TRIGGER_MOVE_UP -> ABNORMAL_EVENT\r");
        #endif
        break; 
        case FSM_TRIGGER_MOVE_DOWN:
		  #ifdef TRACE_FLOW
           Print("TRIGGER_MOVE_DOWN -> ABNORMAL_EVENT\r");
		   #endif	 
        break;
        case FSM_PREPARE_TO_MOVE:
		  #ifdef TRACE_FLOW
          Print("PREPARE_TO_MOVE -> ABNORMAL_EVENT\r");
		   #endif	
        break; 
        case FSM_MOVING:
		  #ifdef TRACE_FLOW
          Print("MOVING -> ABNORMAL_EVENT\r");
		   #endif	
        break;	
        case FSM_TRIGGER_CAR_STOP:
		    #ifdef TRACE_FLOW 
           Print("TRIGGER_CAR_STOP -> ABNORMAL_EVENT\r");
		    #endif	
        break;
        case FSM_WAIT_TILL_CAR_STOPPED:
		    #ifdef TRACE_FLOW
		    Print("WAIT_TILL_CAR_STOPPED -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_PREPARE_DOORS_TO_ALIGN:
		    #ifdef TRACE_FLOW
         	Print("PREPARE_DOORS_TO_ALIGN -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_WAIT_TILL_DOORS_ALIGN:
		    #ifdef TRACE_FLOW
         	Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_TRIGGER_DOOR_OPEN:
		   #ifdef TRACE_FLOW
          	Print("TRIGGER_DOOR_OPEN -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_WAIT_TILL_DOOR_START_OPEN:
		    #ifdef TRACE_FLOW
        	Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT\r");
			 #endif	
        break;	
		case FSM_WAIT_TILL_DOOR_OPENED:
		    #ifdef TRACE_FLOW
		     Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT\r");
			 #endif	 
        break; 
        case FSM_PREPARE_USER_ENTRY_AND_EXIT:
		    #ifdef TRACE_FLOW
         	Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_USER_ENTRY_AND_EXIT:
		    #ifdef TRACE_FLOW 
         	Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_PREPARE_FOR_DOOR_CLOSE:
		   #ifdef TRACE_FLOW
         	Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT\r");
			 #endif	
        break;
       	case FSM_TRIGGER_DOOR_CLOSE:
		    #ifdef TRACE_FLOW
         	Print("TRIGGER_DOOR_CLOSE -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_WAIT_TILL_DOOR_START_CLOSE:
		    #ifdef TRACE_FLOW 
         	Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT\r");
			 #endif	
        break;
        case FSM_WAIT_TILL_DOOR_CLOSED:
		    #ifdef TRACE_FLOW
      		Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT\r");
			 #endif	
        break;
		case FSM_WAIT_TILL_DOORS_TO_UNALIGN:
		   #ifdef TRACE_FLOW
		   Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT\r");
		    #endif	
        break;
		case FSM_COMPUTE_NEXT_STOP_FLOOR:
           #ifdef TRACE_FLOW	
		   Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT\r");
		    #endif	
        break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("11.23.02", ERROR_OCCURED, appl_status_flag);
	       return appl_status_flag;
	}
	#ifdef TRACE_ERROR
       Print("ERR: event - internal proc abnormal \r");
	#endif
	cur_elevator_ptr->elevator_status = ERR_INTERNAL_PROC_ABNORMAL;
	cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
	return SUCCESS;	
}	 

/*------------------------------------------------------------*
FUNCTION NAME  : Check_Stationary_Limit_SW_Floor

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.24

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Check_Stationary_Limit_SW_Floor(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
    uint16_t ret_status;
	uint8_t proc_bit_field = 0, oper, limit_sw_min_floor, limit_sw_max_floor, cur_limit_sw_floor, *limit_sw_floor_arr_ptr, is_sw_pressed_limit_sw_floor;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("11.24.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(( Validate_Floor(cur_elevator_ptr->cur_floor)) != SUCCESS)
	{
		  appl_status_flag = ERR_FLOOR_INVALID;
	      Error_or_Warning_Proc("11.24.02", ERROR_OCCURED, appl_status_flag);
	      #ifdef TRACE_ERROR 	          
		      Print("ERR: event - current floor invalid \r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_CUR_FLOOR_INVALID;
		   cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;		   
	       return appl_status_flag;
	}
	if(cur_elevator_ptr->cur_floor >= FLOOR_00 && cur_elevator_ptr->cur_floor <= TOP_MOST_FLOOR_ID - 1)
	{
		ret_status = SW_Oper(limit_sw_down_arr[cur_elevator_ptr->cur_floor], DEV_READ_OPER);
	    switch(ret_status)
	    {
		   case SUCCESS:
		      proc_bit_field |= (1 << 0);
              if(cur_elevator_ptr->cur_floor == FLOOR_00)
			  {
				  proc_bit_field |= (1 << 1);	
			  }				  
	       break;
	       case SW_OR_KEY_NOT_PRESSED:
		   #ifdef TRACE_ERROR			
			  uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
	          Print("ERR: limit down floor sw : %u is inactive \r", uint32_temp_disp_data);
	       #endif
		    appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("11.24.03", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->elevator_status = ERR_LIMIT_FLOOR_DOWN_NOT_DETECT_AT_STATIONARY;
			cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;	
	        return appl_status_flag;
		 break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.24.04", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return appl_status_flag;
		 break;
         default:
           	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.24.05", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return appl_status_flag;
	   }
	}
	if(cur_elevator_ptr->cur_floor >= FLOOR_01 && cur_elevator_ptr->cur_floor <= TOP_MOST_FLOOR_ID)
	{
		ret_status = SW_Oper(limit_sw_up_arr[cur_elevator_ptr->cur_floor], DEV_READ_OPER);
	    switch(ret_status)
	    {
		   case SUCCESS:
		      proc_bit_field |= (1 << 1);
              if(cur_elevator_ptr->cur_floor == TOP_MOST_FLOOR_ID)
			  {
				  proc_bit_field |= (1 << 0);	
			  }				  
	       break;
	       case SW_OR_KEY_NOT_PRESSED:
		    #ifdef TRACE_ERROR	  
			  uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
	          Print("ERR: limit up floor sw : %u is inactive \r", uint32_temp_disp_data);
	       #endif
		    appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("11.24.06", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->elevator_status = ERR_LIMIT_FLOOR_UP_NOT_DETECT_AT_STATIONARY;
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		    return appl_status_flag; 
		 break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("11.24.07", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return appl_status_flag; 	 
		 break;
         default:
           	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("11.24.08", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return appl_status_flag; 
	   }
	}
	if((proc_bit_field & (1 << 0)) && (proc_bit_field & (1 << 1)))
	{
		for(oper = UP_SW_PROC; oper <= DOWN_SW_PROC; ++oper)
		{
			switch(oper)
			{
				case UP_SW_PROC:
				    limit_sw_min_floor = FLOOR_01;
					limit_sw_max_floor = TOP_MOST_FLOOR_ID;
					limit_sw_floor_arr_ptr = limit_sw_up_arr;
				break;
				case DOWN_SW_PROC:
				    limit_sw_min_floor = FLOOR_00;
					limit_sw_max_floor = TOP_MOST_FLOOR_ID - 1;
					limit_sw_floor_arr_ptr = limit_sw_down_arr;
				break;
			}
			for(cur_limit_sw_floor = limit_sw_min_floor; cur_limit_sw_floor <= limit_sw_max_floor; ++cur_limit_sw_floor )
			{
				if(cur_limit_sw_floor == cur_elevator_ptr->cur_floor)
				{
					continue;
				}
				if((SW_Present_State_By_IO_Ch(limit_sw_floor_arr_ptr[cur_limit_sw_floor], &is_sw_pressed_limit_sw_floor)) !=SUCCESS)
				{
					switch(is_sw_pressed_limit_sw_floor)
					{
						case STATE_YES:
						   appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
	                       Error_or_Warning_Proc("11.24.09", ERROR_OCCURED, appl_status_flag);
	                       cur_elevator_ptr->elevator_status = ERR_LIMIT_FLOOR_INVALID_AT_STATIONARY;
	                       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
						   return appl_status_flag;
						//break;
						case STATE_NO:
						break;
						default:
						  appl_status_flag = ERR_SW_READ_PROC;
	                      Error_or_Warning_Proc("11.24.10", ERROR_OCCURED, appl_status_flag);
	                      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                  return appl_status_flag; 
					}
				}
			}
		}
	}
	return SUCCESS;
}	
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
