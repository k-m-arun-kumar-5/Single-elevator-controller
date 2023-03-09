/* ********************************************************************
FILE                   : io_lib.c

PURPOSE                : 
                     									 	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
                       
CHANGE LOGS           : 

FILE ID               : 03 

*****************************************************************************/
#include "main.h"

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */
static uint32_t volatile *const io_port_func_ptr[NUM_PORT_CHS + 1] = {(uint32_t *) &PINSEL0,(uint32_t *) &PINSEL1, (uint32_t *) &PINSEL2 };
static uint32_t volatile *const io_port_dir_ptr[NUM_PORT_CHS] = { (uint32_t *) &IO0DIR, (uint32_t *) &IO1DIR };
uint32_t volatile *const io_port_set_ptr[NUM_PORT_CHS] = {(uint32_t *)&IO0SET, (uint32_t *) &IO1SET};
uint32_t volatile *const io_port_clr_ptr[NUM_PORT_CHS] = {(uint32_t *) &IO0CLR, (uint32_t *) &IO1CLR};
uint32_t volatile *const io_port_pin_ptr[NUM_PORT_CHS] = {(uint32_t *) &IO0PIN, (uint32_t *) &IO1PIN};
static uint32_t config_port_pin[NUM_PORT_CHS];
uint32_t default_port_mask[NUM_PORT_CHS] = { 0xFEFFFFFF, 0xFFFF0000};

/* ----------------------------- global variable declaration -------------------- */
extern const io_config_t io_config[];
extern uint8_t SW_Ch_UnConfig_By_IO_Ch(const uint8_t base_sw_io_ch, const uint8_t num_sw_chs);

