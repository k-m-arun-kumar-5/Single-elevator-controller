/* ********************************************************************
FILE                   : elev_fsm_part_1.c

PURPOSE                : Elevator FSM Proc part 1
	 
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

/* ------------------------------ macro defination ------------------------------ */
typedef enum
{
	NONE_SW_PRESSED_DETECT, ONE_SW_PRESSED_DETECT, AT_LEAST_TWO_SW_PRESSED_DETECT
} switches_pressed_detect_states_t;

/* ----------------------------- global variable defination --------------------- */ 
elevator_ctrl_and_status_t elevator_ctrl_and_status[MAX_NUM_ELEVATORS];

uint8_t limit_sw_up_arr[MAX_NUM_FLOORS] = 
            {
				IO_CH_INVALID, LIMIT_SW_UP_FLOOR_1_IO_CH, LIMIT_SW_UP_FLOOR_2_IO_CH, 
				LIMIT_SW_UP_FLOOR_3_IO_CH, LIMIT_SW_UP_FLOOR_4_IO_CH
			};
			
uint8_t limit_sw_down_arr[MAX_NUM_FLOORS] = 
            {
				LIMIT_SW_DOWN_FLOOR_0_IO_CH, LIMIT_SW_DOWN_FLOOR_1_IO_CH, 
				LIMIT_SW_DOWN_FLOOR_2_IO_CH, LIMIT_SW_DOWN_FLOOR_3_IO_CH,
				IO_CH_INVALID
			};	
			
uint8_t in_car_call_arr[MAX_NUM_FLOORS] = 
            {
				IN_CAR_CALL_SW_FLOOR_0_IO_CH, IN_CAR_CALL_SW_FLOOR_1_IO_CH, IN_CAR_CALL_SW_FLOOR_2_IO_CH,
				IN_CAR_CALL_SW_FLOOR_3_IO_CH, IN_CAR_CALL_SW_FLOOR_4_IO_CH
			};			
			
uint8_t hall_call_up_arr[MAX_NUM_FLOORS] = 
            {
				HALL_CALL_UP_FLOOR_0_SW_IO_CH, HALL_CALL_UP_FLOOR_1_SW_IO_CH,
				HALL_CALL_UP_FLOOR_2_SW_IO_CH, HALL_CALL_UP_FLOOR_3_SW_IO_CH,
				IO_CH_INVALID    				
			};

uint8_t hall_call_down_arr[MAX_NUM_FLOORS] = 
            {
				IO_CH_INVALID, HALL_CALL_DOWN_FLOOR_1_SW_IO_CH, HALL_CALL_DOWN_FLOOR_2_SW_IO_CH,
				HALL_CALL_DOWN_FLOOR_3_SW_IO_CH, HALL_CALL_DOWN_FLOOR_4_SW_IO_CH
			};
			
