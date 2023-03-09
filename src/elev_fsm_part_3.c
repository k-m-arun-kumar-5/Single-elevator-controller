/* ********************************************************************
FILE                   : elev_fsm_part_3.c

PURPOSE                : Elevator FSM Proc part 3
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
						
CAUTION               :  
                                    
CHANGE LOGS           :  

FILE ID               : 14

*****************************************************************************/
#include "main.h"
#include "appl.h"
#include "elevator_fsm.h"

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Prepare_Doors_To_Align_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.11

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Prepare_Doors_To_Align_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
    uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.11.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;	
	ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("PREPARE_DOORS_TO_ALIGN -> ABNORMAL_EVENT \r");
		   #endif	  
		   #ifdef TRACE_INFO  
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.11.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.11.03", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW
		    Print("PREPARE_DOORS_TO_ALIGN -> ABNORMAL_EVENT \r");
		#endif
        #ifdef TRACE_ERROR 		
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}
	if((SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
	{
	     appl_status_flag = ERR_DEV_ENABLE_PROC;
	     Error_or_Warning_Proc("14.11.04", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	     return SUCCESS;
	}
    if((Timer_Run(CH_ID_01, TIMER_ID_DOORS_ALIGNED)) != SUCCESS)
	{
	    appl_status_flag = ERR_TIMER_RUN_PROC;
	    Error_or_Warning_Proc("14.11.05", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	    return SUCCESS;
	}
	    #ifdef TRACE_FLOW 
	         Print("PREPARE_DOORS_TO_ALIGN -> WAIT_TILL_DOORS_ALIGN\r");
		#endif
        #ifdef TRACE_REQ			
	        Print("REQ: Doors align to active \r");
	    #endif
	    cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOORS_ALIGN;
	  return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Wait_Till_Doors_Aligned_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.12

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Doors_Aligned_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0, is_pressed_state_sw;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.12.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;	
	ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
		   #endif	  
		   #ifdef TRACE_INFO  
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.12.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}	
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.12.03", ERROR_OCCURED, appl_status_flag);
	        #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
			#endif
            #ifdef TRACE_ERROR			
		      Print("ERR: event - poll hall and in car call process \r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS; 
	}
    ret_status = Timer_Interrupt_Retrieve_Data_Arr(CH_ID_01, &retrieved_timer_or_counter_interrupt_data);
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case TMR_NO_MAX_NUM_TIMEOUT_PROC:
		case TMR_BEFORE_LAST_TIMEOUT_PROC:
		case TMR_AT_LAST_TIMEOUT_PROC:
		break;
		case TMR_MAX_NUM_TIMEOUT_PROC:
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOORS_ALIGNED)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.12.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
			}			
		    #ifdef TRACE_FLOW
		      Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
			#endif
            #ifdef TRACE_ERROR			
			  Print("ERR: Event - req doors align but unalign\r");
		    #endif
			appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("14.12.05", ERROR_OCCURED, appl_status_flag);
			cur_elevator_ptr->elevator_status = ERR_REQ_DOORS_ALIGN_BUT_UNALIGN;
			cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
            return SUCCESS;			
		//break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.12.06", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.12.06", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		   
		    Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
	    #endif
        #ifdef TRACE_ERROR		
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}
	if((SW_Present_State_By_IO_Ch(LIMIT_SW_DOOR_CLOSE_IO_CH, &is_pressed_state_sw)) != SUCCESS)
	{
		 appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	     Error_or_Warning_Proc("14.12.07", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		 return SUCCESS;
	}
	switch(is_pressed_state_sw)
	{
	    case STATE_YES:
		     proc_bit_field |= (1 << 0);  			
   	  break;
	    case STATE_NO:	   
		   cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   cur_elevator_ptr->elevator_status = ERR_DOOR_NOT_CLOSED_BUT_NOT_ALIGNED; 
		   appl_status_flag = ERR_SW_IS_INACTIVE;
		   Error_or_Warning_Proc("14.12.08", ERROR_OCCURED, appl_status_flag);
		   #ifdef TRACE_FLOW		   
		      Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
	       #endif
           #ifdef TRACE_ERROR		
		     Print("ERR: Event - Close limit sw is inactive for doors to align\r");
		   #endif
		  return SUCCESS;
	  //break;
	   default:
        	appl_status_flag = ERR_FORMAT_INVALID;
	        Error_or_Warning_Proc("14.12.09", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	
	}	
	if((SW_Present_State_By_IO_Ch(LIMIT_SW_DOOR_OPEN_IO_CH, &is_pressed_state_sw)) != SUCCESS)
	{
		 appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	     Error_or_Warning_Proc("14.12.10", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		 return SUCCESS;
	}
	switch(is_pressed_state_sw)
	{
	    case STATE_YES:
		   cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   cur_elevator_ptr->elevator_status = ERR_DOOR_OPENED_BUT_NOT_ALIGNED;
		   appl_status_flag = ERR_SW_IS_ACTIVE;
		   Error_or_Warning_Proc("14.12.11", ERROR_OCCURED, appl_status_flag);
		   #ifdef TRACE_FLOW		   
		      Print("WAIT_TILL_DOORS_ALIGN -> ABNORMAL_EVENT \r");
	       #endif
           #ifdef TRACE_ERROR		
		     Print("ERR: Event - Open limit sw is active for doors to align\r");
		   #endif
		  return SUCCESS;   			
   	//  break;
	  case STATE_NO:
	     proc_bit_field |= (1 << 1);
	  break;
	  default:
       	appl_status_flag = ERR_FORMAT_INVALID;
	    Error_or_Warning_Proc("14.12.12", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;	
	}	
    if(((proc_bit_field) & (1 << 0)) && ((proc_bit_field) & (1 << 1)))
    {	  
	   ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	   switch(ret_status)
	   {
	      case SUCCESS:
		    if((Timer_Stop(CH_ID_01)) != SUCCESS)
		    {
		 	  appl_status_flag = ERR_TIMER_STOP_PROC;
	          Error_or_Warning_Proc("14.12.13", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
		   }
		   if((SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		   {
			  appl_status_flag = ERR_DEV_ENABLE_PROC;
	          Error_or_Warning_Proc("14.12.14", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
		   }
		   if((SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		   {
			  appl_status_flag = ERR_DEV_ENABLE_PROC;
	          Error_or_Warning_Proc("14.12.15", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
		   }
		    #ifdef TRACE_FLOW 
		 	  Print("WAIT_TILL_DOORS_ALIGN -> TRIGGER_DOOR_OPEN \r");
		    #endif 
            cur_elevator_ptr->cur_fsm_state = FSM_TRIGGER_DOOR_OPEN;    			
   	      break;
	      case SW_OR_KEY_NOT_PRESSED:
	      break;
	      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	      case WARN_CUR_DATA_ID_DEV_DISABLED:
	        appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.12.16", ERROR_OCCURED, appl_status_flag);
		    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	 
	     //  break;
         default:
         	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.12.17", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	
	   }
	}	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Trigger_Door_Open_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.13 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Trigger_Door_Open_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	  
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.13.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
    ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("TRIGGER_DOOR_OPEN -> ABNORMAL_EVENT \r");
		   #endif
		    #ifdef TRACE_INFO
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.13.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS; 
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.13.05", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		  
		    Print("TRIGGER_DOOR_OPEN -> ABNORMAL_EVENT \r");
	    #endif		
		#ifdef TRACE_ERROR		
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}  
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.13.06", ERROR_OCCURED, appl_status_flag);
	        #ifdef TRACE_FLOW
	          Print("TRIGGER_DOOR_OPEN -> ABNORMAL_EVENT \r");
			#endif		
	     	#ifdef TRACE_ERROR	  
		      Print("ERR: event - poll hall and in car call process\r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS; 
	  }
      if((ret_status = SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
	  {
	     appl_status_flag = ERR_DEV_DISABLE_PROC;
	     Error_or_Warning_Proc("14.13.07", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	 }
	if((ret_status = SW_Oper(MANUAL_DOOR_CLOSE_SW_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
	{
	     appl_status_flag = ERR_DEV_ENABLE_PROC;
	     Error_or_Warning_Proc("14.13.08", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		 return SUCCESS;
	}
    ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		    if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
	        {
		        appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.13.09", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        }
	        if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_HIGH)) != SUCCESS)
	        {
	        	appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.13.10", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        }
			if((Timer_Run(CH_ID_01, TIMER_ID_DETECT_DOOR_OPEN)) != SUCCESS)
	        {
		       appl_status_flag = ERR_TIMER_RUN_PROC;
	           Error_or_Warning_Proc("14.13.11", ERROR_OCCURED, appl_status_flag);
	           Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	        }
		    #ifdef TRACE_FLOW
	          Print("TRIGGER_DOOR_OPEN -> WAIT_TILL_DOOR_START_OPEN \r");
			#endif
			#ifdef TRACE_INFO
			   Print("INFO: Car Door Opening ... \r");
			#endif
            #ifdef TRACE_REQ			
	          Print("REQ: Door close limit SW to inactive \r");
	        #endif
	        cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_START_OPEN;	
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    #ifdef TRACE_ERROR
	          Print("TRIGGER_DOOR_OPEN -> ABNORMAL_EVENT \r");
	          Print("ERR: Event - Doors not aligned properly \r");
	        #endif
			appl_status_flag = ERR_SW_IS_INACTIVE;
	         Error_or_Warning_Proc("14.13.12", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			cur_elevator_ptr->elevator_status = ERR_DOORS_NOT_ALIGNED_PROPERLY;
			return SUCCESS;
		 //break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		     appl_status_flag = ERR_TIMER_RUN_PROC;
	         Error_or_Warning_Proc("14.13.13", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.13.14", ERROR_OCCURED, appl_status_flag);
	       	Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
    return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Wait_Till_Door_Start_Open_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.14 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Door_Start_Open_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.14.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
    ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		    #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
			#endif  
			#ifdef TRACE_INFO  
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.14.02", ERROR_OCCURED, appl_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;	      
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.14.02", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_ERROR		  
		    Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}
     proc_bit_field |= (1 << 0);	
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.14.03", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_ERROR 
	          Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
		      Print("ERR: event - poll hall and in car call \r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;
	}
  
	ret_status = Timer_Interrupt_Retrieve_Data_Arr(CH_ID_01, &retrieved_timer_or_counter_interrupt_data);
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case TMR_NO_MAX_NUM_TIMEOUT_PROC:
		case TMR_BEFORE_LAST_TIMEOUT_PROC:
		case TMR_AT_LAST_TIMEOUT_PROC:
		break;
		case TMR_MAX_NUM_TIMEOUT_PROC:
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DETECT_DOOR_OPEN)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.14.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			#ifdef TRACE_FLOW
		      Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
			#endif  
			#ifdef TRACE_ERROR  
			  Print("ERR: Event - req door open but closed \r");
		    #endif
			cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_OPEN_BUT_CLOSED;
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
            return SUCCESS;			
		//break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.14.05", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:	
             proc_bit_field |= (1 << 1);				
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    #ifdef TRACE_ERROR
	          Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
	          Print("ERR: Event - Doors not aligned properly \r");
	        #endif
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			cur_elevator_ptr->elevator_status = ERR_DOORS_NOT_ALIGNED_PROPERLY;
			return SUCCESS;
		 //break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.14.06", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.14.07", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;  
	}
	 ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	 switch(ret_status)
	 {	
		 case SUCCESS:	
            proc_bit_field |= (1 << 4);			 
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
            proc_bit_field |= (1 << 2);		 
		 break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.14.08", ERROR_OCCURED, appl_status_flag); 
			 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
		// break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.14.09", ERROR_OCCURED, appl_status_flag);
	       	Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS; 
	  }
	 ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	 switch(ret_status)
	 {	
		 case SUCCESS:
		      appl_status_flag = ERR_SW_IS_ACTIVE;
	          Error_or_Warning_Proc("14.14.10", ERROR_OCCURED, appl_status_flag);
		      if(proc_bit_field & (1 << 4))
			  {
                 #ifdef TRACE_FLOW
	                Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
				 #endif	
				 #ifdef TRACE_ERROR	
	                Print("ERR: Event - door open and close detect for door open\r");
	             #endif
				  
	             cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			     cur_elevator_ptr->elevator_status = ERR_DOOR_OPEN_AND_CLOSE_ACTIVE_FOR_OPEN;
			     return SUCCESS;
			  }
			  if(((proc_bit_field & (1 << 2))))
			  {
			      #ifdef TRACE_FLOW
	                Print("WAIT_TILL_DOOR_START_OPEN -> ABNORMAL_EVENT \r");
				  #endif	
				  #ifdef TRACE_ERROR		
	                Print("ERR: Event - fast door opened\r");
	              #endif				   
	              cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			      cur_elevator_ptr->elevator_status = ERR_DOOR_OPEN_FAST;
			      return SUCCESS;
			  }           			   
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:
            proc_bit_field |= (1 << 3);		 
		 break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		     appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.14.11", ERROR_OCCURED, appl_status_flag);
			 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.14.12", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;	 
	  }
      if((proc_bit_field & (1 << 0)) && (proc_bit_field & (1 << 1)) && (proc_bit_field & (1 << 2)) && (proc_bit_field & (1 << 3)))
	  {	
            if((Timer_Stop(CH_ID_01)) != SUCCESS)
			{
				appl_status_flag = ERR_TIMER_STOP_PROC;
	            Error_or_Warning_Proc("14.14.13", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_OPENED_MOTOR)) != SUCCESS)
	        {
		       appl_status_flag = ERR_TIMER_RUN_PROC;
	           Error_or_Warning_Proc("14.14.14", ERROR_OCCURED, appl_status_flag);
	           Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	        }
		    #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOOR_START_OPEN -> WAIT_TILL_DOOR_OPENED \r");
			#endif
            #ifdef TRACE_REQ			
	          Print("REQ: Door open limit SW to active \r");
	        #endif
	        cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_OPENED;	
	  }	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Wait_Till_Door_Opened_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.15 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Door_Opened_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.15.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
			#endif  
			#ifdef TRACE_INFO  
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.15.02", ERROR_OCCURED, appl_status_flag);
	     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;  
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.15.03", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
	    #endif		
		#ifdef TRACE_ERROR		
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}
	proc_bit_field = (1 << 0);
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.15.04", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW		  
		      Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
	       #endif		
		   #ifdef TRACE_ERROR	
		      Print("ERR: event - poll hall and in car call \r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;
	}
   	ret_status = Timer_Interrupt_Retrieve_Data_Arr(CH_ID_01, &retrieved_timer_or_counter_interrupt_data);
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case TMR_NO_MAX_NUM_TIMEOUT_PROC:
		case TMR_BEFORE_LAST_TIMEOUT_PROC:
		case TMR_AT_LAST_TIMEOUT_PROC:
		break;
		case TMR_MAX_NUM_TIMEOUT_PROC:
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOOR_OPENED_MOTOR)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.15.05", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			#ifdef TRACE_FLOW		  
		       Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
	        #endif		
		    #ifdef TRACE_ERROR	
			  Print("ERR: Event - limit sw door open not detect \r");
		    #endif
			cur_elevator_ptr->elevator_status = ERR_DOOR_OPEN_NOT_DETECT;
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
            return SUCCESS;			
		//break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.15.06", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		    proc_bit_field |= (1 << 1);
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    #ifdef TRACE_FLOW		  
		       Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
	        #endif		
		    #ifdef TRACE_ERROR	
	          Print("ERR: Event - Doors not aligned properly \r");
	        #endif
			 appl_status_flag = ERR_SW_IS_INACTIVE;
	         Error_or_Warning_Proc("14.15.07", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			cur_elevator_ptr->elevator_status = ERR_DOORS_NOT_ALIGNED_PROPERLY;
			return SUCCESS;
		// break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		     appl_status_flag = ERR_SW_IS_DISABLED;
	         Error_or_Warning_Proc("14.15.08", ERROR_OCCURED, appl_status_flag);
			 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
		 //break;
		  default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.15.09", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;	 
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	   case SUCCESS:
		  #ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOOR_OPENED -> ABNORMAL_EVENT \r");
	      #endif		
		   #ifdef TRACE_ERROR	
	          Print("ERR: Event - req door open but closed \r");
	      #endif
		   appl_status_flag = ERR_SW_IS_ACTIVE;
	       Error_or_Warning_Proc("14.15.10", ERROR_OCCURED, appl_status_flag);
	      cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		  cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_OPEN_BUT_CLOSED;
		  return SUCCESS;
	  // break;
	   case SW_OR_KEY_NOT_PRESSED:
	        proc_bit_field |= (1 << 2); 
	   break;
	   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	   case WARN_CUR_DATA_ID_DEV_DISABLED:
	       appl_status_flag = ERR_SW_IS_DISABLED;
	       Error_or_Warning_Proc("14.15.11", ERROR_OCCURED, appl_status_flag);
		   Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	   //break;
       default:
        	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.15.12", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS; 
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	   case SUCCESS:
	     if(proc_bit_field == 7)
		 {
		     if((Timer_Stop(CH_ID_01)) != SUCCESS)
		     {
			    appl_status_flag = ERR_TIMER_STOP_PROC;
	            Error_or_Warning_Proc("14.15.13", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
		     }
		     if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
		     {
		    	 appl_status_flag = ERR_TIMER_STOP_PROC;
	             Error_or_Warning_Proc("14.15.14", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
		     }
			 if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
		     {
		    	 appl_status_flag = ERR_TIMER_STOP_PROC;
	             Error_or_Warning_Proc("14.15.15", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
		     }
			 if((SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
	         {
	             appl_status_flag = ERR_DEV_DISABLE_PROC;
	             Error_or_Warning_Proc("14.15.16", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
	         }
			 #ifdef TRACE_INFO
			     Print("INFO: Car door opened \r");
			 #endif
			 #ifdef TRACE_FLOW
			     Print("WAIT_TILL_DOOR_OPENED -> PREPARE_USER_ENTRY_AND_EXIT \r");
			 #endif 
			 cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_USER_ENTRY_AND_EXIT;
		 }		 
	   break;
	   case SW_OR_KEY_NOT_PRESSED:	               
	   break;
	   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	   case WARN_CUR_DATA_ID_DEV_DISABLED:
	       appl_status_flag = ERR_SW_IS_DISABLED;
	       Error_or_Warning_Proc("14.15.18", ERROR_OCCURED, appl_status_flag);
		   Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
//	   break;
       default:
        	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.15.19", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	}	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Prepare_User_Entry_And_Exit_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.16

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Prepare_User_Entry_And_Exit_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	uint8_t cur_floor, min_floor_call, max_floor_call, oper, cur_floor_state, elevator_status, *call_floor_type_arr_ptr ;
		
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.16.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		   #endif	  
		   #ifdef TRACE_INFO  
	           Print("WARN: Event - Emergency stop triggered\r");
	       #endif
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.16.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((Check_Stationary_Door_Opened_State(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		 appl_status_flag = ERR_CHECK_STATIONARY;
	     Error_or_Warning_Proc("14.16.03", ERROR_OCCURED, appl_status_flag);
	     cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
		 #ifdef TRACE_FLOW
	         Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		 #endif	 
		 #ifdef TRACE_ERROR
		     Print("ERR: Event - stationary door opened state invalid \r");
	     #endif
	     return SUCCESS;
	}
	switch(cur_elevator_ptr->before_fsm_state)
	{
		case FSM_STARTUP:
		    if((SW_Oper(OVERLOAD_SENSOR_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		    {
		    	 appl_status_flag = ERR_DEV_ENABLE_PROC;
	             Error_or_Warning_Proc("14.16.04", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		    }
		    for(oper = UP_SW_PROC; oper <= IN_CAR_SW_PROC ; ++oper)
			{
				switch(oper)
				{
					case UP_SW_PROC:
					   min_floor_call = FLOOR_00;
					   max_floor_call = TOP_MOST_FLOOR_ID - 1;
					   call_floor_type_arr_ptr = hall_call_up_arr;
				    break;
                    case DOWN_SW_PROC:
					   min_floor_call = FLOOR_01;
					   max_floor_call = TOP_MOST_FLOOR_ID;
					   call_floor_type_arr_ptr = hall_call_down_arr;
                    break;
                    case IN_CAR_SW_PROC:
                       min_floor_call = FLOOR_00;
					   max_floor_call = TOP_MOST_FLOOR_ID;
					   call_floor_type_arr_ptr = in_car_call_arr;
                    break;					
				}
				for(cur_floor = min_floor_call; cur_floor <= max_floor_call; ++cur_floor)
			    {
					if(cur_floor == cur_elevator_ptr->cur_floor)
					{
						continue;
					}
			     	if((SW_Oper(call_floor_type_arr_ptr[cur_floor], DEV_ENABLE_OPER)) != SUCCESS)
	                {
	                    appl_status_flag = ERR_DEV_ENABLE_PROC;
	                    Error_or_Warning_Proc("14.16.05", ERROR_OCCURED, appl_status_flag);
	                    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                return SUCCESS;
	                }
				}
			}
			break;   
			case FSM_PREPARE_USER_ENTRY_AND_EXIT:	
                if(cur_elevator_ptr->next_stop_floor != cur_elevator_ptr->cur_floor)
			    {
			          #ifdef TRACE_FLOW
	                     Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		              #endif
					  #ifdef TRACE_ERROR
				        Print("ERR: event - next stop floor != cur_floor\r");
				      #endif
			         cur_elevator_ptr->elevator_status = ERR_NEXT_STOP_FLOOR_NOT_STOPPED;
			         cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
                     return SUCCESS;	
			    }
                if(((cur_elevator_ptr->pending_floor_call_bit_field & (1 << cur_elevator_ptr->cur_floor)) == 0))
				{
			        #ifdef TRACE_FLOW
	                    Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		            #endif
                    #ifdef TRACE_ERROR 					
				       Print("ERR: event - next stop floor but not stop list\r");
				    #endif
				    appl_status_flag = ERR_STOP_FLOOR_NOT_LIST;
	                Error_or_Warning_Proc("14.16.06", ERROR_OCCURED, appl_status_flag);
			        cur_elevator_ptr->elevator_status = ERR_NEXT_STOP_FLOOR_REACH_BUT_NOT_IN_STOP_LIST;
			        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
                    return SUCCESS;	
				}
			    if((cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_elevator_ptr->cur_floor)))
	            {
				    cur_elevator_ptr->in_car_floor_call_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
			        --cur_elevator_ptr->num_pending_calls_in_car;
			        if(cur_elevator_ptr->num_pending_calls_in_car == 0 && cur_elevator_ptr->in_car_floor_call_bit_field == 0)
			        {
				       cur_elevator_ptr->cur_min_floor_call_in_car = FLOOR_ID_INVALID;
	                   cur_elevator_ptr->cur_max_floor_call_in_car = FLOOR_ID_INVALID;
			        }
	            }
	            switch(cur_elevator_ptr->elevator_status)
	            {
		           case MOVED_UP_STATIONARY:
		             if((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_elevator_ptr->cur_floor)))
	                 {
						  cur_elevator_ptr->hall_floor_call_up_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
				         --cur_elevator_ptr->num_pending_calls_up;
				         if(cur_elevator_ptr->num_pending_calls_up == 0 && cur_elevator_ptr->hall_floor_call_up_bit_field == 0)
			             {
				            cur_elevator_ptr->cur_min_floor_call_up = FLOOR_ID_INVALID;
	                        cur_elevator_ptr->cur_max_floor_call_up = FLOOR_ID_INVALID;
			             }
	                 }
					 if(cur_elevator_ptr->num_pending_calls_down != 0 && cur_elevator_ptr->cur_floor == cur_elevator_ptr->cur_max_floor_call_down && ((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_elevator_ptr->cur_floor))))
	                 {
						cur_elevator_ptr->hall_floor_call_down_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
			        	--cur_elevator_ptr->num_pending_calls_down;
			        	if(cur_elevator_ptr->num_pending_calls_down == 0 && cur_elevator_ptr->hall_floor_call_down_bit_field == 0)
			            {
				             cur_elevator_ptr->cur_min_floor_call_down = FLOOR_ID_INVALID;
	                         cur_elevator_ptr->cur_max_floor_call_down = FLOOR_ID_INVALID;
			            }
			         }
		           break;
		           case MOVED_DOWN_STATIONARY:
		              if((cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_elevator_ptr->cur_floor)))
	                  {
		                   cur_elevator_ptr->hall_floor_call_down_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
			        	   --cur_elevator_ptr->num_pending_calls_down;
			        	   if(cur_elevator_ptr->num_pending_calls_down == 0 && cur_elevator_ptr->hall_floor_call_down_bit_field == 0)
			               {
				               cur_elevator_ptr->cur_min_floor_call_down = FLOOR_ID_INVALID;
	                           cur_elevator_ptr->cur_max_floor_call_down = FLOOR_ID_INVALID;
			               }
			          }
					 if(cur_elevator_ptr->num_pending_calls_up != 0 && cur_elevator_ptr->cur_floor == cur_elevator_ptr->cur_min_floor_call_up && ((cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_elevator_ptr->cur_floor))))
	                 {
						cur_elevator_ptr->hall_floor_call_up_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
			        	--cur_elevator_ptr->num_pending_calls_up;
			        	if(cur_elevator_ptr->num_pending_calls_up == 0 && cur_elevator_ptr->hall_floor_call_up_bit_field == 0)
			            {
				             cur_elevator_ptr->cur_min_floor_call_up = FLOOR_ID_INVALID;
	                         cur_elevator_ptr->cur_max_floor_call_up = FLOOR_ID_INVALID;
			            }
			         }
		            break;
	            }	 
	            cur_floor_state = (cur_elevator_ptr->in_car_floor_call_bit_field & (1 << cur_elevator_ptr->cur_floor)) || (cur_elevator_ptr->hall_floor_call_up_bit_field & (1 << cur_elevator_ptr->cur_floor))|| (cur_elevator_ptr->hall_floor_call_down_bit_field & (1 << cur_elevator_ptr->cur_floor)) ;
	            if(cur_floor_state == 0)
	            {
	               cur_elevator_ptr->pending_floor_call_bit_field &= ~(1 << cur_elevator_ptr->cur_floor);
		           --cur_elevator_ptr->num_pending_calls;
		          if(cur_elevator_ptr->num_pending_calls == 0 && cur_elevator_ptr->pending_floor_call_bit_field == 0)
		          {
				      cur_elevator_ptr->cur_min_floor_call = FLOOR_ID_INVALID;
	                  cur_elevator_ptr->cur_max_floor_call = FLOOR_ID_INVALID;
		          }
	           }
	           #ifdef TRACE_INFO	    
		          Print("INFO: serviced floor call: %u\r", cur_elevator_ptr->cur_floor);				           
	           #endif 
			break;					
	}
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.16.07", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW
	           Print("PREPARE_USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		   #endif
           #ifdef TRACE_ERROR
		      Print("ERR: event - poll hall and in car call process\r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;
    }
	if((SW_Oper(USER_BLOCKS_DOOR_SENSOR_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
	{
			 appl_status_flag = ERR_DEV_ENABLE_PROC;
	         Error_or_Warning_Proc("14.16.08", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
	}
	if(cur_elevator_ptr->cur_floor != TOP_MOST_FLOOR_ID)
	{			
	   if((SW_Oper(hall_call_up_arr[cur_elevator_ptr->cur_floor], DEV_DISABLE_OPER)) != SUCCESS)
	   {
	       appl_status_flag = ERR_DEV_DISABLE_PROC;
	       Error_or_Warning_Proc("14.16.09", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	       return SUCCESS;
	   }
	}
	if(cur_elevator_ptr->cur_floor != FLOOR_00)
	{
	   if((SW_Oper(hall_call_down_arr[cur_elevator_ptr->cur_floor], DEV_DISABLE_OPER)) != SUCCESS)
	   {
	       appl_status_flag = ERR_DEV_DISABLE_PROC;
	       Error_or_Warning_Proc("14.16.10", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	       return SUCCESS;
	   }
	}
	if((SW_Oper(in_car_call_arr[cur_elevator_ptr->cur_floor], DEV_DISABLE_OPER)) != SUCCESS)
	{
	   	appl_status_flag = ERR_DEV_DISABLE_PROC;
	    Error_or_Warning_Proc("14.16.11", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	    return SUCCESS;
	}	    
	cur_elevator_ptr->before_fsm_state = FSM_PREPARE_USER_ENTRY_AND_EXIT;
    if((ret_status = Compute_Floor_Stop_Datas(ctrl_elevator_ch_id, DOOR_OPENED_STAGE, &elevator_status)) != SUCCESS)
	{
	    appl_status_flag = ERR_NEXT_STOP_FLOOR_PROC;
        Error_or_Warning_Proc("14.16.12", ERROR_OCCURED, appl_status_flag);   
		cur_elevator_ptr->elevator_status = ERR_COMPUTE_NEXT_STOP_FLOOR;
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		return SUCCESS;
	}	
	#ifdef TRACE_FLOW
		Print("PREPARE_USER_ENTRY_AND_EXIT -> USER_ENTRY_AND_EXIT \r");
	#endif
    cur_elevator_ptr->before_fsm_state = FSM_PREPARE_USER_ENTRY_AND_EXIT;
	cur_elevator_ptr->cur_fsm_state = FSM_USER_ENTRY_AND_EXIT;
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_User_Entry_And_Exit_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.17

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_User_Entry_And_Exit_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	static uint8_t loop_flag = 0;
	uint8_t proc_bit_field = 0, elevator_status = 0;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		loop_flag = 0;
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.17.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	ret_status = Ext_Interrupt_Retrieve_Data_Arr(EMER_CAR_STOP_EXT_INTP_CH_ID);	
	switch(ret_status)
	{
		case SEARCH_INTP_SRC_DATA_ARR_NOT_FOUND:
		break;
		case SUCCESS:
		   #ifdef TRACE_FLOW
	          Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		   #endif
           #ifdef TRACE_INFO		   
	          Print("WARN: Event - Emergency stop triggered\r"); 
	       #endif
		    loop_flag = 0;
	       cur_elevator_ptr->elevator_status = TRIGGERED_EMERGENCY_STOP;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;  
		//break;
		default:
		  loop_flag = 0;
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.17.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Stationary_Door_Opened_State(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		loop_flag = 0;
		 appl_status_flag = ERR_CHECK_STATIONARY;
	     Error_or_Warning_Proc("14.17.03", ERROR_OCCURED, appl_status_flag);
		 #ifdef TRACE_FLOW
	          Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		 #endif
		 #ifdef TRACE_ERROR
		     Print("ERR: Event - stationary door opened state invalid \r");
	     #endif
	     cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
	     return SUCCESS;
	}
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   loop_flag = 0;
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.17.04", ERROR_OCCURED, appl_status_flag);
	        #ifdef TRACE_FLOW
	          Print("USER_ENTRY_AND_EXIT -> ABNORMAL_EVENT \r");
		    #endif
		    #ifdef TRACE_ERROR
		      Print("ERR: event - poll hall and in car call process\r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_POLL_HALL_AND_IN_CAR_PROC;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;
	 }
	ret_status = SW_Oper(OVERLOAD_SENSOR_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	        case SUCCESS:
	            if((loop_flag & (1 << 1)) == 0)
				{
                   #ifdef TRACE_INFO
				      Print("WARN: event - car overloaded \r");
				   #endif
                   #ifdef TRACE_REQ				   
					  Print("REQ: Car to be not overload \r");
                   #endif
				   loop_flag |= (1 << 1);
				   loop_flag &= ~(1 << 2);
				   return SUCCESS;
				}
				else
				{
					return SUCCESS;
				}
	        //break;
	        case SW_OR_KEY_NOT_PRESSED:	
                 proc_bit_field |= (1 << 2);                
				 if((loop_flag & (1 << 2)) == 0)
				 {
					 if((loop_flag & (1 << 1)))
					 {
                        #ifdef TRACE_INFO
				          Print("INFO: Car is not overloaded \r");
                        #endif
                     }					
                   	 loop_flag |= (1 << 2);
				 }
				 loop_flag &= ~(1 << 1);
	        break;
	        case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	        case WARN_CUR_DATA_ID_DEV_DISABLED:
			    loop_flag = 0;
			    appl_status_flag = ERR_SW_IS_DISABLED;
	            Error_or_Warning_Proc("14.17.05", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        //break;
            default:
			   loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.17.06", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	}
	if(proc_bit_field & (1 << 2))
	{
			if((Compute_Floor_Stop_Datas(ctrl_elevator_ch_id, DOOR_OPENED_STAGE, &elevator_status)) != SUCCESS)
	        {
				   loop_flag = 0;
	                appl_status_flag = ERR_NEXT_STOP_FLOOR_PROC;
    	            Error_or_Warning_Proc("14.17.07", ERROR_OCCURED, appl_status_flag);   
		            cur_elevator_ptr->elevator_status = ERR_COMPUTE_NEXT_STOP_FLOOR;
		            cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		            return SUCCESS;
	        }	
            switch(elevator_status)
	        {
                 case NO_PENDING_FLOOR_CALLS:
				    if((loop_flag & (1 << 3)) == 0)
	            	{
		               Reset_Elevator_Datas(ctrl_elevator_ch_id, RESET_STOP_DATAS);
		               #ifdef TRACE_INFO
		                  Print("INFO: No Pending Floor Calls \r");
		               #endif
		               loop_flag |= (1 << 3);
		           }
	             case STARTUP_STATIONARY:
                      return SUCCESS;      		  
	             // break;
				 default: 
				     loop_flag = 0;
                   	 #ifdef TRACE_DATA 
				        Print("TRA: After Compute Floor Data - Opened stage \r");
					    Print("=============================================\r");
					    uint32_temp_disp_data =  cur_elevator_ptr->cur_floor;      
	                    Print("cur_floor                     : %u\r", uint32_temp_disp_data);
					    uint32_temp_disp_data =  cur_elevator_ptr->next_stop_floor; 
	                    Print("next_stop_floor               : %u\r", uint32_temp_disp_data);
					    uint32_temp_disp_data = cur_elevator_ptr->pending_floor_call_bit_field;
	                    Print("pending floor calls - bit     : 0x%x\r", uint32_temp_disp_data);
					    Print("=============================================\r");
                     #endif
	        }
			if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_OPENED)) != SUCCESS)
			{
				loop_flag = 0; 
				appl_status_flag = ERR_TIMER_RUN_PROC;
	            Error_or_Warning_Proc("14.17.08", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			if((SW_Oper(MANUAL_DOOR_CLOSE_SW_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		    {
				loop_flag = 0;
			     appl_status_flag = ERR_DEV_ENABLE_PROC;
	             Error_or_Warning_Proc("14.17.09", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		    }
    	    #ifdef TRACE_FLOW
	           Print("USER_ENTRY_AND_EXIT -> PREPARE_FOR_DOOR_CLOSE \r");
			#endif
            #ifdef TRACE_REQ			
               Print("REQ: Wait till Timeout for close or manual door close to active \r");			   
	        #endif
			cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_FOR_DOOR_CLOSE;
	}
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
