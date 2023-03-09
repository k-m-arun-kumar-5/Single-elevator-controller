/**************************************************************************
   FILE          :    lcd.h
 
   PURPOSE       :    LCD Header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :  
 
  CHANGE LOGS    :
	   
 **************************************************************************/
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _LCD_H
#define _LCD_H

/* ---------------------- macro defination ------------------------------------------------ */                           
#define ALL_LINES                  (0)
#define LINE_NUM_01                  (1)
#define LINE_NUM_02                  (2)
#define LINE_NUM_03                  (3)
#define LINE_NUM_04                  (4)

#define COL_NUM_01                   (1)

#define CHECK_BUSY_FLAG             (1)
#define NO_CHECK_BUSY_FLAG          (0)

#define DATA_INTERFACE_8_BITS       (1) 
#define DATA_INTERFACE_4_BITS       (0)

#define CHAR_FONT_5_10_DOTS        (1)
#define CHAR_FONT_5_8_DOTS         (0)

#define MAX_DISP_NUM_LINES_ALL    (1)
#define MAX_DISP_NUM_LINES_1      (0) 

/*for 20 * 4 LCD disp */ 
/* num cols = num of chars in a line */
#define MAX_AVAIL_NUM_COLS                    (20)
#define MAX_AVAIL_NUM_LINES                   (4)
                            
#define BEGIN_LOC_LINE1                      (0X80)
#define BEGIN_LOC_LINE2                      (0xC0)
// (0x94)
#define BEGIN_LOC_LINE3                     (BEGIN_LOC_LINE1 + MAX_AVAIL_NUM_COLS) 
//(0xD4) 
#define BEGIN_LOC_LINE4                     (BEGIN_LOC_LINE2  + MAX_AVAIL_NUM_COLS) 
// (0x93)
#define END_LOC_LINE1                       (BEGIN_LOC_LINE1 + MAX_AVAIL_NUM_COLS - 1)
//(0xD3)
#define END_LOC_LINE2                       (BEGIN_LOC_LINE2  + MAX_AVAIL_NUM_COLS - 1)
//(0xA7)
#define END_LOC_LINE3                       (BEGIN_LOC_LINE3  + MAX_AVAIL_NUM_COLS - 1) 
//(0xE7)
#define END_LOC_LINE4                       (BEGIN_LOC_LINE4 + MAX_AVAIL_NUM_COLS - 1)

#define GIVEN_CHARS_MAX_CONFIG_LINES_AND_COLS (1)
#define GIVEN_XY_MAX_CONFIG_LINES_AND_COLS    (2)
#define GIVEN_CHARS_MAX_XY                    (3)

/* should status be displayed on or off for error, warning and current time left and its count of long key press, no key press, 
  time within process should succeed or failed  */
#define STATUS_DISP_ON                            (1)  
#define STATUS_DISP_OFF                           (0)

#define LCD_INPUT_LOC               (0)
#define LCD_OUTPUT_LOC              (1)
 
#define LCD_CTRL_LOC_TYPE_DATA_POS       (7)

/* ---------------------- data type defination -------------------------------------------- */
typedef struct
 {
	uint8_t base_io_ch_ctrl;                  // IO CH for RS
	uint8_t base_io_ch_data;                  // IO CH for D0 for 8 bit interface or IO CH for D4 for 4 bit interface
	uint8_t access_flag                 : 1;
    uint8_t enable_flag                 : 1; 	
	uint8_t interface                   : 1;
	uint8_t font                        : 1;
    uint8_t check_bf                    : 1;
	uint8_t max_avail_lines_in_index    : 2;   
	uint8_t                             : 1;  
    uint8_t max_config_lines_in_index   : 2;
	uint8_t max_avail_cols_in_index     : 5;	
	uint8_t                             : 1;
	uint8_t max_config_cols_in_index    : 5;
	uint8_t                             : 3;	
} lcd_ctrl_t;

typedef struct
{
	uint8_t line_num_or_index     : 3;
    uint8_t col_num_or_index      : 5;	
} lcd_point_t;
 
typedef struct
{
	lcd_point_t cur_data_start_point_num;
	lcd_point_t next_data_start_point_num;
	uint8_t max_num_chars_config;
} lcd_point_config_t;

