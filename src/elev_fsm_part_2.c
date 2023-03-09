/* ********************************************************************
FILE                   : elev_fsm_part_2.c

PURPOSE                : Elevator FSM Proc part 2
	 
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
FUNCTION NAME  : FSM_Car_Moving_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.08 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Car_Moving_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	uint8_t car_movement_state, cur_floor, is_sw_pressed_state_floor, oper, limit_sw_min_floor, limit_sw_max_floor;
	static uint8_t limit_sw_cur_floor, previous_floor, proc_bit_field = 0, limit_sw_up_cur_floor, limit_sw_down_cur_floor, 
	   limit_sw_move_cur_floor, pressed_limit_sw_floor, *limit_sw_arr_ptr, *pressed_limit_sw_arr_ptr ;
  
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.08.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("MOVING -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.08.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
    if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
		   Error_or_Warning_Proc("14.08.03", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW 
	          Print("PREPARE_TO_MOVE -> ABNORMAL_EVENT \r");
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
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_NEXT_FLOOR)
			{
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.08.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}			
		    #ifdef TRACE_FLOW
		      Print("MOVING -> ABNORMAL_EVENT \r");			 
		    #endif
			switch(cur_elevator_ptr->elevator_status)
			{
				case MOVE_UP:
				case MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN:
				case MOVE_UP_CUR_UP_TO_NEXT_DOWN:
				case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
				case MOVE_UP_NO_CUR_UP_TO_NEXT_UP:
				case MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN:
				case MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP:
				   if(previous_floor != FLOOR_00)
				   {
				      if((SW_Present_State_By_IO_Ch(limit_sw_up_arr[previous_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		              {
			             appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                     Error_or_Warning_Proc("14.08.05", ERROR_OCCURED, appl_status_flag);
	                     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                 return SUCCESS;
		              }
		              switch(is_sw_pressed_state_floor)
		              {
				         case STATE_YES:
					        #ifdef TRACE_ERROR
				               Print("ERR: Event - move up but unmoved \r");
				            #endif
            		        cur_elevator_ptr->elevator_status = ERR_MOVE_UP_BUT_UNMOVED; 
				         break;		 
				         case STATE_NO:
					        cur_elevator_ptr->elevator_status = ERR_MOVING_UP_NEXT_FLOOR_NOT_DETECT;
					        for(cur_floor = FLOOR_01; cur_floor < MAX_NUM_FLOORS; ++cur_floor)
						    {
							   if(cur_floor == previous_floor || cur_floor == limit_sw_cur_floor)
							   {
								   continue;
							   }
					           if((SW_Present_State_By_IO_Ch(limit_sw_up_arr[cur_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		                       {
			                       appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                               Error_or_Warning_Proc("14.08.06", ERROR_OCCURED, appl_status_flag);
	                               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                           return SUCCESS;
		                       }
							   switch(is_sw_pressed_state_floor)
							   {
								   case STATE_YES:
								     cur_elevator_ptr->elevator_status = ERR_MOVE_UP_NEXT_FLOOR_INVALID; 
								   break;
								   case STATE_NO:
								   break;
								   default:
				                      appl_status_flag = ERR_FORMAT_INVALID;
	                                  Error_or_Warning_Proc("14.08.07", ERROR_OCCURED, appl_status_flag);
	                                  Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                              return SUCCESS;
							   }
							   if(cur_elevator_ptr->elevator_status == ERR_MOVE_UP_NEXT_FLOOR_INVALID)
							   {
								    break;
							   }
						    }
							switch(cur_elevator_ptr->elevator_status)
							{
								case ERR_MOVE_UP_NEXT_FLOOR_INVALID:
								   #ifdef TRACE_ERROR
				                      Print("ERR: Event - moving up next floor invalid \r");
				                   #endif
								break;
                                case ERR_MOVING_UP_NEXT_FLOOR_NOT_DETECT:								
						           #ifdef TRACE_ERROR
				                      Print("ERR: Event - moving up next floor not detected \r");
				                   #endif
								break;
							}								
					     break;
					     default:
				            appl_status_flag = ERR_FORMAT_INVALID;
	                        Error_or_Warning_Proc("14.08.08", ERROR_OCCURED, appl_status_flag);
	                        Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                    return SUCCESS;
				      }	
				   }
                   else
				   {
					  if((SW_Present_State_By_IO_Ch(limit_sw_down_arr[previous_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		              {
			             appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                     Error_or_Warning_Proc("14.08.09", ERROR_OCCURED, appl_status_flag);
	                     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                 return SUCCESS;
		              }
					  switch(is_sw_pressed_state_floor)
					  {
					     case STATE_YES:
					        #ifdef TRACE_ERROR
				               Print("ERR: Event - move up but unmoved \r");
				            #endif
            		        cur_elevator_ptr->elevator_status = ERR_MOVE_UP_BUT_UNMOVED; 
				         break;		 
				         case STATE_NO:
						 break;
						 default:
						   appl_status_flag = ERR_FORMAT_INVALID;
	                       Error_or_Warning_Proc("14.08.10", ERROR_OCCURED, appl_status_flag);
	                       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                   return SUCCESS;
					  }
				   }					   
				break;   
				case MOVE_DOWN:
				case MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP:
				case MOVE_DOWN_CUR_DOWN_TO_NEXT_UP:
				case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:
                case MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN:
				case MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP:
                case MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN:				
				   if(previous_floor != TOP_MOST_FLOOR_ID)
				   {
				      if((SW_Present_State_By_IO_Ch(limit_sw_down_arr[previous_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		              {
			             appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                     Error_or_Warning_Proc("14.08.11", ERROR_OCCURED, appl_status_flag);
	                     Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                 return SUCCESS;
		              }
		              switch(is_sw_pressed_state_floor)
		              {
				         case STATE_YES:
					       #ifdef TRACE_ERROR
				             Print("ERR: Event - move down but unmoved \r");
				           #endif
            		       cur_elevator_ptr->elevator_status = ERR_MOVE_DOWN_BUT_UNMOVED; 
				         break;		 
				         case STATE_NO:
					       cur_elevator_ptr->elevator_status = ERR_MOVING_DOWN_NEXT_FLOOR_NOT_DETECT;
					       for(cur_floor = FLOOR_00; cur_floor < TOP_MOST_FLOOR_ID; ++cur_floor)
						   {
							  if(cur_floor == previous_floor || cur_floor == limit_sw_cur_floor)
							  {
							 	 continue;
							  }
					          if((SW_Present_State_By_IO_Ch(limit_sw_down_arr[cur_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		                      {
			                     appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                             Error_or_Warning_Proc("14.08.12", ERROR_OCCURED, appl_status_flag);
	                             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                         return SUCCESS;
		                      }
							  switch(is_sw_pressed_state_floor)
							  {
								case STATE_YES:
								  cur_elevator_ptr->elevator_status = ERR_MOVE_DOWN_NEXT_FLOOR_INVALID; 
								break;
								case STATE_NO:
								break;
								default:
				                   appl_status_flag = ERR_FORMAT_INVALID;
	                               Error_or_Warning_Proc("14.08.13", ERROR_OCCURED, appl_status_flag);
	                               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                           return SUCCESS;
							  }
							  if(cur_elevator_ptr->elevator_status == ERR_MOVE_DOWN_NEXT_FLOOR_INVALID)
							  {
								   break;
							  }
						   }
						   switch(cur_elevator_ptr->elevator_status)
						   {
							   case ERR_MOVE_DOWN_NEXT_FLOOR_INVALID:
							      #ifdef TRACE_ERROR
				                    Print("ERR: Event - moving down next floor invalid \r");
				                  #endif
							   break;
                               case ERR_MOVING_DOWN_NEXT_FLOOR_NOT_DETECT:
							      #ifdef TRACE_ERROR
				                     Print("ERR: Event - moving down next floor not detected \r");
				                  #endif
							  break;
						   }							  
					    break;
					    default:
				           appl_status_flag = ERR_FORMAT_INVALID;
	                       Error_or_Warning_Proc("14.08.14", ERROR_OCCURED, appl_status_flag);
	                       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                   return SUCCESS;
					  }
				   }
				   else
				   {
					   if((SW_Present_State_By_IO_Ch(limit_sw_up_arr[previous_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		               {
			              appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                      Error_or_Warning_Proc("14.08.15", ERROR_OCCURED, appl_status_flag);
	                      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                  return SUCCESS;
		               }
					   switch(is_sw_pressed_state_floor)
					   {
					     case STATE_YES:
					        #ifdef TRACE_ERROR
				               Print("ERR: Event - move down but unmoved \r");
				            #endif
            		        cur_elevator_ptr->elevator_status = ERR_MOVE_DOWN_BUT_UNMOVED; 
				         break;		 
				         case STATE_NO:
						 break;
						 default:
						   appl_status_flag = ERR_FORMAT_INVALID;
	                       Error_or_Warning_Proc("14.08.16", ERROR_OCCURED, appl_status_flag);
	                       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                   return SUCCESS;
					   }
				   }
				break;
				default:
				   appl_status_flag = ERR_FORMAT_INVALID;
	               Error_or_Warning_Proc("14.08.17", ERROR_OCCURED, appl_status_flag);
	               Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		           return SUCCESS;
			}
			#ifdef TRACE_ERROR
			   Print("ERR: Event - next floor not detect \r");
			#endif
         	appl_status_flag = ERR_NOT_DETECT;		
	        Error_or_Warning_Proc("14.08.18", ERROR_OCCURED, appl_status_flag);
            cur_elevator_ptr->elevator_status = ERR_NEXT_FLOOR_NOT_DETECT; 			
		    cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;  
            return SUCCESS;			
		//break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.08.19", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS; 
	}
	if((Check_Move_Condition(ctrl_elevator_ch_id, cur_elevator_ptr->cur_fsm_state)) != SUCCESS)
	{
		 appl_status_flag = ERR_CHECK_MOVE;
	     Error_or_Warning_Proc("14.08.20", ERROR_OCCURED, appl_status_flag);	
		 cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		 return SUCCESS; 
	}
	if((Disp_Elevator_Status(ctrl_elevator_ch_id)) != SUCCESS)
	{
		appl_status_flag = ERR_ELEVATOR_DISP_PROC;
	    Error_or_Warning_Proc("14.08.21", ERROR_OCCURED, appl_status_flag);
	    #ifdef TRACE_FLOW
	         Print("MOVING -> ABNORMAL_EVENT \r");
	    #endif		 
		#ifdef TRACE_ERROR 	 
	         Print("ERR: event - disp status process \r");
	    #endif
		cur_elevator_ptr->elevator_status = ERR_DISP_STATUS;
	    cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		return SUCCESS; 
	}
	if(((proc_bit_field & (1 << 0)) == 0))
	{
	    switch(cur_elevator_ptr->elevator_status)
	    {
	       case MOVE_UP:
		      if(cur_elevator_ptr->cur_floor == FLOOR_00)
			  {				  
				 limit_sw_up_cur_floor = FLOOR_ID_INVALID;				 
			  }
			  else
			  {
				 limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor;					  
			  }
			  limit_sw_cur_floor = cur_elevator_ptr->cur_floor + 1; 
		      previous_floor = cur_elevator_ptr->cur_floor;
			  limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor;			  	
			  limit_sw_arr_ptr = limit_sw_down_arr;
		      limit_sw_move_cur_floor = limit_sw_down_cur_floor;
		      #ifdef TRACE_REQ
		         uint32_temp_disp_data = limit_sw_move_cur_floor;
		         Print("REQ: limit sw down : %u to inactive \r", uint32_temp_disp_data);
		      #endif
	       break;
		   case MOVE_UP_CUR_UP_TO_NEXT_DOWN:
		      limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor;
              pressed_limit_sw_arr_ptr = limit_sw_up_arr;
              pressed_limit_sw_floor = limit_sw_up_cur_floor; 			  
           case MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN:
		       limit_sw_cur_floor = cur_elevator_ptr->cur_floor + 1; 
		       previous_floor = cur_elevator_ptr->cur_floor;
               limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor + 1;			   
			   limit_sw_arr_ptr =  limit_sw_down_arr; 
			   limit_sw_move_cur_floor = limit_sw_down_cur_floor;
               #ifdef TRACE_REQ
		          uint32_temp_disp_data = limit_sw_move_cur_floor;
		          Print("REQ: limit sw down : %u to active \r", uint32_temp_disp_data);
		      #endif			   
		   break;
		   case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
		       limit_sw_cur_floor = cur_elevator_ptr->cur_floor + 1; 
		       previous_floor = cur_elevator_ptr->cur_floor; 
		       limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor;
			   limit_sw_move_cur_floor = limit_sw_up_cur_floor;
			   limit_sw_arr_ptr =  limit_sw_up_arr; 
			   #ifdef TRACE_REQ
		          uint32_temp_disp_data = limit_sw_move_cur_floor;
		          Print("REQ: limit sw up : %u to inactive \r", uint32_temp_disp_data);
		      #endif 
		       limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor + 1;
		       pressed_limit_sw_arr_ptr = limit_sw_down_arr;
			   pressed_limit_sw_floor = limit_sw_down_cur_floor; 
		   break;
		   case MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN:
		       limit_sw_cur_floor = cur_elevator_ptr->cur_floor + 1; 
		       previous_floor = cur_elevator_ptr->cur_floor;
		       limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor;
			   limit_sw_move_cur_floor = limit_sw_up_cur_floor;
			   limit_sw_arr_ptr =  limit_sw_up_arr;
		   break;
		   case MOVE_UP_NO_CUR_UP_TO_NEXT_UP:
		       limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor + 1;
		       pressed_limit_sw_arr_ptr = limit_sw_down_arr;
			   pressed_limit_sw_floor = limit_sw_down_cur_floor; 
		   case MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP: 
               limit_sw_cur_floor = cur_elevator_ptr->cur_floor + 1; 
		       previous_floor = cur_elevator_ptr->cur_floor;		   
		       limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor + 1;
			   limit_sw_arr_ptr =  limit_sw_up_arr; 
			   limit_sw_move_cur_floor = limit_sw_up_cur_floor;
               #ifdef TRACE_REQ
		          uint32_temp_disp_data = limit_sw_move_cur_floor;
		          Print("REQ: limit sw up : %u to active \r", uint32_temp_disp_data);
		      #endif			   
		   break;
	       case MOVE_DOWN:
		      if(cur_elevator_ptr->cur_floor == TOP_MOST_FLOOR_ID)
			  {
				 limit_sw_down_cur_floor = FLOOR_ID_INVALID;
			  }
			  else
			  {
				  limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor;
			  }
			  limit_sw_cur_floor = cur_elevator_ptr->cur_floor - 1; 
		      previous_floor = cur_elevator_ptr->cur_floor;
			  limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor;
			  limit_sw_arr_ptr =  limit_sw_up_arr;
		      limit_sw_move_cur_floor = limit_sw_up_cur_floor;
		      #ifdef TRACE_REQ
		        uint32_temp_disp_data = limit_sw_move_cur_floor;
		        Print("REQ: limit sw up : %u to inactive \r", uint32_temp_disp_data);
		      #endif
	       break;
		   case MOVE_DOWN_CUR_DOWN_TO_NEXT_UP:
		      pressed_limit_sw_arr_ptr = limit_sw_down_arr;
			  limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor;
			  pressed_limit_sw_floor = limit_sw_down_cur_floor;			  
		   case MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP:
		      limit_sw_cur_floor = cur_elevator_ptr->cur_floor - 1; 
		      previous_floor = cur_elevator_ptr->cur_floor;
		      limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor - 1;			  
			  limit_sw_arr_ptr =  limit_sw_up_arr;
			  limit_sw_move_cur_floor = limit_sw_up_cur_floor;
			  #ifdef TRACE_REQ
		        uint32_temp_disp_data = limit_sw_move_cur_floor;
		        Print("REQ: limit sw up : %u to active \r", uint32_temp_disp_data);
		      #endif
		   break;
		   case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:
		        limit_sw_cur_floor = cur_elevator_ptr->cur_floor - 1; 
		        previous_floor = cur_elevator_ptr->cur_floor; 
		       limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor;
			   limit_sw_move_cur_floor = limit_sw_down_cur_floor;
			   limit_sw_arr_ptr =  limit_sw_down_arr; 
			   #ifdef TRACE_REQ
		          uint32_temp_disp_data = limit_sw_move_cur_floor;
		          Print("REQ: limit sw down : %u to inactive \r", uint32_temp_disp_data);
		      #endif 
		      limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor - 1;
		       pressed_limit_sw_arr_ptr = limit_sw_up_arr;
			   pressed_limit_sw_floor = limit_sw_up_cur_floor; 
		   break;
		   case MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP:
		      limit_sw_cur_floor = cur_elevator_ptr->cur_floor - 1; 
		      previous_floor = cur_elevator_ptr->cur_floor;
		      limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor;
			  limit_sw_move_cur_floor = limit_sw_down_cur_floor;
			  limit_sw_arr_ptr =  limit_sw_down_arr; 
		   break;
		   case MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN:
		      limit_sw_up_cur_floor = cur_elevator_ptr->cur_floor - 1;
		      pressed_limit_sw_arr_ptr = limit_sw_up_arr;
			  pressed_limit_sw_floor = limit_sw_up_cur_floor; 
		   case MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN:
              limit_sw_cur_floor = cur_elevator_ptr->cur_floor - 1; 
		      previous_floor = cur_elevator_ptr->cur_floor;		   
		      limit_sw_down_cur_floor = cur_elevator_ptr->cur_floor - 1;
			  limit_sw_arr_ptr =  limit_sw_down_arr;
		      limit_sw_move_cur_floor = limit_sw_down_cur_floor;
			  #ifdef TRACE_REQ
		        uint32_temp_disp_data = limit_sw_move_cur_floor;
		        Print("REQ: limit sw down : %u to active \r", uint32_temp_disp_data);
		      #endif			  
		   break;
	       default:
	         appl_status_flag = ERR_FORMAT_INVALID;
	         Error_or_Warning_Proc("14.08.22", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
	         return SUCCESS;
	    }
		proc_bit_field |= (1 << 0);
	}
	switch(cur_elevator_ptr->elevator_status)
	{
		case MOVE_UP:
		case MOVE_DOWN:
          ret_status = SW_Oper(limit_sw_arr_ptr[limit_sw_move_cur_floor], DEV_READ_OPER);
	      switch(ret_status)
	      {
	          case SUCCESS:	      
		      break;
		      case SW_OR_KEY_NOT_PRESSED:
		        proc_bit_field &= ~(1 << 0);
		        if(cur_elevator_ptr->elevator_status == MOVE_UP)
		        {
			        
					if(cur_elevator_ptr->cur_floor == FLOOR_00)
			        {
				     	  cur_elevator_ptr->elevator_status = MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN;
						  #ifdef TRACE_FLOW
						     Print("MOVE_UP -> MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN \r"); 
						  #endif
			        }                   	
                    else
					{
						if(cur_elevator_ptr->cur_floor + 1 == TOP_MOST_FLOOR_ID)
					    {
				           cur_elevator_ptr->elevator_status = MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP;
					       #ifdef TRACE_FLOW
						      Print("MOVE_UP -> MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP\r"); 
					       #endif
			            }
						else
						{
						    cur_elevator_ptr->elevator_status = MOVE_UP_CUR_UP_TO_NEXT_DOWN;
					        #ifdef TRACE_FLOW
						       Print("MOVE_UP -> MOVE_UP_CUR_UP_TO_NEXT_DOWN \r"); 
					        #endif
						}
					}						
		        }
                else
				{
					if(cur_elevator_ptr->cur_floor == TOP_MOST_FLOOR_ID)
					{
						cur_elevator_ptr->elevator_status = MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP;
						#ifdef TRACE_FLOW
						   Print("MOVE_DOWN -> MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP\r"); 
						#endif
					}
					else
					{
						if(cur_elevator_ptr->cur_floor - 1 == FLOOR_00)
					    {
				           cur_elevator_ptr->elevator_status = MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN;
					       #ifdef TRACE_FLOW
						      Print("MOVE_DOWN -> MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN\r"); 
					       #endif
			            }
                        else
					    {
					    	cur_elevator_ptr->elevator_status = MOVE_DOWN_CUR_DOWN_TO_NEXT_UP;
						    #ifdef TRACE_FLOW
						       Print("MOVE_DOWN -> MOVE_DOWN_CUR_DOWN_TO_NEXT_UP\r"); 
						    #endif
					    }
					}					
				}					
		      break;
		      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		      case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.23", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		      // break;
              default:
			     proc_bit_field &= ~(1 << 0);		       
				#ifdef TRACE_ERROR
				   uint32_temp_disp_data = limit_sw_arr_ptr[limit_sw_move_cur_floor];
				   Print("ERR: sw_io_ch: %u, ", uint32_temp_disp_data);
				   if(limit_sw_arr_ptr == limit_sw_down_arr)
				   {
					   Print("limit_down ");
				   }
                   else 
				   {
					   if(limit_sw_arr_ptr == limit_sw_up_arr)
					   {
						   Print("limit_up ");
					   }
				   }					   
				   uint32_temp_disp_data = limit_sw_move_cur_floor;
				   Print("floor: %u \r", uint32_temp_disp_data);
				#endif
				appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.08.24", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	      }
		break;
		case MOVE_UP_CUR_UP_TO_NEXT_DOWN:
		case MOVE_DOWN_CUR_DOWN_TO_NEXT_UP:		  
		  ret_status = SW_Oper(pressed_limit_sw_arr_ptr[pressed_limit_sw_floor], DEV_READ_OPER);
	      switch(ret_status)
	      {
	          case SUCCESS:	
		      break;
		      case SW_OR_KEY_NOT_PRESSED:
                 proc_bit_field &= ~(1 << 0);
				 #ifdef TRACE_ERROR
				      Print("MOVING -> ABNORMAL_EVENT \r");
					  if(pressed_limit_sw_arr_ptr == limit_sw_up_arr)
					  {
						  Print("ERR: Event - limit up ");
					  }
					  else
					  {
						   Print("ERR: Event - limit down ");
					  }
					  uint32_temp_disp_data = pressed_limit_sw_floor;
					  Print(": %u is not active \r", uint32_temp_disp_data);
				 #endif
		         appl_status_flag = ERR_SW_IS_INACTIVE;
	             Error_or_Warning_Proc("14.08.25", ERROR_OCCURED, appl_status_flag);				 
	             cur_elevator_ptr->elevator_status = ERR_CUR_LIMIT_SW_INACTIVE;	   
	             cur_elevator_ptr->cur_fsm_state =  FSM_ABNORMAL_EVENT;		  
			     return SUCCESS; 			  
		     // break;
		      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		      case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.26", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		      // break;
              default:
			     proc_bit_field &= ~(1 << 0);
		        appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.08.27", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	      }
		case MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN:		
		case MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP:
		    ret_status = SW_Oper(limit_sw_arr_ptr[limit_sw_move_cur_floor], DEV_READ_OPER);
	        switch(ret_status)
	        {
	             case SUCCESS:	 
                    proc_bit_field &= ~(1 << 0);
			    	switch(cur_elevator_ptr->elevator_status)
			     	{					
					   case MOVE_UP_CUR_UP_TO_NEXT_DOWN:
					      cur_elevator_ptr->elevator_status = MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP;
                          #ifdef TRACE_FLOW							    
						    Print("MOVE_UP_CUR_UP_TO_NEXT_DOWN -> MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP\r");
						  #endif
					   break;
					   case MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN:
					       cur_elevator_ptr->elevator_status = MOVE_UP_NO_CUR_UP_TO_NEXT_UP;
						    #ifdef TRACE_FLOW							    
								Print("MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN -> MOVE_UP_NO_CUR_UP_TO_NEXT_UP\r");
						    #endif
					   break;					   					
					   case MOVE_DOWN_CUR_DOWN_TO_NEXT_UP:
					        cur_elevator_ptr->elevator_status = MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN; 
							 #ifdef TRACE_FLOW							    
								Print("MOVE_DOWN_CUR_DOWN_TO_NEXT_UP -> MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN\r");
						     #endif 
					   break;	  
					   case MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP:
					       cur_elevator_ptr->elevator_status = MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN;
							  #ifdef TRACE_FLOW							    
								Print("MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP -> MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN\r");
						      #endif
					    break;
					}						
		          break;
		          case SW_OR_KEY_NOT_PRESSED:		        				
		          break;
		          case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		          case WARN_CUR_DATA_ID_DEV_DISABLED:
		              proc_bit_field &= ~(1 << 0);
		              appl_status_flag = ERR_SW_IS_DISABLED;
	                  Error_or_Warning_Proc("14.08.28", ERROR_OCCURED, appl_status_flag);
	                  Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		              return SUCCESS;
		         // break;
                default:
			         proc_bit_field &= ~(1 << 0);
		             appl_status_flag = ERR_SW_READ_PROC;
	                 Error_or_Warning_Proc("14.08.29", ERROR_OCCURED, appl_status_flag);
	                 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		             return SUCCESS;
	        }
		break;
		case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
		case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:
		   switch(cur_elevator_ptr->elevator_status)
		   {
			   case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
			      if(pressed_limit_sw_floor == TOP_MOST_FLOOR_ID)
				  {
					  #ifdef TRACE_FLOW
					     Print("MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP -> MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN\r");
                     #endif					 
					 cur_elevator_ptr->elevator_status = MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN; 
				  }
			   break;
			   case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:
			      if(pressed_limit_sw_floor == FLOOR_00)
				  {
					 #ifdef TRACE_FLOW
					     Print("MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN -> MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP\r");					    
                     #endif
					  cur_elevator_ptr->elevator_status = MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP;
				  }
			   break;
		   }
		   if(cur_elevator_ptr->elevator_status == MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN || cur_elevator_ptr->elevator_status == MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP)
		   {
			   proc_bit_field &= ~(1 << 0);
			   break;
		   }
		   ret_status = SW_Oper(pressed_limit_sw_arr_ptr[pressed_limit_sw_floor], DEV_READ_OPER);
	       switch(ret_status)
	       {
	          case SUCCESS:               		        					
		      break;
		      case SW_OR_KEY_NOT_PRESSED:
			     proc_bit_field &= ~(1 << 0);
				 #ifdef TRACE_ERROR
				      Print("MOVING -> ABNORMAL_EVENT \r");
					  if(pressed_limit_sw_arr_ptr == limit_sw_up_arr)
					  {
						  Print("ERR: Event - limit up ");
					  }
					  else
					  {
						   Print("ERR: Event - limit down ");
					  }
					  uint32_temp_disp_data = pressed_limit_sw_floor;
					  Print(": %u is not active\r", uint32_temp_disp_data);
				 #endif
                 appl_status_flag = ERR_SW_IS_INACTIVE;
	             Error_or_Warning_Proc("14.08.30", ERROR_OCCURED, appl_status_flag);
	             cur_elevator_ptr->elevator_status = ERR_CUR_LIMIT_SW_INACTIVE;	   
	             cur_elevator_ptr->cur_fsm_state =  FSM_ABNORMAL_EVENT;		  
			     return SUCCESS;			  
		      //break;
		      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		      case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.31", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		      // break;
              default:
			     proc_bit_field &= ~(1 << 0);
		        appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.08.32", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	       }
		  case MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN: 
          case MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP:
	   	    ret_status = SW_Oper(limit_sw_arr_ptr[limit_sw_move_cur_floor], DEV_READ_OPER);
	        switch(ret_status)
	        {
	           case SUCCESS:			    			 
		       break;
		       case SW_OR_KEY_NOT_PRESSED:
                 proc_bit_field &= ~(1 << 0);	
                 switch(cur_elevator_ptr->elevator_status)
				 {
					 case MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP:
				 		#ifdef TRACE_FLOW
				           Print("MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP -> MOVE_UP_NO_CUR_UP_TO_NEXT_UP\r");
				        #endif					    
					    cur_elevator_ptr->elevator_status = MOVE_UP_NO_CUR_UP_TO_NEXT_UP; 
					 break;
                     case MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN:
					    #ifdef TRACE_FLOW
				           Print("MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN -> MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP \r");
				        #endif
					    cur_elevator_ptr->elevator_status = MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP; 
					 break;
                     case MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN:	
                        #ifdef TRACE_FLOW
				           Print("MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN -> MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN\r");
				        #endif					 
					    cur_elevator_ptr->elevator_status = MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN;
					 break;
                     case MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP:
             			 #ifdef TRACE_FLOW
				           Print("MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP -> MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN\r");
				        #endif
					    cur_elevator_ptr->elevator_status = MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN; 
                      break;						
		         }			 
		       break;
		       case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		       case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.33", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		       // break;
               default:
			     proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_READ_PROC;
	             Error_or_Warning_Proc("14.08.34", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
	       }
		 break;
		 case MOVE_UP_NO_CUR_UP_TO_NEXT_UP:
		 case MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN:		 
		   ret_status = SW_Oper(pressed_limit_sw_arr_ptr[pressed_limit_sw_floor], DEV_READ_OPER);
	       switch(ret_status)
	       {
	          case SUCCESS:               		        					
		      break;
		      case SW_OR_KEY_NOT_PRESSED:
			     proc_bit_field &= ~(1 << 0);
				 #ifdef TRACE_ERROR
				      Print("MOVING -> ABNORMAL_EVENT \r");
					  if(pressed_limit_sw_arr_ptr == limit_sw_up_arr)
					  {
						  Print("ERR: Event - limit up ");
					  }
					  else
					  {
						   Print("ERR: Event - limit down ");
					  }
					  uint32_temp_disp_data = pressed_limit_sw_floor;
					  Print(": %u is not active \r", uint32_temp_disp_data);
				 #endif
                 appl_status_flag = ERR_SW_IS_INACTIVE;
	             Error_or_Warning_Proc("14.08.35", ERROR_OCCURED, appl_status_flag);
	             cur_elevator_ptr->elevator_status = ERR_CUR_LIMIT_SW_INACTIVE;	   
	             cur_elevator_ptr->cur_fsm_state =  FSM_ABNORMAL_EVENT;		  
			     return SUCCESS;			  
		      //break;
		      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		      case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.36", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		      // break;
              default:
			     proc_bit_field &= ~(1 << 0);
		        appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.08.37", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	       }
	     case MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP:
		 case MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN:		  
		   ret_status = SW_Oper(limit_sw_arr_ptr[limit_sw_move_cur_floor], DEV_READ_OPER);
	       switch(ret_status)
	       {
	          case SUCCESS:
			     proc_bit_field |= (1 << 1);
                 proc_bit_field &= ~(1 << 0);
				 if(cur_elevator_ptr->elevator_status == MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP || cur_elevator_ptr->elevator_status == MOVE_UP_NO_CUR_UP_TO_NEXT_UP)
				 {	
			        #ifdef TRACE_FLOW
					  if(cur_elevator_ptr->elevator_status == MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP) 
					  {
						  Print("MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP -> MOVE_UP\r");
					  }
					  else
					  {
						   Print("MOVE_UP_NO_CUR_UP_TO_NEXT_UP -> MOVE_UP \r");
					  }
					#endif
                    cur_elevator_ptr->elevator_status = MOVE_UP;
					if( cur_elevator_ptr->before_fsm_state == FSM_STARTUP)
					{
						cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_CAR_STOP;						
			            cur_elevator_ptr->elevator_status = MOVED_UP_STATIONARY;
						#ifdef TRACE_FLOW
						  Print("STARTUP: UP - MOVING -> TRIGGER_CAR_STOP \r");
						#endif 				
					}
				 }
                 else
				 {
					 #ifdef TRACE_FLOW
					  if(cur_elevator_ptr->elevator_status == MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN) 
					  {
						  Print("MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN -> MOVE_DOWN\r");
					  }
					  else
					  {
						   Print("MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN -> MOVE_DOWN\r");
					  }
					#endif	 
					cur_elevator_ptr->elevator_status = MOVE_DOWN;
					if( cur_elevator_ptr->before_fsm_state == FSM_STARTUP)
					{
						cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_CAR_STOP;
			            cur_elevator_ptr->elevator_status = MOVED_DOWN_STATIONARY;
						#ifdef TRACE_FLOW
						  Print("STARTUP: DOWN - MOVING -> TRIGGER_CAR_STOP \r");
						#endif  
					}
				 }					 
		      break;
		      case SW_OR_KEY_NOT_PRESSED:		        				
		      break;
		      case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		      case WARN_CUR_DATA_ID_DEV_DISABLED:
		         proc_bit_field &= ~(1 << 0);
		         appl_status_flag = ERR_SW_IS_DISABLED;
	             Error_or_Warning_Proc("14.08.38", ERROR_OCCURED, appl_status_flag);
	             Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		         return SUCCESS;
		      // break;
              default:
			     proc_bit_field &= ~(1 << 0);
		        appl_status_flag = ERR_SW_READ_PROC;
	            Error_or_Warning_Proc("14.08.39", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
	      }
		 break;
		 default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.08.40", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		  return SUCCESS;
	}
	if((proc_bit_field & (1 << 1)))
	{			  
	    if((Timer_Stop(CH_ID_01)) != SUCCESS)
	    {
			 appl_status_flag = ERR_TIMER_STOP_PROC;
	         Error_or_Warning_Proc("14.08.41", ERROR_OCCURED, appl_status_flag);
	         Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		     return SUCCESS;
	    }
		proc_bit_field |= (1 << 2);
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
		    for(cur_floor = limit_sw_min_floor; cur_floor <= limit_sw_max_floor; ++cur_floor)
		    {
			    if(cur_floor == limit_sw_cur_floor)
			    {
			    	continue;
			    }
		        if((SW_Present_State_By_IO_Ch(limit_sw_arr_ptr[cur_floor], &is_sw_pressed_state_floor)) != SUCCESS)
		        {
			        appl_status_flag = ERR_SW_PRESENT_STATE_PROC;
	                Error_or_Warning_Proc("14.08.42", ERROR_OCCURED, appl_status_flag);
	                Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		            return SUCCESS;
		        }
		        switch(is_sw_pressed_state_floor)
		        {
		    	     case STATE_YES:
				         proc_bit_field &= ~(1 << 2);
						 proc_bit_field &= ~(1 << 1);
			             #ifdef TRACE_ERROR
	                        Print("MOVING -> ABNORMAL_EVENT \r");
	                        Print("ERR: Event - floor limit sws detect > 1 \r");
		                 #endif
			             appl_status_flag = ERR_MORE_THAN_ONE_SW_DETECT;
	                     Error_or_Warning_Proc("14.08.43", ERROR_OCCURED, appl_status_flag);
			             cur_elevator_ptr->elevator_status = ERR_AT_LEAST_TWO_LIMIT_SW_DETECT;	   
	                     cur_elevator_ptr->cur_fsm_state =  FSM_ABNORMAL_EVENT;		  
			            return SUCCESS;
			        //break;
                    case STATE_NO:			            
                    break;
			        default:
			            proc_bit_field &= ~(1 << 2);
                        proc_bit_field &= ~(1 << 1);						
                        appl_status_flag = ERR_FORMAT_INVALID;
	                    Error_or_Warning_Proc("14.08.44", ERROR_OCCURED, appl_status_flag);
	                    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                return SUCCESS;  
			     }
		    }   
        }
		if((proc_bit_field & (1 << 2)))
	    {
		    proc_bit_field &= ~(1 << 1);
		    proc_bit_field &= ~(1 << 2);		
		    cur_elevator_ptr->cur_floor = limit_sw_cur_floor;
			if(cur_elevator_ptr->before_fsm_state == FSM_PREPARE_USER_ENTRY_AND_EXIT)
			{
	           if((Car_Movement_Direction(ctrl_elevator_ch_id, cur_elevator_ptr->cur_floor, &car_movement_state)) != SUCCESS)
		       {
		          appl_status_flag = ERR_CAR_MOVE_STATE_PROC;
	              Error_or_Warning_Proc("14.08.45", ERROR_OCCURED, appl_status_flag);
	              #ifdef TRACE_FLOW 
	                 Print("MOVING -> ABNORMAL_EVENT \r");
				  #endif	 
				  #ifdef TRACE_ERROR 
		             Print("ERR: event - car movement proc invalid \r");
	              #endif
		          cur_elevator_ptr->elevator_status = ERR_CAR_MOVEMENT_PROC;
	              cur_elevator_ptr->cur_fsm_state = FSM_ABNORMAL_EVENT;
		          return SUCCESS;	 
	          }
	          switch(car_movement_state)
		      {
		          case MOVE_UP:			     
		          case MOVED_UP_STATIONARY:
					 switch(car_movement_state)
					 {
                         case MOVED_UP_STATIONARY:
						   #ifdef TRACE_FLOW
	                           Print("MOVING -> TRIGGER_CAR_STOP \r");	                  
	                       #endif
		                   cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_CAR_STOP;
			               cur_elevator_ptr->elevator_status = MOVED_UP_STATIONARY;
						 break;
						 case MOVE_UP:						    
						    if((Timer_Run(CH_ID_01, TIMER_ID_NEXT_FLOOR)) != SUCCESS)
	                        {
	                         	appl_status_flag = ERR_TIMER_RUN_PROC;
	                            Error_or_Warning_Proc("14.08.46", ERROR_OCCURED, appl_status_flag);
	                            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                        return SUCCESS;
	                        } 
						    #ifdef TRACE_INFO
							   uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
	                           Print("INFO: Moving Up non stop - current floor: %u, ", uint32_temp_disp_data); 
                               uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
							   Print("stop floor : %u\r", uint32_temp_disp_data); 
	                       #endif
						 break;
					 }	
                   break;
                   case MOVE_DOWN:			     
			       case MOVED_DOWN_STATIONARY:
				      switch(car_movement_state)
				      {
                         case MOVED_DOWN_STATIONARY:
				     	   #ifdef TRACE_FLOW
	                           Print("MOVING -> TRIGGER_CAR_STOP \r");	                  
	                       #endif
		                   cur_elevator_ptr->cur_fsm_state =  FSM_TRIGGER_CAR_STOP;
			               cur_elevator_ptr->elevator_status = MOVED_DOWN_STATIONARY;
					     break;
					     case MOVE_DOWN:						    
                           if((Timer_Run(CH_ID_01, TIMER_ID_NEXT_FLOOR)) != SUCCESS)
	                       {
	                        	appl_status_flag = ERR_TIMER_RUN_PROC;
	                            Error_or_Warning_Proc("14.08.47", ERROR_OCCURED, appl_status_flag);
	                            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		                        return SUCCESS;
	                       }  						 
					       #ifdef TRACE_INFO
					         uint32_temp_disp_data = cur_elevator_ptr->cur_floor;
	                         Print("INFO: Moving Down non stop, current floor: %u, ", uint32_temp_disp_data); 
                             uint32_temp_disp_data = cur_elevator_ptr->next_stop_floor;
						     Print("stop floor : %u\r", uint32_temp_disp_data); 							   
	                      #endif
					     break;
				      }
                   break;
			       default:
                     appl_status_flag = ERR_FORMAT_INVALID;
	                 Error_or_Warning_Proc("14.08.48", ERROR_OCCURED, appl_status_flag);
	                 Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		             return SUCCESS;	
		       }
	      }
	   }
	}    
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  :  FSM_Trigger_Car_Stop_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.09 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Trigger_Car_Stop_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	uint16_t ret_status;
	 
	if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.09.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("TRIGGER_CAR_STOP -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.09.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((IO_Channel_Write(CAR_UP_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.09.03", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		return SUCCESS; 
	}
	if((IO_Channel_Write(CAR_DOWN_IO_CH, STATE_LOW)) != SUCCESS)
	{
		appl_status_flag = ERR_IO_CH_WRITE;
	    Error_or_Warning_Proc("14.09.04", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((Timer_Run(CH_ID_01, TIMER_ID_MOTOR_STOP_TO_DOOR_OPEN)) != SUCCESS)
	{
		appl_status_flag = ERR_TIMER_RUN_PROC;
	    Error_or_Warning_Proc("14.09.05", ERROR_OCCURED, appl_status_flag);
	    Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
    #ifdef TRACE_FLOW
	   Print("TRIGGER_CAR_STOP -> WAIT_TILL_CAR_STOPPED \r");
	#endif
	#ifdef TRACE_INFO	
	   Print("INFO: Car triggered to stop moving \r");
    #endif	
    #ifdef TRACE_REQ	
	   Print("REQ: Wait till car stops \r");
    #endif	
    cur_elevator_ptr->cur_fsm_state =  FSM_WAIT_TILL_CAR_STOPPED;	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : FSM_Wait_Till_Car_Stopped_Proc 

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 14.10 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t FSM_Wait_Till_Car_Stopped_Proc(const uint8_t ctrl_elevator_ch_id)
{
	elevator_ctrl_and_status_t *cur_elevator_ptr;
	timer_or_counter_data_t retrieved_timer_or_counter_interrupt_data;
	uint16_t ret_status;
	 
	 if(ctrl_elevator_ch_id >= MAX_NUM_ELEVATORS)
	{
		appl_status_flag = ERR_ELEVATOR_CH_ID_EXCEEDS;
	    Error_or_Warning_Proc("14.10.01", ERROR_OCCURED, appl_status_flag);
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
	          Print("WAIT_TILL_CAR_STOPPED -> ABNORMAL_EVENT \r");
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
	      Error_or_Warning_Proc("14.10.02", ERROR_OCCURED, appl_status_flag);
	      Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}
	if((Poll_Hall_And_In_Car_Floor_Calls_Proc(ctrl_elevator_ch_id)) != SUCCESS)
	{
		   appl_status_flag = ERR_POLL_FLOOR_CALLS_PROC;
		   Error_or_Warning_Proc("14.10.03", ERROR_OCCURED, appl_status_flag);
	       #ifdef TRACE_FLOW 
	          Print("WAIT_TILL_CAR_STOPPED -> ABNORMAL_EVENT \r");
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
            if(retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id != TIMER_ID_MOTOR_STOP_TO_DOOR_OPEN)
			{
				#ifdef TRACE_ERROR
				  uint32_temp_disp_data = retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id;
				  Print("ERR: timer_id : %u", retrieved_timer_or_counter_interrupt_data.timer_or_counter_run_id);
				#endif
				appl_status_flag = ERR_TMR_ID_INVALID;
	            Error_or_Warning_Proc("14.10.04", ERROR_OCCURED, appl_status_flag);
	            Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		        return SUCCESS;
			}
			#ifdef TRACE_INFO
			   uint32_temp_disp_data = cur_elevator_ptr->cur_floor; 
			   Print("INFO: Car stopped at floor : %u \r", uint32_temp_disp_data);
			#endif
			 #ifdef TRACE_FLOW
		      Print("WAIT_TILL_CAR_STOPPED -> PREPARE_DOORS_TO_ALIGN \r");			  
		    #endif
			cur_elevator_ptr->cur_fsm_state = FSM_PREPARE_DOORS_TO_ALIGN;            			
		break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
	       Error_or_Warning_Proc("14.10.05", ERROR_OCCURED, appl_status_flag);
	       Internal_Error_Elevator_Proc(ctrl_elevator_ch_id);
		   return SUCCESS;
	}	
	return SUCCESS;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
