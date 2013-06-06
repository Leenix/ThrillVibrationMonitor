/** ###################################################################
**     Filename    : Events.c
**     Project     : ProcessorExpert
**     Processor   : MC9S08JM60CLDE
**     Component   : Events
**     Version     : Driver 01.02
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2013-05-23, 15:23, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/* MODULE Events */


#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  wirelessSerial_OnRxChar (module Events)
**
**     Component   :  wirelessSerial [SynchroMaster]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**         
**         [ Version specific information for Freescale HCS08 and RS08
**         and HCS12 and HCS12X and ColdFireV1 derivatives ] 
**         The <Bidirectional mode> property setting doesn't affect
**         this event. For more information please see <Bidirectional
**         mode behavior> paragraph.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void wirelessSerial_OnRxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  wirelessSerial_OnTxChar (module Events)
**
**     Component   :  wirelessSerial [SynchroMaster]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void wirelessSerial_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  wirelessSerial_OnError (module Events)
**
**     Component   :  wirelessSerial [SynchroMaster]
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
void wirelessSerial_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  batteryMonitor_OnEnd (module Events)
**
**     Component   :  batteryMonitor [ADC]
**     Description :
**         This event is called after the measurement (which
**         consists of <1 or more conversions>) is/are finished.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  batteryMonitor_OnEnd(void)
{
    /* Write your code here ... */
}


/*
** ===================================================================
**     Event       :  accelerometer_OnEnd (module Events)
**
**     Component   :  accelerometer [ADC]
**     Description :
**         This event is called after the measurement (which
**         consists of <1 or more conversions>) is/are finished.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void  accelerometer_OnEnd(void)
{
    /* Write your code here ... */
}


/*
** ===================================================================
**     Event       :  samplingTimer_OnInterrupt (module Events)
**
**     Component   :  samplingTimer [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void samplingTimer_OnInterrupt(void)
{
  /* Write your code here ... */

}

/* END Events */

/*
** ###################################################################
**
**     This file was created by Processor Expert 10.0 [05.03]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
