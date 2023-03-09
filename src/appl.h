/**************************************************************************
   FILE          :    appl.h
 
   PURPOSE       :    application header.  
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _APPL_H
#define _APPL_H

 
/* ---------------------- macro defination ------------------------------------------------ */

/* ---------------------- data type defination -------------------------------------------- */
typedef enum
{
	ERR_UART_NOT_DATA_SRC = NUM_SYS_STATUS, ERR_FLOOR_INVALID, ERR_ELEVATOR_CH_ID_EXCEEDS, ERR_ELEVATOR_DISP_STATUS, ERR_ELEVATOR_FSM_PROC, 
	ERR_ELEVATOR_FSM_INVALID, ERR_ELEVATOR_BEFORE_FSM_INVALID, ERR_ELEVATOR_DISP_PROC, ERR_ELEVATOR_CUR_FSM_INVALID, ERR_CAR_MOVE_STATE_PROC, 
	ERR_DELAY_TIME_ELEVATOR_PROC, ERR_REQ_CONDITION_PROC, ERR_POLL_FLOOR_CALLS_PROC, ERR_CAR_UNMOVED, ERR_NEXT_FLOOR_INVALID, ERR_NEXT_STOP_FLOOR_PROC, 
	ERR_STOP_NOT_CUR_FLOOR, ERR_MORE_THAN_ONE_SW_DETECT, ERR_STARTUP_OPER, ERR_CHECK_MOVE, ERR_SW_IS_INACTIVE,  ERR_SW_IS_ACTIVE, ERR_SW_IS_DISABLED,
	ERR_STOP_FLOOR_NOT_LIST, ERR_CHECK_STATIONARY, ERR_OVERLOAD_NO_USER, ERR_NO_PENDING_CALL_DOOR_CLOSED, ERR_COMPUTE_STOP_FLOOR_DATAS, 
	WARN_FLOORS_CALL_PROC, WARN_IN_CAR_FLOOR_CALL_IS_ACTIVE, WARN_HALL_CALL_UP_FLOOR_IS_ACTIVE, WARN_HALL_CALL_DOWN_FLOOR_IS_ACTIVE, ERR_HALL_CALL_UP_FLOOR_INVALID, 
	ERR_HALL_CALL_DOWN_FLOOR_INVALID, ERR_FLOOR_CALLS_NOT_ALLOWED, WARN_FLOOR_CALL_IS_ACTIVE, ERR_NOT_DETECT, ERR_LIMIT_SW_FLOOR_FAIL_AT_STATIONARY, 
	NUM_APPL_STATUS
} appl_status_t;
 
typedef enum
{
	RESET_STOP_DATAS, RESET_ABNORMAL_STATE_DATAS, RESET_WHOLE_DATAS
} elevator_reset_type_t;
 
 /* -------------------- public variable declaration --------------------------------------- */
extern uint32_t appl_status_flag;

/* -------------------- public function declaration --------------------------------------- */
uint16_t Appl_Reset(const uint8_t reset_type);
uint16_t Appl_Reset_Proc(const uint8_t ctrl_elevator_ch_id);
#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