typedef struct
{
	lcd_point_t	start_point_num;
	disp_num_fmt_t  disp_num_fmt;
	char *lcd_disp_str;
	int32_t lcd_disp_num;
	uint8_t use_start_point_flag       :  1;
	uint8_t use_data_status_flag       :  1;
	uint8_t data_till_end_point_flag     :  1;
	uint8_t                            :  5;
} lcd_disp_ctrl_t;

typedef struct
{
	lcd_point_t start_point_num;
	lcd_point_t end_point_num;
	uint8_t start_loc;
	uint8_t end_loc;
	uint8_t max_num_chars_alloc;		
} cur_data_lcd_para_t;	

typedef struct
{
	cur_data_lcd_para_t cur_data_lcd_para;
	uint8_t lcd_cur_disp_loc;
	uint8_t lcd_cur_input_loc;
	/* currently displayed data in each line starts from 1 ie use array index as line num for us. index 0 can be used as all lines */
    uint8_t cur_line_disp_data[5];	
	uint8_t data_till_end_point_flag : 1;
	uint8_t lcd_input_loc_flag     : 1;
	uint8_t                        : 6;
} lcd_status_t;

typedef enum
{
    ERROR_DISP, WARNING_DISP, FATAL_DISP, BLANK_LINE_DISP, NUM_LCD_GNRL_DISP_DATAS 
} lcd_general_disp_data_t;

/* -------------------- public variable declaration --------------------------------------- */
extern lcd_ctrl_t lcd_ctrl[NUM_OUTPUT_DEV_ID_LCD_CHS]; 
extern lcd_status_t lcd_status[NUM_OUTPUT_DEV_ID_LCD_CHS];

/* -------------------- public prototype declaration --------------------------------------- */
uint8_t LCD_Init(const uint8_t lcd_ch_id, const uint8_t init_mode);
uint8_t LCD_Clear_Screen(const uint8_t lcd_ch_id);
uint8_t Write_LCD_Command_NO_BF(const uint8_t lcd_ch_id, const uint8_t lcd_cmd);
uint8_t LCD_Write_Command(const uint8_t lcd_ch_id, const uint8_t lcd_cmd);
uint8_t LCD_Disp_Char(const uint8_t lcd_ch_id, const char lcd_data);
uint8_t LCD_Disp_Str(const uint8_t lcd_ch_id, const char *const lcd_disp_str);
uint8_t LCD_Disp_Num(const uint8_t lcd_ch_id, const uint8_t num_data_type, const disp_num_fmt_t disp_num_fmt, const int32_t lcd_num);
uint8_t LCD_Disp_Goto_XY(const uint8_t lcd_ch_id, const uint8_t start_line_num, const uint8_t start_col_num);
uint8_t LCD_Input_Goto_XY(const uint8_t lcd_ch_id, const uint8_t start_line_num, const uint8_t start_col_num);
uint8_t LCD_From_XY_To_Loc(const uint8_t lcd_ch_id, const lcd_point_t *const start_lcd_point, uint8_t *const lcd_loc_ptr);
uint8_t LCD_From_Loc_to_XY(const uint8_t lcd_ch_id, const uint8_t loc_lcd, lcd_point_t *const lcd_point_num_ptr);
void Set_Cur_Loc_LCD(const uint8_t lcd_ch_id, const char set_input_loc_flag,const uint8_t set_input_loc, const char set_disp_loc_flag, const uint8_t set_disp_loc);
uint8_t LCD_Write(const uint8_t lcd_ch_id, const void *const lcd_rcv_disp_num_ptr);
uint8_t LCD_Disable(const uint8_t lcd_ch_id);
uint8_t LCD_Enable(const uint8_t lcd_ch_id);
uint8_t Disable_All_LCDs(void);
uint8_t LCD_Allow_Access(const uint8_t lcd_ch_id);
uint8_t LCD_No_Access(const uint8_t lcd_ch_id);
uint8_t LCD_DeInit(const uint8_t lcd_ch_id, const uint8_t deinit_mode);
uint8_t LCD_Cur_Data_By_NumChars_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr);
uint8_t LCD_Cur_Data_By_XY_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr);
uint8_t LCD_Cur_Data_By_NumChars_and_XY_Calc_Para(const uint8_t lcd_ch_id, const lcd_point_config_t *const lcd_point_config_ptr);
uint8_t Reset_LCD_Cur_Data_Status_Para(uint8_t lcd_ch_id);
uint8_t LCD_Loc_Ctrl_Proc(const uint8_t lcd_ch_id, const uint8_t lcd_ctrl_format, const char lcd_rcvd_char );
#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
