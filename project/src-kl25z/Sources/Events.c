/* ###################################################################
 **     Filename    : Events.c
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
 ** @file Events.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */         
/*!
 **  @addtogroup Events_module Events module documentation
 **  @{
 */         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 
extern bool waiting;

/* User includes (#include below this line is not maintained by Processor Expert) */

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
void Cpu_OnNMIINT(void)
{
	/* Write your code here ... */
}

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
void timer_OnInterrupt(void)
{
	if (wait_interval == 0){
		//timer_Disable();
		autorizacao  = 1;
	}

	if (wait_interval > 0)   wait_interval--;
	
	tick_counter++;

}

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
void Conversor_OnEnd(void)
{
	waiting = TRUE;
	/* Write your code here ... */
}

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
void Conversor_OnCalibrationEnd(void)
{
	/* Write your code here ... */
}

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
void EInt1_OnInterrupt(void)
{
	static unsigned int last_tick = 0;
	
	if (tick_counter - last_tick > ceil(120000 / INTERRUPT_PERIOD) ) {
	/* Write your code here ... */
		get_keys(buttons);
		isReady = 1;
	}
	
	last_tick = tick_counter;

}

/*
** ===================================================================
**     Event       :  Bluetooth_OnError (module Events)
**
**     Component   :  Bluetooth [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Bluetooth_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  Bluetooth_OnRxChar (module Events)
**
**     Component   :  Bluetooth [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Bluetooth_OnRxChar(void)
{
  /* Write your code here ... */
	hasReceivedChar = 1;	
}

/*
** ===================================================================
**     Event       :  Bluetooth_OnTxChar (module Events)
**
**     Component   :  Bluetooth [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Bluetooth_OnTxChar(void)
{
  /* Write your code here ... */
	isReadyToSend = 1;
}

/*
** ===================================================================
**     Event       :  STM32_OnError (module Events)
**
**     Component   :  STM32 [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void STM32_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  STM32_OnRxChar (module Events)
**
**     Component   :  STM32 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void STM32_OnRxChar(void)
{
   hasReceivedCharFromSTM32 = 1;
}

/*
** ===================================================================
**     Event       :  STM32_OnTxChar (module Events)
**
**     Component   :  STM32 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void STM32_OnTxChar(void)
{
	isReadyToSendToSTM32 = 1;
}

/*
** ===================================================================
**     Event       :  STM32_OnFullRxBuf (module Events)
**
**     Component   :  STM32 [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void STM32_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
