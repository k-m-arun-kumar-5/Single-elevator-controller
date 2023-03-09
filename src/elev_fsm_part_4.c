/* ********************************************************************
FILE                   : elev_fsm_part_4.c

PURPOSE                : Elevator FSM Proc part 4
	 
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
FUNCTION NAME  :  FSM_Prepare_Door_Close_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.18

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Prepare_Door_Close_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	static uint8_t loop_flag = 0;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		loop_flag = 0; 
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.18.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.18.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Stationary_Door_Opened_State(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		loop_flag = 0;
		 appl_status_flag = ERR_CHECK_STATIONARY;
	     Error_or_Warning_Proc("14.18.03", ERROR_OCCURED, appl_status_flag);
		 #ifdef TRACE_FLOW 
	         Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT \r");
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
	       Error_or_Warning_Proc("14.18.04", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW 
	         Print("PREPARE_FOR_DOOR_CLOSE -> ABNORMAL_EVENT \r");
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
				   if((Timer_Pause(CH_ID_01)) != SUCCESS)
				   {
					   appl_status_flag = ERR_TIMER_PAUSE_PROC;
	                   Error_or_Warning_Proc("14.18.05", ERROR_OCCURED, appl_status_flag);
	                   Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
				   }
				   return SUCCESS;
				}
				else
				{
					return SUCCESS;
				}
	        //break;
	        case SW_OR_KEY_NOT_PRESSED:	
                 proc_bit_field |= (1 << 1);                
				 if((loop_flag & (1 << 2)) == 0)
				 {
					 if((loop_flag & (1 << 1)))
					 {
                        #ifdef TRACE_INFO
				          Print("INFO: Car is not overloaded \r");
                        #endif
					 }					
                   	 loop_flag |= (1 << 2);
					 if((Timer_Pause(CH_ID_01)) != SUCCESS)
				     {
						 loop_flag = 0;
					     appl_status_flag = ERR_TIMER_PAUSE_PROC;
	                     Error_or_Warning_Proc("14.18.06", ERROR_OCCURED, appl_status_flag);
	                     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
						 return SUCCESS;
				     }
					 if((Timer_Resume(CH_ID_01, TIMER_ID_DOOR_OPENED)) != SUCCESS)
					 {
						  loop_flag = 0;
						appl_status_flag = ERR_TIMER_RESUME_PROC;
	                    Error_or_Warning_Proc("14.18.07", ERROR_OCCURED, appl_status_flag);
	                    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                return SUCCESS; 
					 }
				 }
				 loop_flag &= ~(1 << 1);
	        break;
	        case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	        case WARN_CUR_DATA_ID_DEV_DISABLED:
			    loop_flag = 0;
			    appl_status_flag = ERR_SW_IS_DISABLED;
	            Error_or_Warning_Proc("14.18.08", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        //break;
            default:
			      loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.18.09", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
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
		     loop_flag = 0;  
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOOR_OPENED)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.18.10", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			#ifdef TRACE_FLOW
		      Print("PREPARE_FOR_DOOR_CLOSE -> TRIGGER_DOOR_CLOSE \r");			 
		    #endif		
	        cur_elevator_ptr->cur_fsm_state = FSM_TRIGGER_DOOR_CLOSE; 
            return SUCCESS;			
		//break;
		default:
		    loop_flag = 0;
		    appl_status_flag = ERR_FORMAT_INVALID;
	        Error_or_Warning_Proc("14.18.11", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((proc_bit_field & (1 << 1)))
	{
	   ret_status = SW_Oper(MANUAL_DOOR_CLOSE_SW_IO_CH, DEV_READ_OPER);
	   switch(ret_status)
	   {
	      case SUCCESS:
		     loop_flag = 0;   
             if((ret_status = Timer_Stop(CH_ID_01)) != SUCCESS)
		     {
			    appl_status_flag = ERR_TIMER_STOP_PROC;
	            Error_or_Warning_Proc("14.18.14", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	         }
		     #ifdef TRACE_INFO
			    Print("INFO: Manual Door Close triggered\r");
			 #endif
             #ifdef TRACE_FLOW			 
		        Print("PREPARE_FOR_DOOR_CLOSE -> TRIGGER_DOOR_CLOSE \r");			 
		     #endif		
	         cur_elevator_ptr->cur_fsm_state = FSM_TRIGGER_DOOR_CLOSE; 
			 return SUCCESS;
      	  //break;
	      case SW_OR_KEY_NOT_PRESSED:               				
	      break;
	      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	      case WARN_CUR_DATA_ID_DEV_DISABLED:
		      loop_flag = 0; 
		     appl_status_flag = ERR_SW_IS_DISABLED;
	         Error_or_Warning_Proc("14.18.15", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
	    //break;	     
          default:
		      loop_flag = 0;
              appl_status_flag = ERR_SW_READ_PROC;
	          Error_or_Warning_Proc("14.18.16", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
	   }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Trigger_Door_Close_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.19

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Trigger_Door_Close_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	static uint8_t loop_flag = 0;
	uint8_t proc_bit_field = 0;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		loop_flag = 0;
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.19.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("TRIGGER_DOOR_CLOSE -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.19.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
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
                 proc_bit_field |= (1 << 1);                
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
	            Error_or_Warning_Proc("14.19.03", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        //break;
            default:
			     loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.19.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	    }
	if((proc_bit_field & (1 << 1)))
	{
		loop_flag = 0;
	   if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
	   {
		  
	    	appl_status_flag = ERR_IO_CH_WRITE;
	        Error_or_Warning_Proc("14.19.05", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	   }
	   if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_HIGH)) != SUCCESS)
	   {
		   appl_status_flag = ERR_IO_CH_WRITE;
	       Error_or_Warning_Proc("14.19.06", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	   }
	   if((SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
	   {		   
	    	appl_status_flag = ERR_DEV_ENABLE_PROC;
	        Error_or_Warning_Proc("14.19.07", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	   }
	   if((SW_Oper(MANUAL_DOOR_CLOSE_SW_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
	   {  
	    	 appl_status_flag = ERR_DEV_DISABLE_PROC;
	         Error_or_Warning_Proc("14.19.08", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	   }
	   if((Timer_Run(CH_ID_01, TIMER_ID_DETECT_DOOR_CLOSE)) != SUCCESS)
	   {
		    appl_status_flag = ERR_TIMER_RUN_PROC;
	        Error_or_Warning_Proc("14.19.09", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	   }
	   #ifdef TRACE_FLOW
	      Print("TRIGGER_DOOR_CLOSE -> WAIT_TILL_DOOR_START_CLOSE \r");
	   #endif
	   #ifdef TRACE_INFO	  
	      Print("INFO: Car Door closing ... \r");
	   #endif
       #ifdef TRACE_REQ	  
	      Print("REQ: Door open limit SW to inactive \r");
	   #endif
	   cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_START_CLOSE;
	}	   
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Wait_Till_Door_Start_Close_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.20 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Door_Start_Close_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t proc_bit_field = 0;
	static uint8_t loop_flag = 0;
  
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		loop_flag = 0;
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.20.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.20.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		loop_flag = 0;
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.20.03", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT \r");
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
             proc_bit_field |= (1 << 1);				
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    loop_flag = 0;
		    #ifdef TRACE_FLOW		  
		      Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT \r");
		    #endif
            #ifdef TRACE_ERROR 
	          Print("ERR: Event - Doors not aligned properly \r");
	        #endif
			appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("14.20.04", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			cur_elevator_ptr->elevator_status = ERR_DOORS_NOT_ALIGNED_PROPERLY;
			return SUCCESS;
		 //break;
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		     loop_flag = 0;
		     appl_status_flag = ERR_SW_IS_DISABLED;
	         Error_or_Warning_Proc("14.20.05", ERROR_OCCURED, appl_status_flag);
			 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	
		 //break;
         default:
		    loop_flag = 0;
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.20.06", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	 
	}
	 ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
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
		     loop_flag = 0;
		     appl_status_flag = ERR_SW_IS_DISABLED;
	         Error_or_Warning_Proc("14.20.07", ERROR_OCCURED, appl_status_flag);
			 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;	
		// break;
         default:
		    loop_flag = 0;
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.20.08", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	 
	  }
	 ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	 switch(ret_status)
	 {	
		 case SUCCESS:
		      appl_status_flag = ERR_SW_IS_ACTIVE;
	          Error_or_Warning_Proc("14.20.09", ERROR_OCCURED, appl_status_flag);
		      if(proc_bit_field & (1 << 4))
			  {
                 #ifdef TRACE_FLOW		  
		            Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT \r");
		         #endif
                 #ifdef TRACE_ERROR 
	                Print("ERR: Event - door open and close active for close\r");
	             #endif
	             cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			     cur_elevator_ptr->elevator_status = ERR_DOOR_OPEN_AND_CLOSE_ACTIVE_FOR_CLOSE;
			     return SUCCESS;
			  }
              if(proc_bit_field & (1 << 2))
			  {
			      #ifdef TRACE_FLOW		  
		             Print("WAIT_TILL_DOOR_START_CLOSE -> ABNORMAL_EVENT \r");
		          #endif
                  #ifdef TRACE_ERROR 
	                Print("ERR: Event - fast door close \r");
	              #endif
	              cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			      cur_elevator_ptr->elevator_status = ERR_DOOR_CLOSE_FAST;
			      return SUCCESS;
			  }   
		 //break;
		 case SW_OR_KEY_NOT_PRESSED:
            proc_bit_field |= (1 << 3);		 
		 break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		     loop_flag = 0;
		     appl_status_flag = ERR_SW_IS_DISABLED;
	          Error_or_Warning_Proc("14.20.10", ERROR_OCCURED, appl_status_flag);
			  Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS; 
		 //break;
         default:
		   loop_flag = 0; 
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.20.11", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS; 
	  }
	  ret_status = SW_Oper(OVERLOAD_SENSOR_IO_CH, DEV_READ_OPER);
	   switch(ret_status)
	  {
	      case SUCCESS:
                   #ifdef TRACE_INFO
				      Print("WARN: event - car overloaded \r");
				   #endif
                   #ifdef TRACE_REQ	  
					  Print("REQ: Car to be not overload \r");
                   #endif
				   loop_flag |= (1 << 1);
				   loop_flag &= ~(1 << 2);
				   if((Timer_Stop(CH_ID_01)) != SUCCESS)
		           {
		              appl_status_flag = ERR_TIMER_STOP_PROC;
	                  Error_or_Warning_Proc("14.20.12", ERROR_OCCURED, appl_status_flag);
	                  Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		              return SUCCESS;
		           }
		           if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
	               {
	                 appl_status_flag = ERR_IO_CH_WRITE;
	                 Error_or_Warning_Proc("14.20.13", ERROR_OCCURED, appl_status_flag);
	                 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		             return SUCCESS;
	             }
	             if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
	             {
	             	appl_status_flag = ERR_IO_CH_WRITE;
	                Error_or_Warning_Proc("14.20.14", ERROR_OCCURED, appl_status_flag);
	                Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		            return SUCCESS;
	             }
				 #ifdef TRACE_FLOW
				    Print("WAIT_TILL_DOOR_START_CLOSE -> WAIT_FOR_DOOR_CLOSE_TO_START_OPEN \r");
				 #endif
		         if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_CLOSE_TO_START_OPEN)) != SUCCESS)
	             {
	             	appl_status_flag = ERR_TIMER_RUN_PROC;
	                Error_or_Warning_Proc("14.20.15", ERROR_OCCURED, appl_status_flag);
					Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	                return appl_status_flag;
	             }
				 #ifdef TRACE_INFO		
				    Print("INFO: Car door stopped closing \r");
                 #endif	
				  #ifdef TRACE_REQ		
				    Print("REQ: Wait for motor from door close to open \r");
                 #endif	
                cur_elevator_ptr->cur_fsm_state = FSM_WAIT_FOR_DOOR_CLOSE_TO_START_OPEN;
			    return SUCCESS;
	        break;
	        case SW_OR_KEY_NOT_PRESSED:	
                 proc_bit_field |= (1 << 5);                
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
	            Error_or_Warning_Proc("14.20.16", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        //break;
            default:
			   loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.20.17", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	    }
	   if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	   {
		   loop_flag = 0;
		    appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	        Error_or_Warning_Proc("14.20.18", ERROR_OCCURED, appl_status_flag);
            #ifdef TRACE_FLOW
				Print("WAIT_TILL_DOOR_START_CLOSE -> WAIT_FOR_DOOR_CLOSE_TO_START_OPEN \r");
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
		    loop_flag = 0;
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DETECT_DOOR_CLOSE)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.20.19", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			#ifdef TRACE_FLOW
				Print("WAIT_TILL_DOOR_START_CLOSE -> WAIT_FOR_DOOR_CLOSE_TO_START_OPEN \r");
			#endif
			#ifdef TRACE_ERROR 	 
			  Print("ERR: Event - req door close but opened \r");
		    #endif
			cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_CLOSE_BUT_OPENED;
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
            return SUCCESS;			
		//break;
		default:
		   loop_flag = 0;
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.20.20", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
      if((proc_bit_field & (1 << 0)) && (proc_bit_field & (1 << 1)) && (proc_bit_field & (1 << 2)) && (proc_bit_field & (1 << 3)) && (proc_bit_field & (1 << 5)))
	  {	
           loop_flag = 0;
            if(( Timer_Stop(CH_ID_01)) != SUCCESS)
			{
				appl_status_flag = ERR_TIMER_STOP_PROC;
	            Error_or_Warning_Proc("14.20.21", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
	        #ifdef TRACE_FLOW
	          Print("WAIT_TILL_DOOR_START_CLOSE -> WAIT_TILL_DOOR_CLOSED \r");
			#endif
			if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_CLOSED_MOTOR)) != SUCCESS)
	        {
		       appl_status_flag = ERR_TIMER_RUN_PROC;
	           Error_or_Warning_Proc("11.20.22", ERROR_OCCURED, appl_status_flag);
	           Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	        }
            #ifdef TRACE_REQ	  
	          Print("REQ: Door close limit SW to active \r");
	        #endif
	        cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_CLOSED;	
	  }			
	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Wait_Till_Door_Closed_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.21

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Door_Closed_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
  static uint8_t loop_flag = 0;
	uint8_t proc_bit_field = 0;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		loop_flag = 0;
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.21.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.21.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Stationary_Limit_SW_Floor(ctrl_elevator_ch_id)) != SUCCESS)
	{
		loop_flag = 0;
		cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		appl_status_flag = ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY;
		Error_or_Warning_Proc("14.21.03", ERROR_OCCURED, appl_status_flag);
		#ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
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
		    proc_bit_field |= (1 << 1);
		 break;
		 case SW_OR_KEY_NOT_PRESSED:
		    #ifdef TRACE_FLOW		  
		      Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
		    #endif
            #ifdef TRACE_ERROR 
	          Print("ERR: Event - Doors not aligned properly \r");
	        #endif
			loop_flag = 0;
			appl_status_flag = ERR_SW_IS_INACTIVE;
	        Error_or_Warning_Proc("14.21.07", ERROR_OCCURED, appl_status_flag);
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			cur_elevator_ptr->elevator_status = ERR_DOORS_NOT_ALIGNED_PROPERLY;
			return SUCCESS;
		// break;	
		 case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		 case WARN_CUR_DATA_ID_DEV_DISABLED:
		    loop_flag = 0;
		    appl_status_flag = ERR_SW_IS_DISABLED;
	        Error_or_Warning_Proc("14.21.08", ERROR_OCCURED, appl_status_flag);
		    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	
		 //break;
         default:
		    loop_flag = 0;
          	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.21.09", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;	 
	}
	ret_status = SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	   case SUCCESS:
		  #ifdef TRACE_FLOW		  
		    Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
		  #endif
          #ifdef TRACE_ERROR 
	          Print("ERR: Event - req door close but opened \r");
	      #endif
		   loop_flag = 0;
		  appl_status_flag = ERR_SW_IS_ACTIVE;
	       Error_or_Warning_Proc("14.21.10", ERROR_OCCURED, appl_status_flag); 
	      cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   cur_elevator_ptr->elevator_status = ERR_REQ_DOOR_CLOSE_BUT_OPENED;
          return SUCCESS;
	   // break;
	   case SW_OR_KEY_NOT_PRESSED:
	        proc_bit_field |= (1 << 2); 
	   break;
	   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	   case WARN_CUR_DATA_ID_DEV_DISABLED:
	       loop_flag = 0;
	       appl_status_flag = ERR_SW_IS_DISABLED;
	       Error_or_Warning_Proc("14.21.11", ERROR_OCCURED, appl_status_flag);
		   Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;	
	   //break;
       default:
	       loop_flag = 0;
        	appl_status_flag = ERR_SW_READ_PROC;
	        Error_or_Warning_Proc("14.21.12", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	}
	 if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	 {
		     loop_flag = 0;
		     appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
	         Error_or_Warning_Proc("14.21.13", ERROR_OCCURED, appl_status_flag);
	         #ifdef TRACE_FLOW		  
		       Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
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
		    loop_flag = 0;
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_DOOR_CLOSED_MOTOR)
			{				
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.21.14", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}			
		    #ifdef TRACE_FLOW		  
		       Print("WAIT_TILL_DOOR_CLOSED -> ABNORMAL_EVENT \r");
		    #endif
            #ifdef TRACE_ERROR 
			  Print("ERR: Event - door closed not detected \r");
		    #endif
			cur_elevator_ptr->elevator_status = ERR_DOOR_CLOSE_NOT_DETECT;
		    cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT; 
            return SUCCESS;			
		//break;
		default:
		   loop_flag = 0;
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.21.15", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}  
	if(proc_bit_field == 0x07)
	{
		proc_bit_field = 0;
		ret_status = SW_Oper(OVERLOAD_SENSOR_IO_CH, DEV_READ_OPER);
	    switch(ret_status)
	    {
	      case SUCCESS:
		         #ifdef TRACE_INFO
				      Print("WARN: event - car overloaded \r");
				#endif
                #ifdef TRACE_REQ		  
				      Print("REQ: Car to be not overload \r");
                 #endif
				loop_flag |= (1 << 1); 
				loop_flag &= ~(1 << 2);	
				proc_bit_field |= (1 << 3);
	        break;
	        case SW_OR_KEY_NOT_PRESSED:	
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
	            Error_or_Warning_Proc("14.21.16", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	        //break;
            default:
			    loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.21.17", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	    }
	   	ret_status = SW_Oper(USER_BLOCKS_DOOR_SENSOR_IO_CH, DEV_READ_OPER);
	    switch(ret_status)
	    {
		       case SUCCESS:
			         proc_bit_field |= (1 << 6);  
					 #ifdef TRACE_INFO
					     Print("INFO: User blocks the closing door, so door will open \r");
					 #endif
	           break;
	           case SW_OR_KEY_NOT_PRESSED:
                  	proc_bit_field |= (1 << 7);  		   
		       break;
		       case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		       case WARN_CUR_DATA_ID_DEV_DISABLED:
			     loop_flag = 0;
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.21.18", ERROR_OCCURED, appl_status_flag);
			     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		     //break;
              default:
			    loop_flag = 0;
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.21.19", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;	 
	    }
		if((proc_bit_field & (1 << 7)))
		{
		      ret_status = SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_READ_OPER);
	           switch(ret_status)
	           {
		          case SUCCESS:
			         proc_bit_field |= (1 << 5);  
					 #ifdef TRACE_INFO
					     Print("INFO: Manual door open triggered\r");
					 #endif
	              break;
	              case SW_OR_KEY_NOT_PRESSED:             	  	
		          break;
		          case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		          case WARN_CUR_DATA_ID_DEV_DISABLED:
				     loop_flag = 0;
		             appl_status_flag = ERR_SW_IS_DISABLED;
	                 Error_or_Warning_Proc("14.21.22", ERROR_OCCURED, appl_status_flag);
			         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		             return SUCCESS;
		        //break;
                  default:
				    loop_flag = 0;
                	appl_status_flag = ERR_SW_READ_PROC;
	                Error_or_Warning_Proc("14.21.23", ERROR_OCCURED, appl_status_flag);
	                Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		            return SUCCESS;	 
	            }
			}
		}
		if(((proc_bit_field & (1 << 5))) || ((proc_bit_field & (1 << 6))) || ((proc_bit_field & (1 << 3))))
		{
			loop_flag = 0;
		   if((Timer_Stop(CH_ID_01)) != SUCCESS)
		   {
		       appl_status_flag = ERR_TIMER_STOP_PROC;
	           Error_or_Warning_Proc("14.21.24", ERROR_OCCURED, appl_status_flag);
	           Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
		   }
		   if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
	       {
	        	appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.21.25", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	       }
	       if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
	       {
	        	appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.21.26", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	       }
		   if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_CLOSE_TO_START_OPEN)) != SUCCESS)
	       {
	        	appl_status_flag = ERR_TIMER_RUN_PROC;
	            Error_or_Warning_Proc("14.21.27", ERROR_OCCURED, appl_status_flag);
				Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	            return appl_status_flag;
	       }
		   #ifdef TRACE_FLOW
				 Print("WAIT_TILL_DOOR_CLOSED -> WAIT_FOR_DOOR_CLOSE_TO_START_OPEN \r");
		   #endif
		   #ifdef TRACE_INFO	 
				 Print("INFO: Car door stopped closing \r");
           #endif 
           #ifdef TRACE_REQ		 
				 Print("REQ: Wait for motor from door close to open \r");
           #endif	
           cur_elevator_ptr->cur_fsm_state = FSM_WAIT_FOR_DOOR_CLOSE_TO_START_OPEN;
           return SUCCESS;		   
		}
		ret_status = SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_READ_OPER);
	    switch(ret_status)
	    {
	       case SUCCESS:
              loop_flag = 0;		   
			 if((Timer_Stop(CH_ID_01)) != SUCCESS)
			 {
				appl_status_flag = ERR_TIMER_STOP_PROC;
	            Error_or_Warning_Proc("14.21.28", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			 }
			 if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
			 {
				appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.21.29", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
			 }
			 if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_LOW)) != SUCCESS)
			 {
				appl_status_flag = ERR_IO_CH_WRITE;
	            Error_or_Warning_Proc("14.21.30", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
			 }
			 if((SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
			 {
				 appl_status_flag = ERR_DEV_DISABLE_PROC;
	             Error_or_Warning_Proc("14.21.32", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
			 }
			 if((SW_Oper(USER_BLOCKS_DOOR_SENSOR_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
			 {
				 appl_status_flag = ERR_DEV_DISABLE_PROC;
	             Error_or_Warning_Proc("14.21.33", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
			 }
			 if((Timer_Run(CH_ID_01, TIMER_ID_DOORS_UNALIGNED )) != SUCCESS)
			 {
				appl_status_flag = ERR_TIMER_RUN_PROC;
	            Error_or_Warning_Proc("14.21.34", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			 }
			 #ifdef TRACE_FLOW
			      Print("WAIT_TILL_DOOR_CLOSED -> WAIT_TILL_DOORS_TO_UNALIGN \r");
			#endif
			#ifdef TRACE_INFO
			   Print("INFO: Car door closed \r");
			#endif
            #ifdef TRACE_REQ		  
				  Print("REQ: Doors align to inactive \r");
			 #endif
			 cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOORS_TO_UNALIGN;
			 return SUCCESS;
			 //  break;
	       case SW_OR_KEY_NOT_PRESSED:	
	       break;
	       case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
	       case WARN_CUR_DATA_ID_DEV_DISABLED:
	         appl_status_flag = ERR_SW_IS_DISABLED;
	         Error_or_Warning_Proc("14.21.35", ERROR_OCCURED, appl_status_flag);
		     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;	
	      // break;
           default:
              appl_status_flag = ERR_SW_READ_PROC;
	          Error_or_Warning_Proc("14.21.36", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
		}
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
