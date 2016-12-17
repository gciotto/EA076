/* ###################################################################
**     Filename    : Events.h
**     Project     : ea076-exp2
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2016-03-15, 18:39, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "DataCmd.h"
#include "BitsIoLdd1.h"
#include "Enable.h"
#include "BitIoLdd1.h"
#include "RS.h"
#include "BitIoLdd2.h"
#include "timer.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
#include "LED.h"
#include "BitIoLdd3.h"
#include "Button.h"
#include "BitIoLdd4.h"
#include "Conversor.h"
#include "AdcLdd1.h"
#include "C1.h"
#include "BitIoLdd5.h"
#include "C2.h"
#include "BitIoLdd6.h"
#include "C3.h"
#include "BitIoLdd7.h"
#include "L1.h"
#include "BitIoLdd8.h"
#include "L2.h"
#include "BitIoLdd9.h"
#include "L3.h"
#include "BitIoLdd10.h"
#include "L4.h"
#include "BitIoLdd11.h"
#include "EInt1.h"
#include "ExtIntLdd1.h"
#include "WAIT1.h"
#include "EE241.h"
#include "GI2C1.h"
#include "CI2C1.h"
#include "ConversorDA.h"
#include "DacLdd1.h"
#include "KSDK1.h"

#define INTERRUPT_PERIOD 50.0

#ifdef __cplusplus
extern "C" {
#endif 

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void);


/*
** ===================================================================
**     Event       :  timer_OnInterrupt (module Events)
**
**     Component   :  timer [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
int wait_interval, autorizacao;
unsigned int tick_counter;
void timer_OnInterrupt(void);

void Conversor_OnEnd(void);
/*
** ===================================================================
**     Event       :  Conversor_OnEnd (module Events)
**
**     Component   :  Conversor [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void Conversor_OnCalibrationEnd(void);
/*
** ===================================================================
**     Event       :  Conversor_OnCalibrationEnd (module Events)
**
**     Component   :  Conversor [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

int* buttons[4];
int isReady;
void EInt1_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  EInt1_OnInterrupt (module Events)
**
**     Component   :  EInt1 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