/* ----------------------------- global function declaration -------------------- */
static uint8_t Config_GPIO_Func_Set(const io_config_t *const cur_io_config_ptr);
static uint8_t Debug_IO_Func_Set_Validate(const io_config_t *const cur_io_config_ptr);
static uint8_t Trace_IO_Func_Set_Validate(const io_config_t *const cur_io_config_ptr);
static uint8_t GPIO_Func_Set(const uint8_t base_io_ch, const uint8_t num_io_chs);
static uint8_t Config_Port_Pin_Validate(const uint8_t io_ch);
/*------------------------------------------------------------*
FUNCTION NAME  : IO_Channels_Func_Set

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.01  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t IO_Channels_Func_Set(const io_config_t *const cur_io_config_ptr)
{
	 uint8_t port, port_pin, io_ch, ret_status = SUCCESS ;
		
	 if(cur_io_config_ptr == NULL_PTR )
	 {
		 system_status_flag =  ERR_IO_CONFIG_NULL_PTR;
		 Error_or_Warning_Proc("03.01.01", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag; 
	 }
	 
	       if((ret_status = Config_Port_Pin_Range_Validate(cur_io_config_ptr )) != SUCCESS)
			{
				   system_status_flag = ret_status;
				   Error_or_Warning_Proc("03.01.02", ERROR_OCCURED, system_status_flag); 
					 goto IO_INIT_EXIT;	
					 //break;
			}
			     		
	   // common to all IO pin's function process 
	  	switch(cur_io_config_ptr->func)
	  	{
		     case IO_FUNC_GPIO:
		        if((ret_status = Config_GPIO_Func_Set(cur_io_config_ptr)) != SUCCESS)
		        {
						  	system_status_flag = ERR_GPIO_FUNC_SET;
							 Error_or_Warning_Proc("03.01.03", ERROR_OCCURED, system_status_flag); 
							  goto IO_INIT_EXIT;	
							  //break;
		        }	
           goto IO_INIT_EXIT;					
				//break;
			    case IO_FUNC_UNUSED:
			    case IO_FUNC_RESERVE:			         	
                   goto IO_INIT_EXIT;	
                //break;				
		}    
		 	 port  = io_ch / NUM_PINS_PER_PORT;
		   port_pin = io_ch % NUM_PINS_PER_PORT;
	    switch(port)
		  {			 
		     case	PORT_CH_00:
				     switch(port_pin)
				     {
				    	 case PIN_00:
					 	     switch(cur_io_config_ptr->func)
		             {
                    case IO_FUNC_UART:
			              break;
			              case IO_FUNC_PWM:
			              break;
							    	default:
									    system_status_flag = ERR_IO_CH_00_FUNC_SET;
										 Error_or_Warning_Proc("03.01.04", ERROR_OCCURED, system_status_flag);
							    		ret_status = system_status_flag;
							      	goto IO_INIT_EXIT;	
                    //break; 										
						     }
					     break;
				   }
       break;
       case PORT_CH_01:
				 switch(port_pin)
				 {
					   case PIN_16:
							  switch(cur_io_config_ptr->func)
	  	          {
		               case IO_FUNC_TRACE:
										 if((ret_status = Trace_IO_Func_Set_Validate(cur_io_config_ptr)) != SUCCESS)
										 {
											 system_status_flag = ERR_TRACE_FUNC_SET;
											  Error_or_Warning_Proc("03.01.05", ERROR_OCCURED, system_status_flag);
											 goto IO_INIT_EXIT;	
                       //break;											 
										 }
										 *io_port_func_ptr[2] &= ~(STATE_HIGH << 3);
										 *io_port_func_ptr[2] |= (IO_FUNC_MODE_01 << 3);
                   break;
									 default:
									    system_status_flag = ERR_IO_CH_48_FUNC_SET;
										 Error_or_Warning_Proc("03.01.06", ERROR_OCCURED, system_status_flag);
										 ret_status = system_status_flag;
							       goto IO_INIT_EXIT;	
                   // break;									 
						    }
							    							
						 break;
             case PIN_26:
							  switch(cur_io_config_ptr->func)
	  	          {
		               case IO_FUNC_DEBUG:
										 if((ret_status = Debug_IO_Func_Set_Validate(cur_io_config_ptr)) != SUCCESS)
										 {
										   system_status_flag = ERR_TRACE_FUNC_SET;
										    Error_or_Warning_Proc("03.01.07", ERROR_OCCURED, system_status_flag);
											  goto IO_INIT_EXIT;	
                       //break;											 
										 }
										 *io_port_func_ptr[2] &= ~(STATE_HIGH << 2);
										 *io_port_func_ptr[2] |= (IO_FUNC_MODE_01 << 2);
                   break;
									 default:
									   system_status_flag = ERR_IO_CH_58_FUNC_SET;
									    Error_or_Warning_Proc("03.01.08", ERROR_OCCURED, system_status_flag);
										 ret_status = system_status_flag;
							       goto IO_INIT_EXIT;
									 //break;
						 	    	
						    }         							
						 break;	
             default:
			           system_status_flag = ERR_PORT1_PIN_FUNC_SET;
					    Error_or_Warning_Proc("03.01.09", ERROR_OCCURED, system_status_flag);
                 ret_status = system_status_flag;
					       goto IO_INIT_EXIT;
						 //break;
					  	 
				 }				 
        break;
				default:
				system_status_flag = ERR_PORT_INVALID;
				  Error_or_Warning_Proc("03.01.10", ERROR_OCCURED, system_status_flag);
					ret_status = system_status_flag; 
				break;
		  }			 
   	 
IO_INIT_EXIT:  
   if(ret_status != SUCCESS)
    {				
          Reset_IO_Chs();       		
	  }		
	   return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Port_Write

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.02  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Port_Write(const uint8_t io_channel, const void *const data_ptr)
{
	 consucc_bit_t *consucc_bit_ptr = (consucc_bit_t *)data_ptr, consucc_bits_1;
	 uint8_t port, ret_status = SUCCESS;
 	 
	 if((ret_status = Check_GPIO_Func(io_channel, data_ptr)) != SUCCESS)
	 {
		 system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
		 Error_or_Warning_Proc("03.02.01", ERROR_OCCURED, system_status_flag);
	     return system_status_flag;		 
	 }
	 port = io_channel / NUM_PINS_PER_PORT;
	 consucc_bits_1.start_bit_pos = consucc_bit_ptr->start_bit_pos;
	 consucc_bits_1.bits_len = consucc_bit_ptr->bits_len;	
	 consucc_bits_1.write_or_config_consucc_val = 0;
     if((ret_status = Config_Consucc_Bits(FLAG_CONSUCC_BITS_1, &consucc_bits_1)) != SUCCESS )
	 {
		 system_status_flag = ERR_PORT_WRITE_INIT_VAL;
		  Error_or_Warning_Proc("03.02.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }
	 if(consucc_bits_1.write_or_config_consucc_val != (*io_port_dir_ptr[port] & consucc_bits_1.write_or_config_consucc_val ))
	 {
		 system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
      Error_or_Warning_Proc("03.02.03", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }
	 if(consucc_bits_1.write_or_config_consucc_val != (consucc_bits_1.write_or_config_consucc_val | (consucc_bit_ptr->write_or_config_consucc_val << consucc_bit_ptr->start_bit_pos)))
	 {
		  system_status_flag = ERR_PORT_WRITE_BIT_VAL;
		    Error_or_Warning_Proc("03.02.04", ERROR_OCCURED, system_status_flag);
		 // to set data in port not within valid range for a bit len  ie for bit len = 3, then valid value range is [0,7] 
		 return system_status_flag;
	 }
	 *io_port_pin_ptr[port] = (*io_port_pin_ptr[port] & ~(consucc_bits_1.write_or_config_consucc_val)) | (consucc_bit_ptr->write_or_config_consucc_val << consucc_bit_ptr->start_bit_pos);
     return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : IO_Channel_Write

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.03  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t IO_Channel_Write(const uint8_t io_channel, const uint8_t data)
{
	 consucc_bit_t io_ch_write;
	 uint8_t port, port_pin, ret_status;
	 
     port = io_channel / NUM_PINS_PER_PORT;
	 port_pin = io_channel % NUM_PINS_PER_PORT;
	 io_ch_write.start_bit_pos = port_pin;
	 io_ch_write.bits_len = 1;	 
	 if((ret_status = Check_GPIO_Func(io_channel, &io_ch_write)) != SUCCESS)
	 {
    	 system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
		 Error_or_Warning_Proc("03.03.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }	 	 
	 if(!(*io_port_dir_ptr[port] & (1 << port_pin)))
	 {
		 system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
		 Error_or_Warning_Proc("03.03.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 } 
	switch(data)
	{
		case STATE_HIGH:
		  // *io_port_set_ptr[port] |= (1 << port_pin); 
              *io_port_pin_ptr[port] = *io_port_pin_ptr[port] | (1 << port_pin);		  
		 break;
		 case STATE_LOW:
  	      //  *io_port_clr_ptr[port] |= (1 << port_pin);
             *io_port_pin_ptr[port] = *io_port_pin_ptr[port] & (~(1 << port_pin));		  
         break;
         case STATE_TOGGLE:
             *io_port_pin_ptr[port] = *io_port_pin_ptr[port] ^ (1 << port_pin);        	        
         break;    
         default:
	        system_status_flag = ERR_IO_CH_WRITE_DATA;
		    Error_or_Warning_Proc("03.03.03", ERROR_OCCURED, system_status_flag);
            return system_status_flag;			 
    }
	return SUCCESS;
}
/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Ports

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.04  

BUGS           :              
-*------------------------------------------------------------*/
void Reset_Ports(void)
{
	uint8_t i;
	
    for(i = 0; i < NUM_PORT_CHS + 1; ++i)
	 {
	    *io_port_func_ptr[i] = IO_FUNC_GPIO;
	 }
	 for(i = 0; i < NUM_PORT_CHS; ++i)
	 {
		   *io_port_dir_ptr[i] = 0;
		   *io_port_pin_ptr[i] =  0 & default_port_mask[i];
		   config_port_pin[i] = 0;		  
	 }
}

