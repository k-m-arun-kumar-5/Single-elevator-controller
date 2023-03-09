/**************************************************************************
   FILE          :    std_lib.h
 
   PURPOSE       :   Standard library Header
 
   AUTHOR        :   K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _STD_LIB_H
 #define _STD_LIB_H
 
 /* ---------------------- macro defination ------------------------------------------------ */ 
#define FLAG_CONSUCC_BITS_1         (1)
#define FLAG_CONSUCC_BITS_0         (2)
#define FLAG_CONSUCC_BITS_TOGGLE    (3)
#define FLAG_CONSUCC_BITS_VAL       (4) 
#define TEST_OK_1_CONSUCC_BITS      (5)
#define TEST_FAIL_1_CONSUCC_BITS    (6)
#define TEST_OK_0_CONSUCC_BITS      (7)
#define TEST_FAIL_0_CONSUCC_BITS    (8)
#define TEST_OK_VAL_CONSUCC_BITS    (9)
#define TEST_FAIL_VAL_CONSUCC_BITS  (10)     

 /* Bit Operation macros */
     /* Set bit pos  in byte data   */
#define Set_Bit_in_Data(data_ptr , bit_pos)                         ((*(data_ptr)) |=   (1<<(bit_pos)))     
      /* Clear bit pos in byte data */ 
#define Clear_Bit_in_Data(data_ptr ,bit_pos)                         ((*(data_ptr)) &= (~(1<<(bit_pos))))      
    /* flip bit pos in byte data  */ 
#define Toggle_Bit_in_Data(data_ptr , bit_pos)                       ((*(data_ptr)) ^=   (1<<(bit_pos)))        
    /* Test if bit pos in byte data  is set   */
#define Test_Bit_Is_Set_in_Data(data_ptr ,bit_pos)                    ((*(data_ptr)) & (1<<bit_pos))       
   /* Test if bit pos in byte data is clear */  
#define Test_Bit_Is_Clear_in_Data(data_ptr ,bit_pos)                  (!((*(data_ptr)) & (1<<bit_pos))) 

#define  DATA_ID_INVALID               (NUM_DATA_IDS + 1)
#define  DATA_NUM_INVALID              (0xFFFFFFFF)

#define MAX_NUM(X,Y)                      (((X) > (Y)) ? (X) : (Y))
#define MIN_NUM(X,Y)                      (((X) < (Y)) ? (X) : (Y))

#define NUM_CHARS_TRACE_CODE              (8)

/* ---------------------- data type defination -------------------------------------------- */
typedef struct 
{
	uint32_t read_or_test_consucc_val;
	uint32_t write_or_config_consucc_val;
	uint8_t start_bit_pos;
	uint8_t bits_len;
} consucc_bit_t;

typedef enum
{
   DATA_ID_READ_MODE_TILL_ALLOC , DATA_ID_READ_MODE_TILL_TERMINATOR
} cur_data_input_mode_t;

typedef enum 
{
	RCVD_CHAR_DONT_DISP_FORMAT, RCVD_CHAR_PLAIN_DISP_FORMAT, RCVD_CHAR_HIDDEN_DISP_FORMAT   
}  cur_data_disp_format_t;
 
 typedef enum 
{
	DATA_ID_RESET_INIT_STATUS, DATA_ID_RESET_WHOLE_STATUS, DATA_ID_RESET_RETRY_STATUS, DATA_ID_RESET_APPEND_INPUT_STATUS_FLAG    
}  cur_data_reset_status_t;

typedef enum 
{
	DATA_BUFFER_STREAM, DATA_CHAR_STREAM, DATA_NO_STREAM
} cur_data_stream_format_t;

typedef struct 
{
	 uint8_t data_id;
	 uint8_t data_read_num_char_enable_flag                  :  1;
	 uint8_t data_read_small_english_alpha_char_enable_flag  :  1;
	 uint8_t data_read_big_english_alpha_char_enable_flag    :  1;
	 uint8_t data_read_special_char_enable_flag              :  1;
	 uint8_t data_read_ctrl_char_enable_flag                 :  1;
	 uint8_t data_input_stream_format                        :  2;
	 uint8_t                                                 :  1; 
     uint8_t data_disp_format                                :  2;
	 uint8_t output_data_type                                :  3;
	 uint8_t input_data_type                                 :  3;
	 uint8_t data_output_stream_format                       :  2;
	 uint8_t data_tx_stream_format                           :  2;
	 uint8_t data_rcv_stream_format                          :  2;
	 uint8_t                                                 :  2;
	 char data_read_terminator_char; 
	 uint8_t max_num_data_read_try;	
     uint8_t max_req_data_len;		 	
} data_id_ctrl_para_t;

typedef struct
{
	num_value_t    data_num;
	#ifdef DMA_DATA_ID_STR
	   char *data_str;
	#else
	   char data_str[DATA_MAX_NUM_ALLOCATED_CHARS];	  
    #endif
	uint8_t data_str_len_or_pos;
} data_value_t;

typedef union
{
	const void  *dev_output_ctrl_para_ptr;	
	const void  *dev_transmit_ctrl_para_ptr;
} data_write_ctrl_ptr_t;

