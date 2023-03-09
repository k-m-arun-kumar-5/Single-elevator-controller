/**************************************************************************
   FILE          :    keyboard.c
 
   PURPOSE       :     
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
	CAUTION        :
	
  NOTE           :   
  
  CHANGE LOGS    :
  
  FILE ID        : 04  
	   
 **************************************************************************/
  
 #include "main.h" 
 
 #ifdef KEYBOARD_MOD_ENABLE
 
 #include <stdarg.h> 
 #include <stdlib.h>
 
/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */
keyboard_status_t keyboard_status[NUM_INPUT_DEV_ID_KEYBOARD_CHS];
static uint32_t cur_pressed_key_or_sw;	
stream_t stdin_keyboard;

/* ----------------------------- global variable declaration -------------------- */
extern uint8_t last_sw_ch_id;

/* ----------------------------- global function declaration -------------------- */
static uint8_t Keyboard_Col_Scan(keyboard_ctrl_t *const cur_keyboard_ctrl_ptr, const uint8_t offset_row_select );
static int16_t Stream_Stdin_Oper(const uint8_t keyboard_ch_id);

/* ----------------------------- function pointer defination -------------------- */

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Read

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : long key press and no key press timeout is not implemented.                 

Func ID        : 04.01  

BUGS           :     
-*------------------------------------------------------------*/
uint8_t Keyboard_Read(const uint8_t keyboard_ch_id, void *const dev_input_ctrl_para_ptr)
{
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR; 
	 dev_read_ctrl_para_t *rcvd_dev_input_ctrl_para_ptr = (dev_read_ctrl_para_t *)dev_input_ctrl_para_ptr;
	 uint8_t ret_status = SUCCESS, temp_ret_status, error_status_flag = NO_ERROR, retrieve_data_str_len, row_select_state, offset_row_select;
	 static uint8_t prev_str_len = 0;  //SHOULD_REMOVE
	 
	   if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS || rcvd_dev_input_ctrl_para_ptr == NULL_PTR)
		 {
			   system_status_flag = ERR_DEV_CH_ID_OR_NULL_PTR;
			    Error_or_Warning_Proc("04.01.01", ERROR_OCCURED, system_status_flag);
			   return system_status_flag;
		 }		   
       cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
	   if(  cur_keyboard_ctrl_ptr->access_flag != STATE_YES) 
	   {  
          system_status_flag = WARN_CUR_DATA_ID_DEV_NO_ACCESS;			 
          return  system_status_flag;
	   }
	   if( cur_keyboard_ctrl_ptr->enable_flag != STATE_YES) 
	   {  
          system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;		 
          return system_status_flag;
	   }
	   if(rcvd_dev_input_ctrl_para_ptr->cur_data_read_status.max_try_reached_flag == STATE_YES || rcvd_dev_input_ctrl_para_ptr->cur_data_read_status.read_and_ready_to_process_flag == STATE_YES)
	   {
		   Keyboard_Disable(keyboard_ch_id);
		   system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;
		   return system_status_flag;
	   }
	    #ifdef SHOULD_REMOVE	
		  	if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_NUM_CHARS_READ, &retrieve_data_str_len)) != SUCCESS)
			  {
					error_status_flag = ERR_APPL_DATA_RETRIEVE;
					 goto KEYPAD_READ_EXIT;
			   }
			 if(prev_str_len != retrieve_data_str_len)
			 {
			prev_str_len =  retrieve_data_str_len;
             			
		     disp_trace_num_fmt.sign_flag = STATE_NO;
		     disp_trace_num_fmt.least_digits_flag = STATE_YES;
		    disp_trace_num_fmt.left_format_flag = STATE_YES;
		    disp_trace_num_fmt.num_digits = 3;
			UART_Transmit_Str(TRACE_UART_CH_ID, "\r keyboard: ");
			UART_Transmit_Num(TRACE_UART_CH_ID, DATA_TYPE_IN_DECIMAL, disp_trace_num_fmt, retrieve_data_str_len );
			 }
			 #endif
	   ret_status = Keyboard_BS_SW_Read_Proc(keyboard_ch_id);
	   switch(ret_status)
	   {
		   case SUCCESS:
		      rcvd_dev_input_ctrl_para_ptr->read_data_char = BACKSPACE_CHAR;
			  if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_NUM_CHARS_READ, &retrieve_data_str_len)) != SUCCESS)
			  {
					error_status_flag = ERR_APPL_DATA_RETRIEVE;
					 goto KEYPAD_READ_EXIT;
			   }			  
			   if(retrieve_data_str_len == 1)
			   {
				   ret_status = Keyboard_Enter_SW_Oper(keyboard_ch_id, DEV_DISABLE_OPER);
				   switch(ret_status)
				   {
					   case SUCCESS:
					   case ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG:		               
		               break;
		               default:
		                error_status_flag = ERR_DEV_OPER_DISABLE_FUNC;		     			 
                        goto KEYPAD_READ_EXIT;
	               }
				   ret_status = Keyboard_BS_SW_Oper(keyboard_ch_id, DEV_DISABLE_OPER);
			       switch(ret_status)
			       {
				      case SUCCESS:
				      case ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG:		               
		              break;
		              default:
		                error_status_flag = ERR_DEV_OPER_DISABLE_FUNC;		     			 
                        goto KEYPAD_READ_EXIT;
			       }
			   }			  
			   return DATA_CHAR_READ;
		  // break;
		   case SW_OR_KEY_NOT_PRESSED:
		   case ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG:
		   case WARN_CUR_DATA_ID_DEV_DISABLED:
		   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		   break;
		   default:
		     error_status_flag = ERR_KEYBOARD_BS_SW_PROC;		     			 
             goto KEYPAD_READ_EXIT;
	   }
	   ret_status = Keyboard_Enter_SW_Read_Proc(keyboard_ch_id);
	   switch(ret_status)
	   {
		   case SUCCESS:
		       rcvd_dev_input_ctrl_para_ptr->read_data_char = ENTER_CHAR;
			   ret_status = Keyboard_Enter_SW_Oper(keyboard_ch_id, DEV_DISABLE_OPER);
			   switch(ret_status)
			   {
				   case SUCCESS:
				   case ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG:		               
		           break;
		           default:
		              error_status_flag = ERR_DEV_OPER_DISABLE_FUNC;		     			 
                      goto KEYPAD_READ_EXIT;
	          }			
			  ret_status = Keyboard_BS_SW_Oper(keyboard_ch_id, DEV_DISABLE_OPER);
			  switch(ret_status)
			  {
				  case SUCCESS:
				  case ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG:		               
		          break;
		          default:
		             error_status_flag = ERR_DEV_OPER_DISABLE_FUNC;		     			 
                     goto KEYPAD_READ_EXIT;
			  }
			  return DATA_CHAR_READ;
		  // break;
		   case SW_OR_KEY_NOT_PRESSED:
		   case ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG:
		   case WARN_CUR_DATA_ID_DEV_DISABLED:
		   case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		   break;
		   default:
		     error_status_flag = ERR_KEYBOARD_BS_SW_PROC;		    		 
             goto KEYPAD_READ_EXIT;
	   }
	   
       row_select_state = cur_keyboard_ctrl_ptr->keypad_pressed_state;	
	   //KEYPAD_ROWA_SELECT for offset_row_select = 0, and so on.
       for(offset_row_select = 0; offset_row_select < KEYPAD_NUM_ROWS; ++offset_row_select )
	   {
		   if((ret_status = IO_Channel_Write(cur_keyboard_ctrl_ptr->io_ch_rowa + offset_row_select,  !row_select_state)) != SUCCESS)
		   {	
	          error_status_flag = ERR_KEYPAD_ROW_WRITE;	         
              goto KEYPAD_READ_EXIT;
		  }		
	  }
	  //KEYPAD_ROWA_SELECT for offset_row_select = 0, and so on.
	  for(offset_row_select = 0; offset_row_select < KEYPAD_NUM_ROWS ; ++offset_row_select )
	  {
		   if((ret_status = IO_Channel_Write(cur_keyboard_ctrl_ptr->io_ch_rowa + offset_row_select,  row_select_state)) != SUCCESS)
		   {	
	          error_status_flag = ERR_KEYPAD_ROW_WRITE;	         
              goto KEYPAD_READ_EXIT;
		   }
		   ret_status = Keyboard_Col_Scan(cur_keyboard_ctrl_ptr, offset_row_select);
		   if((temp_ret_status = IO_Channel_Write(cur_keyboard_ctrl_ptr->io_ch_rowa + offset_row_select,  !row_select_state)) != SUCCESS)
		   {						 
			  error_status_flag = ERR_KEYPAD_ROW_WRITE;	
			  goto KEYPAD_READ_EXIT;
		   }
         switch(ret_status)
		 {
		    case DATA_CHAR_READ:
			     rcvd_dev_input_ctrl_para_ptr->read_data_char = cur_pressed_key_or_sw;
				if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_NUM_CHARS_READ, &retrieve_data_str_len)) != SUCCESS)
			   {
					error_status_flag = ERR_APPL_DATA_RETRIEVE;
					goto KEYPAD_READ_EXIT;
			   }
			   if(retrieve_data_str_len == 0)
			   {
				   ret_status = Keyboard_Enter_SW_Oper(keyboard_ch_id, DEV_ENABLE_OPER);
				   switch(ret_status)
				   {
					   case SUCCESS:
					   case ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG:		               
		               break;
		               default:
		                error_status_flag = ERR_DEV_OPER_ENABLE_FUNC;		     			 
                        goto KEYPAD_READ_EXIT;
	               }					
				   ret_status = Keyboard_BS_SW_Oper(keyboard_ch_id, DEV_ENABLE_OPER); 
				   switch(ret_status)
				   {
					   case SUCCESS:
					   case ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG:		               
		               break;
		               default:
		                error_status_flag = ERR_DEV_OPER_ENABLE_FUNC;		     			 
                        goto KEYPAD_READ_EXIT;
	               }	
			   }
			  return DATA_CHAR_READ;
			//break;	
			case SW_OR_KEY_NOT_PRESSED:
            case WARN_CUR_DATA_ID_DEV_DISABLED:
			case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
			break;	
			default:
			  error_status_flag = ERR_KEYBOARD_COL_SCAN_PROC;		    		 
             goto KEYPAD_READ_EXIT;			  
		 } 		   
	 }	 
