/* ********************************************************************
FILE                   : elev_fsm_part_5.c

PURPOSE                : Elevator FSM Proc part 5
	 
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
FUNCTION NAME  : FSM_Wait_Till_Doors_Unaligned_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.22

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Doors_Unaligned_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.22.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.22.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((ret_status = Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.22.03", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
		#endif	
		#ifdef TRACE_ERROR	
			Print("ERR: Event - limit sw floor stationary invalid \r");
		#endif
		return SUCCESS; 
	}
	proc_bit_field |= (1 << 0);
	ret_status = SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
		 case SUCCESS:
		   return SUCCESS;
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:
		    proc_bit_field |= (1 << 1); 
		 break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.22.07", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		 //break;
         default:
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.22.08", ERROR_OCCURED, appl_status_flag);
	       	Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	   case SUCCESS:
		  #ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
		  #endif	
		  #ifdef TRACE_ERROR				  
	          Print("ERR: Event - req door close but opened \r");
	      #endif
		  appl_status_flag = ERR_SW_IS_ACTIVE;
	      Error_or_Warning_Proc("14.22.09", ERROR_OCCURED, appl_status_flag);			
	      cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		  cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_CLOSE_BUT_OPENED;
          return SUCCESS;
	   // break;
	   case SW_OR_KEY_NOT_PRESSED:
	        proc_bit_field |= (1 << 2); 
	   break;
	   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	   case WARN_CUR_DATA_ID_DEV_DISABLED:
	        appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.22.10", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	  // break;
       default:
        	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.22.11", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	   case SUCCESS:
	     proc_bit_field |= (1 << 3); 
	   break;
	   case SW_OR_KEY_NOT_PRESSED:  	   
          #ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
		  #endif	
		  #ifdef TRACE_ERROR	
	          Print("ERR: Event - req door closed but not closed \r");
	      #endif
		  appl_status_flag = ERR_SW_IS_INACTIVE;
	      Error_or_Warning_Proc("14.22.12", ERROR_OCCURED, appl_status_flag);	 
	      cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		  cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_CLOSED_BUT_NOT_CLOSED;
          return SUCCESS;
	   //break;
	   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	   case WARN_CUR_DATA_ID_DEV_DISABLED:
	        appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.22.13", ERROR_OCCURED, appl_status_flag);
			Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	   //break;
       default:
        	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.22.14", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	}
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.22.15", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
		   #endif	
		   #ifdef TRACE_ERROR	
		       Print("ERR: event - poll hall and in car call process\r");
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
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOORS_UNALIGNED)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.22.16", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}			
		    #ifdef TRACE_FLOW		  
		      Print("WAIT_TILL_DOORS_TO_UNALIGN -> ABNORMAL_EVENT \r");
		    #endif	
		    #ifdef TRACE_ERROR	
			  Print("ERR: Event - req doors unalign but aligned \r");
		    #endif
			cur_elevator_ptr->elevator_status = ERR_REQ_DOORS_UNALIGN_BUT_ALIGN;
		    cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
            return SUCCESS;			
		//break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.22.17", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	}
	
	if((proc_bit_field & (1 << 0)) && (proc_bit_field & (1 << 1)) && (proc_bit_field & (1 << 2)) && (proc_bit_field & (1 << 3)))
	{
	    if((Timer_Stop(CH_ID_01)) != SUCCESS)
	    {
			appl_status_flag = ERR_TIMER_STOP_PROC;
	        Error_or_Warning_Proc("14.22.18", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
		}
		switch(cur_elevator_ptr->before_fsm_state) 
		{
			case FSM_STARTUP:
			   #ifdef TRACE_FLOW
			      Print("WAIT_TILL_DOORS_TO_UNALIGN -> DECIDE_CAR_MOVEMENT \r");
               #endif	
               cur_elevator_ptr->cur_fsm_state = FSM_DECIDE_CAR_MOVEMENT;	   
            break;
			case FSM_PREPARE_USER_ENTRY_AND_EXIT:
			   #ifdef TRACE_FLOW
			      Print("WAIT_TILL_DOORS_TO_UNALIGN -> COMPUTE_NEXT_STOP_FLOOR \r");
               #endif	
               cur_elevator_ptr->cur_fsm_state = FSM_COMPUTE_NEXT_STOP_FLOOR;	
			break;
			default:
			   appl_status_flag = ERR_FORMAT_INVALID;
	           Error_or_Warning_Proc("14.22.19", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
		}
	}			
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Compute_Next_Stop_Floor_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.23

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Compute_Next_Stop_Floor_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	uint8_t elevator_status = 0;
		
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.23.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.23.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Compute_Floor_Stop_Datas(ctrl_elevator_ch_id, DOOR_CLOSED_STAGE, &elevator_status)) != SUCCESS)
	{
		 appl_status_flag = ERR_COMPUTE_STOP_FLOOR_DATAS;
	     Error_or_Warning_Proc("14.23.03", ERROR_OCCURED, appl_status_flag);
		 cur_elevator_ptr->elevator_status = ERR_COMPUTE_NEXT_STOP_FLOOR;
		 cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		 return SUCCESS;
	}
	switch(elevator_status)
	{
		case NO_PENDING_FLOOR_CALLS:
		   #ifdef TRACE_FLOW
	          Print("COMPUTE_NEXT_STOP_FLOOR -> ABNORMAL_EVENT \r");
		   #endif
           #ifdef TRACE_ERROR		   
	          Print("ERR: Event - no pending floor call at door closed \r");
	       #endif
		    appl_status_flag = ERR_NO_PENDING_CALL_DOOR_CLOSED;
	        Error_or_Warning_Proc("14.23.04", ERROR_OCCURED, appl_status_flag);
		   cur_elevator_ptr->elevator_status = ERR_COMPUTE_NEXT_STOP_FLOOR;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
           return SUCCESS;		   
		//break;
		case ONE_PENDING_FLOOR_CALL:
		  //one pending floor calls
        case STARTUP_STATIONARY:
           //startup stationary		
        case TRIGGER_MOVE_UP_NO_DIR_CHANGE:						
		    //trigger car to move up		
		case TRIGGER_MOVE_UP_DIR_CHANGE:
		  //trigger car to move up
		case TRIGGER_MOVE_DOWN_NO_DIR_CHANGE:						
		   //trigger car to move down		  
		case TRIGGER_MOVE_DOWN_DIR_CHANGE:
		   //trigger car to move down
		   #ifdef TRACE_DATA 
			   Print("TRA: After Compute Floor Data - Closed stage \r");
			   Print("===========================================\r");
			   uint32_temp_disp_data = cur_elevator_ptr->cur_fsm_state;
               Print("cur fsm                       : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data =  cur_elevator_ptr->cur_floor;      
	           Print("cur_floor                     : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data =  cur_elevator_ptr->next_stop_floor; 
	           Print("next_stop_floor               : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data =  cur_elevator_ptr->cur_min_floor_call; 
	           Print("min_stop_floor                : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data =  cur_elevator_ptr->cur_max_floor_call; 
	           Print("max_stop_floor                : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data = cur_elevator_ptr->elevator_status; 
	           Print("status                        : %u\r", uint32_temp_disp_data);
			   uint32_temp_disp_data = cur_elevator_ptr->pending_floor_calls_bit_field;
	           Print("pending floor calls - bit     : 0x%x\r", uint32_temp_disp_data);
			   Print("===========================================\r");
			#elseif defined TRACE_INFO
			    uint32_temp_disp_data =  cur_elevator_ptr->cur_floor;  
			   Print("INFO: current floor = %u, ", uint32_temp_disp_data);
			   uint32_temp_disp_data =  cur_elevator_ptr->next_stop_floor; 
	           Print("next stop floor = %u \r", uint32_temp_disp_data);
            #endif
            #ifdef TRACE_FLOW			
               Print("COMPUTE_NEXT_STOP_FLOOR -> DECIDE_CAR_MOVEMENT \r");
            #endif	   
          	cur_elevator_ptr->cur_fsm_state = FSM_DECIDE_CAR_MOVEMENT;	   
		break;  
        default:	
            appl_status_flag = ERR_FORMAT_INVALID;
		    Error_or_Warning_Proc("14.23.05", ERROR_OCCURED, appl_status_flag);
		   	Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;				   
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Wait_For_Door_Close_To_Start_Open_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.24

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_For_Door_Close_To_Start_Open_Proc(const uint8_t ctrl_elevator_ch_id)
{
    elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	 
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.24.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_FOR_DOOR_CLOSE_TO_START_OPEN -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.24.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((ret_status = Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	       Error_or_Warning_Proc("14.24.03", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW
	           Print("WAIT_FOR_DOOR_CLOSE_TO_START_OPEN -> ABNORMAL_EVENT \r");
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
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOOR_CLOSE_TO_START_OPEN)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.24.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			 if(( IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
			 {
				appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.24.05", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
			 }
			 if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_HIGH)) != SUCCESS)
			 {
				appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.24.05", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
			 }
			 if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_OPENED_MOTOR)) != SUCCESS)
	         {
		           appl_status_flag = ERR_TIMER_RUN_PROC;
	               Error_or_Warning_Proc("14.24.06", ERROR_OCCURED, appl_status_flag);
	               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		           return SUCCESS;
	         }           
		     #ifdef TRACE_FLOW
		       Print("WAIT_FOR_DOOR_CLOSE_TO_START_OPEN -> WAIT_TILL_DOOR_OPENED \r");
			 #endif
             #ifdef TRACE_REQ 			 
               Print("REQ: Door open limit SW to active \r");			  
		     #endif	
             cur_elevator_ptr->before_fsm_state = FSM_WAIT_FOR_DOOR_CLOSE_TO_START_OPEN;				
		     cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_OPENED; 
		break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.24.07", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Abnormal_Event_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.25

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Abnormal_Event_Proc(const uint8_t ctrl_elevator_ch_id)
{
     elevator_ctrl_and_status_t *cur_elevator_ptr;	
	 uint8_t io_ch_read_status_1, io_ch_read_status_2;
	 
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.25.01", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
	if((Timer_Stop(CH_ID_01)) != SUCCESS)
	{
		appl_status_flag = ERR_TIMER_STOP_PROC;
	    Error_or_Warning_Proc("14.25.02", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Write(ALARM_LED_IO_CH, STATE_HIGH)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.25.03", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Read(ALARM_LED_IO_CH, &io_ch_read_status_1)) != SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_READ;
		 Error_or_Warning_Proc("14.25.04", ERROR_OCCURED, system_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}	
	#ifdef TRACE_INFO
	if(io_ch_read_status_1 == STATE_HIGH)
	{
	    Print("WARN: ALARM is ON \r");
	}
	#endif
	if((IO_Channel_Read(CAR_DOOR_OPEN_IO_CH, &io_ch_read_status_1)) != SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_READ;
		 Error_or_Warning_Proc("14.25.05", ERROR_OCCURED, system_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		 return SUCCESS;
	}
	if((IO_Channel_Read(CAR_DOOR_CLOSE_IO_CH, &io_ch_read_status_2)) != SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_READ;
		 Error_or_Warning_Proc("14.25.06", ERROR_OCCURED, system_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		 return SUCCESS; 
	}
	
	if(io_ch_read_status_1 == STATE_LOW)
	{
		if(io_ch_read_status_2 == STATE_HIGH )
		{
			#ifdef TRACE_INFO 
	           Print("WARN: Car door stopped closing \r");	  
	        #endif
		}
	}
    if(io_ch_read_status_2 == STATE_LOW)
	{
		if(io_ch_read_status_1 == STATE_HIGH )
		{
			#ifdef TRACE_INFO 
	           Print("WARN: Car door stopped opening \r");	  
	        #endif
		}
	}	
	if((IO_Channel_Read(CAR_UP_IO_CH, &io_ch_read_status_1)) != SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_READ;
		 Error_or_Warning_Proc("14.25.07", ERROR_OCCURED, system_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Read(CAR_DOWN_IO_CH, &io_ch_read_status_2)) != SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_READ;
		 Error_or_Warning_Proc("14.25.08", ERROR_OCCURED, system_status_flag);
		 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if(io_ch_read_status_1 == STATE_LOW)
	{
		if(io_ch_read_status_2 == STATE_HIGH )
		{
			#ifdef TRACE_INFO 
	           Print("WARN: Car stopped moving down \r");	  
	        #endif
		}
	}
    if(io_ch_read_status_2 == STATE_LOW)
	{
		if(io_ch_read_status_1 == STATE_HIGH )
		{
			#ifdef TRACE_INFO 
	           Print("WARN : Car stopped moving up \r");	  
	        #endif
		}
	}	
	if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.25.09", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.25.10", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Write(CAR_UP_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.25.11", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
	if((IO_Channel_Write(CAR_DOWN_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.25.12", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS;
	}
    #ifdef TRACE_FLOW 
	  Print("ABNORMAL_EVENT -> IDLE \r");	  
	#endif
	cur_elevator_ptr->before_fsm_state = FSM_ABNORMAL_EVENT;
	cur_elevator_ptr->cur_fsm_state = FSM_IDLE;	
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