/*------------------------------------------------------------*
FUNCTION NAME  : IO_Channel_Read

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.05  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t IO_Channel_Read(const uint8_t io_ch, uint8_t *const read_data_ptr)
{
	 consucc_bit_t io_ch_read;
	 uint8_t port, port_pin, ret_status  ;
	 
	 port = io_ch / NUM_PINS_PER_PORT;
	 port_pin = io_ch % NUM_PINS_PER_PORT;
	 io_ch_read.start_bit_pos = port_pin;
	 io_ch_read.bits_len = 1;	 
	 if((ret_status = Check_GPIO_Func(io_ch, &io_ch_read)) != SUCCESS)
	 {
		 system_status_flag = ERR_IO_CH_READ_NON_READ;
		 Error_or_Warning_Proc("03.05.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }
	 // commited due to read an write enabled io_ch 
	/* if(*io_port_dir_ptr[port] & (1 << port_pin))
	 {
		 system_status_flag = ERR_IO_CH_READ_NON_READ;
		  Error_or_Warning_Proc("03.05.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 } */
	 if(*io_port_pin_ptr[port] & (1 << port_pin))
	 {
		   *read_data_ptr = STATE_HIGH;
	 }
	 else
	 {
	     *read_data_ptr = STATE_LOW;	 
	 }
	 return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Config_GPIO_Func_Set

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : for lpc2138 

