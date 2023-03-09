
/**************************************************************************
   FILE          :    port.h
 
   PURPOSE       :    port header - define port and its pin assignment.
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :    port header 
 To avoid this header file to be included more than once, conditional directive is used  
	
  CHANGE LOGS     :  
	   
 **************************************************************************/
#ifndef _PORT_H
#define _PORT_H

/* ---------------------- macro defination ------------------------------------------------ */
	

/* ---------------------- data type defination -------------------------------------------- */
typedef enum 
{
 	ALARM_LED_IO_CH = (IO_CH_30), CAR_UP_IO_CH = (IO_CH_14), CAR_DOWN_IO_CH = (IO_CH_27), CAR_DOOR_OPEN_IO_CH = (IO_CH_28), 
	CAR_DOOR_CLOSE_IO_CH = (IO_CH_29), IO_CH_SEG7_CH_00_A_LED = (IO_CH_16), IO_CH_SEG7_CH_00_UNIT_DIGIT = (IO_CH_25)
} io_ch_map_op_ch_t;
 
typedef enum 
{
 	IO_CH_SW_CH_00 = (IO_CH_02), OVERLOAD_SENSOR_IO_CH = IO_CH_SW_CH_00,           IO_CH_SW_CH_01 = (IO_CH_03), USER_BLOCKS_DOOR_SENSOR_IO_CH = IO_CH_SW_CH_01,
	IO_CH_SW_CH_02 = (IO_CH_04), LIMIT_SW_DOWN_FLOOR_0_IO_CH = IO_CH_SW_CH_02,     IO_CH_SW_CH_03 = (IO_CH_05), LIMIT_SW_DOWN_FLOOR_1_IO_CH = IO_CH_SW_CH_03,
	IO_CH_SW_CH_04 = (IO_CH_06), LIMIT_SW_DOWN_FLOOR_2_IO_CH = IO_CH_SW_CH_04,     IO_CH_SW_CH_05 = (IO_CH_07), LIMIT_SW_DOWN_FLOOR_3_IO_CH = IO_CH_SW_CH_05,
	IO_CH_SW_CH_06 = (IO_CH_10), CAR_START_SW_IO_CH = IO_CH_SW_CH_06,              IO_CH_SW_CH_07 = (IO_CH_11), DOORS_ALIGNED_SW_IO_CH = IO_CH_SW_CH_07,
    IO_CH_SW_CH_08 = (IO_CH_12), MANUAL_DOOR_OPEN_SW_IO_CH = IO_CH_SW_CH_08,       IO_CH_SW_CH_09 = (IO_CH_13), MANUAL_DOOR_CLOSE_SW_IO_CH = IO_CH_SW_CH_09,	
	IO_CH_SW_CH_10 = (IO_CH_15), LIMIT_SW_DOOR_CLOSE_IO_CH = IO_CH_SW_CH_10,       IO_CH_SW_CH_11 = (IO_CH_23), LIMIT_SW_DOOR_OPEN_IO_CH = IO_CH_SW_CH_11,
	IO_CH_SW_CH_12 = (IO_CH_31), LIMIT_SW_UP_FLOOR_1_IO_CH = IO_CH_SW_CH_12,       IO_CH_SW_CH_13 = (IO_CH_48), LIMIT_SW_UP_FLOOR_2_IO_CH = IO_CH_SW_CH_13,
	IO_CH_SW_CH_14 = (IO_CH_49), LIMIT_SW_UP_FLOOR_3_IO_CH = IO_CH_SW_CH_14,       IO_CH_SW_CH_15 = (IO_CH_50), LIMIT_SW_UP_FLOOR_4_IO_CH = IO_CH_SW_CH_15,
	IO_CH_SW_CH_16 = (IO_CH_51), IN_CAR_CALL_SW_FLOOR_0_IO_CH = IO_CH_SW_CH_16,    IO_CH_SW_CH_17 = (IO_CH_52), IN_CAR_CALL_SW_FLOOR_1_IO_CH = IO_CH_SW_CH_17,
	IO_CH_SW_CH_18 = (IO_CH_53), IN_CAR_CALL_SW_FLOOR_2_IO_CH = IO_CH_SW_CH_18,    IO_CH_SW_CH_19 = (IO_CH_54), IN_CAR_CALL_SW_FLOOR_3_IO_CH = IO_CH_SW_CH_19,
	IO_CH_SW_CH_20 = (IO_CH_55), IN_CAR_CALL_SW_FLOOR_4_IO_CH = IO_CH_SW_CH_20,    IO_CH_SW_CH_21 = (IO_CH_56), HALL_CALL_UP_FLOOR_0_SW_IO_CH = IO_CH_SW_CH_21,
	IO_CH_SW_CH_22 = (IO_CH_57), HALL_CALL_UP_FLOOR_1_SW_IO_CH = IO_CH_SW_CH_22,   IO_CH_SW_CH_23 = (IO_CH_58), HALL_CALL_DOWN_FLOOR_1_SW_IO_CH = IO_CH_SW_CH_23, 
	IO_CH_SW_CH_24 = (IO_CH_59), HALL_CALL_UP_FLOOR_2_SW_IO_CH = IO_CH_SW_CH_24,   IO_CH_SW_CH_25 = (IO_CH_60), HALL_CALL_DOWN_FLOOR_2_SW_IO_CH = IO_CH_SW_CH_25,
	IO_CH_SW_CH_26 = (IO_CH_61), HALL_CALL_UP_FLOOR_3_SW_IO_CH = IO_CH_SW_CH_26,   IO_CH_SW_CH_27 = (IO_CH_62), HALL_CALL_DOWN_FLOOR_3_SW_IO_CH = IO_CH_SW_CH_27,
	IO_CH_SW_CH_28 = (IO_CH_63), HALL_CALL_DOWN_FLOOR_4_SW_IO_CH = IO_CH_SW_CH_28   	
} io_ch_map_ip_ch_t; 

/* -------------------- public variable declaration --------------------------------------- */
 
/* -------------------- public function declaration --------------------------------------- */

 
#endif 

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