KEYPAD_READ_EXIT: 
   if(error_status_flag != NO_ERROR)
   {
	     Error_or_Warning_Proc("04.01.04", ERROR_OCCURED, error_status_flag);
		 system_status_flag = ERR_KEYBOARD_READ_OPER;
		 Keyboard_Disable(keyboard_ch_id);
         return system_status_flag;	  	 
   }
   return ret_status;	 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Col_Scan

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : long key press and no key press timeout is not implemented.
                 halt CPU process by using while(KEY == KEY_PRESSED); ie till key is released is not used
                 
Func ID        : 04.02  

BUGS           :     
-*------------------------------------------------------------*/
static uint8_t Keyboard_Col_Scan(keyboard_ctrl_t *const cur_keyboard_ctrl_ptr, const uint8_t offset_row_select )
{
	uint8_t ret_status = SUCCESS, pressed_status = 0, offset_col_select;
	char keypad_char[]   = {'1','2', '3', '4', '5', '6','7','8','9','*', '0', '#'};
	
	if(cur_keyboard_ctrl_ptr->keypad_keys_enable_flag == STATE_NO)
	{
		  system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;
		   Error_or_Warning_Proc("04.02.01", ERROR_OCCURED, system_status_flag);
      return  system_status_flag;
	}
	//KEYPAD_COL1_SELECT for offset_col_select = 0, and so on.
	for(offset_col_select = 0; offset_col_select < KEYPAD_NUM_COLS; ++offset_col_select)
	{
		if((ret_status = SW_Press_Proc(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_col_select, NULL_PTR)) == SUCCESS)
	    {
	    	pressed_status = 0x01;
			cur_pressed_key_or_sw = keypad_char[(offset_row_select * KEYPAD_NUM_COLS) + offset_col_select]; //latest pressed key/switch			 
	    	break;		 
	   }
	}
	if(pressed_status == 0x01)
    {
		if(( ret_status = Entered_Key_No_Long_Press_Proc(cur_keyboard_ctrl_ptr)) != SUCCESS)
	    {
	        return ret_status;
	    }
        return DATA_CHAR_READ;
   }
	return SW_OR_KEY_NOT_PRESSED;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Entered_Key_No_Long_Press_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : make sure that entered key is within max available lcd loc and line, currently cur_data_conf_parameter.cur_data_allocated_max_num_chars_to_rcv 
	 sures that entered key is within max available lcd loc and within max line 

Func ID        : 04.03

BUGS           :   
-*------------------------------------------------------------*/
uint8_t Entered_Key_No_Long_Press_Proc(keyboard_ctrl_t *const cur_keyboard_ctrl_ptr)
{ 
    return SUCCESS; 
} 

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Disable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.04  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Disable(const uint8_t keyboard_ch_id)
{
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	 uint8_t offset_sw_ch_id;	
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("04.04.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;  
     cur_keyboard_ctrl_ptr->enable_flag = STATE_NO;
	 cur_keyboard_ctrl_ptr->keypad_keys_enable_flag = STATE_NO;	
	 for(offset_sw_ch_id = 0; offset_sw_ch_id < KEYPAD_NUM_COLS; ++offset_sw_ch_id)
	 {
		SW_Disable(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
	 }
	if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	{
		SW_Disable(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
		++offset_sw_ch_id;
	}
	if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	{
		SW_Disable(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);		
	}
   return SUCCESS;
}
/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Enable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.05  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Enable(const uint8_t keyboard_ch_id)
{
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	    
	if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	{
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.05.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	}
	cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
  cur_keyboard_ctrl_ptr->enable_flag = STATE_YES;
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Disable_All_Keyboards

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.06  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Disable_All_Keyboards(void)
{
	 uint8_t keyboard_ch_id =0;
	
	 for(keyboard_ch_id = 0; keyboard_ch_id < NUM_INPUT_DEV_ID_KEYBOARD_CHS; ++keyboard_ch_id)
	 {
		   Keyboard_Disable(keyboard_ch_id);
	 }
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Allow_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.07  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Allow_Access(const uint8_t keyboard_ch_id)
{
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
     uint8_t offset_sw_ch_id;
	 
     if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("04.07.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
    cur_keyboard_ctrl_ptr->access_flag = STATE_YES;
	for(offset_sw_ch_id = 0; offset_sw_ch_id < KEYPAD_NUM_COLS ; ++offset_sw_ch_id)
	{
		SW_Allow_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);	
	}
	if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	{
		SW_Allow_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
		++offset_sw_ch_id;
	}
	if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	{
		SW_Allow_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);					
	}
	 return SUCCESS;
}
/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_No_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.08  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_No_Access(const uint8_t keyboard_ch_id)
{
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
     uint8_t offset_sw_ch_id;
	 
   if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.08.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
    cur_keyboard_ctrl_ptr->access_flag = STATE_NO;
	Keyboard_Disable(keyboard_ch_id);
	for(offset_sw_ch_id = 0; offset_sw_ch_id < KEYPAD_NUM_COLS ; ++offset_sw_ch_id)
	{
		SW_No_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);	
	}
	if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	{
		SW_No_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
		++offset_sw_ch_id;
	}
	if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	{
		SW_No_Access(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);				
	}   
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.09  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Init(const uint8_t keyboard_ch_id, const uint8_t init_mode)
{
	 stream_t *stdin_ptr = &stdin_keyboard;
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	 io_config_t keyboard_config;
	 uint8_t ret_status = SUCCESS, alloc_base_sw_ch_id, offset_sw_ch_id;
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("04.09.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	  cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;	  
	 keyboard_config.io_ch = cur_keyboard_ctrl_ptr->io_ch_rowa;
	 keyboard_config.signal = PIN_SIG_DIGITAL;
	 keyboard_config.func = IO_FUNC_GPIO;
	 keyboard_config.dir = IO_DIR_OUTPUT;
	 keyboard_config.state = STATE_LOW;
	 keyboard_config.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	 keyboard_config.port_pin_len = KEYPAD_NUM_ROWS;		 
	 if((ret_status = IO_Channels_Func_Set(&keyboard_config)) != SUCCESS)
	 {
		system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("04.09.02", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	 }
	 offset_sw_ch_id = KEYPAD_NUM_COLS;
	  keyboard_config.io_ch = cur_keyboard_ctrl_ptr->io_ch_col1;
	  keyboard_config.port_pin_len = offset_sw_ch_id;
	  if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	  {
		  ++offset_sw_ch_id;
	  }
	  if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	  {
		  ++offset_sw_ch_id;
	  }
	  keyboard_config.state = cur_keyboard_ctrl_ptr->keypad_pressed_state;
	  keyboard_config.func_type = IO_FUNC_TYPE_GPIO_SW;
	  keyboard_config.ch_id_alloc_type = CH_ID_ALLOC_DYNAMIC;
	  keyboard_config.dir = IO_DIR_INPUT;
	  if((ret_status = SW_Ch_ID_Check_And_Alloc( keyboard_config.ch_id_alloc_type, &alloc_base_sw_ch_id, offset_sw_ch_id)) != SUCCESS)
      {
		 system_status_flag = ERR_SW_CONFIG;
		 Error_or_Warning_Proc("04.09.03", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	  } 
	  if((ret_status = SW_Para_Init(alloc_base_sw_ch_id, cur_keyboard_ctrl_ptr->io_ch_col1, keyboard_config.port_pin_len, cur_keyboard_ctrl_ptr->keypad_pressed_state )) != SUCCESS)
	  {
		 system_status_flag = ERR_SW_CONFIG;
		 Error_or_Warning_Proc("04.09.04", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	  }
	  cur_keyboard_ctrl_ptr->base_sw_ch_id = alloc_base_sw_ch_id;
	 if((ret_status = IO_Channels_Func_Set(&keyboard_config)) != SUCCESS)
	 {
		system_status_flag = ERR_GPIO_FUNC_SET;
		Error_or_Warning_Proc("04.09.05", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	 }
	  offset_sw_ch_id = KEYPAD_NUM_COLS;
	  if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	  {
		 keyboard_config.io_ch = cur_keyboard_ctrl_ptr->io_ch_enter_sw;
	     keyboard_config.port_pin_len = 1;
	     keyboard_config.state = cur_keyboard_ctrl_ptr->enter_sw_pressed_state;
	     keyboard_config.func_type = IO_FUNC_TYPE_GPIO_SW;
	     keyboard_config.ch_id_alloc_type = CH_ID_ALLOC_FIXED;		 
	     keyboard_config.dir = IO_DIR_INPUT;		 
		  if((ret_status = SW_Para_Init(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id, cur_keyboard_ctrl_ptr->io_ch_enter_sw, keyboard_config.port_pin_len, cur_keyboard_ctrl_ptr->enter_sw_pressed_state)) != SUCCESS)
		 {
			  system_status_flag = ERR_SW_CONFIG;
		      Error_or_Warning_Proc("04.09.06", ERROR_OCCURED, system_status_flag);
		      return system_status_flag;
		 }		 
	     if((ret_status = IO_Channels_Func_Set(&keyboard_config)) != SUCCESS)
	     {
		    system_status_flag = ERR_GPIO_FUNC_SET;
		    Error_or_Warning_Proc("04.09.07", ERROR_OCCURED, system_status_flag);
		    return system_status_flag;
	     }
		 ++offset_sw_ch_id;
	  }
	  if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	  {
	     keyboard_config.io_ch = cur_keyboard_ctrl_ptr->io_ch_bs_sw;
	     keyboard_config.port_pin_len = 1;
	     keyboard_config.state = cur_keyboard_ctrl_ptr->bs_sw_pressed_state;
	     keyboard_config.func_type = IO_FUNC_TYPE_GPIO_SW;
	     keyboard_config.ch_id_alloc_type = CH_ID_ALLOC_FIXED;
	     keyboard_config.dir = IO_DIR_INPUT;                 	
		 if((ret_status = SW_Para_Init(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id, cur_keyboard_ctrl_ptr->io_ch_bs_sw, keyboard_config.port_pin_len, cur_keyboard_ctrl_ptr->bs_sw_pressed_state)) != SUCCESS)
		 {
			  system_status_flag = ERR_SW_CONFIG;
		      Error_or_Warning_Proc("04.09.08", ERROR_OCCURED, system_status_flag);
		      return system_status_flag;
		 }
	     if((ret_status = IO_Channels_Func_Set(&keyboard_config)) != SUCCESS)
	     {
		    system_status_flag = ERR_GPIO_FUNC_SET;
		    Error_or_Warning_Proc("04.09.09", ERROR_OCCURED, system_status_flag);
		    return system_status_flag;
	     }
	  }
	  stdin_ptr->stream_buffer_type = STREAM_BUFFER_INPUT;
      Stream_Flush(stdin_ptr);   
	  return SUCCESS; 	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_DeInit

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.10 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_DeInit(const uint8_t keyboard_ch_id, const uint8_t deinit_mode)
{
	 stream_t *stdin_ptr = &stdin_keyboard;
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	 io_config_t keyboard_unconfig;	 
	 uint8_t ret_status = SUCCESS;
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.10.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 Keyboard_No_Access(keyboard_ch_id);
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
	 keyboard_unconfig.io_ch = cur_keyboard_ctrl_ptr->io_ch_rowa;
	 keyboard_unconfig.func = IO_FUNC_GPIO;
	 keyboard_unconfig.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	 keyboard_unconfig.port_pin_len = KEYPAD_NUM_ROWS;	 
	 if((ret_status = IO_Ch_Func_Reset(&keyboard_unconfig)) != SUCCESS)
	 {
		system_status_flag = ERR_IO_CH_FUNC_RESET;
		 Error_or_Warning_Proc("04.10.02", ERROR_OCCURED, system_status_flag);
        return system_status_flag;		
	 }
	 
	 keyboard_unconfig.io_ch = cur_keyboard_ctrl_ptr->io_ch_col1;
	 keyboard_unconfig.port_pin_len = KEYPAD_NUM_COLS;	
	 keyboard_unconfig.func_type = IO_FUNC_TYPE_GPIO_SW;
	 keyboard_unconfig.ch_id_alloc_type = CH_ID_ALLOC_DYNAMIC;
	 cur_keyboard_ctrl_ptr->base_sw_ch_id = CH_ID_INVALID;
	 if((ret_status = IO_Ch_Func_Reset(&keyboard_unconfig)) != SUCCESS)
	 {
		system_status_flag = ERR_IO_CH_FUNC_RESET;
		 Error_or_Warning_Proc("04.10.03", ERROR_OCCURED, system_status_flag);
        return system_status_flag;		
	 }
	 if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	 {
		 keyboard_unconfig.io_ch = cur_keyboard_ctrl_ptr->io_ch_enter_sw;
	     keyboard_unconfig.port_pin_len = 1;	
	     keyboard_unconfig.func_type = IO_FUNC_TYPE_GPIO_SW;
	     keyboard_unconfig.ch_id_alloc_type = CH_ID_ALLOC_FIXED;
		 if((ret_status = IO_Ch_Func_Reset(&keyboard_unconfig)) != SUCCESS)
	     {
		     system_status_flag = ERR_IO_CH_FUNC_RESET;
		     Error_or_Warning_Proc("04.10.04", ERROR_OCCURED, system_status_flag);
            return system_status_flag;		
	     }
	 }
	 if(cur_keyboard_ctrl_ptr->io_ch_bs_sw != IO_CH_INVALID)
	 {
		 keyboard_unconfig.io_ch = cur_keyboard_ctrl_ptr->io_ch_bs_sw;
	     keyboard_unconfig.port_pin_len = 1;	
	     keyboard_unconfig.func_type = IO_FUNC_TYPE_GPIO_SW;
	     keyboard_unconfig.ch_id_alloc_type = CH_ID_ALLOC_FIXED;
		 if((ret_status = IO_Ch_Func_Reset(&keyboard_unconfig)) != SUCCESS)
	     {
		     system_status_flag = ERR_IO_CH_FUNC_RESET;
		     Error_or_Warning_Proc("04.10.05", ERROR_OCCURED, system_status_flag);
            return system_status_flag;		
	     }
	 }
	 Stream_Flush(stdin_ptr);
     stdin_ptr->stream_buffer_type = STREAM_BUFFER_INVALID;
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Part_Oper_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.11 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Part_Oper_Proc(const uint8_t keyboard_ch_id, const uint8_t keyboard_part_and_oper_data)
{
	typedef uint8_t (*keyboard_part_oper_t)(const uint8_t keyboard_ch_id, const uint8_t dev_oper);
	keyboard_part_oper_t Keyboard_Part_Oper_Func_Ptr;
	uint8_t ret_status;
	
	if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	{
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.11.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	}
	switch(keyboard_part_and_oper_data >> KEYBOARD_PART_DATA_POS )
	{
		case PART_KEYPAD:
		  Keyboard_Part_Oper_Func_Ptr = Keyboard_Keypad_Oper;
		break;  
        case PART_ENTER_SW:
		  Keyboard_Part_Oper_Func_Ptr = Keyboard_Enter_SW_Oper;
		break;
        case PART_BS_SW:
		   Keyboard_Part_Oper_Func_Ptr = Keyboard_BS_SW_Oper;
		break;		
		default:
		  system_status_flag = ERR_FORMAT_INVALID;
		  Error_or_Warning_Proc("04.11.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;		  
	}
	if((ret_status = (*Keyboard_Part_Oper_Func_Ptr)(keyboard_ch_id, (keyboard_part_and_oper_data & 0x0F)))!= SUCCESS)
	{
		system_status_flag = ERR_KEYBOARD_PART_OPER;
		Error_or_Warning_Proc("04.11.03", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Keypad_Oper

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.12  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Keypad_Oper(const uint8_t keyboard_ch_id, const uint8_t dev_oper)
{
	 typedef uint8_t (*keypad_oper_t)(const uint8_t sw_ch_id);
	 keypad_oper_t Keypad_Oper_Func_Ptr;
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	 uint8_t offset_sw_ch_id;	
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.12.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id; 
     switch(dev_oper)
	 {
        case DEV_DISABLE_OPER: 		 
           cur_keyboard_ctrl_ptr->keypad_keys_enable_flag = STATE_NO;
		   Keypad_Oper_Func_Ptr = SW_Disable;
		break;   
        case DEV_ENABLE_OPER:
          cur_keyboard_ctrl_ptr->keypad_keys_enable_flag = STATE_YES;
		  Keypad_Oper_Func_Ptr = SW_Enable;
        break;  
        case DEV_NO_ACCESS_OPER:
           	Keypad_Oper_Func_Ptr = SW_No_Access;
        break;
		case DEV_ALLOW_ACCESS_OPER:
     		Keypad_Oper_Func_Ptr = SW_Allow_Access;
		break;	
		default:
          system_status_flag = ERR_FORMAT_INVALID;
		  Error_or_Warning_Proc("04.12.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;	
	 }	 
	 for(offset_sw_ch_id = 0; offset_sw_ch_id < KEYPAD_NUM_COLS; ++offset_sw_ch_id)
	 {
		(*Keypad_Oper_Func_Ptr)(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
	 }	
     return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Enter_SW_Oper

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.13  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Enter_SW_Oper(const uint8_t keyboard_ch_id, const uint8_t dev_oper)
{
	 typedef uint8_t (*enter_sw_oper_t)(const uint8_t sw_ch_id);
	 enter_sw_oper_t Enter_SW_Oper_Func_Ptr;
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;	 
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.13.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id; 
     switch(dev_oper)
	 {
        case DEV_DISABLE_OPER: 	           
		    Enter_SW_Oper_Func_Ptr = SW_Disable;
		break;   
        case DEV_ENABLE_OPER:         
		   Enter_SW_Oper_Func_Ptr = SW_Enable;
        break;  
        case DEV_NO_ACCESS_OPER:
           Enter_SW_Oper_Func_Ptr = SW_No_Access;
        break;
		case DEV_ALLOW_ACCESS_OPER:
     		Enter_SW_Oper_Func_Ptr = SW_Allow_Access;
		break;
        default:
          system_status_flag = ERR_FORMAT_INVALID;
		  Error_or_Warning_Proc("04.13.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;	
	 }	
	if(cur_keyboard_ctrl_ptr->io_ch_enter_sw == IO_CH_INVALID)
	{
		 system_status_flag = ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG;
		 Error_or_Warning_Proc("04.13.03", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;		
	} 		
    (*Enter_SW_Oper_Func_Ptr)(cur_keyboard_ctrl_ptr->base_sw_ch_id + KEYPAD_NUM_COLS);
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_BS_SW_Oper

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.14  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_BS_SW_Oper(const uint8_t keyboard_ch_id, const uint8_t dev_oper)
{
	 typedef uint8_t (*bs_sw_oper_t)(const uint8_t sw_ch_id);
	 bs_sw_oper_t BS_SW_Oper_Func_Ptr;
	 keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;	 
	 uint8_t offset_sw_ch_id;
	 
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.14.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id; 
     switch(dev_oper)
	 {
        case DEV_DISABLE_OPER: 	           
		    BS_SW_Oper_Func_Ptr = SW_Disable;
		break;   
        case DEV_ENABLE_OPER:         
		   BS_SW_Oper_Func_Ptr = SW_Enable;
        break;  
        case DEV_NO_ACCESS_OPER:
           BS_SW_Oper_Func_Ptr = SW_No_Access;
        break;
		case DEV_ALLOW_ACCESS_OPER:
     	   BS_SW_Oper_Func_Ptr = SW_Allow_Access;
		break;
        default:
          system_status_flag = ERR_FORMAT_INVALID;
		  Error_or_Warning_Proc("04.14.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;	
	 }	
	if(cur_keyboard_ctrl_ptr->io_ch_bs_sw == IO_CH_INVALID)
	{
		system_status_flag = ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG;
		Error_or_Warning_Proc("04.14.03", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	offset_sw_ch_id = KEYPAD_NUM_COLS;
    if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	{
		++offset_sw_ch_id; 
	}
    (*BS_SW_Oper_Func_Ptr)(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id);
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Enter_SW_Read_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.15  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Enter_SW_Read_Proc(const uint8_t keyboard_ch_id)
{
	keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	uint8_t ret_status;
  
	if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.15.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
     if(cur_keyboard_ctrl_ptr->io_ch_enter_sw == IO_CH_INVALID)
	{
		 system_status_flag = ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG;
		 Error_or_Warning_Proc("04.15.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;		
	} 		
    ret_status = SW_Press_Proc(cur_keyboard_ctrl_ptr->base_sw_ch_id + KEYPAD_NUM_COLS, NULL_PTR);
	switch(ret_status)
	{
		case SUCCESS:		
		case SW_OR_KEY_NOT_PRESSED:
		case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		case WARN_CUR_DATA_ID_DEV_DISABLED:
		   return ret_status;
        //break;        
	}	
    system_status_flag = ERR_KEYBOARD_ENTER_SW_PROC;
	Error_or_Warning_Proc("04.15.03", ERROR_OCCURED, system_status_flag);
	return system_status_flag;		
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_BS_SW_Read_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.16  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_BS_SW_Read_Proc(const uint8_t keyboard_ch_id)
{
	keyboard_ctrl_t *cur_keyboard_ctrl_ptr = NULL_PTR;
	uint8_t ret_status, offset_sw_ch_id;
  
	if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.16.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_keyboard_ctrl_ptr = keyboard_ctrl + keyboard_ch_id;
     if(cur_keyboard_ctrl_ptr->io_ch_bs_sw == IO_CH_INVALID)
	{
		 system_status_flag = ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG;
		 Error_or_Warning_Proc("04.16.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;		
	}
	offset_sw_ch_id = KEYPAD_NUM_COLS;
    if(cur_keyboard_ctrl_ptr->io_ch_enter_sw != IO_CH_INVALID)
	{
		++offset_sw_ch_id;
	}
    ret_status = SW_Press_Proc(cur_keyboard_ctrl_ptr->base_sw_ch_id + offset_sw_ch_id, NULL_PTR);
	switch(ret_status)
	{
		case SUCCESS:
		case SW_OR_KEY_NOT_PRESSED:
		case WARN_CUR_DATA_ID_DEV_NO_ACCESS:
		case WARN_CUR_DATA_ID_DEV_DISABLED:
		   return ret_status;
        //break;        
	}	
    system_status_flag = ERR_KEYBOARD_BS_SW_PROC;
	Error_or_Warning_Proc("04.16.03", ERROR_OCCURED, system_status_flag);
	return system_status_flag;		
}

/*------------------------------------------------------------*
FUNCTION NAME  : Keyboard_Ready_Read

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.17 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Keyboard_Ready_Read(const uint8_t keyboard_ch_id)
{
  uint8_t ret_status;
  
	 if(keyboard_ch_id >= NUM_INPUT_DEV_ID_KEYBOARD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("04.18.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }	 
	Keyboard_Keypad_Oper(keyboard_ch_id, DEV_ALLOW_ACCESS_OPER);
	Keyboard_Keypad_Oper(keyboard_ch_id, DEV_ENABLE_OPER);
	ret_status = Keyboard_Enter_SW_Oper(keyboard_ch_id, DEV_ALLOW_ACCESS_OPER);
	switch(ret_status)
	{
		case ERR_KEYBOARD_ENTER_SW_OPER_BUT_NOT_CONFIG:
		break;
		case SUCCESS:
		    Keyboard_Enter_SW_Oper(keyboard_ch_id, DEV_ENABLE_OPER);
		break; 
	}
    ret_status = Keyboard_BS_SW_Oper(keyboard_ch_id, DEV_ALLOW_ACCESS_OPER);
	switch(ret_status)
	{
		case ERR_KEYBOARD_BS_SW_OPER_BUT_NOT_CONFIG:
		break;
		case SUCCESS:
		    Keyboard_BS_SW_Oper(keyboard_ch_id, DEV_ENABLE_OPER);
		break; 
	}
	Keyboard_Allow_Access(keyboard_ch_id);	
	Keyboard_Enable(keyboard_ch_id);
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Scan

DESCRIPTION    : Scan() operation is similar to scanf()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.18 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Scan(const char *const format_str,...) 
{ 
    char buffer[STREAM_BUFFER_LEN];
    char *first_non_base_int_char_loc_ptr, *read_format_str, *read_str, read_char;
    va_list arg;
	uint8_t cur_pos_conv_char_and_unrecog_char = 0, cur_pos_format_str = 0, num_scanned_data = 0, num_chars_read_str ;
	
	if(format_str == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
	    Error_or_Warning_Proc("04.18.01", ERROR_OCCURED, system_status_flag);
	    return 0;
	}
	read_format_str = Get_Str(buffer);
	if(read_format_str == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
	    Error_or_Warning_Proc("04.18.02", ERROR_OCCURED, system_status_flag);
	    return 0;
	}
	va_start(arg, format_str);
    while (format_str[cur_pos_conv_char_and_unrecog_char])
    {
       if(format_str[cur_pos_conv_char_and_unrecog_char] == '%')
	   {
          ++cur_pos_conv_char_and_unrecog_char; 
          switch (format_str[cur_pos_conv_char_and_unrecog_char])
	      {
             case 'c': 
	            *(char *)va_arg( arg, char* ) = buffer[cur_pos_format_str];
                ++cur_pos_format_str;
                ++num_scanned_data;
             break;
             case 'd': 
               *(int *)va_arg( arg, int* ) = strtol(&buffer[cur_pos_format_str], &first_non_base_int_char_loc_ptr, 10);
                cur_pos_format_str+= first_non_base_int_char_loc_ptr -&buffer[cur_pos_format_str];
                ++num_scanned_data;
             break;
             case 'x': 
                *(unsigned int *)va_arg( arg, unsigned int* ) = strtol(&buffer[cur_pos_format_str], &first_non_base_int_char_loc_ptr, 16);
                cur_pos_format_str+= first_non_base_int_char_loc_ptr - &buffer[cur_pos_format_str];
                ++num_scanned_data;
             break;
			 case 'o':
               *(unsigned int *)va_arg( arg, unsigned int* ) = strtol(&buffer[cur_pos_format_str], &first_non_base_int_char_loc_ptr, 8);
                cur_pos_format_str+= first_non_base_int_char_loc_ptr - &buffer[cur_pos_format_str];
                ++num_scanned_data;
             break;
             case 'u':
               *(unsigned int *)va_arg( arg, unsigned int* ) = strtol(&buffer[cur_pos_format_str], &first_non_base_int_char_loc_ptr, 10);
                cur_pos_format_str+= first_non_base_int_char_loc_ptr - &buffer[cur_pos_format_str];
                ++num_scanned_data;
             break;
			 case 's':
			   read_str = (char *)va_arg( arg, char* );
         num_chars_read_str = 0;			      				 
        do 
				{
					read_char = read_str[num_chars_read_str] = buffer[cur_pos_format_str];
					++num_chars_read_str;
          ++cur_pos_format_str;					
				} 
				while(read_char != NULL_CHAR && read_char != ' ' && read_char != ENTER_CHAR && read_char != HORIZONTAL_TAB && read_char != VERTICAL_TAB);
				read_str[num_chars_read_str] = NULL_CHAR;   
                ++num_scanned_data;
             break;
			 default:
               system_status_flag = ERR_FORMAT_INVALID;
		       Error_or_Warning_Proc("04.18.03", ERROR_OCCURED, system_status_flag );	
               return 0; 	
          } 
	   }
     else 
	   {
          ++cur_pos_format_str;
          ++cur_pos_conv_char_and_unrecog_char;
     }
    }
    va_end(arg);
	return num_scanned_data;
} 

/*------------------------------------------------------------*
FUNCTION NAME  : Get_Char

DESCRIPTION    : Get_Char() operation is similar to getchar()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.19 

Bugs           :  
-*------------------------------------------------------------*/
char Get_Char(void)
{
	dev_id_t keyboard_or_uart_dev_id;
	int16_t last_read_char = NULL_CHAR;
	uint8_t ret_status;	
	
	keyboard_or_uart_dev_id.dev_id = INPUT_DEV_ID_KEYBOARD;
	if((ret_status = Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &keyboard_or_uart_dev_id, DEV_INPUT_TYPE))!= SUCCESS)
	{
	    system_status_flag = ERR_CUR_DATA_DEV_NOT_SRC;
	    Error_or_Warning_Proc("04.19.01", ERROR_OCCURED, system_status_flag);
	    return NULL_CHAR;
	}	
	if((ret_status = Keyboard_Ready_Read(keyboard_or_uart_dev_id.dev_ch_id)) != SUCCESS)
	{
		system_status_flag = ERR_KEYBOARD_READY_READ;
	    Error_or_Warning_Proc("04.19.02", ERROR_OCCURED, system_status_flag);
	    return NULL_CHAR;
	}
  do
	{
        last_read_char = Stream_Stdin_Oper(keyboard_or_uart_dev_id.dev_ch_id);
	}
    while(last_read_char == NULL_CHAR && last_read_char != EOF);	
	#ifdef UART_MOD_ENABLE	    
       keyboard_or_uart_dev_id.dev_id = COMM_DEV_ID_UART;
	   if((ret_status = Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &keyboard_or_uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	   {
	        keyboard_or_uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;	    
	   }	
       if((ret_status = UART_Transmit_Char(keyboard_or_uart_dev_id.dev_ch_id, last_read_char)) != SUCCESS)
	   {
		  system_status_flag = ERR_UART_TRANSMIT_OPER;
		  Error_or_Warning_Proc("04.19.04", ERROR_OCCURED, system_status_flag);
		  return NULL_CHAR;
	   }
    #endif	
	return last_read_char;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Get_Str

DESCRIPTION    : Get_Str() operation is similar to gets()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.20 

Bugs           :  
-*------------------------------------------------------------*/
char *Get_Str(char *const get_str)
{
	static char read_str[STREAM_BUFFER_LEN];
    dev_id_t keyboard_dev_id, uart_dev_id;
	int16_t last_read_char = NULL_CHAR;
	uint8_t ret_status, num_chars_read_str = 0;	
	
	keyboard_dev_id.dev_id = INPUT_DEV_ID_KEYBOARD;
	if((ret_status = Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &keyboard_dev_id, DEV_INPUT_TYPE))!= SUCCESS)
	{
	    system_status_flag = ERR_CUR_DATA_DEV_NOT_SRC;
	    Error_or_Warning_Proc("04.20.01", ERROR_OCCURED, system_status_flag);
	    return NULL_PTR;
	}	
	if((ret_status = Keyboard_Ready_Read(keyboard_dev_id.dev_ch_id)) != SUCCESS)
	{
		system_status_flag = ERR_KEYBOARD_READY_READ;
	    Error_or_Warning_Proc("04.20.02", ERROR_OCCURED, system_status_flag);
	    return NULL_PTR;
	}
	uart_dev_id.dev_id = COMM_DEV_ID_UART;
	if((ret_status = Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	{
	     uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;	    
	}
	memset(read_str, NULL_CHAR, STREAM_BUFFER_LEN);
    do
	{
        last_read_char = Stream_Stdin_Oper(keyboard_dev_id.dev_ch_id);
		if(last_read_char != NULL_CHAR && last_read_char != EOF )
		{
			#ifdef UART_MOD_ENABLE               
               if((ret_status = UART_Transmit_Char(uart_dev_id.dev_ch_id, last_read_char)) != SUCCESS)
	           {
		          system_status_flag = ERR_UART_TRANSMIT_OPER;
		          Error_or_Warning_Proc("04.20.03", ERROR_OCCURED, system_status_flag);
		          return NULL_PTR;
	           }
           #endif
		   if(num_chars_read_str + 1 < STREAM_BUFFER_LEN && last_read_char != BACKSPACE_CHAR && last_read_char != ENTER_CHAR)
		   {
			   read_str[num_chars_read_str] = last_read_char;
			   ++num_chars_read_str;
			   read_str[num_chars_read_str] = NULL_CHAR;
		   }
		   else
		   {
			   if(last_read_char == BACKSPACE_CHAR && num_chars_read_str > 0 )
			   {
				   --num_chars_read_str;
				   read_str[num_chars_read_str] = NULL_CHAR;				   
			   }
		   }
		}			
	}
    while(last_read_char != ENTER_CHAR && last_read_char != EOF);
	memcpy(get_str, read_str, STREAM_BUFFER_LEN); 
	return read_str;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Stream_Stdin_Oper

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 04.21 

Bugs           :  
-*------------------------------------------------------------*/
static int16_t Stream_Stdin_Oper(const uint8_t keyboard_ch_id)
{
	stream_t *stdin_ptr = &stdin_keyboard;
	dev_read_ctrl_para_t dev_read_ctrl_para;
	uint8_t ret_status;
	char last_read_char = NULL_CHAR;
	
    ret_status =  Keyboard_Read(keyboard_ch_id, &dev_read_ctrl_para);
	switch(ret_status)
    {
        case DATA_CHAR_READ:
	      last_read_char = dev_read_ctrl_para.read_data_char;
		  if(last_read_char != BACKSPACE_CHAR)
		  {
			 if(stdin_ptr->num_chars_stream_buffer + 1 >= STREAM_BUFFER_LEN)
			 {
					 return EOF;                     
			 }
			  stdin_ptr->stream_buffer[stdin_ptr->num_chars_stream_buffer] = last_read_char;
			  ++stdin_ptr->num_chars_stream_buffer; 
			  stdin_ptr->stream_buffer[stdin_ptr->num_chars_stream_buffer] = NULL_CHAR;
	      }				
          else
		  {
		     //read char is BACKSPACE_CHAR
			 if(stdin_ptr->num_chars_stream_buffer > 0)
			 {
				 --stdin_ptr->num_chars_stream_buffer; 
			     stdin_ptr->stream_buffer[stdin_ptr->num_chars_stream_buffer] = NULL_CHAR;			     							                     
			 }
	      } 	
	    break;              
        case SW_OR_KEY_NOT_PRESSED:
        break;		   
        default: 
            system_status_flag = ERR_DEV_OPER_READ_FUNC;
		    Error_or_Warning_Proc("04.21.01", ERROR_OCCURED, system_status_flag);
		    return EOF;
    }
	return last_read_char;
}		
#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