Func ID        : 03.06  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t Config_GPIO_Func_Set(const io_config_t *const cur_io_config_ptr)
{
	  uint8_t port, start_port_pin, ret_status = SUCCESS, i ;
	 	
	  port = cur_io_config_ptr->io_ch / NUM_PINS_PER_PORT;
	  start_port_pin = cur_io_config_ptr->io_ch % NUM_PINS_PER_PORT;
	
	  if((ret_status =	GPIO_Func_Set(cur_io_config_ptr->io_ch, cur_io_config_ptr->port_pin_len)) != SUCCESS)
	  {
			system_status_flag = ERR_GPIO_FUNC_SET;
			 Error_or_Warning_Proc("03.06.01", ERROR_OCCURED, system_status_flag);
		    return system_status_flag; 			
	  }	
	  switch(cur_io_config_ptr->signal)
	  {
		   case PIN_SIG_DIGITAL:
		  	 switch(cur_io_config_ptr->dir)
		     {
				  case IO_DIR_INPUT:
				     // first clear or set io port corresponding to input io channels, according to pressed state
				     for(i = 0; i < cur_io_config_ptr->port_pin_len ; ++i)
					 {
					    *io_port_dir_ptr[port] &= ~(STATE_HIGH << (start_port_pin + i));
					    *io_port_dir_ptr[port] |= (IO_DIR_OUTPUT << (start_port_pin + i));
					  } 
                      switch(!cur_io_config_ptr->state)
					  {
						  case STATE_LOW:
							for(i = 0; i < cur_io_config_ptr->port_pin_len; ++i)
						 	{
								*io_port_clr_ptr[port] = 1 << (start_port_pin + i);								
							}
						  break;
						  case STATE_HIGH:
							for(i = 0; i < cur_io_config_ptr->port_pin_len; ++i)
						    {
							    *io_port_set_ptr[port] = 1 << (start_port_pin + i);
						    }
						  break;
					  }
			
				      if((ret_status = GPIO_Func_Set(cur_io_config_ptr->io_ch, cur_io_config_ptr->port_pin_len)) != SUCCESS)
	                  {
		                 system_status_flag = ERR_GPIO_INPUT_FUNC_STATE;
						  Error_or_Warning_Proc("03.06.02", ERROR_OCCURED, system_status_flag);
                         return system_status_flag;					    
	                   }	
					   for(i = 0; i < cur_io_config_ptr->port_pin_len ; ++i)
					   {
							*io_port_dir_ptr[port] &= ~(STATE_HIGH << (start_port_pin + i));
							*io_port_dir_ptr[port] |= (IO_DIR_INPUT << (start_port_pin + i));
					   }
				        switch(cur_io_config_ptr->state)
			  			{
							   case STATE_LOW:
							   case STATE_HIGH: 
							   break;
							   default:
								  if(cur_io_config_ptr->func_type == IO_FUNC_TYPE_GPIO_SW)
								  {
								     system_status_flag = ERR_GPIO_INPUT_FUNC_STATE;
									  Error_or_Warning_Proc("03.06.03", ERROR_OCCURED, system_status_flag);
                                     return system_status_flag;							             
								  }
							   break;																	
						}
                
              break;		
							case IO_DIR_OUTPUT:
								if(cur_io_config_ptr->func_type == IO_FUNC_TYPE_GPIO_SW)
								{
									  system_status_flag = ERR_GPIO_OUTPUT_FUNC_STATE;
									   Error_or_Warning_Proc("03.06.04", ERROR_OCCURED, system_status_flag);
                                      return system_status_flag;
								}
							    for(i = 0; i < cur_io_config_ptr->port_pin_len ; ++i)
							    {
								    *io_port_dir_ptr[port] &= ~(STATE_HIGH << (start_port_pin + i));
								    *io_port_dir_ptr[port] |= (IO_DIR_OUTPUT << (start_port_pin + i));
							    }
								switch(cur_io_config_ptr->state)
								{
									  case STATE_LOW:
											for(i = 0; i < cur_io_config_ptr->port_pin_len; ++i)
									  	{
												*io_port_clr_ptr[port] = 1 << (start_port_pin + i);
											}
										break;
							  		case STATE_HIGH:
											for(i = 0; i < cur_io_config_ptr->port_pin_len; ++i)
									  	   {
											    *io_port_set_ptr[port] = 1 << (start_port_pin + i);
											}
										break;
                    default:
					              system_status_flag = ERR_GPIO_OUTPUT_DATA;
								   Error_or_Warning_Proc("03.06.05", ERROR_OCCURED, system_status_flag);
                                 return system_status_flag;																
								}
							  
				        break;													
					 }
					 break;
					 default: 
					   system_status_flag = ERR_PIN_SIGNAL;
					    Error_or_Warning_Proc("03.06.06", ERROR_OCCURED, system_status_flag);
                       return system_status_flag;					    
					  	// break;										 
		 }	     		 
	  
      return SUCCESS; 								 
}

