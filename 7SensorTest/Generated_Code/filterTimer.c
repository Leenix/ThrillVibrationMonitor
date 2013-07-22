/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : filterTimer.c
**     Project     : ProcessorExpert
**     Processor   : MC9S08JM60CLHE
**     Component   : TimerOut
**     Version     : Component 03.030, Driver 01.24, CPU db: 3.00.050
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2013-07-11, 13:24, # CodeGen: 43
**     Abstract    :
**         This component implements a periodic signal generator 1:1 (Flip-Flop Output).
**         The signal alternates the "0" and "1" output values in exactly
**         regular intervals; that is why the time length of the "0" part
**         of one pulse is equal to the "1" part of one pulse.
**     Settings    :
**         Used output pin             : 
**             ----------------------------------------------------
**                Number (on package)  |    Name
**             ----------------------------------------------------
**                       8             |  PTF4_TPM2CH0
**             ----------------------------------------------------
**
**         Timer name                  : TPM2 (16-bit)
**         Compare name                : TPM20
**           Counter shared            : No
**
**         High speed mode
**             Prescaler               : divide-by-1
**             Clock                   : 24000000 Hz
**           Pulse width
**             Xtal ticks              : 400
**             microseconds            : 33
**             seconds (real)          : 0.000033333333
**             Hz                      : 30000
**             kHz                     : 30
**
**         Runtime setting             : none
**
**         Initialization:
**              Output level           : low
**              Timer                  : Enabled
**              Events                 : Enabled
**
**         Timer registers
**              Counter                : TPM2CNT   [$0061]
**              Mode                   : TPM2SC    [$0060]
**              Run                    : TPM2SC    [$0060]
**              Prescaler              : TPM2SC    [$0060]
**
**         Compare registers
**              Compare                : TPM2C0V   [$0066]
**
**         Flip-flop registers
**              Mode                   : TPM2C0SC  [$0065]
**     Contents    :
**         No public methods
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file filterTimer.c
** @version 01.24
** @brief
**         This component implements a periodic signal generator 1:1 (Flip-Flop Output).
**         The signal alternates the "0" and "1" output values in exactly
**         regular intervals; that is why the time length of the "0" part
**         of one pulse is equal to the "1" part of one pulse.
*/         
/*!
**  @addtogroup filterTimer_module filterTimer module documentation
**  @{
*/         

/* MODULE filterTimer. */

#include "PE_Error.h"
#include "filterTimer.h"


/* Internal method prototypes */

/*
** ===================================================================
**     Method      :  filterTimer_InitTO (component TimerOut)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void filterTimer_InitTO(void)
{
  /* TPM2SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  setReg8(TPM2SC, 0x00U);              /* Stop HW; disable overflow interrupt and set prescaler to 0 */ 
  /* TPM2C0SC: CH0F=0,CH0IE=0,MS0B=0,MS0A=1,ELS0B=0,ELS0A=1,??=0,??=0 */
  setReg8(TPM2C0SC, 0x14U);            /* Set output comp. mode (tgg. on output); disable. comp. int. */ 
  filterTimer_SetCV(0x031FU);          /* Store appropriate value to the compare register if selected High speed CPU mode */
  /* TPM2CNTH: BIT15=0,BIT14=0,BIT13=0,BIT12=0,BIT11=0,BIT10=0,BIT9=0,BIT8=0 */
  setReg8(TPM2CNTH, 0x00U);            /* Reset HW Counter */ 
  /* TPM2SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
  setReg8(TPM2SC, 0x08U);              /* Set prescaler and run counter */ 
}


/* END filterTimer. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.2 [05.06]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/