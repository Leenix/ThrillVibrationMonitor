/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : AD1.h
**     Project   : VibrationSensor01
**     Processor : MC9S08JM60CLDE
**     Component : ADC
**     Version   : Component 01.587, Driver 01.29, CPU db: 3.00.046
**     Compiler  : CodeWarrior HCS08 C Compiler
**     Date/Time : 3/12/2011, 2:26 PM
**     Abstract  :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings  :
**         AD control register         : ADCSC1      [$0010]
**         AD control register         : ADCCFG      [$0016]
**         AD control register         : ADCRH       [$0012]
**         AD control register         : ADCRL       [$0013]
**         AD control register         : ADCCVH      [$0014]
**         AD control register         : ADCCVL      [$0015]
**         AD control register         : ADCSC2      [$0011]
**         AD control register         : APCTL1      [$0017]
**         AD control register         : APCTL2      [$0018]
**         Interrupt name              : Vadc
**         Priority                    : 
**         User handling procedure     : AD1_OnEnd
**         Number of conversions       : 1
**         AD resolution               : 12-bit
**
**         Input pins
**
**              Port name              : PTB
**              Bit number (in port)   : 0
**              Bit mask of the port   : $0001
**              Port data register     : PTBD        [$0002]
**              Port control register  : PTBDD       [$0003]
**
**              Port name              : PTB
**              Bit number (in port)   : 1
**              Bit mask of the port   : $0002
**              Port data register     : PTBD        [$0002]
**              Port control register  : PTBDD       [$0003]
**
**              Port name              : PTB
**              Bit number (in port)   : 2
**              Bit mask of the port   : $0004
**              Port data register     : PTBD        [$0002]
**              Port control register  : PTBDD       [$0003]
**
**         Initialization:
**              Conversion             : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-1
**     Contents  :
**         MeasureChan - byte AD1_MeasureChan(bool WaitForResult, byte Channel);
**         GetValue16  - byte AD1_GetValue16(word *Values);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __AD1
#define __AD1

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* MODULE AD1. */

#include "Cpu.h"




__interrupt void AD1_Interrupt(void);
/*
** ===================================================================
**     Method      :  AD1_Interrupt (component ADC)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void AD1_HWEnDi(void);
/*
** ===================================================================
**     Method      :  AD1_HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


byte AD1_MeasureChan(bool WaitForResult,byte Channel);
/*
** ===================================================================
**     Method      :  AD1_MeasureChan (component ADC)
**
**     Description :
**         This method performs measurement on one channel. (Note: If
**         the <number of conversions> is more than one the conversion
**         of the A/D channel is performed specified number of times.)
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for a result of
**                           conversion. If the <interrupt service> is
**                           disabled and at the same time a <number of
**                           conversions> is greater than 1, the
**                           WaitForResult parameter is ignored and the
**                           method waits for each result every time.
**         Channel         - Channel number. If only one
**                           channel in the component is set this
**                           parameter is ignored, because the parameter
**                           is set inside this method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
**                           ERR_RANGE - Parameter "Channel" out of range
** ===================================================================
*/

byte AD1_GetValue16(word *Values);
/*
** ===================================================================
**     Method      :  AD1_GetValue16 (component ADC)
**
**     Description :
**         This method returns the last measured values of all channels
**         justified to the left. Compared with <GetValue> method this
**         method returns more accurate result if the <number of
**         conversions> is greater than 1 and <AD resolution> is less
**         than 16 bits. In addition, the user code dependency on <AD
**         resolution> is eliminated.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Values          - Pointer to the array that contains
**                           the measured data.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
** ===================================================================
*/

void AD1_Init(void);
/*
** ===================================================================
**     Method      :  AD1_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/



/* END AD1. */

#endif /* ifndef __AD1 */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.09 [04.41]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/