/*------------------------------------------------------------*
FUNCTION NAME  : IO_Ch_Validate 

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : for lpc2138 io channel 

Func ID        : 03.07  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t IO_Ch_Validate(const uint8_t base_io_ch, const uint8_t num_io_chs)
{
	  uint8_t port, start_port_pin, end_port_pin ;
	
	  if(num_io_chs <= 0 || num_io_chs > NUM_PINS_PER_PORT)
		{
			 system_status_flag = ERR_IO_PIN_RANGE;
			 Error_or_Warning_Proc("03.07.01", ERROR_OCCURED, system_status_flag);
			 return system_status_flag;
		}			
	  port  = base_io_ch / NUM_PINS_PER_PORT;
	  start_port_pin = base_io_ch % NUM_PINS_PER_PORT; 
	  end_port_pin = start_port_pin + num_io_chs - 1;
	  if(start_port_pin >= NUM_PINS_PER_PORT || end_port_pin >= NUM_PINS_PER_PORT)
	  {
		  system_status_flag = ERR_IO_PIN_RANGE;
		  Error_or_Warning_Proc("03.07.02", ERROR_OCCURED, system_status_flag);
			return system_status_flag;
	  }
	  switch(port)
	  {
			  case PORT_CH_00:
					if( (start_port_pin <= PIN_24 && end_port_pin >= PIN_24))
					{
					  	// PORT0's pin 24 is reserved
						  #ifdef TRACE_ERROR
						     uint32_temp_disp_data = base_io_ch;
						     Print("ERR: base_io_ch: %u, ", uint32_temp_disp_data);
							 uint32_temp_disp_data = start_port_pin;
						     Print("start_pin : %u, ", uint32_temp_disp_data);
							  uint32_temp_disp_data = end_port_pin;
						     Print("end_pin : %u\r", uint32_temp_disp_data);
						  #endif
						  system_status_flag = ERR_IO_CH_24_PIN;
						  Error_or_Warning_Proc("03.07.03", ERROR_OCCURED, system_status_flag); 
						  return system_status_flag;
					}
					
        break;
				case PORT_CH_01:
					if(start_port_pin <= PIN_15 || end_port_pin <= PIN_15)
					{
						// port1's pin [0,15] are reserved
						 system_status_flag = ERR_IO_CH_32_TO_47;
						 Error_or_Warning_Proc("03.07.04", ERROR_OCCURED, system_status_flag); 
						 return system_status_flag;
					}
					
        break;
        default:
		    system_status_flag = ERR_INVALID_PORT;
			Error_or_Warning_Proc("03.07.05", ERROR_OCCURED, system_status_flag);
			return system_status_flag;				
		}					
		return SUCCESS; 
}


/*------------------------------------------------------------*
FUNCTION NAME  : Debug_IO_Func_Set_Validate

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : for lpc2138 io channel 

Func ID        : 03.08  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t Debug_IO_Func_Set_Validate(const io_config_t *const cur_io_config_ptr)
{	   	 	
	  if(cur_io_config_ptr->io_ch >= PIN_26 && cur_io_config_ptr->io_ch <= PIN_31 && cur_io_config_ptr->port_pin_len != (PIN_31 - cur_io_config_ptr->io_ch + 1))
		{
			 system_status_flag = ERR_CONFIG_DEBUG_FUNC_SET;
			 Error_or_Warning_Proc("03.08.01", ERROR_OCCURED, system_status_flag);
			 return system_status_flag;			  
		}
		return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Trace_IO_Func_Set_Validate

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : for lpc2138 io channel 

Func ID        : 03.09  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t Trace_IO_Func_Set_Validate(const io_config_t *const cur_io_config_ptr)
{
	  if(cur_io_config_ptr->io_ch >= PIN_16 && cur_io_config_ptr->io_ch <= PIN_25 && cur_io_config_ptr->port_pin_len != (PIN_25 - cur_io_config_ptr->io_ch + 1))
		{
			 system_status_flag = ERR_CONFIG_TRACE_FUNC_SET;
			 Error_or_Warning_Proc("03.09.01", ERROR_OCCURED, system_status_flag);
			 return system_status_flag;			  
		}
		return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_IO_Chs

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.10  

BUGS           :              
-*------------------------------------------------------------*/
void Reset_IO_Chs(void)
{
   Reset_Ports();
  #if defined(SW_MOD_ENABLE) || defined(KEYBOARD_MOD_ENABLE)
	   Reset_SW_Chs();
  #endif
}

