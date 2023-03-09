/**************************************************************************
   FILE          :    lcd.c
 
   PURPOSE       :   LCD library 
 
   AUTHOR        :  K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  CAUTION        :
	
  NOTE           :   
  
  CHANGE LOGS    :
  
  FILE ID        : 09  
	   
 **************************************************************************/

#include "main.h"

#ifdef LCD_MOD_ENABLE

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */
lcd_status_t lcd_status[NUM_OUTPUT_DEV_ID_LCD_CHS];

/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */
static uint8_t Check_LCD_Busy(const uint8_t lcd_ch_id);
static uint8_t Read_LCD_Command(const uint8_t lcd_ch_id, uint8_t *const lcd_read_command);

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.01

BUGS           :    
-*------------------------------------------------------------*/
uint8_t LCD_Init(const uint8_t lcd_ch_id, const uint8_t init_mode)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	io_config_t lcd_config;
	uint32_t lcd_func_command = 0x20;	
	uint8_t ret_status;
  
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.01.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;	
	if(cur_lcd_ctrl_ptr->max_config_lines_in_index > cur_lcd_ctrl_ptr->max_avail_lines_in_index || cur_lcd_ctrl_ptr->max_config_cols_in_index > cur_lcd_ctrl_ptr->max_avail_cols_in_index)
	{
		 system_status_flag = ERR_LCD_CONFIG_INVALID;
		  Error_or_Warning_Proc("09.01.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
     lcd_config.io_ch = cur_lcd_ctrl_ptr->base_io_ch_ctrl;
	 lcd_config.signal = PIN_SIG_DIGITAL;
	 lcd_config.func = IO_FUNC_GPIO;
	 lcd_config.dir = IO_DIR_OUTPUT;
	 lcd_config.state = STATE_LOW;
	 lcd_config.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	 lcd_config.port_pin_len = 3;	 
	 if((ret_status = IO_Channels_Func_Set(&lcd_config)) != SUCCESS)
	 {
		system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("09.01.03", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	 }
	  lcd_config.io_ch = cur_lcd_ctrl_ptr->base_io_ch_data;
	  if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_8_BITS)
	  {
          lcd_config.port_pin_len = 8;
	  }
      else
	  {		  
	      lcd_config.port_pin_len = 4;
	  }	  
	 if((ret_status = IO_Channels_Func_Set(&lcd_config)) != SUCCESS)
	 {
		system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("09.01.04", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	 }			
	/* wait for more than 15ms after LCD VSS rises to 4.5V, Busy Flag in LCD (BF) cannot be checked */
	SW_Time_Delay(MAX_ICOUNT_LCD_1500MS, MAX_JCOUNT_LCD_1500MS);
    Write_LCD_Command_NO_BF(lcd_ch_id, 0x30);
	/* wait for more than 4.1 ms, Busy Flag in LCD (BF) cannot be checked */
	SW_Time_Delay(MAX_ICOUNT_LCD_10MS, MAX_JCOUNT_LCD_10MS);
    Write_LCD_Command_NO_BF(lcd_ch_id, 0x30);
	/* wait for more than 100 us, Busy Flag in LCD (BF) cannot be checked */
	SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    Write_LCD_Command_NO_BF(lcd_ch_id, 0x30);
	SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_4_BITS)
	{		
	   Write_LCD_Command_NO_BF(lcd_ch_id, 0x20); 
	}	
    Write_Bit_in_Data(&lcd_func_command , 4, cur_lcd_ctrl_ptr->interface);
	if(cur_lcd_ctrl_ptr->max_config_lines_in_index + 1 == LINE_NUM_01)
	{
	    Write_Bit_in_Data(&lcd_func_command , 3, MAX_DISP_NUM_LINES_1);
	}
	else
	{
		 Write_Bit_in_Data(&lcd_func_command , 3, MAX_DISP_NUM_LINES_ALL);
	}
	Write_Bit_in_Data(&lcd_func_command , 2, cur_lcd_ctrl_ptr->font);
	LCD_Write_Command(lcd_ch_id, lcd_func_command);   	
    LCD_Write_Command(lcd_ch_id, 0x08);
	LCD_Clear_Screen(lcd_ch_id);
	LCD_Write_Command(lcd_ch_id, 0x06);  
	LCD_Write_Command(lcd_ch_id, 0x0E);
	lcd_status[lcd_ch_id].lcd_cur_disp_loc = BEGIN_LOC_LINE1;
	lcd_status[lcd_ch_id].lcd_cur_input_loc = BEGIN_LOC_LINE2;
	return SUCCESS;
} 
/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Clear_Screen

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.02

BUGS           :  
-*------------------------------------------------------------*/
uint8_t LCD_Clear_Screen(const uint8_t lcd_ch_id)
{
	LCD_Write_Command(lcd_ch_id, 0x01);
	//__delay_us(LCD_CLEAR_EXEC_IN_USEC);	
	SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	return SUCCESS;
}
 
/*------------------------------------------------------------*
FUNCTION NAME  : Write_LCD_Command_NO_BF

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.03

BUGS           :   
-*------------------------------------------------------------*/
uint8_t Write_LCD_Command_NO_BF(const uint8_t lcd_ch_id, const uint8_t lcd_cmd)
{
   lcd_ctrl_t *cur_lcd_ctrl_ptr;
   consucc_bit_t lcd_port_data;
   uint8_t ret_status, io_ch_D4_pin = 0;
  
   cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;   
   //LCD_RW_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 1, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.03.01", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }	   
   
   //LCD_RS_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.03.02", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   } 
   
   //__delay_us(LCD_ADDR_SETUP_IN_USEC);   
   SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
   //  LCD_EN_PIN = 1;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.03.03", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   } 	
   // __delay_us(LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC);
    SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	
	lcd_port_data.bits_len  = 4;
	lcd_port_data.write_or_config_consucc_val = lcd_cmd >> 4;
	if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_8_BITS)
	{
		io_ch_D4_pin = 4;
	}
	 lcd_port_data.start_bit_pos  = (cur_lcd_ctrl_ptr->base_io_ch_data + io_ch_D4_pin) % NUM_PINS_PER_PORT;
     if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data + io_ch_D4_pin, &lcd_port_data)) != SUCCESS) 
	 {
		  system_status_flag = ERR_LCD_WRITE;
		   Error_or_Warning_Proc("09.03.04", ERROR_OCCURED, system_status_flag);
	      return system_status_flag;
	 }
  
    //__delay_us(LCD_DATA_SETUP_IN_USEC);
	SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
   // LCD_EN_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.03.05", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }
    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
    SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    return SUCCESS;	     
}
/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Write_Command

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.04

BUGS           :   
-*------------------------------------------------------------*/
uint8_t LCD_Write_Command(const uint8_t lcd_ch_id, const uint8_t lcd_cmd)
{
    lcd_ctrl_t *cur_lcd_ctrl_ptr; 
	consucc_bit_t lcd_port_data;
	uint8_t ret_status;
  
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.04.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	
	if(cur_lcd_ctrl_ptr->check_bf == CHECK_BUSY_FLAG)
	{
		Check_LCD_Busy(lcd_ch_id); 
	}
    else
	{
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	
	}		
   //LCD_RW_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 1, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.04.02", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }	   
   //LCD_RS_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.04.03", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }    
     // __delay_us(LCD_ADDR_SETUP_IN_USEC); 
     SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	 
    //LCD_EN_PIN = 1;
	if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
    {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.04.04", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
    } 	
    //__delay_us(LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC);
	 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	 
	 if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_4_BITS)
	{
	    lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 4;
	    lcd_port_data.write_or_config_consucc_val = lcd_cmd >> 4;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.04.05", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
        //__delay_us(LCD_DATA_SETUP_IN_USEC);
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
       // LCD_EN_PIN = 0;
	    if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
        {
	        system_status_flag = ERR_LCD_WRITE;
			 Error_or_Warning_Proc("09.04.06", ERROR_OCCURED, system_status_flag);
	        return system_status_flag;
        } 	
	    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 	 
        SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
        //__delay_us(LCD_ADDR_SETUP_IN_USEC);   
		 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
        //LCD_EN_PIN = 1;
		if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
        {
	        system_status_flag = ERR_LCD_WRITE;
			 Error_or_Warning_Proc("09.04.07", ERROR_OCCURED, system_status_flag);
	        return system_status_flag;
        } 
		//  __delay_us(LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC);
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	    lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 4;
	    lcd_port_data.write_or_config_consucc_val = lcd_cmd & 0x0F;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.04.08", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
	}
    else
	{	
        lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 8;
	    lcd_port_data.write_or_config_consucc_val = lcd_cmd;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.04.09", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
	}
	 //__delay_us(LCD_DATA_SETUP_IN_USEC);
	 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    // LCD_EN_PIN = 0;
    if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
    {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.04.10", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
    }
    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
    SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    return SUCCESS;	      
}
/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Disp_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.05

