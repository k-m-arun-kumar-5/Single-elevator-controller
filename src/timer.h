/**************************************************************************
   FILE          :    timer.h
 
   PURPOSE       :    timer header.  
 
   AUTHOR        :   K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _TIMER_H
#define _TIMER_H

 
/* ---------------------- macro defination ------------------------------------------------ */
#define TMR_OR_COUNTER_STOP_ID                     (0x1F)
#define TMR_OR_COUNTER_SYSTEM_TICK_STATE           (0x1E)
#define TMR_OR_COUNTER_PAUSE_ID                    (0x1D)

#define TMR_PRESET_TIME_DELAY_IN_MULTIPLE            (0U)
#define TMR_POSTSET_TIME_DELAY_IN_MULTIPLE           (1U) 

#define NO_MAX_NUM_TIMEOUTS_OR_COUNT                  (0) 
#define TIMER_MODE                                    (0)
#define COUNTER_MODE                                  (1)

#define NUM_MATCH_REGS                                (4)
/* ---------------------- data type defination -------------------------------------------- */
 typedef struct
 {
	 uint8_t timer_or_counter_id;	
	 uint8_t mode_type                              : 1;
	 uint8_t match_or_capture_id                    : 2;
	 uint8_t match_interrupt_or_capture_rising_flag : 1;
	 uint8_t capture_falling_flag                   : 1;
	 uint8_t capture_interrupt_flag                 : 1;
	 uint8_t state                                  : 2;
	 uint16_t conf_timeout_in_ms_or_max_count;
     uint8_t max_num_timeouts                       : 7;
     uint8_t timer_max_num_timeout_occured_flag     : 1; 
	 uint8_t elapsed_num_timeouts;
	 uint16_t actual_timeout_in_ms_or_max_count;
	 uint16_t elapsed_time_in_ms_or_count;	
	 uint8_t max_count_per_timeout;
	 uint8_t elapsed_count_per_timeout;
 } timer_or_counter_ctrl_t;

 typedef struct
 {	
	 uint8_t timer_or_counter_cur_run_id; 
	 uint8_t timer_or_counter_last_run_id                : 7;	    
	 uint8_t exec_timer_interrupt_enable_proc            : 1;	
 } timer_or_counter_status_t;
 
 typedef struct
 {
	 uint16_t appl_proc_exec_time_interval_in_ms; 
	 uint8_t match_ch_id                            : 2;
	 uint8_t                                        : 6;  
 } timer_0_ctrl_t;
 
 typedef struct
 {
	 uint8_t timer_or_counter_run_id;
	 uint8_t elapsed_num_timeouts_or_count;
 } timer_or_counter_data_t;
 
 typedef enum 
 {
	 TMR_OR_COUNTER_IDLE_STATE, TMR_OR_COUNTER_RUNNING_STATE, TMR_OR_COUNTER_STOPPED_STATE, TMR_OR_COUNTER_PAUSED_STATE
 } tmr_or_counter_states_t;
 
/* -------------------- public variable declaration --------------------------------------- */
extern timer_or_counter_ctrl_t timer_or_counter_ctrl[];
extern volatile timer_or_counter_status_t timer_or_counter_status[];

/* -------------------- public function declaration --------------------------------------- */
uint16_t Timer_Run(const uint8_t timer_ch_id, const uint8_t run_timer_run_id); 
uint16_t Timer_Stop(const uint8_t timer_ch_id );
uint16_t Timer_0_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_0_or_counter_data_ptr);
uint16_t Timer_1_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_1_or_counter_data_ptr);
uint16_t Disable_Timer_Interrupt(const uint8_t timer_ch_id);
uint16_t Timer_0_Run(const uint8_t match_ch_id, const uint16_t appl_proc_exec_conf_time_interval_in_ms);
uint16_t Timer_Interrupt_Retrieve_Data_Arr(const uint8_t timer_ch_id, timer_or_counter_data_t *const retrieved_timer_or_counter_interrupt_data_ptr);
uint16_t Timer_Pause(const uint8_t timer_ch_id);
uint16_t Timer_Resume(const uint8_t timer_ch_id, const uint8_t resume_timer_run_id);

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
