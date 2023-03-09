/**************************************************************************
   FILE          :    isr.h
 
   PURPOSE       :    interrupt service routine header.  
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _ISR_H
#define _ISR_H
 
/* ---------------------- macro defination ------------------------------------------------ */
#define MAX_NUM_EXT_INTERRUPTS           (4)

#define EXT_INTERRUPT_LOW_LEVEL          (0)
#define EXT_INTERRUPT_HIGH_LEVEL         (1)
#define EXT_INTERRUPT_FALLING_EDGE       (2)
#define EXT_INTERRUPT_RISING_EDGE        (3) 
#define EXT_INTERRUPT_SENSE_NA           (4)

#define INTERRUPT_REQ_VECTOR_IRQ          (0)
#define INTERRUPT_REQ_FIQ                 (1)
#define INTERRUPT_REQ_NON_VECTOR_IRQ      (2) 
#define INTERRUPT_REQ_NA                  (3) 

#define VIC_VECTOR_ADDR(slot)           (VICVectAddr##slot) 
#define VIC_VECTOR_CTRL(slot)           (VICVectCntl##slot)

#define ISR_EXT_INTERRUPT(ext_interrupt_id) (uint32_t) (ISR_Ext_Interrupt_##ext_interrupt_id)
  
/* ---------------------- data type defination -------------------------------------------- */
typedef enum 
{ 
   INTP_REQ_WDT, INTP_REQ_RESERVED_SW_INTP, INTP_REQ_ARM_CORE0, INTP_REQ_ARM_CORE1, INTP_REQ_TMR0, INTP_REQ_TMR1, INTP_REQ_UART0, INTP_REQ_UART1,
   INTP_REQ_PWM0, INTP_REQ_I2C0, INTP_REQ_SPI0, INTP_REQ_SPI1_OR_SSP, INTP_REQ_PLL, INTP_REQ_RTC, INTP_REQ_EINT0, INTP_REQ_EINT1,
   INTP_REQ_EINT2, INTP_REQ_EINT3, INTP_REQ_AD0, INTP_REQ_I2C1, INTP_REQ_BOD, INTP_REQ_AD1, NUM_INTP_REQ_SRCS, INTP_REQ_SRC_INVALID
} interrupt_request_src_t;

typedef enum 
{ 
   VECTOR_SLOT_00,  VECTOR_SLOT_01,  VECTOR_SLOT_02,  VECTOR_SLOT_03,  VECTOR_SLOT_04,  VECTOR_SLOT_05, VECTOR_SLOT_06, VECTOR_SLOT_07, 
   VECTOR_SLOT_08,  VECTOR_SLOT_09,  VECTOR_SLOT_10,  VECTOR_SLOT_11,  VECTOR_SLOT_12,  VECTOR_SLOT_13, VECTOR_SLOT_14, VECTOR_SLOT_15,
   NUM_VECTOR_SLOTS, VECTOR_SLOT_NA   
} vector_slot_id_t;

typedef struct 
{
	uint32_t isr_vector_irq; 
	uint8_t  enable_interrupt_request_src                    : 5;    
	uint8_t  interrupt_type                           : 2;
	uint8_t  next_triggered_irqs_while_proc_isr_flag  : 1;  // Process ISRs triggered by next successive valid vectored IRQ sources other than current processing ISR while processing ISR
	uint8_t  vector_irq_slot                          : 5;
	uint8_t  same_triggered_irqs_while_proc_isr_flag  : 1;  // Process ISRs triggered by next successive valid vectored IRQ sources while processing ISR
    uint8_t                                           : 2;	
} interrupt_request_ctrl_t;

typedef struct
{
	uint8_t ext_interrupt_src        : 2;
    uint8_t ext_interrupt_sense      : 3;
    uint8_t ext_interrupt_func_mode  : 3;	
} ext_interrupt_ctrl_t;

typedef struct 
{
  volatile void *src_interrupt_data_ptr;
  uint16_t process_interrupt_status;
  uint8_t src_interrupt_dev;  
} src_interrupt_dev_data_t;

/* -------------------- public variable declaration --------------------------------------- */
extern interrupt_request_ctrl_t  interrupt_request_ctrl[NUM_INTP_REQ_SRCS];
extern ext_interrupt_ctrl_t  ext_interrupt_ctrl[MAX_NUM_EXT_INTERRUPTS];
extern volatile src_interrupt_dev_data_t src_interrupt_dev_data[];

/* -------------------- public function declaration --------------------------------------- */
uint16_t Enable_Interrupt_Src(const uint8_t enable_interrupt_request_src); 
uint16_t Enable_Ext_Interrupt(const uint8_t ext_interrupt_id);
uint16_t Ext_Interrupt_0_Proc(void);
uint16_t Ext_Interrupt_1_Proc(void);
uint16_t Ext_Interrupt_2_Proc(void);
uint16_t Ext_Interrupt_3_Proc(void);
uint16_t Disable_Interrupt_Src(const uint8_t disable_interrupt_request_src);
uint16_t Disable_Ext_Interrupt(const uint8_t ext_interrupt_ch_id);
void Init_Interrupt(void);
uint16_t Reset_Src_Interrupt_Data_Arr(const uint8_t src_interrupt_data_arr_index);
uint16_t Search_Src_Interrupt_Data_Arr_Free_Index(uint8_t *const src_interrupt_data_arr_free_index_ptr);
uint16_t Search_Src_Interrupt_Data_Arr_By_Src(const uint8_t search_src_interrupt_dev, uint8_t *const src_interrupt_data_arr_free_index_ptr);
uint16_t Delete_Src_Interrupt_Data_Arr_By_Src(const uint8_t delete_src_interrupt_dev);
uint16_t Ext_Interrupt_Retrieve_Data_Arr(const uint8_t ext_intp_ch_id);
void ISR_Timer_0(void) __irq;
void ISR_Timer_1(void) __irq;
void ISR_UART_0(void) __irq;
void ISR_UART_1(void) __irq;
void ISR_Ext_Interrupt_0(void) __irq;
void ISR_Ext_Interrupt_1(void) __irq;
void ISR_Ext_Interrupt_2(void) __irq;
void ISR_Ext_Interrupt_3(void) __irq;
void ISR_Non_Vector_IRQ(void) __irq;

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/