typedef struct
{
	 uint8_t read_valid_terminator_char_flag          : 1;
     uint8_t reach_max_alloc_read_chars_flag          : 1;
	 uint8_t data_read_mode                           : 1;	
	 uint8_t max_try_reached_flag                      : 1;
	 uint8_t read_and_ready_to_process_flag            : 1;
	 uint8_t last_try_read_and_ready_to_process_flag   : 1;
	 uint8_t                                           : 2;
} data_read_status_t;

typedef struct 
{
	 data_value_t data_value;
	 uint8_t data_id;
	 uint8_t max_allocated_data_len;	 
	 uint8_t data_read_num_try;
	 data_read_status_t data_read_status;
} data_id_status_para_t;

typedef enum
{
	DATA_RETRIEVE_CTRL_MAX_REQ_DATA_CHARS, DATA_RETRIEVE_CTRL_MAX_NUM_DATA_READ_TRY, DATA_RETRIEVE_CTRL_DATA_DISP_FORMAT, DATA_RETRIEVE_CTRL_OUTPUT_DATA_TYPE, DATA_RETRIEVE_CTRL_INPUT_DATA_TYPE,
	DATA_RETRIEVE_CTRL_INPUT_DATA_STREAM, DATA_RETRIEVE_CTRL_OUTPUT_DATA_STREAM, DATA_RETRIEVE_CTRL_TRANSMIT_DATA_STREAM, DATA_RETRIEVE_CTRL_RECEIVE_DATA_STREAM, 
	DATA_RETRIEVE_CTRL_READ_NUM_CHAR_ENABLE_FLAG, DATA_RETRIEVE_CTRL_READ_SMALL_ENGLISH_ALPHA_CHAR_ENABLE_FLAG, DATA_RETRIEVE_CTRL_READ_BIG_ENGLISH_ALPHA_CHAR_ENABLE_FLAG,
	DATA_RETRIEVE_CTRL_READ_SPECIAL_CHAR_ENABLE_FLAG, DATA_RETRIEVE_CTRL_READ_CTRL_CHAR_ENABLE_FLAG, DATA_RETRIEVE_CTRL_DATA_READ_TERMINATOR_CHAR, 
	DATA_RETRIEVE_STATUS_NUM_CHARS_READ, DATA_RETRIEVE_STATUS_DATA_STR, DATA_RETRIEVE_STATUS_DATA_CHAR, DATA_RETRIEVE_STATUS_DATA_INT32, DATA_RETRIEVE_STATUS_DATA_UINT8
} appl_data_retrieve_form_t;

typedef enum
{
	RESET_WHOLE, RESET_DATA_IDS_AND_APPL, RESET_APPL
} reset_status_t;

/* -------------------- public variable declaration --------------------------------------- */
 extern data_id_status_para_t data_id_status_para[NUM_DATA_IDS];
 extern uint8_t cur_data_id;
 extern const data_id_ctrl_para_t  data_id_ctrl_para[NUM_DATA_IDS]; 
 extern uint8_t internal_error_state;
 extern num_value_t disp_trace_num;
 extern disp_num_fmt_t disp_trace_num_fmt;
 
/* -------------------- public function declaration --------------------------------------- */ 
void SW_Time_Delay(const uint32_t max_i_count, const uint32_t max_j_count);
uint8_t Config_Consucc_Bits( const uint8_t flag_consucc_bit, void *const data_ptr);
uint8_t Test_Consucc_Bits(const uint8_t flag_consucc_bit, const void *const data_ptr );
uint8_t Str_to_Num_Conv(int32_t *const num_conv_from_str, const char *const num_in_str );
uint8_t Str_Len(const char *const str);
uint8_t Reset_Process(const uint8_t reset_type);
uint8_t Reset_Data_ID_Status(const uint8_t data_id, const uint8_t set_cur_data_status_reset_type);
void Reset_Data_IDs_Status(void);
uint8_t Next_Data_Conf_Parameter(const uint8_t set_cur_data_id);
uint8_t Data_IDs_Set_Para(void);
uint8_t Data_ID_Set_Para(const uint8_t data_id, const uint8_t max_alloc_data_len);
uint32_t Power_Of(const uint8_t base, const uint8_t power);
void Delay_MS(const uint32_t max_elapsed_time_in_ms );
void Delay_US(const uint32_t max_elapsed_time_in_us );
uint8_t Write_Bit_in_Data( uint32_t *const data_ptr, const uint8_t bit_pos, const uint8_t set_bit_val );
uint8_t Error_or_Warning_Proc(const char *const error_trace_str, const uint8_t warn_or_error_format, const uint32_t warning_or_error_code);
uint8_t Appl_Data_Retrieve_Para(const uint8_t data_id, const uint8_t retrieve_data_form, void *const retrive_data_ptr);
uint8_t Set_Status_Reset_Data_IDs(const uint8_t set_data_status_reset_type);
char *Num_Convert(const uint32_t num_to_convert, const uint8_t base );
int16_t Stream_Flush(stream_t *const stream_ptr);

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
