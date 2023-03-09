/**************************************************************************
   FILE          :    elevator_fsm.h
 
   PURPOSE       :    elevator fsm proc header.  
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _ELEVATOR_FSM_H
#define _ELEVATOR_FSM_H
 
/* ---------------------- macro defination ------------------------------------------------ */
#define FLOOR_ID_INVALID        (MAX_NUM_FLOORS  + 1)
#define TOP_MOST_FLOOR_ID       (MAX_NUM_FLOORS  - 1)

#define MAX_NUM_ELEVATORS       (1)
#define CTRL_ELEVATOR_CH_ID     (0)

#define UP_SW_PROC              (0)
#define DOWN_SW_PROC            (1)
#define IN_CAR_SW_PROC          (2) 
                   
/* ---------------------- data type defination -------------------------------------------- */
typedef enum 
{
	MOVE_UP, MOVE_DOWN, STARTUP_STATIONARY, MOVED_UP_STATIONARY, MOVED_DOWN_STATIONARY,  NO_PENDING_FLOOR_CALLS, 
	TRIGGER_MOVE_UP_NO_DIR_CHANGE, TRIGGER_MOVE_UP_DIR_CHANGE, TRIGGER_MOVE_DOWN_NO_DIR_CHANGE, TRIGGER_MOVE_DOWN_DIR_CHANGE, 
	TRIGGERED_EMERGENCY_STOP, ERR_MOVE_UP_NEXT_FLOOR_INVALID, ONE_PENDING_FLOOR_CALL, ERR_MOVE_DOWN_NEXT_FLOOR_INVALID, ERR_DOOR_OPEN_NOT_DETECT, 
	ERR_DOOR_CLOSE_NOT_DETECT, ERR_REQ_DOORS_ALIGN_BUT_UNALIGN, ERR_AT_LEAST_TWO_LIMIT_SW_DETECT, ERR_STARTUP_AT_LEAST_TWO_LIMIT_SW_DETECT, 
	ERR_MOVING_UP_NEXT_FLOOR_NOT_DETECT, ERR_MOVING_DOWN_NEXT_FLOOR_NOT_DETECT, ERR_DOORS_NOT_ALIGNED_PROPERLY, ERR_REQ_DOOR_CLOSE_BUT_OPENED, 
	ERR_REQ_DOOR_OPEN_BUT_CLOSED, ERR_MOVE_UP_BUT_UNMOVED, ERR_MOVE_DOWN_BUT_UNMOVED, ERR_STARTUP_CUR_FLOOR_NOT_DETECT, ERR_DOOR_OPEN_AND_CLOSE_ACTIVE_FOR_OPEN, 
	ERR_DOOR_OPEN_AND_CLOSE_ACTIVE_FOR_CLOSE, ERR_DOOR_OPEN_FAST, ERR_DOOR_CLOSE_FAST, ERR_REQ_DOOR_CLOSED_BUT_NOT_CLOSED, ERR_REQ_DOORS_UNALIGN_BUT_ALIGN, 
	ERR_DOOR_OPENED_BUT_READY_MOVE, ERR_DOOR_NOT_CLOSED_BUT_READY_MOVE, ERR_STARTUP_DOOR_OPEN_AND_CLOSE_ACTIVE, ERR_DOORS_UNALIGNED_BUT_OPENED,
	ERR_DOORS_UNALIGNED_BUT_NOT_CLOSED, ERR_DOORS_ALIGNED_FOR_MOVE, ERR_DOORS_UNALIGNED_AT_STATIONARY, ERR_DOOR_NOT_OPENED_AT_STATIONARY,
	ERR_DOORS_CLOSED_AT_STATIONARY, ERR_LIMIT_FLOOR_NOT_DETECT_AT_STATIONARY, ERR_OVERLOAD_BUT_NO_IN_CAR_USER_PRESENT, 
	ERR_NEXT_STOP_FLOOR_REACH_BUT_NOT_IN_STOP_LIST, ERR_MAX_STOP_FLOOR_REACH_BUT_NOT_IN_STOP_LIST, ERR_MIN_STOP_FLOOR_REACH_BUT_NOT_IN_STOP_LIST,
	ERR_NEXT_STOP_MORE_THAN_MAX_STOP_FLOOR, ERR_NEXT_STOP_LESS_THAN_MIN_STOP_FLOOR, ERR_COMPUTE_NEXT_FLOOR_STOP_PROC, ERR_ELEVATOR_NOT_STATIONARY, 
	ERR_MIN_STOP_FLOOR_INVALID, ERR_MIN_STOP_FLOOR_NOT_IN_STOP_LIST, ERR_MAX_STOP_FLOOR_INVALID, WARN_FLOOR_CALL_ALREADY_SELECT, WARN_FLOOR_CALL_SAME_CUR_FLOOR,
	ERR_COMPUTE_NEXT_STOP_FLOOR, ERR_NEXT_STOP_FLOOR_NOT_STOPPED, ERR_INTERNAL_PROC_ABNORMAL, ERR_CUR_FLOOR_INVALID, ERR_POLL_HALL_AND_IN_CAR_PROC, 
	ERR_DELAY_TIME_PROC, ERR_CAR_MOVEMENT_PROC, ERR_DISP_STATUS, ERR_TOP_FLOOR_DETECT_LIMIT_SW_DOWN, ERR_BOTTOM_FLOOR_DETECT_LIMIT_SW_UP,
	ERR_LIMIT_SW_UP_AND_DOWN_FLOOR_INVALID, ERR_ATLEAST_TWO_LIMIT_SW_UP_OR_DOWN_FLOOR, MOVE_UP_NONE_CUR_UP_TO_NEXT_DOWN, MOVE_UP_CUR_UP_TO_NEXT_DOWN, MOVE_UP_NEXT_DOWN_TO_NO_CUR_UP,
	 MOVE_UP_NO_CUR_UP_TO_NEXT_UP, MOVE_UP_CUR_UP_TO_NONE_NEXT_DOWN, MOVE_UP_NONE_NEXT_DOWN_TO_NEXT_UP, MOVE_DOWN_NONE_CUR_DOWN_TO_NEXT_UP,
	MOVE_DOWN_CUR_DOWN_TO_NEXT_UP, MOVE_DOWN_NEXT_UP_TO_NO_CUR_DOWN, MOVE_DOWN_NO_CUR_DOWN_TO_NEXT_DOWN, MOVE_DOWN_CUR_DOWN_TO_NONE_NEXT_UP, MOVE_DOWN_NONE_NEXT_UP_TO_NEXT_DOWN, 
	ERR_CUR_LIMIT_SW_INACTIVE, ERR_NEXT_FLOOR_NOT_DETECT, ERR_DOORS_ALIGNED_BUT_CAR_IN_BETWEEN_FLOORS, ERR_DOOR_OPENED_BUT_CAR_IN_BETWEEN_FLOORS, ERR_DOOR_NOT_CLOSED_BUT_CAR_IN_BETWEEN_FLOORS,
	ERR_LIMIT_FLOOR_UP_NOT_DETECT_AT_STATIONARY, ERR_LIMIT_FLOOR_DOWN_NOT_DETECT_AT_STATIONARY, ERR_LIMIT_FLOOR_INVALID_AT_STATIONARY, ERR_DOOR_NOT_CLOSED_BUT_NOT_ALIGNED,
	ERR_DOOR_OPENED_BUT_NOT_ALIGNED, NUM_ELEVATOR_STATUS
} elevator_status_t;