/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */
elevator_fsm_proc_func_t Elevator_FSM_Proc_Func_Ptr[NUM_ELEVATOR_FSM_STATES] = 
         { 
		    FSM_Idle_Proc, FSM_Wait_Till_Start_Oper_Proc, FSM_StartUp_Proc, FSM_Decide_Car_Move_Proc, FSM_Trigger_Move_Up_Proc,
			FSM_Trigger_Move_Down_Proc, FSM_Prepare_To_Move_Proc, FSM_Car_Moving_Proc, FSM_Trigger_Car_Stop_Proc, 
			FSM_Wait_Till_Car_Stopped_Proc, FSM_Prepare_Doors_To_Align_Proc, FSM_Wait_Till_Doors_Aligned_Proc, FSM_Trigger_Door_Open_Proc, 
			FSM_Wait_Till_Door_Start_Open_Proc, FSM_Wait_Till_Door_Opened_Proc, FSM_Prepare_User_Entry_And_Exit_Proc,FSM_User_Entry_And_Exit_Proc, 
			FSM_Prepare_Door_Close_Proc, FSM_Trigger_Door_Close_Proc, FSM_Wait_Till_Door_Start_Close_Proc, FSM_Wait_Till_Door_Closed_Proc,
			FSM_Wait_Till_Doors_Unaligned_Proc, FSM_Compute_Next_Stop_Floor_Proc, FSM_Wait_For_Door_Close_To_Start_Open_Proc, FSM_Abnormal_Event_Proc
		 };

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Idle_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.01  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Idle_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
		  
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.01.01", ERROR_OCCURED, appl_status_flag);
	    return appl_status_flag;
    }	
    cur_elevator_ptr = elevator_ctrl_and_status + ctrl_elevator_ch_id;
	if(cur_elevator_ptr->before_fsm_state == FSM_IDLE)
	{
		if((Enable_Ext_Interrupt(EMER_CAR_STOP_EXT_INTP_CH_ID) ) != SUCCESS)
		{
		    appl_status_flag = ERR_ENABLE_INTERRUPT;
	        Error_or_Warning_Proc("14.01.02", ERROR_OCCURED, appl_status_flag);
		    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
			return SUCCESS;
		} 
	}
    if(cur_elevator_ptr->before_fsm_state == FSM_IDLE || cur_elevator_ptr->before_fsm_state == FSM_ABNORMAL_EVENT)	
	{
           	if((SW_Oper(CAR_START_SW_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		    {
			     appl_status_flag = ERR_DEV_ENABLE_PROC;
	             Error_or_Warning_Proc("14.01.03", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
				return SUCCESS;
		    } 				
			#ifdef TRACE_FLOW 
	           Print("IDLE -> WAIT_FOR_START_OPER \r");			  
	        #endif
			#ifdef TRACE_REQ
              Print("REQ: Car Start Oper \r");	
            #endif			
		   cur_elevator_ptr->cur_fsm_state = FSM_WAIT_FOR_START_OPER;	
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Wait_Till_Start_Oper_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.02  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Start_Oper_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	uint8_t limit_sw_cur_floor, oper, limit_sw_min_floor, limit_sw_max_floor, alarm_io_ch_read_status, *limit_sw_arr_ptr;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.02.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_FOR_START_OPER -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.02.02", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	} 
	ret_status = SW_Oper(CAR_START_SW_IO_CH, DEV_READ_OPER);
	switch(ret_status)
	{
	  	case SUCCESS:		      
	          if((SW_Oper(CAR_START_SW_IO_CH, DEV_DISABLE_OPER)) != SUCCESS)
	          {
		           appl_status_flag = ERR_DEV_DISABLE_PROC;
	               Error_or_Warning_Proc("14.02.03", ERROR_OCCURED, appl_status_flag);
	               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		           return SUCCESS;
			   }
			  appl_status_flag = NO_ERROR;	
		      if((IO_Channel_Write(ALARM_LED_IO_CH, STATE_LOW)) != SUCCESS)
	          {
	          	 appl_status_flag = ERR_IO_CH_WRITE;
	             Error_or_Warning_Proc("14.02.04", ERROR_OCCURED, appl_status_flag);
				 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id); 
	             return SUCCESS;
	          }	
			  if((IO_Channel_Read(ALARM_LED_IO_CH, &alarm_io_ch_read_status)) != SUCCESS)
	          {
		         system_status_flag = ERR_IO_CH_READ;
		         Error_or_Warning_Proc("14.02.05", ERROR_OCCURED, system_status_flag);
				 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
	          }	
			  #ifdef TRACE_INFO
			       Print("INFO: ALARM is OFF \r");
              #endif			  
			  Reset_Elevator_Datas(ctrl_elevator_ch_id, RESET_ABNORMAL_STATE_DATAS);
			  if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
			  {
				  appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	              Error_or_Warning_Proc("14.02.06", ERROR_OCCURED, appl_status_flag);
				  Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	              return SUCCESS;
			  }
			   for(oper = UP_SW_PROC; oper <= DOWN_SW_PROC; ++oper)
			   {
				   switch(oper)
				   {
					   case UP_SW_PROC:
					     limit_sw_min_floor = FLOOR_01;
						 limit_sw_max_floor = TOP_MOST_FLOOR_ID;
						 limit_sw_arr_ptr = limit_sw_up_arr;
					   break;
                       case DOWN_SW_PROC:
                         limit_sw_min_floor = FLOOR_00;
						 limit_sw_max_floor = TOP_MOST_FLOOR_ID - 1;
						 limit_sw_arr_ptr = limit_sw_down_arr;
					   break;
				   }					   
			       for( limit_sw_cur_floor = limit_sw_min_floor; limit_sw_cur_floor <= limit_sw_max_floor; ++limit_sw_cur_floor)
	               {
	                  if((SW_Oper(limit_sw_arr_ptr[limit_sw_cur_floor], DEV_ENABLE_OPER)) != SUCCESS)
	                  {
	                     appl_status_flag = ERR_DEV_ENABLE_PROC;
	                     Error_or_Warning_Proc("14.02.07", ERROR_OCCURED, appl_status_flag);
	                     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                 return SUCCESS;
		             }
				  }					 
	           }
			   if((SW_Oper(DOORS_ALIGNED_SW_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		       {
			      appl_status_flag = ERR_DEV_ENABLE_PROC;
	              Error_or_Warning_Proc("14.02.08", ERROR_OCCURED, appl_status_flag);
	               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		          return SUCCESS;
		       }   	
		       if((SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		       {
			      appl_status_flag = ERR_DEV_ENABLE_PROC;
	              Error_or_Warning_Proc("14.02.09", ERROR_OCCURED, appl_status_flag);
	              Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		          return SUCCESS;
		       }
		       if((SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_ENABLE_OPER) ) != SUCCESS)
		       {
			        appl_status_flag = ERR_DEV_ENABLE_PROC;
	                Error_or_Warning_Proc("14.02.10", ERROR_OCCURED, appl_status_flag);
	                Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		            return SUCCESS;
		       }			
             #ifdef TRACE_FLOW
	             Print("WAIT_FOR_START_OPER -> STARTUP \r");	               			   
	         #endif
			 #ifdef TRACE_REQ
			    Print("REQ: Startup - car floor position \r");
			 #endif
	         if((Timer_Run(CH_ID_01, TIMER_ID_STARTUP_CUR_FLOOR)) != SUCCESS)
	         {
	            appl_status_flag = ERR_TIMER_RUN_PROC;
	            Error_or_Warning_Proc("14.02.11", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	         }
	         cur_elevator_ptr->cur_fsm_state = FSM_STARTUP;
		    break;
		    case SW_OR_KEY_NOT_PRESSED:
		    break;
		    case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		    case WARN_CUR_DATA_ID_DEV_DISABLED:
			    appl_status_flag = ERR_SW_IS_DISABLED;
	            Error_or_Warning_Proc("14.02.12", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
		   // break;
            default:
            	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.02.13", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
	   }	   
	   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_StartUp_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.03  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_StartUp_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t limit_sw_cur_floor, proc_bit_field = 0, up_cur_floor = FLOOR_ID_INVALID, down_cur_floor = FLOOR_ID_INVALID, 
	   limit_sw_min_floor, limit_sw_max_floor, oper, oper_min, oper_max, *limit_sw_arr_ptr, is_pressed_limit_sw, 
	   is_sw_pressed_state_align, is_sw_pressed_state_close, is_sw_pressed_state_open;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.03.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("STARTUP -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.03.02", ERROR_OCCURED, appl_status_flag);
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
           if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_STARTUP_CUR_FLOOR)
	       {
		      appl_status_flag = ERR_TMR_ID_INVALID;
	          Error_or_Warning_Proc("14.03.03", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;
	      }				
           #ifdef TRACE_FLOW
	           Print("STARTUP -> ABNORMAL_EVENT \r");
		   #endif	   
		   #ifdef TRACE_ERROR   
		       Print("ERR: Event - startup cur floor not detected \r");			
		  #endif
		  appl_status_flag = ERR_STARTUP_OPER;
	      Error_or_Warning_Proc("14.03.04", ERROR_OCCURED, appl_status_flag);
          cur_elevator_ptr->elevator_status = ERR_STARTUP_CUR_FLOOR_NOT_DETECT;			
		  cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;            			
          return SUCCESS;			
      //break;
       default:
          appl_status_flag = ERR_FORMAT_INVALID;
	      Error_or_Warning_Proc("14.03.05", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	for(oper = UP_SW_PROC; oper <= DOWN_SW_PROC; ++oper)
	{
		switch(oper)
		{
			case UP_SW_PROC:
			  limit_sw_min_floor = FLOOR_01;
			  limit_sw_max_floor = TOP_MOST_FLOOR_ID;
			  limit_sw_arr_ptr = limit_sw_up_arr;
			break;
			case DOWN_SW_PROC:
			  limit_sw_min_floor = FLOOR_00;
			  limit_sw_max_floor = TOP_MOST_FLOOR_ID - 1;
			  limit_sw_arr_ptr = limit_sw_down_arr;
			break;
		}
		for(limit_sw_cur_floor = limit_sw_min_floor; limit_sw_cur_floor <= limit_sw_max_floor; ++limit_sw_cur_floor)
	    {		
	       ret_status = SW_Oper(limit_sw_arr_ptr[limit_sw_cur_floor], DEV_READ_OPER);
	       switch(ret_status)
	       {
	         case SUCCESS:
			    switch(oper)
				{
					case UP_SW_PROC:
	                   up_cur_floor = limit_sw_cur_floor; 
					   proc_bit_field |= (1 << 0);
                    break;
                    case DOWN_SW_PROC:
                       down_cur_floor = limit_sw_cur_floor;	
                       proc_bit_field |= (1 << 1);					   
			        break;
				}
		     break;
		     case SW_OR_KEY_NOT_PRESSED:
		     break;
		     case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		     case WARN_CUR_DATA_ID_DEV_DISABLED:
			    appl_status_flag = ERR_SW_IS_DISABLED;
	            Error_or_Warning_Proc("14.03.06", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
		    // break;
             default:
               	appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.03.07", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		       return SUCCESS;
	       }
		   if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
	       {
		       appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	           Error_or_Warning_Proc("14.03.08", ERROR_OCCURED, appl_status_flag);
	           #ifdef TRACE_FLOW 
	              Print("STARTUP -> ABNORMAL_EVENT \r");
			   #endif	  
			   #ifdef TRACE_ERROR 	  
		          Print("ERR: event - disp status \r");
	           #endif
			  cur_elevator_ptr->elevator_status = ERR_DISP_STATUS;
	          cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			  return SUCCESS;
	       }
	    }
	}
	if((proc_bit_field & (1 << 0)))
	{
		if(up_cur_floor == TOP_MOST_FLOOR_ID)
		{
			if((proc_bit_field & (1 << 1)))
			{
				appl_status_flag = ERR_FLOOR_INVALID;
			    Error_or_Warning_Proc("14.03.09", ERROR_OCCURED, appl_status_flag);
	            #ifdef TRACE_FLOW
	              Print("STARTUP -> ABNORMAL_EVENT \r");
				#endif 
				 #ifdef TRACE_ERROR  
		          Print("ERR: event - in top floor, down limit sw detect\r");
	            #endif
			   cur_elevator_ptr->elevator_status = ERR_TOP_FLOOR_DETECT_LIMIT_SW_DOWN;
	           cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			   return SUCCESS;
			}
			proc_bit_field |= (1 << 2);
		}
		else
		{
			proc_bit_field |= (1 << 3);
		}		
	}
    if((proc_bit_field & (1 << 1)))
	{
	  	if(down_cur_floor == FLOOR_00)
	    {
			if((proc_bit_field & (1 << 0)))
			{				
				appl_status_flag = ERR_FLOOR_INVALID;
			    Error_or_Warning_Proc("14.03.10", ERROR_OCCURED, appl_status_flag);
	            #ifdef TRACE_FLOW 
	              Print("STARTUP -> ABNORMAL_EVENT \r");
				#endif 
				#ifdef TRACE_ERROR   
		          Print("ERR: event - in bottom floor, up limit sw detect\r");
	            #endif
			   cur_elevator_ptr->elevator_status = ERR_BOTTOM_FLOOR_DETECT_LIMIT_SW_UP;
	           cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			   return SUCCESS;
			}
			proc_bit_field |= (1 << 4);
	    }
		else
		{
			proc_bit_field |= (1 << 3);
		}
	}
	if((proc_bit_field & (1 << 2)) || (proc_bit_field & (1 << 3)) || (proc_bit_field & (1 << 4)))
	{
		
		if(!(((up_cur_floor != FLOOR_ID_INVALID && down_cur_floor != FLOOR_ID_INVALID) && ( up_cur_floor == down_cur_floor || up_cur_floor + 1 == down_cur_floor))
			|| (up_cur_floor == FLOOR_ID_INVALID && (down_cur_floor == FLOOR_01 || down_cur_floor == FLOOR_00)) 
		    || (down_cur_floor == FLOOR_ID_INVALID && (up_cur_floor == TOP_MOST_FLOOR_ID - 1  || up_cur_floor == TOP_MOST_FLOOR_ID))))
		{
			appl_status_flag = ERR_FLOOR_INVALID;
			Error_or_Warning_Proc("14.03.11", ERROR_OCCURED, appl_status_flag);
	        #ifdef TRACE_FLOW
	            Print("STARTUP -> ABNORMAL_EVENT \r");
			#endif 
			#ifdef TRACE_ERROR   	
		        Print("ERR: event - limit sw up and down floor invalid\r");
	        #endif
			cur_elevator_ptr->elevator_status = ERR_LIMIT_SW_UP_AND_DOWN_FLOOR_INVALID;
	        cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			return SUCCESS;	
		}
	}
	if((proc_bit_field & (1 << 3)))
	{
		 oper_min = UP_SW_PROC;
		 oper_max = DOWN_SW_PROC;
	}
	else
	{
	   if((proc_bit_field & (1 << 2)))
	   {
		   oper_min = UP_SW_PROC;
		   oper_max = UP_SW_PROC;
	   }
	   else
       {
          if((proc_bit_field & (1 << 4)))
	      {
		      oper_min = DOWN_SW_PROC;
		      oper_max = DOWN_SW_PROC; 
	      }
       }
	}	   
	for(oper = oper_min; oper <= oper_max; ++oper)
	{
	    switch(oper)
	    {
	       case UP_SW_PROC:
			  limit_sw_min_floor = FLOOR_01;
			  limit_sw_max_floor = TOP_MOST_FLOOR_ID;
			  limit_sw_arr_ptr = limit_sw_up_arr;
		   break;
		   case DOWN_SW_PROC:
			  limit_sw_min_floor = FLOOR_00;
			  limit_sw_max_floor = TOP_MOST_FLOOR_ID - 1;
			  limit_sw_arr_ptr = limit_sw_down_arr;
		   break;
	    }
		for(limit_sw_cur_floor = limit_sw_min_floor; limit_sw_cur_floor <= limit_sw_max_floor; ++limit_sw_cur_floor)
	    {	
             switch(oper)
			 {
				 case UP_SW_PROC:
				    if(up_cur_floor == limit_sw_cur_floor)
					{
						continue;
					}
				 break;
				 case DOWN_SW_PROC:
				    if(down_cur_floor == limit_sw_cur_floor)
					{
						continue;
					}
				 break;
			 }				 
	         if((SW_Present_State_By_IO_Ch(limit_sw_arr_ptr[limit_sw_cur_floor], &is_pressed_limit_sw)) != SUCCESS)
			 {
				 appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	             Error_or_Warning_Proc("14.03.12", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
			 }
	         switch(is_pressed_limit_sw)
	         {
	            case STATE_YES:
				   appl_status_flag = ERR_FLOOR_INVALID;
			       Error_or_Warning_Proc("14.03.13", ERROR_OCCURED, appl_status_flag);
	               #ifdef TRACE_FLOW 
	                 Print("STARTUP -> ABNORMAL_EVENT \r");
					#endif 
				    #ifdef TRACE_ERROR    
		             Print("ERR: event - limit sw up or down floor > 1 \r");
	              #endif
			      cur_elevator_ptr->elevator_status = ERR_ATLEAST_TWO_LIMIT_SW_UP_OR_DOWN_FLOOR;
	              cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			      return SUCCESS;
				//break;
				case STATE_NO:
				break;
				default:
				   appl_status_flag = ERR_FORMAT_INVALID;
	               Error_or_Warning_Proc("14.03.14", ERROR_OCCURED, appl_status_flag);
	               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		           return SUCCESS;
	         }
		    if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
	        {
		       appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	           Error_or_Warning_Proc("14.03.15", ERROR_OCCURED, appl_status_flag);
	            #ifdef TRACE_FLOW
	              Print("STARTUP -> ABNORMAL_EVENT \r");
				#endif 
				#ifdef TRACE_ERROR     
		          Print("ERR: event - disp status \r");
	            #endif
			  cur_elevator_ptr->elevator_status = ERR_DISP_STATUS;
	          cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
			  return SUCCESS;
	        }
	    }
	}
	if((proc_bit_field & (1 << 2)) || (proc_bit_field & (1 << 3)) || (proc_bit_field & (1 << 4)))
	{
	    if((Timer_Stop(CH_ID_01)) != SUCCESS)
	    {
	        appl_status_flag = ERR_TIMER_STOP_PROC;
	        Error_or_Warning_Proc("14.03.16", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	        return SUCCESS; 
	    }
		if((SW_Oper(USER_BLOCKS_DOOR_SENSOR_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		{
		    appl_status_flag = ERR_DEV_ENABLE_PROC;
	        Error_or_Warning_Proc("14.03.17", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		}
		if((SW_Oper(MANUAL_DOOR_OPEN_SW_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		{
		    appl_status_flag = ERR_DEV_ENABLE_PROC;
	        Error_or_Warning_Proc("14.03.19", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		}	
		if((SW_Oper(MANUAL_DOOR_CLOSE_SW_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		{
		  	 appl_status_flag = ERR_DEV_ENABLE_PROC;
	         Error_or_Warning_Proc("14.03.20", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
		}
        if((SW_Oper(LIMIT_SW_DOOR_OPEN_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		{
		   	 appl_status_flag = ERR_DEV_ENABLE_PROC;
	         Error_or_Warning_Proc("14.03.21", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
		}	
		if((SW_Oper(LIMIT_SW_DOOR_CLOSE_IO_CH, DEV_ENABLE_OPER)) != SUCCESS)
		{
		  	 appl_status_flag = ERR_DEV_ENABLE_PROC;
	         Error_or_Warning_Proc("14.03.22", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
		}
        if((SW_Present_State_By_IO_Ch(DOORS_ALIGNED_SW_IO_CH, &is_sw_pressed_state_align)) != SUCCESS)
	    {
	       	appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	        Error_or_Warning_Proc("14.03.23", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;   
	    }
		if((SW_Present_State_By_IO_Ch(LIMIT_SW_DOOR_CLOSE_IO_CH, &is_sw_pressed_state_close)) != SUCCESS)
	    {
	      	appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	        Error_or_Warning_Proc("14.03.24", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	    }
		if((SW_Present_State_By_IO_Ch(LIMIT_SW_DOOR_OPEN_IO_CH, &is_sw_pressed_state_open)) != SUCCESS)
	    {
	       	appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	        Error_or_Warning_Proc("14.03.25", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
	    }
        if(is_sw_pressed_state_close == STATE_YES )
	    {	
	        if(is_sw_pressed_state_open == STATE_YES)
		    {
			   #ifdef TRACE_FLOW
	               Print("STARTUP -> ABNORMAL_EVENT \r");
				#endif 
				#ifdef TRACE_ERROR      
	               Print("ERR: Event - startup door open and close are active \r");
	           #endif
			   appl_status_flag = ERR_STARTUP_OPER;
	           Error_or_Warning_Proc("14.03.26", ERROR_OCCURED, appl_status_flag);
	           cur_elevator_ptr->elevator_status = ERR_STARTUP_DOOR_OPEN_AND_CLOSE_ACTIVE;
	           cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		       return SUCCESS;
		    }			   
		}
	    if(up_cur_floor == FLOOR_ID_INVALID && down_cur_floor == FLOOR_00)
	    {
	       cur_elevator_ptr->cur_floor = down_cur_floor;
	    }
	    else
	    {
	    	if(down_cur_floor == FLOOR_ID_INVALID && up_cur_floor == TOP_MOST_FLOOR_ID )
		    {
			   cur_elevator_ptr->cur_floor = up_cur_floor;
		    }
		    else 
		    {
			   if(down_cur_floor != FLOOR_ID_INVALID && up_cur_floor == down_cur_floor)
			   {
			    	cur_elevator_ptr->cur_floor = up_cur_floor;
			   }
			   else
			   {
				   //elevator car lies in between 2 floors
				   #ifdef TRACE_INFO
				      Print("WARN: Startup - Car lies in between floors \r");
				   #endif
				   if(up_cur_floor != FLOOR_ID_INVALID && down_cur_floor != FLOOR_ID_INVALID && up_cur_floor + 1 == down_cur_floor)
				   {
					   #if defined  STARTUP_CAR_IN_BETWEEN_FLOORS == MOVE_UP
					    	cur_elevator_ptr->cur_floor = up_cur_floor;                         
                            cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_UP;	
                            cur_elevator_ptr->elevator_status = MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP;	
                            #ifdef TRACE_INFO
							   uint32_temp_disp_data = down_cur_floor;
							   Print("INFO: Startup - Move up to floor : %u \r", uint32_temp_disp_data);
                            #endif  							
					   #else
					    	cur_elevator_ptr->cur_floor = down_cur_floor;
						    cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_DOWN;		
						    cur_elevator_ptr->elevator_status = MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN;
							#ifdef TRACE_INFO
							   uint32_temp_disp_data = down_cur_floor - 1;
							   Print("INFO: Startup - Move down to floor : %u\r", uint32_temp_disp_data);
                            #endif 
					   #endif
				    }
				    else
				    {
					   if(up_cur_floor == FLOOR_ID_INVALID && down_cur_floor == FLOOR_01)
					   {
						   #if defined STARTUP_CAR_IN_BETWEEN_FLOORS == MOVE_UP
						  	   //trigger to move up
							   cur_elevator_ptr->cur_floor = down_cur_floor - 1;
							   cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_UP;	
							   cur_elevator_ptr->elevator_status = MOVE_UP_NO_CUR_UP_TO_NEXT_UP;
							   #ifdef TRACE_INFO
							      uint32_temp_disp_data = down_cur_floor;
							      Print("INFO: Startup - Move up to floor : %u\r", uint32_temp_disp_data);
                               #endif
					       #else							   
						       //trigger to move down
							   cur_elevator_ptr->cur_floor = down_cur_floor;
						       cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_DOWN;
                               cur_elevator_ptr->elevator_status = MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN;
                               #ifdef TRACE_INFO
							      uint32_temp_disp_data = down_cur_floor - 1;
							      Print("INFO: Startup - Move down to floor : %u\r", uint32_temp_disp_data);
                               #endif							   
					       #endif
					    }
					    else
					    {
						   if((down_cur_floor == FLOOR_ID_INVALID && up_cur_floor == TOP_MOST_FLOOR_ID - 1 ))
						   {
							    #if defined STARTUP_CAR_IN_BETWEEN_FLOORS == MOVE_UP
						  	       //trigger to move up	
								    cur_elevator_ptr->cur_floor = up_cur_floor;
							    	cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_UP;	
								    cur_elevator_ptr->elevator_status = MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP;
									#ifdef TRACE_INFO
							            uint32_temp_disp_data = up_cur_floor + 1;
							            Print("INFO: Startup - Move up to floor : %u\r", uint32_temp_disp_data);
                                    #endif
					           #else
						          //trigger to move down
								  cur_elevator_ptr->cur_floor = up_cur_floor + 1;
							      cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_DOWN;
							      cur_elevator_ptr->elevator_status = MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN;
								  #ifdef TRACE_INFO
							         uint32_temp_disp_data = up_cur_floor ;
							         Print("INFO: Startup - Move down to floor : %u\r", uint32_temp_disp_data);
                                  #endif
					           #endif
						   }
					   }
				   }
			    }
		   }
	    }
	    #ifdef TRACE_INFO
	       uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
	       Print("INFO: Considered startup car floor : %u \r", uint32_temp_disp_data );				 			 
        #endif	
	    switch(cur_elevator_ptr->cur_fsm_state)
	    {	
           case FSM_STARTUP:	
	            if(is_sw_pressed_state_align == STATE_YES)
	            { 			        
			       if(is_sw_pressed_state_open == STATE_YES)
			       {
					   #ifdef TRACE_FLOW
					      Print("STARTUP -> PREPARE_USER_ENTRY_AND_EXIT \r");	
						#endif
					    cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_USER_ENTRY_AND_EXIT;
				   }
                   else
                   {
					   if(is_sw_pressed_state_close == STATE_YES)
			           {
					         #ifdef TRACE_FLOW
					           Print("STARTUP -> TRIGGER_DOOR_OPEN \r");	
						     #endif
					         cur_elevator_ptr->cur_fsm_state = FSM_TRIGGER_DOOR_OPEN;
				       }
                       else
                       {	
				           if((IO_Channel_Write(CAR_DOOR_OPEN_IO_CH, STATE_HIGH)) != SUCCESS)
	                       {
	                        	appl_status_flag = ERR_IO_CH_WRITE;
	                            Error_or_Warning_Proc("14.03.27", ERROR_OCCURED, appl_status_flag);
	                            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                        return SUCCESS;
	                       }
	                       if((IO_Channel_Write(CAR_DOOR_CLOSE_IO_CH, STATE_LOW)) != SUCCESS)
	                       {
	                        	appl_status_flag = ERR_IO_CH_WRITE;
	                            Error_or_Warning_Proc("14.03.28", ERROR_OCCURED, appl_status_flag);
	                            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                        return SUCCESS;
	                       } 
					       if((Timer_Run(CH_ID_01, TIMER_ID_DOOR_OPENED_MOTOR)) != SUCCESS)
	                       {
		                      appl_status_flag = ERR_TIMER_RUN_PROC;
	                          Error_or_Warning_Proc("11.03.29", ERROR_OCCURED, appl_status_flag);
	                          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                      return SUCCESS;
	                       }
		                  #ifdef TRACE_FLOW
	                        Print("STARTUP -> WAIT_TILL_DOOR_OPENED \r");
						  #endif
						  #ifdef TRACE_INFO
						     Print("INFO: Car Door opening ...\r");
						  #endif
                          #ifdef TRACE_REQ						 
	                        Print("REQ: Door open limit SW to active \r");
	                      #endif
	                      cur_elevator_ptr->cur_fsm_state = FSM_WAIT_TILL_DOOR_OPENED;	 
					   }
                   }	
				}
				else
	            { 
			       if(is_sw_pressed_state_open == STATE_YES)
			       {
					    #ifdef TRACE_FLOW
	                      Print("STARTUP -> ABNORMAL_EVENT \r");
						#endif 
				        #ifdef TRACE_ERROR     
	                      Print("ERR: Event - doors unaligned and opened \r");
	                   #endif
					   appl_status_flag = ERR_STARTUP_OPER;
	                   Error_or_Warning_Proc("14.03.30", ERROR_OCCURED, appl_status_flag);
	                   cur_elevator_ptr->elevator_status = ERR_DOORS_UNALIGNED_BUT_OPENED;
	                   cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		               return SUCCESS; 
				   }
                   else
                   {
					    #ifdef TRACE_FLOW
					      Print("STARTUP -> PREPARE_DOORS_TO_ALIGN \r");	
						#endif
					   cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_DOORS_TO_ALIGN;
                   } 
			   }				
	       break;
           case FSM_TRIGGER_MOVE_UP:
		   case FSM_TRIGGER_MOVE_DOWN:
		       if(is_sw_pressed_state_align == STATE_YES)
	           {
				    #ifdef TRACE_FLOW
	                   Print("STARTUP -> ABNORMAL_EVENT \r");
					#endif 
				    #ifdef TRACE_ERROR      
	                   Print("ERR: Event - doors aligned, but car lied in between floors\r");
	               #endif
				   appl_status_flag = ERR_STARTUP_OPER;
	               Error_or_Warning_Proc("14.03.31", ERROR_OCCURED, appl_status_flag);
	               cur_elevator_ptr->elevator_status = ERR_DOORS_ALIGNED_BUT_CAR_IN_BETWEEN_FLOORS;
	               cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		           return SUCCESS; 			   
			   }
               if(is_sw_pressed_state_open == STATE_YES)
			   {
				    #ifdef TRACE_FLOW
	                   Print("STARTUP -> ABNORMAL_EVENT \r");
					#endif 
				    #ifdef TRACE_ERROR      
	                   Print("ERR: Event - door opened, but car lied in between floors\r");
	               #endif
				   appl_status_flag = ERR_STARTUP_OPER;
	               Error_or_Warning_Proc("14.03.32", ERROR_OCCURED, appl_status_flag);
	               cur_elevator_ptr->elevator_status = ERR_DOOR_OPENED_BUT_CAR_IN_BETWEEN_FLOORS;
	               cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		           return SUCCESS;  
			   }
			   if(is_sw_pressed_state_close == STATE_NO)
			   {
				   #ifdef TRACE_FLOW
	                   Print("STARTUP -> ABNORMAL_EVENT \r");
					#endif 
				    #ifdef TRACE_ERROR      
	                   Print("ERR: Event - door not closed, but car lied in between floors\r");
	               #endif
				   appl_status_flag = ERR_STARTUP_OPER;
	               Error_or_Warning_Proc("14.03.33", ERROR_OCCURED, appl_status_flag);
	               cur_elevator_ptr->elevator_status = ERR_DOOR_NOT_CLOSED_BUT_CAR_IN_BETWEEN_FLOORS;
	               cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		           return SUCCESS;  
			   }
		   break;
	    }
		#ifdef TRACE_FLOW
		    switch(cur_elevator_ptr->cur_fsm_state)
	        {	
		       case FSM_TRIGGER_MOVE_UP:	         
	             Print("STARTUP -> TRIGGER_MOVE_UP \r");		      		 
               break;
               case FSM_TRIGGER_MOVE_DOWN:	          
	             Print("STARTUP -> TRIGGER_MOVE_DOWN \r");		     
               break;
		    }		   
		#endif
		cur_elevator_ptr->before_fsm_state = FSM_STARTUP;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Decide_Car_Move_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.04 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Decide_Car_Move_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;	
	uint16_t ret_status;
	uint8_t car_movement_state;
	
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.04.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("DECIDE_CAR_MOVEMENT -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.04.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS; 
	}
  if((Car_Movement_Direction(ctrl_elevator_ch_id, cur_elevator_ptr->cur_floor, &car_movement_state)) != SUCCESS)
	{
	    appl_status_flag = ERR_CAR_MOVE_STATE_PROC;
	    Error_or_Warning_Proc("14.04.03", ERROR_OCCURED, appl_status_flag);
	    #ifdef TRACE_FLOW 
	       Print("DECIDE_CAR_MOVEMENT -> ABNORMAL_EVENT \r");
		#endif 
		#ifdef TRACE_ERROR      
		   Print("ERR: event - Car movement proc \r");
	    #endif
		cur_elevator_ptr->elevator_status = ERR_CAR_MOVEMENT_PROC;
	    cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		return SUCCESS;	 
	}
	switch(car_movement_state)
	{
		   case STARTUP_STATIONARY:
		       #ifdef TRACE_FLOW
	              Print("DECIDE_CAR_MOVEMENT -> USER_ENTRY_AND_EXIT \r");	                  
	           #endif			   
			   cur_elevator_ptr->cur_fsm_state =  FSM_USER_ENTRY_AND_EXIT;			 
		   break;
		   case MOVED_DOWN_STATIONARY:
		   case MOVED_UP_STATIONARY:
		       #ifdef TRACE_FLOW
	              Print("DECIDE_CAR_MOVEMENT -> TRIGGER_CAR_STOP \r");	                  
	           #endif
		       cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_CAR_STOP;			  
		   break;
           case MOVE_UP:
		       #ifdef TRACE_FLOW
			      Print("DECIDE_CAR_MOVEMENT -> TRIGGER_MOVE_UP\r");                  
               #endif			   
			   #ifdef TRACE_INFO
			     uint32_temp_disp_data = cur_elevator_ptr->cur_floor; 
				 Print("INFO: current floor: %u & ", uint32_temp_disp_data);	
                 uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor; 
                 Print("next floor stop: %u\r", uint32_temp_disp_data);	 
	           #endif 
               cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_UP;			  
           break;
           case MOVE_DOWN:
		       #ifdef TRACE_FLOW
			     Print("DECIDE_CAR_MOVEMENT -> TRIGGER_MOVE_DOWN \r" );
               #endif			   
                #ifdef TRACE_INFO
			     uint32_temp_disp_data = cur_elevator_ptr->cur_floor; 
				 Print("INFO: current floor: %u & ", uint32_temp_disp_data);	
                 uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor; 
                 Print("next floor stop: %u\r", uint32_temp_disp_data);	 
	           #endif 	          
               cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_MOVE_DOWN;			  
           break;
		   default:
              appl_status_flag = ERR_FORMAT_INVALID;
	          Error_or_Warning_Proc("14.04.04", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS;	
	 }
	 cur_elevator_ptr->elevator_status = car_movement_state;	
     return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Trigger_Move_Up_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.05 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Trigger_Move_Up_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.05.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("TRIGGER_MOVE_UP -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.05.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}

	if((Check_Move_Condition(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		  appl_status_flag = ERR_CHECK_MOVE;
	      Error_or_Warning_Proc("14.05.03", ERROR_OCCURED, appl_status_flag);
		 cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;		 
		 return SUCCESS; 
	}
	
       if((IO_Channel_Write(CAR_DOWN_IO_CH, STATE_LOW)) != SUCCESS)
	   {
		  appl_status_flag = ERR_IO_CH_WRITE;
	      Error_or_Warning_Proc("14.05.04", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;	
	   }
	   if((IO_Channel_Write(CAR_UP_IO_CH, STATE_HIGH)) != SUCCESS)
	   {
	      appl_status_flag = ERR_IO_CH_WRITE;
	      Error_or_Warning_Proc("14.05.05", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS; 
	   }
        #ifdef TRACE_INFO
	     Print("INFO: Car moving up ... \r");
	   #endif   
	   #ifdef TRACE_FLOW
	     Print("TRIGGER_MOVE_UP -> PREPARE_TO_MOVE \r");
	   #endif
	   cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_TO_MOVE;		   
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Trigger_Move_Down_Proc  

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.06 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Trigger_Move_Down_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
		
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.06.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("TRIGGER_MOVE_DOWN -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.06.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((Check_Move_Condition(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		  appl_status_flag = ERR_CHECK_MOVE;
	      Error_or_Warning_Proc("14.06.03", ERROR_OCCURED, appl_status_flag);
		  cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		  return SUCCESS; 
	}
	   if((IO_Channel_Write(CAR_UP_IO_CH, STATE_LOW)) != SUCCESS)
	   {
		   appl_status_flag = ERR_IO_CH_WRITE;
	       Error_or_Warning_Proc("14.06.04", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	   }
	   if((IO_Channel_Write(CAR_DOWN_IO_CH, STATE_HIGH)) != SUCCESS)
	   {
		   appl_status_flag = ERR_IO_CH_WRITE;
	       Error_or_Warning_Proc("14.06.05", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	   }
       	#ifdef TRACE_INFO
	     Print("INFO: Car moving down ... \r");
	   #endif    
	   #ifdef TRACE_FLOW
	      Print("TRIGGER_MOVE_DOWN -> PREPARE_TO_MOVE \r");
	   #endif
	   cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_TO_MOVE;		   
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Prepare_To_Move_Proc  

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.07 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Prepare_To_Move_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	 
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.07.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("PREPARE_TO_MOVE -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.07.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;	
	}
	if(cur_elevator_ptr->before_fsm_state == FSM_PREPARE_USER_ENTRY_AND_EXIT)
	{
		if((Validate_Floor(cur_elevator_ptr->cur_floor)) != SUCCESS)
		{
			appl_status_flag = ERR_FLOOR_INVALID;
	        Error_or_Warning_Proc("14.07.04", ERROR_OCCURED, appl_status_flag);
	        #ifdef TRACE_FLOW
	          Print("PREPARE_TO_MOVE -> ABNORMAL_EVENT \r");
			#endif 
			#ifdef TRACE_ERROR     
		      Print("ERR: event - Current floor invalid \r");
	       #endif
		   cur_elevator_ptr->elevator_status = ERR_CUR_FLOOR_INVALID;
	       cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		   return SUCCESS;
		}
		if(cur_elevator_ptr->cur_floor != TOP_MOST_FLOOR_ID)
		{
		   if((SW_Oper(hall_call_up_arr[cur_elevator_ptr->cur_floor], DEV_ENABLE_OPER)) != SUCCESS)
		   {
			  appl_status_flag = ERR_DEV_ENABLE_PROC;
	          Error_or_Warning_Proc("14.07.05", ERROR_OCCURED, appl_status_flag);
	          Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		      return SUCCESS; 
		   }
		}
		if(cur_elevator_ptr->cur_floor != FLOOR_00)
		{
		   if((SW_Oper(hall_call_down_arr[cur_elevator_ptr->cur_floor], DEV_ENABLE_OPER)) != SUCCESS)
		   {
		    	appl_status_flag = ERR_DEV_ENABLE_PROC;
	            Error_or_Warning_Proc("14.07.06", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS; 
		   }
		}
		if((SW_Oper(in_car_call_arr[cur_elevator_ptr->cur_floor], DEV_ENABLE_OPER)) != SUCCESS)
		{
			appl_status_flag = ERR_DEV_ENABLE_PROC;
	        Error_or_Warning_Proc("14.07.07", ERROR_OCCURED, appl_status_flag);
	        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		    return SUCCESS;
		}
	}
	if((Timer_Run(CH_ID_01, TIMER_ID_NEXT_FLOOR)) != SUCCESS)
	{
		appl_status_flag = ERR_TIMER_RUN_PROC;
	    Error_or_Warning_Proc("14.07.08", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS; 
	}
	#ifdef TRACE_FLOW
	  Print("PREPARE_TO_MOVE -> MOVING \r");
	#endif
	cur_elevator_ptr->cur_fsm_state = FSM_MOVING;
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