/*------------------------------------------------------------*
FUNCTION NAME  : Config_Port_Pin_Range_Validate

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.11  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Config_Port_Pin_Range_Validate(const io_config_t *const cur_io_config_ptr )
{
	 consucc_bit_t set_port_pins;
	 uint8_t ret_status = SUCCESS, port, start_port_pin;
	 
	if((ret_status = IO_Ch_Validate(cur_io_config_ptr->io_ch, cur_io_config_ptr->port_pin_len )) != SUCCESS)
	{
		system_status_flag = ERR_IO_PIN_RANGE;
		Error_or_Warning_Proc("03.11.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	port = cur_io_config_ptr->io_ch / NUM_PINS_PER_PORT;
	start_port_pin = cur_io_config_ptr->io_ch % NUM_PINS_PER_PORT;
	set_port_pins.start_bit_pos = start_port_pin;
	set_port_pins.bits_len = cur_io_config_ptr->port_pin_len;
	set_port_pins.write_or_config_consucc_val = 0;
	if((ret_status = Config_Consucc_Bits(FLAG_CONSUCC_BITS_1, &set_port_pins)) != SUCCESS)
	{
		 system_status_flag = ERR_CONFIG_PORT_INIT_VAL;
		 Error_or_Warning_Proc("03.11.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
	if((config_port_pin[port] & set_port_pins.write_or_config_consucc_val) != 0)
	{
		system_status_flag = ERR_CONFIG_PIN_RANGE;
		Error_or_Warning_Proc("03.11.03", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	config_port_pin[port] |= set_port_pins.write_or_config_consucc_val;
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Port_Read

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.12  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Port_Read(const uint8_t io_channel, void *const data_ptr)
{
	 consucc_bit_t *consucc_bit_ptr = (consucc_bit_t *)data_ptr, consucc_bits_1;
	 uint8_t port, ret_status = SUCCESS;
 	 
	 if((ret_status = Check_GPIO_Func(io_channel, data_ptr)) != SUCCESS)
	 {
		 system_status_flag = ERR_IO_CH_READ_NON_READ;
		 Error_or_Warning_Proc("03.12.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }	
	 port = io_channel / NUM_PINS_PER_PORT;
	 consucc_bits_1.start_bit_pos = consucc_bit_ptr->start_bit_pos;
	 consucc_bits_1.bits_len = consucc_bit_ptr->bits_len;
	 consucc_bits_1.write_or_config_consucc_val = 0;
     if((ret_status = Config_Consucc_Bits(FLAG_CONSUCC_BITS_1, &consucc_bits_1)) != SUCCESS )
	 {
		 system_status_flag = ERR_CONFIG_PORT_INIT_VAL;
		 Error_or_Warning_Proc("03.12.02", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag;
	 }
	 if(consucc_bits_1.write_or_config_consucc_val != (*io_port_dir_ptr[port] ^ consucc_bits_1.write_or_config_consucc_val ) & consucc_bits_1.write_or_config_consucc_val )
	 {
		 system_status_flag = ERR_IO_CH_READ_NON_READ;
		 Error_or_Warning_Proc("03.12.03", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag;
	 } 
	 consucc_bit_ptr->read_or_test_consucc_val = (*io_port_pin_ptr[port] & (consucc_bits_1.write_or_config_consucc_val)) >> consucc_bit_ptr->start_bit_pos;
	 return SUCCESS;
}	

/*------------------------------------------------------------*
FUNCTION NAME  : IO_Ch_Func_Reset

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.13  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t IO_Ch_Func_Reset(const io_config_t *const io_unconfig_ptr)
{
	consucc_bit_t mask_io_ch ;	
	uint8_t ret_status, port, start_port_pin, i ;
	
	if(io_unconfig_ptr == NULL_PTR )
	{
		 system_status_flag =  ERR_IO_CONFIG_NULL_PTR;
		 Error_or_Warning_Proc("03.13.01", ERROR_OCCURED, system_status_flag); 
		  return system_status_flag; 
	}
	if((ret_status = IO_Ch_Validate(io_unconfig_ptr->io_ch, io_unconfig_ptr->port_pin_len)) != SUCCESS)
	 {
		 system_status_flag = ERR_IO_CH_UNCONFIG;
		  Error_or_Warning_Proc("03.13.02", ERROR_OCCURED, system_status_flag); 
	     return system_status_flag;
	 }
	 port = io_unconfig_ptr->io_ch / NUM_PINS_PER_PORT;	 
	 start_port_pin =  io_unconfig_ptr->io_ch % NUM_PINS_PER_PORT;
	 mask_io_ch.start_bit_pos = start_port_pin;
	 mask_io_ch.bits_len = io_unconfig_ptr->port_pin_len;
	 mask_io_ch.write_or_config_consucc_val = 0;
   if((ret_status = Config_Consucc_Bits(FLAG_CONSUCC_BITS_1, &mask_io_ch)) != SUCCESS )
	 {
		 system_status_flag = ERR_IO_CH_UNCONFIG;
		  Error_or_Warning_Proc("03.13.03", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag;
	 }
	 if((config_port_pin[port] & mask_io_ch.write_or_config_consucc_val) == 0)
	 {
		 // already io ch reset
		 return SUCCESS;
	 }
	 if(mask_io_ch.write_or_config_consucc_val != (config_port_pin[port] & mask_io_ch.write_or_config_consucc_val))
	 {
	  	system_status_flag = ERR_CONFIG_PIN_RANGE;
		 Error_or_Warning_Proc("03.13.04", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag; 
	 }
	 #if defined(SW_MOD_ENABLE) || defined(KEYBOARD_MOD_ENABLE)
	 if(io_unconfig_ptr->func == IO_FUNC_GPIO && io_unconfig_ptr->func_type == IO_FUNC_TYPE_GPIO_SW ) 
	 {
		   if((ret_status = SW_Ch_UnConfig_By_IO_Ch(io_unconfig_ptr->io_ch, io_unconfig_ptr->port_pin_len)) != SUCCESS)
       {
		      system_status_flag = ERR_SW_CH_GPIO_FUNC_SET;
			   Error_or_Warning_Proc("03.13.05", ERROR_OCCURED, system_status_flag); 
		      return system_status_flag;	  
       }               
	 } 
	 #endif 
	 for(i = 0; i < io_unconfig_ptr->port_pin_len ; ++i)
	 {
		*io_port_dir_ptr[port] &= ~(STATE_HIGH << (start_port_pin + i));
	    *io_port_dir_ptr[port] |= (IO_DIR_INPUT << (start_port_pin + i));
	 }
	 if((ret_status = GPIO_Func_Set(io_unconfig_ptr->io_ch, io_unconfig_ptr->port_pin_len)) != SUCCESS)
	 {
		 system_status_flag = ERR_IO_CH_UNCONFIG;
		  Error_or_Warning_Proc("03.13.06", ERROR_OCCURED, system_status_flag); 
		 return system_status_flag; 
	 }	 
	 config_port_pin[port] &= ~mask_io_ch.write_or_config_consucc_val;
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : GPIO_Func_Set

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.14  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t GPIO_Func_Set(const uint8_t base_io_ch, const uint8_t num_io_chs)
{
	 uint8_t port, start_port_pin, end_port_pin, i, cur_port_pin, cur_pin_sel, cur_pin_sel_bit ;
	 
	  port = base_io_ch / NUM_PINS_PER_PORT;
	  start_port_pin = base_io_ch % NUM_PINS_PER_PORT;
     switch(port)
	 {
         case PORT_CH_00:
			 for(i = 0; i < num_io_chs; ++i)
			 {
			   	cur_port_pin = start_port_pin + i;
			    cur_pin_sel = cur_port_pin / PIN_16;
			   	cur_pin_sel_bit = cur_port_pin % PIN_16;
				*io_port_func_ptr[cur_pin_sel] &= ~(IO_FUNC_MODE_03 << ( cur_pin_sel_bit * 2));
			  	 *io_port_func_ptr[cur_pin_sel] |= (IO_FUNC_MODE_00 << ( cur_pin_sel_bit * 2));
			 }
		break;
	    case PORT_CH_01:
			end_port_pin = start_port_pin + num_io_chs - 1;						  
		    if(start_port_pin >= PIN_16 && start_port_pin <= PIN_25)
			{								
			     if(end_port_pin >= PIN_16 && end_port_pin <= PIN_25)
			     {
					  *io_port_func_ptr[2] &= ~(STATE_HIGH << 3);
  				      *io_port_func_ptr[2] |= (IO_FUNC_MODE_00 << 3);
				       break; 
			     }
				 else
				 {
				   if(end_port_pin >= PIN_26 && end_port_pin <= PIN_31)
				    {
						*io_port_func_ptr[2] &= ~(STATE_HIGH << 3);
					    *io_port_func_ptr[2] |= (IO_FUNC_MODE_00 << 3);
						*io_port_func_ptr[2] &= ~(STATE_HIGH << 2);
					    *io_port_func_ptr[2] |= (IO_FUNC_MODE_00 << 2); 
                        break;									 
					}
		        }
			}
			else
			{
			   if(start_port_pin >= PIN_26 && start_port_pin <= PIN_31)
			   {								
			         if(end_port_pin <= PIN_31)
			         {
						  *io_port_func_ptr[2] &= ~(STATE_HIGH << 2);
				     	  *io_port_func_ptr[2] |= (IO_FUNC_MODE_00 << 2);	
                          break;											 
				     }
			   }
			}
			// invalid port 1 pin [0,15] or [32, +infinity ] 
			system_status_flag = ERR_PORT1_IO_PIN;
			 Error_or_Warning_Proc("03.14.01", ERROR_OCCURED, system_status_flag); 
			return system_status_flag;
	//	break;	
		default:
		   system_status_flag = ERR_GPIO_CH_SET_PORT;
		    Error_or_Warning_Proc("03.14.02", ERROR_OCCURED, system_status_flag); 
           return system_status_flag;
		   //	break;
	 }
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Config_Port_Pin_Validate

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.15  

BUGS           :              
-*------------------------------------------------------------*/
static uint8_t Config_Port_Pin_Validate(const uint8_t io_ch)
{
	uint8_t port, port_pin, ret_status;
	
	if((ret_status = IO_Ch_Validate(io_ch, 1)) != SUCCESS)
	{
		system_status_flag = ERR_IO_PIN_RANGE;
		 Error_or_Warning_Proc("03.15.01", ERROR_OCCURED, system_status_flag); 
		return system_status_flag;
	}
	port = io_ch / NUM_PINS_PER_PORT;
	port_pin = io_ch % NUM_PINS_PER_PORT;
	  
    if(config_port_pin[port] & (1 << port_pin))
	{
		system_status_flag = ERR_CONFIG_PIN_RANGE;
		 Error_or_Warning_Proc("03.15.02", ERROR_OCCURED, system_status_flag); 
		return system_status_flag;
	}
	config_port_pin[port] |= (1 << port_pin);
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Check_GPIO_Func

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.16  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Check_GPIO_Func(const uint8_t io_channel, const void *const data_ptr)
{
   consucc_bit_t *consucc_bit_ptr = (consucc_bit_t *)data_ptr;
	 uint8_t port, start_port_pin, end_port_pin, cur_port_pin, cur_pin_sel, cur_pin_sel_bit, ret_status, i;
 	 
	 end_port_pin = consucc_bit_ptr->start_bit_pos + consucc_bit_ptr->bits_len - 1;
	 port = io_channel / NUM_PINS_PER_PORT;
	 start_port_pin = io_channel % NUM_PINS_PER_PORT;
	 
     if(consucc_bit_ptr == NULL_PTR || start_port_pin != consucc_bit_ptr->start_bit_pos || consucc_bit_ptr->bits_len <= 0 || end_port_pin >= NUM_PINS_PER_PORT )
	 {
		   system_status_flag = ERR_CHECK_GPIO_FUNC;
		    Error_or_Warning_Proc("03.16.01", ERROR_OCCURED, system_status_flag); 
		   return system_status_flag;
	 } 
	 if((ret_status = IO_Ch_Validate(io_channel, consucc_bit_ptr->bits_len)) != SUCCESS)
	 {
	     system_status_flag = ERR_CHECK_GPIO_FUNC;
		  Error_or_Warning_Proc("03.16.02", ERROR_OCCURED, system_status_flag); 
	     return system_status_flag;
	 }
   switch(port)
   {
	 case PORT_CH_00:
	    for(i = 0; i < consucc_bit_ptr->bits_len; ++i)
		{
		   	cur_port_pin = start_port_pin + i;
		    cur_pin_sel = cur_port_pin / PIN_16;
		   	cur_pin_sel_bit = cur_port_pin % PIN_16;
     
			if((*io_port_func_ptr[cur_pin_sel] & (IO_FUNC_MODE_03 << (cur_pin_sel_bit * 2))))
			{
        Print("\r ERR: IO_PORT_FUNC : %u, IO_CH: %u", ((*io_port_func_ptr[cur_pin_sel] >> (cur_pin_sel_bit * 2)) & IO_FUNC_MODE_03), io_channel);
					system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
					Error_or_Warning_Proc("03.16.03", ERROR_OCCURED, system_status_flag); 
		            return system_status_flag; 
			} 
     
		}
     break;
		 case PORT_CH_01:
		    if(start_port_pin >= PIN_16 && start_port_pin <= PIN_25)
			{								
			     if(end_port_pin >= PIN_16 && end_port_pin <= PIN_25)
			     {
					  if(*io_port_func_ptr[2] & (1 << 3))
					  {
						  system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
						  Error_or_Warning_Proc("03.16.04", ERROR_OCCURED, system_status_flag); 
		                  return system_status_flag; 
					  }
					  break; 
			     }
				 else
				 {
				   if(end_port_pin >= PIN_26 && end_port_pin <= PIN_31)
				    {
					  if(*io_port_func_ptr[2] & (1 << 3) || *io_port_func_ptr[2] & (1 << 2))
					  {
						  system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
						  Error_or_Warning_Proc("03.16.05", ERROR_OCCURED, system_status_flag); 
		                  return system_status_flag; 
					  }
					  break;									 
					}
		        }
			}
			else
			{
			   if(start_port_pin >= PIN_26 && start_port_pin <= PIN_31)
			   {								
			       if(*io_port_func_ptr[2] & (1 << 2))
				   {
					  system_status_flag = ERR_IO_CH_WRITE_NON_WRITE;
					  Error_or_Warning_Proc("03.16.06", ERROR_OCCURED, system_status_flag); 
		              return system_status_flag; 
				   }
                   break;
			   }
			}
		 break;
	 }
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Non_GPIO_Func_Set

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : for simplicity, Non_GPIO_Func_Set() is used as an alternative to IO_Channels_Func_Set() to set non GPIO Func 

Func ID        : 03.17  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Non_GPIO_Func_Set(uint8_t io_ch, uint8_t func_mode)
{
	uint8_t ret_status, pin_sel_id, pin_sel_bit;
	
	if((ret_status = Config_Port_Pin_Validate(io_ch)) != SUCCESS)
	{
		system_status_flag = ERR_NON_GPIO_FUNC_SET;
		Error_or_Warning_Proc("03.17.01", ERROR_OCCURED, system_status_flag); 
		return system_status_flag;
	}
	if(io_ch > IO_CH_31 || func_mode > IO_FUNC_MODE_03 || func_mode == IO_FUNC_MODE_00 )
	{
		system_status_flag = ERR_NON_GPIO_FUNC_SET;
		Error_or_Warning_Proc("03.17.02", ERROR_OCCURED, system_status_flag); 
		return system_status_flag;
	}	
	pin_sel_bit = io_ch % PIN_16;
	pin_sel_id = (io_ch % NUM_PINS_PER_PORT) / PIN_16;
	*io_port_func_ptr[pin_sel_id] &= ~(IO_FUNC_MODE_03 << (2 * pin_sel_bit)); 	
	*io_port_func_ptr[pin_sel_id] |= (func_mode << (2 * pin_sel_bit));  	
	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Output_Dev_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.18  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Output_Dev_Init(uint8_t base_io_ch, const uint8_t num_io_chs)
{
	io_config_t output_config;
	uint8_t ret_status;
	
	output_config.io_ch = base_io_ch;
	output_config.signal = PIN_SIG_DIGITAL;
	output_config .func = IO_FUNC_GPIO;
	output_config.dir = IO_DIR_OUTPUT;
	output_config.state = STATE_LOW;
	output_config.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	output_config.port_pin_len = num_io_chs;	 
	 if((ret_status = IO_Channels_Func_Set(&output_config)) != SUCCESS)
	 {
		system_status_flag = ERR_GPIO_FUNC_SET;
		 Error_or_Warning_Proc("03.18.01", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	 }
	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Output_Dev_DeInit

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 03.19  

BUGS           :              
-*------------------------------------------------------------*/
uint8_t Output_Dev_DeInit(uint8_t base_io_ch, const uint8_t num_io_chs)
{
	io_config_t output_unconfig;
	uint8_t ret_status;
	
	output_unconfig.io_ch = base_io_ch;
	output_unconfig.signal = PIN_SIG_DIGITAL;
	output_unconfig.func = IO_FUNC_GPIO;
	output_unconfig.dir = IO_DIR_OUTPUT;
	output_unconfig.state = STATE_LOW;
	output_unconfig.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
	output_unconfig.port_pin_len = num_io_chs;	
	 if((ret_status = IO_Ch_Func_Reset(&output_unconfig)) != SUCCESS)
	 {
	  	system_status_flag = ERR_IO_CH_FUNC_RESET;
		  Error_or_Warning_Proc("03.19.01", ERROR_OCCURED, system_status_flag);
      return system_status_flag;		
	 }	
	
	return SUCCESS;
}
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