typedef enum 
{
	DISP_MOVE_UP, DISP_MOVE_DOWN, DISP_STATIONARY, DISP_ERROR, NUM_ELEVATOR_DISP_STATUS
} disp_elevator_status_code_t;

typedef enum
{
	FSM_IDLE, FSM_WAIT_FOR_START_OPER, FSM_STARTUP, FSM_DECIDE_CAR_MOVEMENT, FSM_TRIGGER_MOVE_UP, FSM_TRIGGER_MOVE_DOWN, 
	FSM_PREPARE_TO_MOVE, FSM_MOVING, FSM_TRIGGER_CAR_STOP, FSM_WAIT_TILL_CAR_STOPPED, FSM_PREPARE_DOORS_TO_ALIGN, 
	FSM_WAIT_TILL_DOORS_ALIGN, FSM_TRIGGER_DOOR_OPEN, FSM_WAIT_TILL_DOOR_START_OPEN, FSM_WAIT_TILL_DOOR_OPENED, 
	FSM_PREPARE_USER_ENTRY_AND_EXIT, FSM_USER_ENTRY_AND_EXIT, FSM_PREPARE_FOR_DOOR_CLOSE, FSM_TRIGGER_DOOR_CLOSE, 
	FSM_WAIT_TILL_DOOR_START_CLOSE, FSM_WAIT_TILL_DOOR_CLOSED, FSM_WAIT_TILL_DOORS_TO_UNALIGN, FSM_COMPUTE_NEXT_STOP_FLOOR,
    FSM_WAIT_FOR_DOOR_CLOSE_TO_START_OPEN, FSM_ABNORMAL_EVENT, NUM_ELEVATOR_FSM_STATES  	
} elevator_oper_fsm_states_t;

