/* ********************************************************************
FILE                   : std_lib.c

PURPOSE                : 
                     									 	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
                       
CHANGE LOGS           : 

FILE ID               : 02 

*****************************************************************************/

#include "main.h"
#define NUM_CONVERT_BUFFER_LEN   (15)

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */
uint8_t internal_error_state = NO_ERROR_OCCURED;

/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */
static uint8_t Comm_Error_Proc(const char *const error_trace_str, const uint8_t warn_or_error_format, const uint32_t warning_or_error_code);

/*------------------------------------------------------------*
FUNCTION NAME  : SW_Time_Delay

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.01  

BUGS           :              
-*------------------------------------------------------------*/
void SW_Time_Delay(const uint32_t max_i_count, const uint32_t max_j_count)
{
	 uint32_t i, j;
  
	 for(i = 0; i < max_i_count; ++i)
	 { 
         for(j = 0;j < max_j_count; ++j);
	 } 
}

/*------------------------------------------------------------*-
FUNCTION NAME  : Config_Consucc_Bits

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.02 

BUGS           :
-*------------------------------------------------------------*/
uint8_t Config_Consucc_Bits( const uint8_t flag_consucc_bit, void *const data_ptr)
{
	uint32_t from_bit0_consucc_bits = 0;	
	consucc_bit_t *consucc_bit_ptr = (consucc_bit_t *)data_ptr;
	uint8_t i, end_bit_pos;     
	
	end_bit_pos = consucc_bit_ptr->start_bit_pos + consucc_bit_ptr->bits_len - 1;
	if(consucc_bit_ptr == NULL_PTR || end_bit_pos >= INT_BIT_SIZE)
	{
		system_status_flag = ERR_CONSUCC_PARA;
		 Error_or_Warning_Proc("02.02.01", ERROR_OCCURED, system_status_flag);
		return  system_status_flag;
	}
	for(i = 0; i < consucc_bit_ptr->bits_len; ++i)
	{
	  	from_bit0_consucc_bits |= 1 << i;			
	}
	switch(flag_consucc_bit)
	{
         case FLAG_CONSUCC_BITS_1:
		    consucc_bit_ptr->write_or_config_consucc_val |=  (from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos);
	     break;		
	     case FLAG_CONSUCC_BITS_0:
		    consucc_bit_ptr->write_or_config_consucc_val &= ~(from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos);
	     break;
		 case FLAG_CONSUCC_BITS_TOGGLE:
		    consucc_bit_ptr->write_or_config_consucc_val ^= (from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos);
		 break;
		 default:
		    system_status_flag = ERR_CONSUCC_PARA;
			 Error_or_Warning_Proc("02.02.02", ERROR_OCCURED, system_status_flag);
		    return system_status_flag;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*-
FUNCTION NAME  : Test_Consucc_Bits

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.03 

BUGS           :
-*------------------------------------------------------------*/
uint8_t Test_Consucc_Bits( const uint8_t flag_consucc_bit, const void *const data_ptr)
{
	uint32_t from_bit0_consucc_bits = 0, mask_configured_bits ;
	consucc_bit_t *consucc_bit_ptr = (consucc_bit_t *)data_ptr;
	uint8_t i, ret_status = SUCCESS, end_bit_pos;	
	
	end_bit_pos = consucc_bit_ptr->start_bit_pos + consucc_bit_ptr->bits_len - 1;
	if(consucc_bit_ptr == NULL_PTR || end_bit_pos >= INT_BIT_SIZE)
	{
		system_status_flag = ERR_CONSUCC_PARA;
		 Error_or_Warning_Proc("02.03.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	for(i = 0; i < consucc_bit_ptr->bits_len; ++i)
	{
	  	from_bit0_consucc_bits |= 1 << i;			
	}
	switch(flag_consucc_bit)
	{
         case FLAG_CONSUCC_BITS_1:
					 mask_configured_bits = from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos;
		       if( mask_configured_bits == (consucc_bit_ptr->read_or_test_consucc_val & mask_configured_bits))
				   {
					    ret_status = TEST_OK_1_CONSUCC_BITS;
				   }
				   else
				   {
					   system_status_flag = ERR_TEST_FAIL_1_CONSUCC_BITS;
					    ret_status = TEST_FAIL_1_CONSUCC_BITS;
				   }
	     break;		
	     case FLAG_CONSUCC_BITS_0:
				  mask_configured_bits = ~(from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos);
			    if(mask_configured_bits == (consucc_bit_ptr->read_or_test_consucc_val | mask_configured_bits))
					{
					    ret_status = TEST_OK_0_CONSUCC_BITS;
				  }
				  else
				  {
					    system_status_flag = ERR_TEST_FAIL_0_CONSUCC_BITS;
					    ret_status = TEST_FAIL_0_CONSUCC_BITS;
				  }		     
	     break;
		 case FLAG_CONSUCC_BITS_VAL:
		     mask_configured_bits = from_bit0_consucc_bits << consucc_bit_ptr->start_bit_pos;
			 if((consucc_bit_ptr->read_or_test_consucc_val & mask_configured_bits) == ((consucc_bit_ptr->write_or_config_consucc_val << consucc_bit_ptr->start_bit_pos) & mask_configured_bits))
			 {
			    ret_status = TEST_OK_VAL_CONSUCC_BITS;
			 }
			 else
			 {
			    system_status_flag = ERR_TEST_FAIL_VAL_CONSUCC_BITS;
			    ret_status = TEST_FAIL_VAL_CONSUCC_BITS;
			}
		 break;
			 default:
			   system_status_flag = ERR_CONSUCC_PARA;
			   Error_or_Warning_Proc("02.03.02", ERROR_OCCURED, system_status_flag);
		      return system_status_flag;
	}
	return ret_status;
}

#ifdef STR_TO_NUM_FROM_RIGHT_TO_LEFT
/*------------------------------------------------------------*
FUNCTION NAME  : Str_to_Num_Conv

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : digits are extracted from right to left format from least digit in num_in_str
                 and no sign symbol is present at num_in_str[0] 

Func ID        : 02.04  

BUGS           :  
-*------------------------------------------------------------*/
uint8_t Str_to_Num_Conv(int32_t *const num_conv_from_str, const char *const num_in_str  )
{	
	 int32_t num = 0;
	 uint32_t place = 1;
	 int8_t cur_unit;
	 uint8_t num_chars = 0, cur_digit= 0, base = 10, pos = 0 ;
	
	 if(num_conv_from_str == NULL_PTR || num_in_str == NULL_PTR )
	 {
		 system_status_flag = ERR_STR_TO_NUM_PARA;
		 Error_or_Warning_Proc("02.04.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }
	 num_chars = Str_Len(num_in_str);	
     place = 1;
     pos = num_chars - 1;
     cur_unit = num_in_str[pos] - NUM_0_CHAR;
	 if(cur_unit < 0 ||  cur_unit > 9 )
	 {
		 *num_conv_from_str = 0;
         system_status_flag = ERR_STR_TO_NUM_PARA;	
		  Error_or_Warning_Proc("02.04.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }	
     num = place * cur_unit;
     for(cur_digit = 1; cur_digit < num_chars; ++cur_digit)
     {
         place =  place * base;
         pos = num_chars - 1 - cur_digit;
         cur_unit = num_in_str[pos] - NUM_0_CHAR;
	     if(cur_unit < 0 ||  cur_unit > 9 )
	     {
			 *num_conv_from_str = 0;
			 system_status_flag = ERR_STR_TO_NUM_PARA;
			 Error_or_Warning_Proc("02.04.03", ERROR_OCCURED, system_status_flag);
			 return system_status_flag;
		 }			 
         num += (cur_unit * place);     
     }
	 *num_conv_from_str = num; 	 
     return SUCCESS;
}

#else
/*------------------------------------------------------------*
FUNCTION NAME  : Str_to_Num_Conv

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : digits are extracted from left to right format from digit in num_in_str

Func ID        : 02.04  

BUGS           :  
-*------------------------------------------------------------*/
uint8_t Str_to_Num_Conv(int32_t *const num_conv_from_str, const char *const num_in_str  )
{	
	 int32_t num = 0;
	 uint32_t place;
	 int16_t cur_unit;
	 uint8_t num_chars = 0, base = 10, pos = 0, start_num_pos = 0 ;
	
	 if(num_conv_from_str == NULL_PTR || num_in_str == NULL_PTR )
	 {
		 system_status_flag = ERR_STR_TO_NUM_PARA;
		   Error_or_Warning_Proc("02.04.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }
	 if(num_in_str[0] == '+' || num_in_str[0] == '-')
	 {
		 start_num_pos = 1;
	 }
	 else if(num_in_str[0] >= '0' && num_in_str[0] <= '9')
	 {
		  start_num_pos = 0;
	 }
	 else
	 {
		 *num_conv_from_str = 0;
         system_status_flag = ERR_STR_TO_NUM_PARA;
		   Error_or_Warning_Proc("02.04.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }		 
	 num_chars = Str_Len(num_in_str + start_num_pos);
	 pos = start_num_pos; 	     
     for( place = Power_Of(base, num_chars - 1); place >= 1; place /= base, ++pos )
     {
     	 cur_unit = num_in_str[pos] - NUM_0_CHAR;
    	 if(cur_unit < 0 ||  cur_unit > 9 )
    	 {
	    	 *num_conv_from_str = 0;
             system_status_flag = ERR_STR_TO_NUM_PARA;	
			   Error_or_Warning_Proc("02.04.03", ERROR_OCCURED, system_status_flag);
		     return system_status_flag;
	     }		 
         num += (cur_unit * place);		      
     }
	 if(num_in_str[0] == '-')
	 {
		 *num_conv_from_str = -num;  
	 }
	 else
	 {
	     *num_conv_from_str = num; 
	 }
     return SUCCESS;
}
#endif

/*------------------------------------------------------------*
FUNCTION NAME  : Str_Len

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.05  

BUGS           :  
-*------------------------------------------------------------*/
uint8_t Str_Len(const char *const str)
{
    uint8_t num_chars = 0;
	
	  if(str == NULL_PTR)
	  {
		  system_status_flag = ERR_STR_PTR_NULL;	
		  Error_or_Warning_Proc("02.05.01", ERROR_OCCURED, system_status_flag);
		  return 0;
	  }
    while(*(str + num_chars))
	{
		++num_chars;
	}
    return num_chars;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Data_ID_Status

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.06

BUGS           :
-*------------------------------------------------------------*/
uint8_t Reset_Data_ID_Status(const uint8_t data_id, const uint8_t set_cur_data_status_reset_type)
{
	data_id_status_para_t *data_id_status_para_ptr = NULL_PTR;
		
	if(data_id >= NUM_DATA_IDS)
	{
		 system_status_flag = ERR_DATA_ID_EXCEED;
		   Error_or_Warning_Proc("02.06.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	data_id_status_para_ptr = data_id_status_para + data_id;
	switch(set_cur_data_status_reset_type)
	{
		case DATA_ID_RESET_INIT_STATUS:
        #ifdef DMA_DATA_ID_STR
	   if(data_id_status_para_ptr->data_value.data_str != NULL_PTR)
	    {
	    	free(data_id_status_para_ptr->data_value.data_str);
		    data_id_status_para_ptr->data_value.data_str = NULL_PTR;
	    } 
		#endif
		data_id_status_para_ptr->max_allocated_data_len = 0;
		case DATA_ID_RESET_WHOLE_STATUS:		 
	    data_id_status_para_ptr->data_read_num_try = 0;
		data_id_status_para_ptr->data_read_status.max_try_reached_flag = STATE_NO;
		data_id_status_para_ptr->data_read_status.last_try_read_and_ready_to_process_flag = STATE_NO;
		case DATA_ID_RESET_RETRY_STATUS:
		 data_id_status_para_ptr->data_value.data_str_len_or_pos = 0;        
	    data_id_status_para_ptr->data_value.data_num.val_uint_64  = DATA_NUM_INVALID;
	#ifdef DMA_DATA_ID_STR
    if(data_id_status_para_ptr->data_value.data_str != NULL_PTR)    		
	  {
         if(set_cur_data_status_reset_type != DATA_ID_RESET_INIT_STATUS)
         {
            memset(data_id_status_para_ptr->data_value.data_str, NULL_CHAR, data_id_status_para_ptr->max_allocated_data_len * sizeof(char) );
         }         
	  } 
  #else
    memset(data_id_status_para_ptr->data_value.data_str, NULL_CHAR, DATA_MAX_NUM_ALLOCATED_CHARS );
  #endif
	  case DATA_ID_RESET_APPEND_INPUT_STATUS_FLAG:
	    data_id_status_para_ptr->data_read_status.read_valid_terminator_char_flag = STATE_NO;
	    data_id_status_para_ptr->data_read_status.reach_max_alloc_read_chars_flag = STATE_NO; 
		data_id_status_para_ptr->data_read_status.read_and_ready_to_process_flag = STATE_NO; 		 
     break;
	 default:
    	system_status_flag = ERR_FORMAT_INVALID;
		 Error_or_Warning_Proc("02.06.02", ERROR_OCCURED, system_status_flag);
      return system_status_flag;	 
	}
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Data_IDs_Status

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.07

BUGS           :
-*------------------------------------------------------------*/
void Reset_Data_IDs_Status(void)
{
	 Set_Status_Reset_Data_IDs(DATA_ID_RESET_INIT_STATUS);
	 cur_data_id = DATA_ID_INVALID;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : Data_IDs_Set_Para

DESCRIPTION    :
								
INPUT          :

OUTPUT         : 

NOTE           : 

Func ID        : 02.08 

Bugs           :   
-*------------------------------------------------------------*/
uint8_t Data_IDs_Set_Para(void)
{
	uint8_t data_id = 0, ret_status, max_req_data_len;
	
	for(data_id = 0; data_id < NUM_DATA_IDS; ++data_id)
	{
	  	 if((ret_status = Appl_Data_Retrieve_Para(data_id, DATA_RETRIEVE_CTRL_MAX_REQ_DATA_CHARS, &max_req_data_len)) != SUCCESS)
		 {
		     system_status_flag = ERR_APPL_DATA_RETRIEVE;
		     Error_or_Warning_Proc("02.08.01", ERROR_OCCURED, system_status_flag);
             return system_status_flag; 
		 }
		 if((ret_status = Data_ID_Set_Para(data_id,  max_req_data_len)) != SUCCESS)
		 {
			 system_status_flag = ERR_DATA_ID_CONF;
			  Error_or_Warning_Proc("02.08.02", ERROR_OCCURED, system_status_flag);
			 break;
		 }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Data_ID_Set_Para

DESCRIPTION    :
								
INPUT          :

OUTPUT         : 

NOTE           : 

Func ID        : 02.09 

Bugs           :   
-*------------------------------------------------------------*/
uint8_t Data_ID_Set_Para(const uint8_t data_id, const uint8_t max_alloc_data_len)
{
	data_id_status_para_t *data_id_status_para_ptr = NULL_PTR;
	char data_read_terminator_char, ret_status;
	
	if(data_id >= NUM_DATA_IDS)
	{
		 system_status_flag = ERR_DATA_ID_EXCEED;
		  Error_or_Warning_Proc("02.09.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	data_id_status_para_ptr = data_id_status_para + data_id;
	// max_alloc_data_len  = req max allocated data len with appended a NULL_CHAR
	if(max_alloc_data_len > DATA_MAX_NUM_ALLOCATED_CHARS)
	{
				  data_id_status_para_ptr->max_allocated_data_len = 0;
				  system_status_flag = ERR_EXCEEDS_DATA_NUM_CHARS;
				   Error_or_Warning_Proc("02.09.02", ERROR_OCCURED, system_status_flag);
		      return system_status_flag;
	}
			data_id_status_para_ptr->data_id = data_id;	        
		    data_id_status_para_ptr->max_allocated_data_len = max_alloc_data_len;
		if((ret_status =  Appl_Data_Retrieve_Para(data_id, DATA_RETRIEVE_CTRL_DATA_READ_TERMINATOR_CHAR, &data_read_terminator_char)) != SUCCESS)
		{
		   system_status_flag = ERR_APPL_DATA_RETRIEVE;
		   Error_or_Warning_Proc("02.09.03", ERROR_OCCURED, system_status_flag);
           return system_status_flag; 
		}
		if(data_read_terminator_char == NULL_CHAR)
	    {
		      data_id_status_para_ptr->data_read_status.data_read_mode = DATA_ID_READ_MODE_TILL_ALLOC;				 
	    }
	    else
    	{
		     data_id_status_para_ptr->data_read_status.data_read_mode = DATA_ID_READ_MODE_TILL_TERMINATOR;
	    }
		
		#ifdef DMA_DATA_ID_STR
		if(data_id_status_para_ptr->data_value.data_str != NULL_PTR)
	    {
           system_status_flag = ERR_ALREADY_MEM_ALLOC;
		    Error_or_Warning_Proc("02.09.04", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	    }
        data_id_status_para_ptr->data_value.data_str = (char *) malloc(data_id_status_para_ptr->max_allocated_data_len * sizeof(char));	
        #endif	
		Reset_Data_ID_Status(data_id, DATA_ID_RESET_WHOLE_STATUS);      
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Power_Of

DESCRIPTION    :
								
INPUT          :

OUTPUT         : 

NOTE           : 

Func ID        : 02.10 

Bugs           :   
-*------------------------------------------------------------*/
uint32_t Power_Of(const uint8_t base, const uint8_t power )
{
    uint32_t power_val = 1;
    uint8_t i = 0;
  
    if(power == 0)
    {
       return power_val;
    }
    for(i = 1; i <= power; ++i)
    {
      power_val *= base;
    }
    return power_val;
}

#ifdef DELAY_BY_CLOCK
/*------------------------------------------------------------*
FUNCTION NAME  : Delay_MS

DESCRIPTION    :
								
INPUT          :

OUTPUT         : 

NOTE           : each for loop iteration takes 4 instruction cycles  

Func ID        : 02.11 

Bugs           : NOT_WORKING  
-*------------------------------------------------------------*/
void Delay_MS(const uint32_t max_elapsed_time_in_ms )
{
    uint32_t max_elapsed_inst_cycle = max_elapsed_time_in_ms * _XTAL_FREQ * (1000 / 4), elapsed_inst_cycle;
	
	for(elapsed_inst_cycle = 0; elapsed_inst_cycle < max_elapsed_inst_cycle; ++elapsed_inst_cycle)
	{
		   __asm("NOP");
	}
}	
#else
/*------------------------------------------------------------*
FUNCTION NAME  : Delay_MS

DESCRIPTION    :
								
INPUT          :

OUTPUT         : Cclk = 60MHz 

NOTE           :

Func ID        : 02.11 

Bugs           : 
-*------------------------------------------------------------*/
void Delay_MS(const uint32_t max_elapsed_time_in_ms )
{
	uint16_t x, elasped_time_in_ms;
	
	for(elasped_time_in_ms = 0; elasped_time_in_ms < max_elapsed_time_in_ms; ++elasped_time_in_ms)
	{
        for(x = 0; x < 6000; x++);    /* loop to generate 1 millisecond delay with Cclk = 60MHz */
	}
}	
#endif

/*------------------------------------------------------------*
FUNCTION NAME  : Delay_US

DESCRIPTION    :
								
INPUT          :

OUTPUT         : 

NOTE           :  each for loop iteration takes 4 instruction cycles

Func ID        : 02.12 

Bugs           : NOT_WORKING  
-*------------------------------------------------------------*/
void Delay_US(const uint32_t max_elapsed_time_in_us )
{
    uint32_t max_elapsed_inst_cycle = max_elapsed_time_in_us * (_XTAL_FREQ / 4) , elapsed_inst_cycle;
	
	for(elapsed_inst_cycle = 0; elapsed_inst_cycle < max_elapsed_inst_cycle; ++elapsed_inst_cycle)
	{
		   __asm("NOP");
	}
}	

/*------------------------------------------------------------*-
FUNCTION NAME  : Write_Bit_in_Data

DESCRIPTION     : write bit valve to data's bit, without affecting other bits of data .
                . 0 is the least bit pos and 7 is the most bit pos 

INPUT          : 

OUTPUT         : none

NOTE           : 

Func ID        : 02.13 

Bugs           :   
-*------------------------------------------------------------*/
uint8_t Write_Bit_in_Data( uint32_t *const data_ptr, const uint8_t bit_pos, const uint8_t set_bit_val )
{
     if(data_ptr == NULL_PTR)
	 {
		 system_status_flag = ERR_NULL_PTR;
		  Error_or_Warning_Proc("02.13.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }
	 switch(set_bit_val)
     {
		 case STATE_LOW:
		    Clear_Bit_in_Data(data_ptr, bit_pos );
         break;
         case STATE_HIGH:		 
            Set_Bit_in_Data(data_ptr, bit_pos);
		 break;
         default:
           system_status_flag = ERR_FORMAT_INVALID;
		    Error_or_Warning_Proc("02.13.02", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
     }
      return SUCCESS; 
}	

/*------------------------------------------------------------*
FUNCTION NAME  : Error_or_Warning_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : process to do when a error or warning has occured is not implemented.

Func ID        : 02.14  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Error_or_Warning_Proc(const char *const error_trace_str, const uint8_t warn_or_error_format, const uint32_t warning_or_error_code)
{
	uint8_t ret_status;	
	
	if((ret_status = Comm_Error_Proc(error_trace_str,warn_or_error_format, warning_or_error_code ))!= SUCCESS)
	{
		system_status_flag = ERR_PROC_ERROR;		
		return system_status_flag;
	}
	 internal_error_state = warn_or_error_format;
	switch(warn_or_error_format)
	{
		case WARNING_OCCURED:            
		 /*warning occured, process to do  */
        break;
		case ERROR_OCCURED:		  
		  /*error occured, process to do  */
		break;
        case FATAL_OCCURED:	         		
		/* fatal error occured, process to do  */ 
		break; 	    
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Comm_Error_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           :

Func ID        : 02.15 

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t Comm_Error_Proc(const char *const error_trace_str, const uint8_t warn_or_error_format, const uint32_t warning_or_error_code)
{
  #ifdef LCD_MOD_ENABLE 
    lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
  #endif
	const char warning_msg_disp[] = "WRN ", fatal_msg_disp[] = "FTL ", error_msg_disp[] = "ERR "; 
	disp_num_fmt_t disp_error_code;
	uint8_t error_lcd_ch_id = TRACE_LCD_CH_ID, error_uart_ch_id = TRACE_UART_CH_ID, ret_status;
	
  //SHOULD_CALL
  /* 
	if((ret_status = IO_Channel_Write(INTERNAL_ERROR_LED_IO_CH, STATE_HIGH))!= SUCCESS)
	{
		 system_status_flag = ERR_IO_CH_WRITE;		 
		 return system_status_flag;
	} */
	
	 #ifdef TRACE_ERROR
	 	 disp_error_code.sign_flag = STATE_NO;
		 disp_error_code.least_digits_flag = STATE_YES;
		 disp_error_code.left_format_flag = STATE_YES;
		 disp_error_code.num_digits = 3;
		 #ifdef LCD_MOD_ENABLE		 
		 /*   cur_lcd_status_ptr = lcd_status + error_lcd_ch_id;
            if((ret_status = LCD_Disp_Goto_XY(error_lcd_ch_id, ERROR_LINE_NUM, NUM_COL1)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
		    if((ret_status = LCD_Disp_Str(error_lcd_ch_id, error_trace_str)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
			
		    start_col_error_msg  = 1 + NUM_CHARS_TRACE_CODE;
	        start_col_blank_space = start_col_error_msg + strlen(error_msg_disp);
	        // (1) = 1 BLANK SPACE 
	        start_col_error_code = start_col_blank_space  + 1 ; 
		    if((ret_status = LCD_Disp_Goto_XY(error_lcd_ch_id, ERROR_LINE_NUM, start_col_blank_space)) != SUCCESS)
        {
          system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
        }
		    LCD_Write_Char(error_lcd_ch_id, ' ');			
		    LCD_Disp_Goto_XY(error_lcd_ch_id, ERROR_LINE_NUM, start_col_error_code);			
		    if((ret_status = LCD_Disp_Num(error_lcd_ch_id, DATA_TYPE_IN_DECIMAL, disp_error_code, warning_or_error_code)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}*/
        #endif	
		
        #ifdef UART_MOD_ENABLE
		    if((ret_status = UART_Transmit_Char(error_uart_ch_id, ENTER_CHAR)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
		    if((ret_status = UART_Transmit_Str(error_uart_ch_id, error_trace_str)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}				
		    if((ret_status = UART_Transmit_Char(error_uart_ch_id, ' ')) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
		    if((ret_status = UART_Transmit_Num(error_uart_ch_id, DATA_TYPE_IN_DECIMAL, disp_error_code, warning_or_error_code)) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
			if((ret_status = UART_Transmit_Char(error_uart_ch_id, ' ')) != SUCCESS)
			{
				system_status_flag = ERR_PROC_ERROR;		 
		        return system_status_flag;
			}
		#endif          		
	#endif
	switch(warn_or_error_format)
	{
		case WARNING_OCCURED:	
       #ifdef TRACE_ERROR  
	        /* #ifdef LCD_MOD_ENABLE
		        if((ret_status = LCD_Disp_Goto_XY(error_lcd_ch_id, ERROR_LINE_NUM, start_col_error_msg)) != SUCCESS)
				{
					system_status_flag = ERR_PROC_ERROR;		 
		            return system_status_flag;
				}					
	            if((ret_status = LCD_Disp_Str(error_lcd_ch_id, warning_msg_disp)) != SUCCESS)
				{
					system_status_flag = ERR_PROC_ERROR;		 
		            return system_status_flag;
				}
			    cur_lcd_status_ptr->cur_line_disp_data[ERROR_LINE_NUM] = WARNING_DISP;
            #endif */
			
			#ifdef UART_MOD_ENABLE
			    if((ret_status = UART_Transmit_Str(error_uart_ch_id, warning_msg_disp)) != SUCCESS)
				{
					system_status_flag = ERR_PROC_ERROR;		 
		            return system_status_flag;
				}
				if((ret_status = UART_Transmit_Char(error_uart_ch_id, ENTER_CHAR)) != SUCCESS)
				{					
					system_status_flag = ERR_PROC_ERROR;		 
		            return system_status_flag;
				}
			#endif
		#endif		 
        break;
		case ERROR_OCCURED:
		   #ifdef TRACE_ERROR  
	     /*  #ifdef LCD_MOD_ENABLE
		       if((ret_status = LCD_Disp_Goto_XY(error_lcd_ch_id,ERROR_LINE_NUM, start_col_error_msg)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag;
			   }				   
	           if((ret_status = LCD_Disp_Str(error_lcd_ch_id, error_msg_disp)) != SUCCESS)
			   {
				    system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag;
			   }
			   cur_lcd_status_ptr->cur_line_disp_data[ERROR_LINE_NUM] = ERROR_DISP;
			#endif */
          
            #ifdef UART_MOD_ENABLE
               if((ret_status = UART_Transmit_Str(error_uart_ch_id, error_msg_disp)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag; 
			   }
			   if((ret_status = UART_Transmit_Char(error_uart_ch_id, ENTER_CHAR)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag;
			   }
            #endif			   
		  #endif		 
		break;
        case FATAL_OCCURED:	            
		   #ifdef TRACE_ERROR 
	       /* #ifdef LCD_MOD_ENABLE
		       if((ret_status = LCD_Disp_Goto_XY(error_lcd_ch_id, ERROR_LINE_NUM, start_col_error_msg)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag;
			   }				   
	           if((ret_status = LCD_Disp_Str(error_lcd_ch_id, fatal_msg_disp)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag;
			   }
			   cur_lcd_status_ptr->cur_line_disp_data[ERROR_LINE_NUM] = FATAL_DISP;
			#endif */
			
			#ifdef UART_MOD_ENABLE
               if((ret_status = UART_Transmit_Str(error_uart_ch_id, fatal_msg_disp)) != SUCCESS)
			   {
				   system_status_flag = ERR_PROC_ERROR;		 
		           return system_status_flag; 
			   }
			   if((ret_status = UART_Transmit_Char(error_uart_ch_id, ENTER_CHAR)) != SUCCESS)
			   {
				    system_status_flag = ERR_PROC_ERROR;		 
		            return system_status_flag; 
			   }
            #endif
		 #endif	  
		break;
        default:
		/* warning invalid error or warning format*/	
          system_status_flag = ERR_FORMAT_INVALID;
		  return system_status_flag;         		    
	}
	return SUCCESS;
}
	
/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Data_Retrieve_Para

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           :

Func ID        : 02.16 

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Appl_Data_Retrieve_Para(const uint8_t data_id, const uint8_t retrieve_data_form, void *const retrive_data_ptr)
{
	const data_id_ctrl_para_t *data_id_ctrl_para_ptr;
	const data_id_status_para_t *data_id_status_para_ptr;
	int32_t *retrieve_data_int32_ptr;
	uint8_t *retrive_data_uint8_ptr;
	char *retrive_data_char_ptr;
	
	if(retrive_data_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("02.16.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	if(data_id >= NUM_DATA_IDS)
	{
		system_status_flag = ERR_DATA_ID_EXCEED;
		Error_or_Warning_Proc("02.16.02", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	data_id_ctrl_para_ptr = data_id_ctrl_para + data_id;
	data_id_status_para_ptr = data_id_status_para + data_id;
	switch(retrieve_data_form)
	{
		case DATA_RETRIEVE_CTRL_MAX_REQ_DATA_CHARS:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->max_req_data_len;
		break;
        case DATA_RETRIEVE_CTRL_MAX_NUM_DATA_READ_TRY:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->max_num_data_read_try;
        break;
		case DATA_RETRIEVE_CTRL_DATA_DISP_FORMAT:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_disp_format;
		break;
		case DATA_RETRIEVE_CTRL_OUTPUT_DATA_TYPE:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->output_data_type;
		break;
		case DATA_RETRIEVE_CTRL_INPUT_DATA_TYPE:
		    retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->input_data_type;
		break;
		case DATA_RETRIEVE_CTRL_INPUT_DATA_STREAM:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_input_stream_format;
		break;
		case DATA_RETRIEVE_CTRL_OUTPUT_DATA_STREAM:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_output_stream_format;
		break;
        case DATA_RETRIEVE_CTRL_TRANSMIT_DATA_STREAM:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_tx_stream_format;
		break;	
		case DATA_RETRIEVE_CTRL_RECEIVE_DATA_STREAM:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_rcv_stream_format;
		break;
		case DATA_RETRIEVE_CTRL_READ_NUM_CHAR_ENABLE_FLAG:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_read_num_char_enable_flag; 
		break;
		case DATA_RETRIEVE_CTRL_READ_SMALL_ENGLISH_ALPHA_CHAR_ENABLE_FLAG:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_read_small_english_alpha_char_enable_flag; 
		break;
		case DATA_RETRIEVE_CTRL_READ_BIG_ENGLISH_ALPHA_CHAR_ENABLE_FLAG:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_read_big_english_alpha_char_enable_flag; 
		break;
		case DATA_RETRIEVE_CTRL_READ_SPECIAL_CHAR_ENABLE_FLAG:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_read_special_char_enable_flag; 
		break;
		case DATA_RETRIEVE_CTRL_READ_CTRL_CHAR_ENABLE_FLAG:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_ctrl_para_ptr->data_read_ctrl_char_enable_flag; 
		break;
		case DATA_RETRIEVE_CTRL_DATA_READ_TERMINATOR_CHAR:
		    retrive_data_char_ptr = (char *) retrive_data_ptr;
		   *retrive_data_char_ptr = data_id_ctrl_para_ptr->data_read_terminator_char; 
		break;
		case DATA_RETRIEVE_STATUS_NUM_CHARS_READ:
		   retrive_data_uint8_ptr = (uint8_t *) retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_status_para_ptr->data_value.data_str_len_or_pos;		   
		break;
		case DATA_RETRIEVE_STATUS_DATA_STR:
		  retrive_data_char_ptr = (char *)retrive_data_ptr;
		  memcpy(retrive_data_char_ptr, data_id_status_para_ptr->data_value.data_str, data_id_status_para_ptr->data_value.data_str_len_or_pos); 
		  retrive_data_char_ptr[data_id_status_para_ptr->data_value.data_str_len_or_pos] = NULL_CHAR;
		break;
		case DATA_RETRIEVE_STATUS_DATA_CHAR:
		  retrive_data_char_ptr = (char *)retrive_data_ptr;
		  *retrive_data_char_ptr = data_id_status_para_ptr->data_value.data_str[data_id_status_para_ptr->data_value.data_str_len_or_pos];
		break;
		case DATA_RETRIEVE_STATUS_DATA_INT32:
		  retrieve_data_int32_ptr = (int32_t *)retrive_data_ptr;
		   *retrieve_data_int32_ptr = data_id_status_para_ptr->data_value.data_num.val_int_32[0];
		break;
		case DATA_RETRIEVE_STATUS_DATA_UINT8:
		   retrive_data_uint8_ptr = (uint8_t *)retrive_data_ptr;
		   *retrive_data_uint8_ptr = data_id_status_para_ptr->data_value.data_num.val_uint_8[0];
		break;
        default:
           system_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_Proc("02.16.03", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;	
	}
	return SUCCESS;
}	

/*------------------------------------------------------------*
FUNCTION NAME  : Next_Data_Conf_Parameter

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.17

Bugs           :   
-*------------------------------------------------------------*/
uint8_t Next_Data_Conf_Parameter(const uint8_t set_cur_data_id)
{
  static uint8_t prev_data_id = DATA_ID_INVALID;
	uint8_t ret_status;
  
  if(!(set_cur_data_id == DATA_ID_INVALID || set_cur_data_id <= NUM_DATA_IDS))
  {
      system_status_flag = ERR_CUR_DATA_ID_INVALID;
	  Error_or_Warning_Proc("02.17.01", ERROR_OCCURED, system_status_flag);
	  return system_status_flag;
  }  
   if(cur_data_id == set_cur_data_id) 
   {
	   return SUCCESS;
   }	   
	 prev_data_id = cur_data_id;			  
	 cur_data_id = set_cur_data_id;          
	 if((ret_status = Data_Dev_Src_Access_Oper_Func(prev_data_id)) != SUCCESS)
	 {
		 system_status_flag = ERR_NEXT_DATA_CONF;
		 Error_or_Warning_Proc("02.17.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }		 
    return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Set_Status_Reset_Data_IDs

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.18

BUGS           :
-*------------------------------------------------------------*/
uint8_t Set_Status_Reset_Data_IDs(const uint8_t set_data_status_reset_type)
{
	 uint8_t data_id, ret_status;
	
	 for(data_id = 0; data_id < NUM_DATA_IDS; ++data_id)
	 {
		  if((ret_status = Reset_Data_ID_Status(data_id, set_data_status_reset_type)) != SUCCESS)
		  {
			  system_status_flag = ERR_RESET_DATA_ID_STATUS;
		      Error_or_Warning_Proc("02.18.01", ERROR_OCCURED, system_status_flag);
		      return system_status_flag;
		  }			  
	 }	 
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Num_Convert

DESCRIPTION    : Convert integer number into octal, hex, etc.
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.19 

Bugs           :  
-*------------------------------------------------------------*/
char *Num_Convert(const uint32_t num_to_convert, const uint8_t base ) 
{ 
    const char hexa_char[]= "0123456789ABCDEF";
	static char buffer[NUM_CONVERT_BUFFER_LEN]; 
	char *num_in_str_ptr; 
	uint32_t cur_num_to_convert = num_to_convert;
	
	memset(buffer, NULL_CHAR, NUM_CONVERT_BUFFER_LEN);
	num_in_str_ptr = &buffer[NUM_CONVERT_BUFFER_LEN - 1]; 
	*num_in_str_ptr = '\0'; 	
	do 
	{ 
		*--num_in_str_ptr = hexa_char[cur_num_to_convert % base]; 
		cur_num_to_convert /= base; 
	}
	while(cur_num_to_convert != 0); 	
	return(num_in_str_ptr); 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Stream_Flush

DESCRIPTION    : Stream_Flush() operation is similar to fflush()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 02.20 

Bugs           :  
-*------------------------------------------------------------*/
int16_t Stream_Flush(stream_t *const stream_ptr)
{
	if(stream_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("02.20.01", ERROR_OCCURED, system_status_flag);
		return EOF;
	}
	memset(stream_ptr->stream_buffer, NULL_CHAR, STREAM_BUFFER_LEN);
	stream_ptr->num_chars_stream_buffer = 0;
	return SUCCESS;
}
	
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