BUGS           :  
-*------------------------------------------------------------*/
 uint8_t LCD_Disp_Char(const uint8_t lcd_ch_id, const char lcd_data)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr;
	lcd_status_t *cur_lcd_status_ptr;
	lcd_point_t cur_lcd_loc_point_num;
	consucc_bit_t lcd_port_data;
	uint8_t ret_status, cur_lcd_loc;
  
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.05.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	cur_lcd_status_ptr = lcd_status + lcd_ch_id;
	if(cur_lcd_status_ptr->data_till_end_point_flag == STATE_YES)
	{
    	if(cur_lcd_status_ptr->lcd_input_loc_flag == STATE_YES)
	    {
			cur_lcd_loc = cur_lcd_status_ptr->lcd_cur_input_loc;			
		}
		else
		{
			cur_lcd_loc = cur_lcd_status_ptr->lcd_cur_disp_loc;
		}
		LCD_From_Loc_to_XY(lcd_ch_id, cur_lcd_loc, &cur_lcd_loc_point_num);        			  
		if(cur_lcd_loc_point_num.line_num_or_index == cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index )
		{
			if(cur_lcd_loc_point_num.col_num_or_index + 1 > cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index)
			{
			      	system_status_flag = WARN_LCD_END_POINT_REACHED;
	            Error_or_Warning_Proc("09.05.02", WARNING_OCCURED, system_status_flag);
	            return system_status_flag;	
			}
		}			
	}
	if(cur_lcd_ctrl_ptr->check_bf == CHECK_BUSY_FLAG)
	{
		Check_LCD_Busy(lcd_ch_id); 
	}
	else
	{
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	
	}
	//LCD_RW_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 1, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.05.03", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }	   
   //LCD_RS_PIN = 1;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl, STATE_HIGH)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.05.04", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   } 	
	// __delay_us(LCD_ADDR_SETUP_IN_USEC); 
     SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	 
    //LCD_EN_PIN = 1;
	if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
    {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.05.05", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
    } 	
    //__delay_us(LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC);
	 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	 
	 if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_4_BITS)
	{
	    lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 4;
	    lcd_port_data.write_or_config_consucc_val = lcd_data >> 4;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.05.06", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
        //__delay_us(LCD_DATA_SETUP_IN_USEC);
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
       // LCD_EN_PIN = 0;
	    if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
        {
	        system_status_flag = ERR_LCD_WRITE;
			 Error_or_Warning_Proc("09.05.07", ERROR_OCCURED, system_status_flag);
	        return system_status_flag;
        } 	
	    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 	 
        SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
        //__delay_us(LCD_ADDR_SETUP_IN_USEC);   
		 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
        //LCD_EN_PIN = 1;
		if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
        {
	        system_status_flag = ERR_LCD_WRITE;
			 Error_or_Warning_Proc("09.05.08", ERROR_OCCURED, system_status_flag);
	        return system_status_flag;
        } 
		//  __delay_us(LCD_ENABLE_PULSE_BEFORE_WRITE_IN_USEC);
		SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	    lcd_port_data.write_or_config_consucc_val = lcd_data & 0x0F;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.05.09", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
	}
    else
	{	
        lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 8;
	    lcd_port_data.write_or_config_consucc_val = lcd_data;
		if((ret_status = Port_Write(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS) 
	    {
		   system_status_flag = ERR_LCD_WRITE;
		    Error_or_Warning_Proc("09.05.10", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
	    }
	}
	 //__delay_us(LCD_DATA_SETUP_IN_USEC);
	 SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    // LCD_EN_PIN = 0;
    if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
    {
	   system_status_flag = ERR_LCD_WRITE;
	    Error_or_Warning_Proc("09.05.11", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
    }
    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
    SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
    return SUCCESS;	      
}

/*------------------------------------------------------------*
FUNCTION NAME  : Check_LCD_Busy

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.06

BUGS           :   
-*------------------------------------------------------------*/
static uint8_t Check_LCD_Busy(const uint8_t lcd_ch_id)
{
  lcd_ctrl_t *cur_lcd_ctrl_ptr;
  io_config_t lcd_config;
   uint8_t lcd_read_command, ret_status;
	
    //LCD_RW_PIN = 1;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 1, STATE_HIGH)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_READ;
	    Error_or_Warning_Proc("09.06.01", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   }	   
   //LCD_RS_PIN = 0;
   if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl, STATE_LOW)) != SUCCESS)
   {
	   system_status_flag = ERR_LCD_READ;
	    Error_or_Warning_Proc("09.06.02", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
   } 		
    cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	lcd_config.io_ch = cur_lcd_ctrl_ptr->base_io_ch_data;
	lcd_config.signal = PIN_SIG_DIGITAL;
	lcd_config.func = IO_FUNC_GPIO;
	lcd_config.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_8_BITS)
	{
        lcd_config.port_pin_len = 8;
	}
    else
	{		  
	   lcd_config.port_pin_len = 4;
	}	
	 
	if((ret_status = IO_Ch_Func_Reset(&lcd_config)) != SUCCESS)
	{
	   system_status_flag = ERR_IO_CH_FUNC_RESET;
	    Error_or_Warning_Proc("09.06.03", ERROR_OCCURED, system_status_flag);
       return system_status_flag;		
	}
	lcd_config.dir = IO_DIR_INPUT;
	if((ret_status = IO_Channels_Func_Set(&lcd_config)) != SUCCESS)
	{
	  	system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("09.06.04", ERROR_OCCURED, system_status_flag);
	    return system_status_flag;
	}
	
	// send higher nibble first and higher data nibble of LCD PORT must be connected to LCD 	 
     /* busy flag = Bit 7 in LCD PORT, if busy flag == 1, wait till busy flag = 0, then any operation on LCD can be done */
    do
	{
		if((ret_status = Read_LCD_Command(lcd_ch_id, &lcd_read_command)) != SUCCESS)
		{
			system_status_flag = ERR_LCD_READ;
			 Error_or_Warning_Proc("09.06.05", ERROR_OCCURED, system_status_flag);
			return system_status_flag;
		}
    }
	while(Test_Bit_Is_Set_in_Data(&lcd_read_command, 7 ));	
	if((ret_status = IO_Ch_Func_Reset(&lcd_config)) != SUCCESS)
	{
	   system_status_flag = ERR_IO_CH_FUNC_RESET;
	    Error_or_Warning_Proc("09.06.06", ERROR_OCCURED, system_status_flag);
       return system_status_flag;		
	}
	lcd_config.dir = IO_DIR_OUTPUT;
	lcd_config.state = STATE_LOW;
	if((ret_status = IO_Channels_Func_Set(&lcd_config)) != SUCCESS)
	{
	  	system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("09.06.07", ERROR_OCCURED, system_status_flag);
	    return system_status_flag;
	}
	return SUCCESS;
   	   
}
/*------------------------------------------------------------*
FUNCTION NAME  : Read_LCD_Command

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.07

BUGS           : 
-*------------------------------------------------------------*/
static uint8_t Read_LCD_Command(const uint8_t lcd_ch_id, uint8_t *const lcd_read_command_ptr)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr;
	consucc_bit_t lcd_port_data;	 
	uint8_t ret_status;
  
	if(lcd_read_command_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.07.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	// __delay_us(LCD_ADDR_SETUP_IN_USEC);  
    SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);	 
    //LCD_EN_PIN = 1;
	if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
    {
	   system_status_flag = ERR_LCD_READ;
	    Error_or_Warning_Proc("09.07.02", ERROR_OCCURED, system_status_flag);
	   return system_status_flag;
    } 	        
	if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_4_BITS)
	{ 
        lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	    lcd_port_data.bits_len  = 4;
		lcd_port_data.read_or_test_consucc_val = 0;
	    if((ret_status = Port_Read(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS)
		{
			system_status_flag = ERR_LCD_READ;
			 Error_or_Warning_Proc("09.07.03", ERROR_OCCURED, system_status_flag);
			return system_status_flag;
		}        
	    //LCD_EN_PIN = 0;		
	    if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
        {
	       system_status_flag = ERR_LCD_READ;
		    Error_or_Warning_Proc("09.07.04", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
        } 	
	     *lcd_read_command_ptr = (uint8_t)lcd_port_data.read_or_test_consucc_val << 4;	 
	     //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
		  SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
		 
	     //__delay_us(LCD_ADDR_SETUP_IN_USEC);
		  SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
        //  LCD_EN_PIN = 1;
		 if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_HIGH)) != SUCCESS)
         {
	        system_status_flag = ERR_LCD_READ;
			 Error_or_Warning_Proc("09.07.05", ERROR_OCCURED, system_status_flag);
	        return system_status_flag;
         } 	
		 lcd_port_data.read_or_test_consucc_val = 0;
	     // read lower nibble and higher port nibble of LCD PORT is connected to LCD(DB4 - DB7)	
		 if((ret_status = Port_Read(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS)
		{
			system_status_flag = ERR_LCD_READ;
			 Error_or_Warning_Proc("09.07.06", ERROR_OCCURED, system_status_flag);
			return system_status_flag;
		}
         
	     //LCD_EN_PIN = 0;
		 if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
        {
	       system_status_flag = ERR_LCD_READ;
		    Error_or_Warning_Proc("09.07.07", ERROR_OCCURED, system_status_flag);
	       return system_status_flag;
        } 
		 
	    //__delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
		  SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	    *lcd_read_command_ptr = *lcd_read_command_ptr | ( (uint8_t)lcd_port_data.read_or_test_consucc_val & 0x0Fu);
	 }
	 else
	 {
		 lcd_port_data.start_bit_pos  = cur_lcd_ctrl_ptr->base_io_ch_data % NUM_PINS_PER_PORT;
	     lcd_port_data.bits_len  = 8;
		 lcd_port_data.read_or_test_consucc_val = 0;
	     if((ret_status = Port_Read(cur_lcd_ctrl_ptr->base_io_ch_data, &lcd_port_data)) != SUCCESS)
		 {
			system_status_flag = ERR_LCD_READ;
			 Error_or_Warning_Proc("09.07.08", ERROR_OCCURED, system_status_flag);
			return system_status_flag;
		 }
		  *lcd_read_command_ptr  = (uint8_t) lcd_port_data.read_or_test_consucc_val;
	      //LCD_EN_PIN = 0;
		  if((ret_status = IO_Channel_Write(cur_lcd_ctrl_ptr->base_io_ch_ctrl + 2, STATE_LOW)) != SUCCESS)
          {
	         system_status_flag = ERR_LCD_READ;
			  Error_or_Warning_Proc("09.07.09", ERROR_OCCURED, system_status_flag);
	         return system_status_flag;
          } 
		  // __delay_us(LCD_OTHER_INST_EXEC_IN_USEC); 
		  SW_Time_Delay(MAX_ICOUNT_LCD_1MS, MAX_JCOUNT_LCD_1MS);
	 }	 
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Disp_Str

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.08

BUGS           :   
-*------------------------------------------------------------*/
uint8_t LCD_Disp_Str(const uint8_t lcd_ch_id, const char *const char_ptr)
{ 
      char cur_char;
	    uint8_t num_char = 0;
  
	   if(char_ptr == NULL_PTR)
	   {
		   system_status_flag = ERR_NULL_PTR;
		    Error_or_Warning_Proc("09.08.01", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	   }
	   
	   if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	   {
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		    Error_or_Warning_Proc("09.08.02", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	   }
       while(*(char_ptr + num_char))
       {
		    cur_char = *(char_ptr + num_char);
            LCD_Disp_Char(lcd_ch_id, cur_char);
            ++num_char;        
       }
	   
	   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Set_Cur_Loc_LCD

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.10

BUGS           :  
-*------------------------------------------------------------*/
/* make sure that loc are within avail lcd loc limit */
void Set_Cur_Loc_LCD(const uint8_t lcd_ch_id, const char set_input_loc_flag, const uint8_t set_input_loc, const char set_disp_loc_flag, const uint8_t set_disp_loc)
{
    if(set_input_loc_flag == STATE_YES_IN_CHAR)  
       lcd_status[lcd_ch_id].lcd_cur_input_loc = set_input_loc;
   if(set_disp_loc_flag == STATE_YES_IN_CHAR)
      lcd_status[lcd_ch_id].lcd_cur_disp_loc = set_disp_loc; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Disp_Goto_XY

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : max 4 lines and 20 columns

Func ID        : 09.11

BUGS           :    
-*------------------------------------------------------------*/
uint8_t LCD_Disp_Goto_XY(const uint8_t lcd_ch_id, const uint8_t start_line_num, const uint8_t start_col_num)
{
	/* for us, lcd line starts from 1, but for uC, line index starts from 0 */
	/* for us, lcd col starts from 1, but for uC, col index starts from 0 */
	lcd_ctrl_t *cur_lcd_ctrl_ptr; 
	lcd_status_t *cur_lcd_status_ptr;
	lcd_point_t start_lcd_disp_point_index;
	
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		 Error_or_Warning_Proc("09.11.01", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	cur_lcd_status_ptr = lcd_status + lcd_ch_id;
	start_lcd_disp_point_index.line_num_or_index = start_line_num - 1;
	start_lcd_disp_point_index.col_num_or_index = start_col_num - 1; 	
   if(start_lcd_disp_point_index.line_num_or_index > cur_lcd_ctrl_ptr->max_config_lines_in_index || start_lcd_disp_point_index.col_num_or_index > cur_lcd_ctrl_ptr->max_config_cols_in_index )
   {
	   /* error due to invalid lcd DISP loc  */
	     system_status_flag = ERR_LCD_INVALID_LOC;
		  Error_or_Warning_Proc("09.11.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
   }
     switch(start_line_num)
	 {
		 case LINE_NUM_01:
		   cur_lcd_status_ptr->lcd_cur_disp_loc = BEGIN_LOC_LINE1;
		 break;
		 case LINE_NUM_02:
		   cur_lcd_status_ptr->lcd_cur_disp_loc = BEGIN_LOC_LINE2;
		 break;
		 case LINE_NUM_03:
		   cur_lcd_status_ptr->lcd_cur_disp_loc = BEGIN_LOC_LINE3;
		 break;
		 case LINE_NUM_04:
		   cur_lcd_status_ptr->lcd_cur_disp_loc = BEGIN_LOC_LINE4;
		 break;         		 
	 }	
      cur_lcd_status_ptr->lcd_cur_disp_loc += start_lcd_disp_point_index.col_num_or_index;
      LCD_Write_Command(lcd_ch_id, cur_lcd_status_ptr->lcd_cur_disp_loc);       
    return SUCCESS;   
} 

 /*------------------------------------------------------------*
FUNCTION NAME  : LCD_Input_Goto_XY

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : max 4 lines and 20 columns

Func ID        : 09.12

BUGS           :    
-*------------------------------------------------------------*/ 
uint8_t LCD_Input_Goto_XY(const uint8_t lcd_ch_id, const uint8_t start_line_num, const uint8_t start_col_num)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr; 
	lcd_status_t *cur_lcd_status_ptr;
	/* for us, lcd line starts from 1, but for uC, line index starts from 0 */
	/* for us, lcd col starts from 1, but for uC, col index starts from 0 */
	lcd_point_t start_lcd_input_point_index;    
	
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.12.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	cur_lcd_status_ptr = lcd_status + lcd_ch_id;
  start_lcd_input_point_index.line_num_or_index = start_line_num - 1;
	start_lcd_input_point_index.col_num_or_index = start_col_num - 1;
	if(start_lcd_input_point_index.line_num_or_index > cur_lcd_ctrl_ptr->max_config_lines_in_index || start_lcd_input_point_index.col_num_or_index > cur_lcd_ctrl_ptr->max_config_cols_in_index )
   {
	   /* error due to invalid lcd DISP loc  */
	     system_status_flag = ERR_LCD_INVALID_LOC;
		  Error_or_Warning_Proc("09.12.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
   }
	 switch(start_line_num)
	 {
		 case LINE_NUM_01:
		   cur_lcd_status_ptr->lcd_cur_input_loc = BEGIN_LOC_LINE1;
		 break;
		 case LINE_NUM_02:
		   cur_lcd_status_ptr->lcd_cur_input_loc = BEGIN_LOC_LINE2;
		 break;
		 case LINE_NUM_03:
		   cur_lcd_status_ptr->lcd_cur_input_loc = BEGIN_LOC_LINE3;
		 break;
		 case LINE_NUM_04:
		    cur_lcd_status_ptr->lcd_cur_input_loc = BEGIN_LOC_LINE4;
		 break; 		  
	 }
	 cur_lcd_status_ptr->lcd_cur_input_loc += start_lcd_input_point_index.col_num_or_index;
     LCD_Write_Command(lcd_ch_id, cur_lcd_status_ptr->lcd_cur_input_loc); 	 	   
  
    return SUCCESS;
} 

 /*------------------------------------------------------------*
FUNCTION NAME  : LCD_From_XY_To_Loc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           :  max 4 lines and 20 columns

Func ID        : 09.13

BUGS           :  
-*------------------------------------------------------------*/
uint8_t LCD_From_XY_To_Loc(const uint8_t lcd_ch_id, const lcd_point_t *const lcd_point_num_ptr, uint8_t *const lcd_loc_ptr)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr; 
	lcd_point_t lcd_point_index;
	
	if(lcd_loc_ptr == NULL_PTR || lcd_point_num_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.13.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}	
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.13.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	lcd_point_index.line_num_or_index = lcd_point_num_ptr->line_num_or_index - 1;
	lcd_point_index.col_num_or_index = lcd_point_num_ptr->col_num_or_index - 1;
	if(lcd_point_index.line_num_or_index > cur_lcd_ctrl_ptr->max_config_lines_in_index || lcd_point_index.col_num_or_index > cur_lcd_ctrl_ptr->max_config_cols_in_index )
   {
	     system_status_flag = ERR_LCD_INVALID_LOC;
		  Error_or_Warning_Proc("09.13.03", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
   }
     switch(lcd_point_num_ptr->line_num_or_index)
	 {
		 case LINE_NUM_01:
		   *lcd_loc_ptr = BEGIN_LOC_LINE1;
		   break;
		 case LINE_NUM_02:
		   *lcd_loc_ptr = BEGIN_LOC_LINE2;
		   break;
		 case LINE_NUM_03:
		   *lcd_loc_ptr= BEGIN_LOC_LINE3;
		   break;
		  case LINE_NUM_04:
		  *lcd_loc_ptr = BEGIN_LOC_LINE4;
		   break;		 
	 }	
      *lcd_loc_ptr = *lcd_loc_ptr + lcd_point_index.col_num_or_index;           
    return SUCCESS;   
} 

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_From_Loc_to_XY

DESCRIPTION    : loc_lcd's corrosponding line num and col num 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.14

BUGS           :   
-*------------------------------------------------------------*/
uint8_t LCD_From_Loc_to_XY(const uint8_t lcd_ch_id, const uint8_t loc_lcd, lcd_point_t *const lcd_point_num_ptr)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr; 
	uint8_t ret_status = SUCCESS;
	
	if(lcd_point_num_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.14.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		 system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.14.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;
	if(loc_lcd >= BEGIN_LOC_LINE1 && loc_lcd <= END_LOC_LINE1)
	{
	    lcd_point_num_ptr->line_num_or_index = LINE_NUM_01;
	    lcd_point_num_ptr->col_num_or_index = loc_lcd + 1 - BEGIN_LOC_LINE1;	
        goto LOC_TO_XY_EXIT;	    
	}
    if(loc_lcd >= BEGIN_LOC_LINE2 && loc_lcd <= END_LOC_LINE2)
	{
	    lcd_point_num_ptr->line_num_or_index	= LINE_NUM_02;
		lcd_point_num_ptr->col_num_or_index =  loc_lcd + 1 - BEGIN_LOC_LINE2; 
        goto LOC_TO_XY_EXIT;		
	}     
	if(loc_lcd >= BEGIN_LOC_LINE3 && loc_lcd <= END_LOC_LINE3)
	{
	    lcd_point_num_ptr->line_num_or_index = LINE_NUM_03;
	  	lcd_point_num_ptr->col_num_or_index = loc_lcd + 1 - BEGIN_LOC_LINE3;
        goto LOC_TO_XY_EXIT; 		
	}
    if(loc_lcd >= BEGIN_LOC_LINE4 && loc_lcd <= END_LOC_LINE4)
	{
		lcd_point_num_ptr->line_num_or_index = LINE_NUM_04;
		lcd_point_num_ptr->col_num_or_index = loc_lcd + 1 - BEGIN_LOC_LINE4;
		goto LOC_TO_XY_EXIT;
	}
    system_status_flag = ERR_LCD_INVALID_LOC; 
	Error_or_Warning_Proc("09.14.03", ERROR_OCCURED, system_status_flag);
	ret_status = system_status_flag;
 	LOC_TO_XY_EXIT: 
	if(ret_status == SUCCESS)
	{
		if((lcd_point_num_ptr->line_num_or_index - 1) > cur_lcd_ctrl_ptr->max_config_lines_in_index || (lcd_point_num_ptr->col_num_or_index - 1) > cur_lcd_ctrl_ptr->max_config_cols_in_index )
		{
			 system_status_flag = ERR_LCD_INVALID_LOC; 
			   Error_or_Warning_Proc("09.14.04", ERROR_OCCURED, system_status_flag);
	         return system_status_flag;
		}
	}
    return ret_status;   
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Write

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.15

BUGS           :   
-*------------------------------------------------------------*/
uint8_t LCD_Write(const uint8_t lcd_ch_id, const void *const lcd_rcv_disp_num_ptr)
{
	data_write_ctrl_ptr_t *lcd_data_write_ptr;	
    lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	lcd_disp_ctrl_t *lcd_disp_ctrl_ptr; 
	lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
	char retrieved_lcd_data_str[DATA_MAX_NUM_ALLOCATED_CHARS], *cur_lcd_disp_str;
	int32_t lcd_disp_num_int32;
	uint8_t ret_status = SUCCESS, output_data_type ;    
	
    if(lcd_rcv_disp_num_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.15.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
    if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		    Error_or_Warning_Proc("09.15.02", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	}		
	cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id;   
	cur_lcd_status_ptr = lcd_status + lcd_ch_id;
	if(  cur_lcd_ctrl_ptr->access_flag != STATE_YES) 
	{  
        system_status_flag = WARN_CUR_DATA_ID_DEV_NO_ACCESS;
		 Error_or_Warning_Proc("09.15.03", ERROR_OCCURED, system_status_flag);
        return  system_status_flag;
	}		 
	   if( cur_lcd_ctrl_ptr->enable_flag != STATE_YES) 
	   {  
          system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;
		   Error_or_Warning_Proc("09.15.04", ERROR_OCCURED, system_status_flag);
          return  system_status_flag;
	   }     
     lcd_data_write_ptr  = (data_write_ctrl_ptr_t *)lcd_rcv_disp_num_ptr;
	 lcd_disp_ctrl_ptr = &(((dev_write_ctrl_para_t *)lcd_data_write_ptr->dev_output_ctrl_para_ptr)->lcd_disp_para);
     if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_CTRL_OUTPUT_DATA_TYPE, &output_data_type)) != SUCCESS)
	 {
		  system_status_flag = ERR_APPL_DATA_RETRIEVE;
		  Error_or_Warning_Proc("09.15.05", ERROR_OCCURED, system_status_flag);
		  return system_status_flag; 
	 }
     if(lcd_disp_ctrl_ptr->use_start_point_flag == STATE_YES)
	 {		 
	     LCD_Disp_Goto_XY(lcd_ch_id, lcd_disp_ctrl_ptr->start_point_num.line_num_or_index, lcd_disp_ctrl_ptr->start_point_num.col_num_or_index);
		 lcd_disp_ctrl_ptr->use_start_point_flag = STATE_NO;
	 }
	 cur_lcd_status_ptr->data_till_end_point_flag = lcd_disp_ctrl_ptr->data_till_end_point_flag;
	 switch(output_data_type)
	 {
		case DATA_TYPE_IN_STR:
		   switch(lcd_disp_ctrl_ptr->use_data_status_flag)
		   {
		      case STATE_YES:
		         if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_STR, retrieved_lcd_data_str )) != SUCCESS)
		         {
			    	  system_status_flag = ERR_APPL_DATA_RETRIEVE;
				      Error_or_Warning_Proc("09.15.06", ERROR_OCCURED, system_status_flag);
				      return system_status_flag; 
		         }
				 cur_lcd_disp_str = retrieved_lcd_data_str;
              break;
              case STATE_NO:
                 cur_lcd_disp_str = lcd_disp_ctrl_ptr->lcd_disp_str;
              break;
		   }			  
		   LCD_Disp_Str(lcd_ch_id, cur_lcd_disp_str);
		break;
		case DATA_TYPE_IN_CHAR:
		   switch(lcd_disp_ctrl_ptr->use_data_status_flag)
		   {
		      case STATE_YES:
		         if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_CHAR, retrieved_lcd_data_str )) != SUCCESS)
		         {
			    	  system_status_flag = ERR_APPL_DATA_RETRIEVE;
				      Error_or_Warning_Proc("09.15.07", ERROR_OCCURED, system_status_flag);
				      return system_status_flag; 
		         }
				 cur_lcd_disp_str = retrieved_lcd_data_str;
              break;
              case STATE_NO:
                 cur_lcd_disp_str = lcd_disp_ctrl_ptr->lcd_disp_str;
              break;
		   }	
		   LCD_Disp_Char(lcd_ch_id, *cur_lcd_disp_str);		  
		break;
		case DATA_TYPE_IN_BINARY:
		case DATA_TYPE_IN_HEXA:
		case DATA_TYPE_IN_DECIMAL:
		   if(lcd_disp_ctrl_ptr->disp_num_fmt.num_digits <= 0 || lcd_disp_ctrl_ptr->disp_num_fmt.num_digits > MAX_LCD_DIGITS )
		   {
			  system_status_flag = ERR_LCD_DISP_FORMAT;
			  Error_or_Warning_Proc("09.15.08", ERROR_OCCURED, system_status_flag);
			  return system_status_flag;
		   }
		   switch(lcd_disp_ctrl_ptr->use_data_status_flag)
		   {
		      case STATE_YES:
		         if((ret_status = Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_INT32, &lcd_disp_num_int32 )) != SUCCESS)
		         {
			    	  system_status_flag = ERR_APPL_DATA_RETRIEVE;
				      Error_or_Warning_Proc("09.15.09", ERROR_OCCURED, system_status_flag);
				      return system_status_flag; 
		         }				
              break;
              case STATE_NO:
                 lcd_disp_num_int32 = lcd_disp_ctrl_ptr->lcd_disp_num;
              break;
		   }	
		   if((ret_status = LCD_Disp_Num(lcd_ch_id, output_data_type, lcd_disp_ctrl_ptr->disp_num_fmt, lcd_disp_num_int32)) != SUCCESS)
		   {
			  system_status_flag = ERR_LCD_WRITE;
			   Error_or_Warning_Proc("09.15.10", ERROR_OCCURED, system_status_flag);
			  return system_status_flag;
		   }        			
         break;
		 default:
		   system_status_flag = ERR_DATA_TYPE_INVALID;
		    Error_or_Warning_Proc("09.15.11", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Disp_Num

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : only display least numeric digits and from left to right display is implemented. 

Func ID        : 09.16

BUGS           :   
-*------------------------------------------------------------*/
uint8_t LCD_Disp_Num(const uint8_t lcd_ch_id, const uint8_t num_data_type, const disp_num_fmt_t disp_num_fmt, const int32_t lcd_num)
{
	 uint32_t base_power_val, lcd_disp_num;
	 int16_t place_val;
     uint8_t base, place_digit[MAX_LCD_DIGITS];     
	 char hex_data[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; 
	 
	if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	{
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		    Error_or_Warning_Proc("09.16.01", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;
	}
	if(disp_num_fmt.num_digits <= 0 || disp_num_fmt.num_digits > MAX_LCD_DIGITS )
	{
		system_status_flag = ERR_LCD_DISP_FORMAT;
		 Error_or_Warning_Proc("09.16.02", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
     switch(num_data_type)
	 {
		case DATA_TYPE_IN_BINARY:
           base = 2;
	       if(lcd_num < 0)
		   {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	LCD_Disp_Char(lcd_ch_id, '-');
				 }			    
			}
			else
			{
			   if(disp_num_fmt.sign_flag == STATE_YES)
		       {
			    	LCD_Disp_Char(lcd_ch_id, '+');
			   }
		    }		
           lcd_disp_num = lcd_num;   		   
		   LCD_Disp_Str(lcd_ch_id, "0b");
       break;			
	   case DATA_TYPE_IN_HEXA:
		   base = 16;
	       if(lcd_num < 0)
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	LCD_Disp_Char(lcd_ch_id, '-');					
				 }	
				 lcd_disp_num = -lcd_num; 
			 }
			 else
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	LCD_Disp_Char(lcd_ch_id, '+');					
				 }
				 lcd_disp_num = lcd_num; 
			 }           			
			LCD_Disp_Str(lcd_ch_id, "0x");
        break;			
		case DATA_TYPE_IN_DECIMAL:
		    base = 10;
		    if(lcd_num < 0)
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	LCD_Disp_Char(lcd_ch_id, '-');
				 }
			    lcd_disp_num = -lcd_num; 
			 }
			 else
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	LCD_Disp_Char(lcd_ch_id, '+');
				 }
			  	 lcd_disp_num = lcd_num;  
			 }	
		break;
		default:
		  system_status_flag = ERR_LCD_DISP_FORMAT;
		   Error_or_Warning_Proc("09.16.03", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	}
	if(disp_num_fmt.least_digits_flag == STATE_YES && disp_num_fmt.left_format_flag == STATE_YES)
	{			
	       base_power_val =  Power_Of(base, disp_num_fmt.num_digits);
		   for(place_val = disp_num_fmt.num_digits - 1; place_val >= 0; --place_val)
		   {
		      lcd_disp_num %= base_power_val;
		      base_power_val /= base; 
		      place_digit[place_val] = lcd_disp_num /base_power_val;
		      LCD_Disp_Char(lcd_ch_id, hex_data[place_digit[place_val]]);            
		  } 
	}
    else
	{
		//only display least numeric digits and from left to right display is implemented
		system_status_flag = ERR_LCD_DISP_FORMAT;
		 Error_or_Warning_Proc("09.16.04", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}		
    return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Disable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.17  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_Disable(const uint8_t lcd_ch_id)
{
	 lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	
	 if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("09.17.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id ;  
     cur_lcd_ctrl_ptr->enable_flag = STATE_NO;
	 
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Enable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.18  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_Enable(const uint8_t lcd_ch_id)
{
      lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	
	 if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("09.18.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id ;  
     cur_lcd_ctrl_ptr->enable_flag = STATE_YES;
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Disable_All_LCDs

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.19  

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Disable_All_LCDs(void)
{
	 uint8_t lcd_ch_id =0;
	
	 for(lcd_ch_id = 0; lcd_ch_id < NUM_OUTPUT_DEV_ID_LCD_CHS; ++lcd_ch_id)
	 {
		   LCD_Disable(lcd_ch_id);
	 }
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Allow_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.20

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_Allow_Access(const uint8_t lcd_ch_id)
{
	 lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	
	 if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("09.20.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id ;   
     cur_lcd_ctrl_ptr->access_flag = STATE_YES;
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_No_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.21 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_No_Access(const uint8_t lcd_ch_id)
{
	 lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	
	 if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("09.21.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id ;   
     cur_lcd_ctrl_ptr->access_flag = STATE_NO;
     LCD_Disable(lcd_ch_id);
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_DeInit

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.22 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_DeInit(const uint8_t lcd_ch_id, const uint8_t deinit_mode)
{
	 io_config_t lcd_unconfig;
	 lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	 uint8_t ret_status = SUCCESS;
	 
	 if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_Proc("09.22.01", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
	 LCD_No_Access(lcd_ch_id);
	 cur_lcd_ctrl_ptr = lcd_ctrl + lcd_ch_id ; 
	 lcd_unconfig.io_ch  = cur_lcd_ctrl_ptr->base_io_ch_ctrl;	 
	 lcd_unconfig.port_pin_len = 3;
	 lcd_unconfig.func = IO_FUNC_GPIO;
	 lcd_unconfig.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	 if((ret_status = IO_Ch_Func_Reset(&lcd_unconfig)) != SUCCESS)
	 {
		system_status_flag = ERR_IO_CH_FUNC_RESET;
		  Error_or_Warning_Proc("09.22.02", ERROR_OCCURED, system_status_flag);
        return system_status_flag;		
	 }	 
	 lcd_unconfig.io_ch  = cur_lcd_ctrl_ptr->base_io_ch_data;
	 if(cur_lcd_ctrl_ptr->interface == DATA_INTERFACE_8_BITS)
	 {
	      lcd_unconfig.port_pin_len = 8;
	 }
	 else
	 {
		  lcd_unconfig.port_pin_len = 4;
	 }
	 if((ret_status = IO_Ch_Func_Reset(&lcd_unconfig)) != SUCCESS)
	 {
		system_status_flag = ERR_IO_CH_FUNC_RESET;
		  Error_or_Warning_Proc("09.22.03", ERROR_OCCURED, system_status_flag);
        return system_status_flag;		
	 }
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_LCD_Cur_Data_Status_Para

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.23 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t Reset_LCD_Cur_Data_Status_Para(uint8_t lcd_ch_id)
{
   lcd_status_t *cur_lcd_status_ptr = NULL_PTR;	 
   
    if(lcd_ch_id >= NUM_OUTPUT_DEV_ID_LCD_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_Proc("09.23.02", ERROR_OCCURED, system_status_flag);
		  return system_status_flag;
	 }
  cur_lcd_status_ptr = lcd_status +  lcd_ch_id;
  cur_lcd_status_ptr->cur_data_lcd_para.start_loc = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.end_loc = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.line_num_or_index = DATA_INVALID;
  cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.col_num_or_index = DATA_INVALID;
   
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Cur_Data_By_NumChars_Calc_Para

DESCRIPTION    : calc cur input data parameters by using given max num of chars from start cur data input line and start cur data input col.  
  If conf max num of chars for cur input's data > max avail num of chars, then allocate cur data input max num of chars = max avail num of chars,
  whereas max avail num of chars = num of chars from cur input's data start line and cur input's data start column num till end of configure lines and cols,
  else  then allocate num of chars for input data =  given conf max num of chars for cur input's data 
								
INPUT          : 

OUTPUT         : 

NOTE           : FIND MAX loc at which data can be entered from data_start_loc and max num of chars can be entered from data_start_loc,
	             max loc's line num and max loc's column num

Func ID        : 09.24 

Bugs           :  
-*------------------------------------------------------------*/	
uint8_t LCD_Cur_Data_By_NumChars_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr) 
{
	 lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	 lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
	 uint8_t max_config_num_chars_left_from_next_line,  max_avail_num_chars_start_line, max_avail_num_chars_from_next_line,
     	   max_avail_data_numchars, ret_status = SUCCESS; 	
	
	if(lcd_point_config_ptr == NULL_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		return system_status_flag;
	}
    Reset_LCD_Cur_Data_Status_Para(lcd_ch_id);
	cur_lcd_ctrl_ptr = lcd_ctrl +  lcd_ch_id;
	cur_lcd_status_ptr = lcd_status +  lcd_ch_id;
	
   if(lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index > (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1) || lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index > (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1)) 
   { 	  
        /* error invalid lcd loc not within limit */ 
        system_status_flag = ERR_LCD_INVALID_LOC;		
		Error_or_Warning_Proc("09.24.01", ERROR_OCCURED, system_status_flag);
        return system_status_flag;		
   }		
        max_avail_num_chars_start_line = (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) + 1 - lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;		
		max_avail_num_chars_from_next_line = ((cur_lcd_ctrl_ptr->max_config_lines_in_index + 1) - lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index) * (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
		
		if(lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index == (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1))
		{
			max_avail_data_numchars = max_avail_num_chars_start_line;
		}	
		else
		{	
		    max_avail_data_numchars = max_avail_num_chars_start_line + max_avail_num_chars_from_next_line; 
		}	
		if(max_avail_data_numchars < lcd_point_config_ptr->max_num_chars_config )
		{
			cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = max_avail_data_numchars; 
			/*error: exceeds max_avail_data_numchars < lcd_point_config_ptr->max_num_chars_config */				
			 cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index = (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1);
			 cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index = (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
			 ret_status = WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;
			 Error_or_Warning_Proc("09.24.02", WARNING_OCCURED, ret_status);
		}
        else
        {
			cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = lcd_point_config_ptr->max_num_chars_config;
			if(max_avail_num_chars_start_line >= lcd_point_config_ptr->max_num_chars_config)
			{
				 cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
				 cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index  = lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index + lcd_point_config_ptr->max_num_chars_config - 1;
            }
            else
			{				
		    	max_config_num_chars_left_from_next_line = lcd_point_config_ptr->max_num_chars_config - max_avail_num_chars_start_line;
			    cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index = (max_config_num_chars_left_from_next_line / (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1)) + 1 + lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
			    cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index = max_config_num_chars_left_from_next_line % ((cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) + 1);
			}
			ret_status = SUCCESS;
        }
        LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.end_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.end_loc);
		cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.col_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;
		cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.line_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
		LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.start_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.start_loc);
        		
   return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Cur_Data_By_XY_Calc_Para

DESCRIPTION    : calc cur input data parameters by using given cur data input start line, 
   cur data input start column,  next data start line, and next data start column. 
   XY(Cur input data's end line, Cur input data's end column) = previous pos of XY( next data start line, and next data start column).
   Max configured cur input's data max input chars =  num of chars from   XY(Cur input data's start line, Cur input data's start column) 
   to XY(Cur input data's end line, cur input data's end column).
   Max avail chars = num of chars from cur data input start line and cur data input start column, till end of configure max lines and configure max cols. 
   If max configured cur input's data max input chars > max avail chars, then   allocate max num of chars for cur input data = max avail chars,
   else, allocate max num of chars for input data = max configured cur input's data max input chars.
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 09.25 

Bugs           :  
-*------------------------------------------------------------*/		
uint8_t LCD_Cur_Data_By_XY_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr) 
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
	uint8_t  max_avail_num_chars_start_line,  num_chars_inbetween_start_end_lines, data_end_line_num,  data_end_col_num, ret_status = SUCCESS;	
	
	if(lcd_point_config_ptr == NULL_PTR )
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.25.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	 Reset_LCD_Cur_Data_Status_Para(lcd_ch_id);
	 cur_lcd_ctrl_ptr = lcd_ctrl +  lcd_ch_id;
	 cur_lcd_status_ptr = lcd_status +  lcd_ch_id;
   if(!( (((lcd_point_config_ptr->next_data_start_point_num.line_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1)) && (lcd_point_config_ptr->next_data_start_point_num.col_num_or_index <= (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1)) )  ||  
        ((lcd_point_config_ptr->next_data_start_point_num.line_num_or_index <=  (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1 + 1)) && lcd_point_config_ptr->next_data_start_point_num.col_num_or_index == COL_NUM_01 )) && 
       (lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1)) && (lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index <= (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) )))
   {
	   /* error invalid lcd loc not within limit */ 	 
        system_status_flag = ERR_LCD_INVALID_LOC;
		 Error_or_Warning_Proc("09.25.02", ERROR_OCCURED, system_status_flag);
        return system_status_flag;	
   }
      if(lcd_point_config_ptr->next_data_start_point_num.col_num_or_index == COL_NUM_01)
	  {
		  data_end_col_num = (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
		  data_end_line_num = lcd_point_config_ptr->next_data_start_point_num.line_num_or_index - 1;
	  }
      else
      {
		  data_end_col_num = lcd_point_config_ptr->next_data_start_point_num.col_num_or_index - 1;
		  data_end_line_num = lcd_point_config_ptr->next_data_start_point_num.line_num_or_index;
	  }	
	  
	  if(!( (lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index == data_end_line_num && lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index < data_end_col_num )|| lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index < data_end_line_num  ))
	  {
           /* error invalid start data input loc > end data input loc */ 
        	  system_status_flag = ERR_LCD_INVALID_LOC;
		     Error_or_Warning_Proc("09.25.03", ERROR_OCCURED, system_status_flag);
           return system_status_flag;	 
      }
         cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.col_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;
	     cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.line_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
	     LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.start_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.start_loc);
         if(data_end_line_num > (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1))
         {
			cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1);
			/* warning : end line data input exceed configured max line  */
				 ret_status =  WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;	
			  Error_or_Warning_Proc("09.25.03", WARNING_OCCURED, ret_status);
	     }
         else	
			cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  data_end_line_num; 
		
		if(data_end_col_num > (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1))
         {
			cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
			/* warning : end col data input exceed configured max col  */
		    ret_status =  WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;		
			 Error_or_Warning_Proc("09.25.03", WARNING_OCCURED, ret_status);
	     }
        else	
			cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  data_end_col_num; 
		if(lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index < data_end_line_num)
		{
			num_chars_inbetween_start_end_lines = (cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index - lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index - 1) * (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
			max_avail_num_chars_start_line =  (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) + 1 - lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;	
			cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = num_chars_inbetween_start_end_lines + max_avail_num_chars_start_line + cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index; 
		}
        else
        {	          
			cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc =  data_end_col_num - lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index + 1;
        }			
		LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.end_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.end_loc);
		return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Cur_Data_By_NumChars_and_XY_Calc_Para

DESCRIPTION    : calc cur input data parameters by using given cur data input start line, 
  cur data input start column, next data start line, next data start column, and conf num of chars for cur input data. 
  XY(Cur input data's end line, Cur input data's end column) = previous pos of XY( next data start line, and next data start column).
  Max avail num of chars = num of chars from cur data input start line and cur data input start column, till XY(Cur input data's end line, cur input data's end column).
  If conf max num of chars for cur input data > max avail num of chars, then allocate num of chars for cur input data = max avail num of chars, 
	else max allocated data = given configure num of chars for cur input data.
								
INPUT          : 

OUTPUT         : 

NOTE           : (NUM_CHARS_DATA = GIVEN_CHARS_MAX_XY)

Func ID        : 09.26 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_Cur_Data_By_NumChars_and_XY_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr)
{
	lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR; 
	lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
    uint8_t max_avail_num_chars_start_line, config_max_num_chars_from_next_line, 
    num_chars_inbetween_start_end_lines, data_end_line_num,  data_end_col_num, cur_data_max_avail_numchars, ret_status = SUCCESS;
	
	if(lcd_point_config_ptr == NULL_PTR )
	{
		system_status_flag = ERR_NULL_PTR;
		 Error_or_Warning_Proc("09.26.01", ERROR_OCCURED,system_status_flag );
		return system_status_flag;
	}
	Reset_LCD_Cur_Data_Status_Para(lcd_ch_id);
	cur_lcd_ctrl_ptr = lcd_ctrl +  lcd_ch_id;
	cur_lcd_status_ptr = lcd_status +  lcd_ch_id;
    if(!(( ((lcd_point_config_ptr->next_data_start_point_num.line_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1)) && (lcd_point_config_ptr->next_data_start_point_num.col_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1)) )  ||  \
        ((lcd_point_config_ptr->next_data_start_point_num.line_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1 + 1) && lcd_point_config_ptr->next_data_start_point_num.col_num_or_index == COL_NUM_01 ))) && \
      ( lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index <= (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1) && lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index <= (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1))) )	 
    {
		/* error invalid lcd loc not within limit */ 
        system_status_flag = ERR_LCD_INVALID_LOC;
		 Error_or_Warning_Proc("09.26.02", ERROR_OCCURED, system_status_flag);
        return system_status_flag;	
	}
		
		if(lcd_point_config_ptr->next_data_start_point_num.col_num_or_index == COL_NUM_01)
	     {
		    data_end_col_num = (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
		    data_end_line_num = lcd_point_config_ptr->next_data_start_point_num.line_num_or_index - 1;
	     }
         else
         {
		    data_end_col_num = lcd_point_config_ptr->next_data_start_point_num.col_num_or_index - 1;
		    data_end_line_num = lcd_point_config_ptr->next_data_start_point_num.line_num_or_index;
	     }
	     if(!( (lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index == data_end_line_num && 
		    lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index < data_end_col_num )|| 
			lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index < data_end_line_num  ))
		 {
			 /* error invalid start data input loc > end data input loc */
             system_status_flag = ERR_LCD_INVALID_LOC;
		      Error_or_Warning_Proc("09.26.03", ERROR_OCCURED, system_status_flag);
             return system_status_flag;	
		 }
			if(data_end_line_num > (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1))
            {
		    	cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  (cur_lcd_ctrl_ptr->max_config_lines_in_index + 1);
		    	/* warning : end line data input exceed configured max line  */
				ret_status = WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;
			     Error_or_Warning_Proc("09.26.04", WARNING_OCCURED, ret_status);		    		
	        }
            else
			{				
		    	cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  data_end_line_num; 
			}
		    if(data_end_col_num > (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1))
            {
		    	cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
		    	/* warning : end col data input exceed configured max col  */		       
			    ret_status =  WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;
				 Error_or_Warning_Proc("09.26.05", WARNING_OCCURED, ret_status);
	        }
            else
			{				
		    	cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  data_end_col_num; 
			}
		    cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.col_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;
		    cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.line_num_or_index = lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
		    LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.start_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.start_loc); 
			
			if(lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index < data_end_line_num)
		    {
		    	num_chars_inbetween_start_end_lines = (cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index - lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index - 1) * (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1);
		    	max_avail_num_chars_start_line =  (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) + 1 - lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index;	
		    	cur_data_max_avail_numchars = num_chars_inbetween_start_end_lines + max_avail_num_chars_start_line + cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index;                					
		    }
            else
            {	          
			    cur_data_max_avail_numchars =  data_end_col_num - lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index + 1;
            }
            if(lcd_point_config_ptr->max_num_chars_config > cur_data_max_avail_numchars)
			{
			   /* warning : end col data input exceed configured max col  */
		        cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = cur_data_max_avail_numchars;
				cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  data_end_line_num;
				cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  data_end_col_num;
				ret_status =  WARN_LCD_ALLOC_LESS_THAN_CONFIG_CHARS;
				Error_or_Warning_Proc("09.26.06", WARNING_OCCURED, ret_status);	
			}
            else
			{
				cur_lcd_status_ptr->cur_data_lcd_para.max_num_chars_alloc = lcd_point_config_ptr->max_num_chars_config;
			    if(lcd_point_config_ptr->max_num_chars_config < max_avail_num_chars_start_line)	
				{
					cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index =  lcd_point_config_ptr->cur_data_start_point_num.line_num_or_index;
				    cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index =  lcd_point_config_ptr->cur_data_start_point_num.col_num_or_index + lcd_point_config_ptr->max_num_chars_config - 1;					
				}
                else
				{
					config_max_num_chars_from_next_line = lcd_point_config_ptr->max_num_chars_config - max_avail_num_chars_start_line;
					cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.line_num_or_index = (config_max_num_chars_from_next_line / (cur_lcd_ctrl_ptr->max_config_cols_in_index + 1)) + 1 + cur_lcd_status_ptr->cur_data_lcd_para.start_point_num.line_num_or_index;
					cur_lcd_status_ptr->cur_data_lcd_para.end_point_num.col_num_or_index = (config_max_num_chars_from_next_line % ((cur_lcd_ctrl_ptr->max_config_cols_in_index + 1) + 1)) ;
				}						
			}
			LCD_From_XY_To_Loc(lcd_ch_id, &(cur_lcd_status_ptr->cur_data_lcd_para.end_point_num), &cur_lcd_status_ptr->cur_data_lcd_para.end_loc);
			return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : LCD_Loc_Ctrl_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           :  keep track of lcd_cur_<input/disp>_loc as baskspace we need to -- it and also disp timeouts with.
 	             LCD automatically increments due to loc_command(0x06)in our case as after eg LCD_Write_Command(0x80)
	             ie set DDRAM  

Func ID        : 09.27 

Bugs           :  
-*------------------------------------------------------------*/
uint8_t LCD_Loc_Ctrl_Proc(const uint8_t lcd_ch_id, const uint8_t lcd_ctrl_format, const char lcd_rcvd_char )
{	
    lcd_status_t *cur_lcd_status_ptr = NULL_PTR;
	lcd_ctrl_t *cur_lcd_ctrl_ptr = NULL_PTR;
	lcd_point_t cur_loc_point_num, next_loc_point_num;	
    uint8_t ret_status, *lcd_cur_loc_ptr ;
	
	cur_lcd_status_ptr = lcd_status +  lcd_ch_id;
	cur_lcd_ctrl_ptr = lcd_ctrl +  lcd_ch_id;
	switch(lcd_ctrl_format >> LCD_CTRL_LOC_TYPE_DATA_POS)
	{
		case LCD_OUTPUT_LOC:
		  lcd_cur_loc_ptr = &(cur_lcd_status_ptr->lcd_cur_disp_loc);		 
		break;
        case LCD_INPUT_LOC:
          lcd_cur_loc_ptr = &(cur_lcd_status_ptr->lcd_cur_input_loc);		  
	    break;		
	}
    switch(lcd_rcvd_char)
	{	
         case BACKSPACE_CHAR:
		    /* backspace at prev point */  
		    LCD_From_Loc_to_XY(lcd_ch_id, *lcd_cur_loc_ptr, &cur_loc_point_num);
	        if((cur_loc_point_num.line_num_or_index == LINE_NUM_01) && (cur_loc_point_num.col_num_or_index == COL_NUM_01))
	        {	 
	    	   /* reached begin of line num 1 and valid backspace is pressed, retain same loc position */ 
	    	   ret_status =  WARN_LCD_TRY_BS_CHAR_ATBEGIN;			  
			    Error_or_Warning_Proc("09.27.01", WARNING_OCCURED, ret_status);	
	    	   return ret_status;
	       }
	        /* put cur input lcd loc to next location */
	       if(cur_loc_point_num.col_num_or_index == COL_NUM_01)
	       {	 
			   next_loc_point_num.line_num_or_index = cur_loc_point_num.line_num_or_index - 1;
			   next_loc_point_num.col_num_or_index = cur_lcd_ctrl_ptr->max_config_cols_in_index + 1;
	       }
           else
           {
	          next_loc_point_num.line_num_or_index = cur_loc_point_num.line_num_or_index;
	    	  next_loc_point_num.col_num_or_index = cur_loc_point_num.col_num_or_index - 1;
	       }	
	       LCD_From_XY_To_Loc(lcd_ch_id, &next_loc_point_num, lcd_cur_loc_ptr);
		   LCD_Write_Command(lcd_ch_id, *lcd_cur_loc_ptr);
		   LCD_Disp_Char(lcd_ch_id, ' '); 
           LCD_Write_Command(lcd_ch_id, 0x10); //shift cursor to left
		   LCD_Write_Command(lcd_ch_id, *lcd_cur_loc_ptr);
		   switch(lcd_ctrl_format >> LCD_CTRL_LOC_TYPE_DATA_POS)
	       {
		       case LCD_OUTPUT_LOC:
		         LCD_Write_Command(lcd_ch_id, 0x0C);
	           break;
               case LCD_INPUT_LOC:                
		         LCD_Write_Command(lcd_ch_id, 0x0E);
	           break;		
           }           
		 break;		 
		 default:
		    /* disp char at next point */ 
		    LCD_Write_Command(lcd_ch_id, *lcd_cur_loc_ptr);
	        switch(lcd_ctrl_format & 0x03)
            {
               case RCVD_CHAR_PLAIN_DISP_FORMAT:
                  LCD_Disp_Char(lcd_ch_id, lcd_rcvd_char);
               break;
               case RCVD_CHAR_HIDDEN_DISP_FORMAT:
                  LCD_Disp_Char(lcd_ch_id, lcd_rcvd_char);
                  SW_Time_Delay(MAX_ICOUNT_HIDDEN_CHAR_DISP_1500MS, MAX_JCOUNT_HIDDEN_CHAR_DISP_1500MS);
                  LCD_Write_Command(lcd_ch_id, *lcd_cur_loc_ptr); 
                  LCD_Disp_Char(lcd_ch_id, HIDDEN_KEY_DISP_CHAR);
               break;
	           case RCVD_CHAR_DONT_DISP_FORMAT:
	             return SUCCESS;
	           // break;
	           default:
                  /* error: invalid lcd enter char disp format */
                 system_status_flag = ERR_FORMAT_INVALID;
				  Error_or_Warning_Proc("09.27.02", ERROR_OCCURED, system_status_flag );	
                return system_status_flag;		 
            }  
	        LCD_From_Loc_to_XY(lcd_ch_id, *lcd_cur_loc_ptr, &cur_loc_point_num);
	        if((cur_loc_point_num.line_num_or_index == cur_lcd_ctrl_ptr->max_config_lines_in_index + 1) && (cur_loc_point_num.col_num_or_index  == cur_lcd_ctrl_ptr->max_config_cols_in_index + 1))
	        {	 
	    	   /* reached end of max configured line and data to disp, retain same loc position */ 
			    ret_status =  WARN_LCD_TRY_CHAR_ATEND_CONF_LINE;			  
			    Error_or_Warning_Proc("09.27.03", WARNING_OCCURED, ret_status);	
	    	    return ret_status;	    	   
	       }
	        /* put cur input lcd loc to next location */
	       if(cur_loc_point_num.col_num_or_index == cur_lcd_ctrl_ptr->max_config_cols_in_index + 1)
	       {	 
			   next_loc_point_num.line_num_or_index = cur_loc_point_num.line_num_or_index  + 1;
			   next_loc_point_num.col_num_or_index = COL_NUM_01;
	       }
           else
           {
	          next_loc_point_num.line_num_or_index = cur_loc_point_num.line_num_or_index;
	    	  next_loc_point_num.col_num_or_index = cur_loc_point_num.col_num_or_index + 1;
	       }	
	       LCD_From_XY_To_Loc(lcd_ch_id, &next_loc_point_num, lcd_cur_loc_ptr);
		   LCD_Write_Command(lcd_ch_id, *lcd_cur_loc_ptr);
		   switch(lcd_ctrl_format >> LCD_CTRL_LOC_TYPE_DATA_POS)
	       {
		       case LCD_OUTPUT_LOC:
		         LCD_Write_Command(lcd_ch_id, 0x0C);
	           break;
               case LCD_INPUT_LOC:                
		         LCD_Write_Command(lcd_ch_id, 0x0E);
	           break;		
          }    
	 }
	 return SUCCESS;
}
	 
#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