typedef enum 
{
	FLOOR_00, FLOOR_01, FLOOR_02, FLOOR_03, FLOOR_04
} floor_id_t;

typedef struct
{
   uint8_t cur_max_floor_call;
   uint8_t cur_min_floor_call;
   uint8_t cur_max_floor_call_up;
   uint8_t cur_min_floor_call_up;
   uint8_t cur_max_floor_call_down;
   uint8_t cur_min_floor_call_down;
   uint8_t cur_max_floor_call_in_car;
   uint8_t cur_min_floor_call_in_car;
   uint8_t cur_floor;
   uint8_t elevator_status;
   uint8_t pending_floor_call_bit_field;
   uint8_t next_stop_floor;
   uint8_t cur_fsm_state;
   uint8_t before_fsm_state;
   uint8_t in_car_floor_call_bit_field;
   uint8_t hall_floor_call_up_bit_field;
   uint8_t hall_floor_call_down_bit_field;
   uint8_t num_pending_calls_in_car;
   uint8_t num_pending_calls_up;
   uint8_t num_pending_calls_down;
   uint8_t num_pending_calls;
} elevator_ctrl_and_status_t;

typedef enum 
{ 
  DOOR_CLOSED_STAGE, DOOR_OPENED_STAGE, NUM_ELEVATOR_STAGES 
} elevator_stages_t;

typedef uint16_t (*elevator_fsm_proc_func_t)(const uint8_t ctrl_elevator_ch_id);

 /* -------------------- public variable declaration --------------------------------------- */
extern elevator_ctrl_and_status_t elevator_ctrl_and_status[MAX_NUM_ELEVATORS];
extern elevator_fsm_proc_func_t Elevator_FSM_Proc_Func_Ptr[NUM_ELEVATOR_FSM_STATES];
extern uint8_t limit_sw_up_arr[MAX_NUM_FLOORS];
extern uint8_t limit_sw_down_arr[MAX_NUM_FLOORS];
extern uint8_t hall_call_up_arr[MAX_NUM_FLOORS];
extern uint8_t hall_call_down_arr[MAX_NUM_FLOORS];
extern uint8_t in_car_call_arr[MAX_NUM_FLOORS];

/* -------------------- public function declaration --------------------------------------- */
uint16_t Validate_Floor(const uint8_t floor);
uint16_t Reset_Elevator_Datas(const uint8_t ctrl_elevator_ch_id, const uint8_t reset_type);
uint16_t Compute_Next_Floor_Stop(const uint8_t ctrl_elevator_ch_id, uint8_t *const elevator_next_fsm_state_ptr);
uint16_t Disp_Elevator_Status(const uint8_t ctrl_elevator_ch_id);
uint16_t Elevator_FSM_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Idle_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Start_Oper_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_StartUp_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Decide_Car_Move_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Trigger_Move_Up_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Trigger_Move_Down_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Prepare_To_Move_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Car_Moving_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Trigger_Car_Stop_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Car_Stopped_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Prepare_Doors_To_Align_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Doors_Aligned_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Trigger_Door_Open_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Door_Start_Open_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Door_Opened_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Prepare_User_Entry_And_Exit_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_User_Entry_And_Exit_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Prepare_Door_Close_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Trigger_Door_Close_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Door_Start_Close_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Door_Closed_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_Till_Doors_Unaligned_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Compute_Next_Stop_Floor_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Wait_For_Door_Close_To_Start_Open_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t FSM_Abnormal_Event_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t Car_Movement_Direction(const uint8_t ctrl_elevator_ch_id, const uint8_t cur_floor, uint8_t *const elevator_trigger_move_ptr);
uint16_t Delay_Time_Elevator(const uint8_t ctrl_elevator_ch_id, const uint32_t max_delay_count);
uint16_t Check_Move_Condition(const uint8_t ctrl_elevator_ch_id, const uint8_t proc_fsm_state);
uint16_t Check_Stationary_Door_Opened_State(const uint8_t ctrl_elevator_ch_id, const uint8_t proc_fsm_state);
uint16_t Poll_Hall_And_In_Car_Floor_Calls_Proc(const uint8_t ctrl_elevator_ch_id );
uint16_t Internal_Error_Elevator_Proc(const uint8_t ctrl_elevator_ch_id);
uint16_t Compute_Floor_Stop_Datas(const uint8_t ctrl_elevator_ch_id, const uint8_t stage_type, uint8_t *const elevator_status_ptr);
uint16_t Check_Stationary_Limit_SW_Floor(const uint8_t ctrl_elevator_ch_id);

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
