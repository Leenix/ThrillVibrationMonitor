/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : batteryMonitor.c
**     Project     : ProcessorExpert
**     Processor   : MC9S08JM60CLDE
**     Component   : ADC
**     Version     : Component 01.667, Driver 01.25, CPU db: 3.00.048
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2013-05-27, 12:03, # CodeGen: 7
**     Abstract    :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings    :
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
**         User handling procedure     : batteryMonitor_OnEnd
**         Number of conversions       : 1
**         AD resolution               : 12-bit
**
**         Input pins
**
**              Port name              : PTB
**              Bit number (in port)   : 4
**              Bit mask of the port   : $0010
**              Port data register     : PTBD        [$0002]
**              Port control register  : PTBDD       [$0003]
**
**         Initialization:
**              Conversion             : Enabled
**              Event                  : Enabled
**         High speed mode
**             Prescaler               : divide-by-1
**     Contents    :
**         Measure    - byte batteryMonitor_Measure(bool WaitForResult);
**         GetValue16 - byte batteryMonitor_GetValue16(word *Values);
**
**     Copyright : 1997 - 2012 Freescale, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


#include "Events.h"
#include "PE_Error.h"
#include "PE_Cnvrt.h"
#include "batteryMonitor.h"

#pragma MESSAGE DISABLE C5703 /* WARNING C5703: Parameter declared but not referenced */



/* Internal method prototypes */

static void SetOutV(void);
/*
** ===================================================================
**     Method      :  SetOutV (component ADC)
**
**     Description :
**         The method sets the corresponding flag, which indicates that a 
**         channel conversion has been completed.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void  HWEnDi(void);
/*
** ===================================================================
**     Method      :  HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void  batteryMonitor_Interrupt(void);
/*
** ===================================================================
**     Method      :  batteryMonitor_Interrupt (component ADC)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#define STOP       0x00U
#define MEASURE    0x01U
#define CONTINUOUS 0x02U
#define SINGLE     0x03U

static volatile bool OutFlg;
static volatile byte ModeFlg;
extern volatile word PE_Cnvrt_ADResults[];
volatile word batteryMonitor_SumV;     /* Sum of measured values */
volatile word batteryMonitor_OutV;     /* Sum of measured values */

/*
** ===================================================================
**     Method      :  SetOutV (component ADC)
**
**     Description :
**         The method sets the corresponding flag, which indicates that a 
**         channel conversion has been completed.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SetOutV(void)
{
  OutFlg = TRUE;
  batteryMonitor_OutV = batteryMonitor_SumV;
  batteryMonitor_SumV = 0U;
  if (ModeFlg == MEASURE) {
    ModeFlg = STOP;
    PE_Cnvrt_RunSto((PE_Cnvrt_ChanMaskType)0x08U);
  }
  batteryMonitor_OnEnd();
}

/*
** ===================================================================
**     Method      :  HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void  HWEnDi(void)
{
  OutFlg = FALSE;
  batteryMonitor_SumV = 0U;
  EnterCritical();
  if ((ModeFlg != STOP)) {                          /* Enable device? */
    PE_Cnvrt_RunSta((PE_Cnvrt_ChanMaskType)0x08U); /* Enable */
  } else {
    PE_Cnvrt_RunSto((PE_Cnvrt_ChanMaskType)0x08U); /* Disable */
  }
  ExitCritical();
}

/*
** ===================================================================
**     Method      :  batteryMonitor_Interrupt (component ADC)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void  batteryMonitor_Interrupt(void)
{
  if (ModeFlg != STOP) {
    batteryMonitor_SumV = PE_Cnvrt_ADResults[3];
    SetOutV();
  }
}

/*
** ===================================================================
**     Method      :  batteryMonitor_Measure (component ADC)
**
**     Description :
**         This method performs one measurement on all channels that
**         are set in the component inspector. (Note: If the <number of
**         conversions> is more than one the conversion of A/D
**         channels is performed specified number of times.)
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for result of
**                           the conversion
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already
**                           running
** ===================================================================
*/
byte  batteryMonitor_Measure(bool WaitForResult)
{
  if (ModeFlg != STOP) {
    return ERR_BUSY;
  }
  EnterCritical();
  ModeFlg = MEASURE;
  OutFlg = FALSE;
  batteryMonitor_SumV = 0U;
  PE_Cnvrt_RunSta((PE_Cnvrt_ChanMaskType)0x08U);
  ExitCritical();
  if (WaitForResult) {
    while (ModeFlg == MEASURE) {}
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  batteryMonitor_GetValue16 (component ADC)
**
**     Description :
**         This method returns the last measured values of all
**         channels justified to the left. Compared with <GetValue>
**         method this method returns more accurate result if the
**         <number of conversions> is greater than 1 and <AD
**         resolution> is less than 16 bits. In addition, the user
**         code dependency on <AD resolution> is eliminated.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Values          - Pointer to the array that
**                           contains the measured data.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_OVERRUN - External trigger overrun
**                           flag was detected after last value(s)
**                           was obtained (for example by GetValue).
**                           This error may not be supported on some
**                           CPUs (see generated code).
** ===================================================================
*/
byte batteryMonitor_GetValue16(word *Values)
{
  if (!OutFlg) {
    return ERR_NOTAVAIL;
  }
  *Values = (word)((batteryMonitor_OutV) << 4); /* Save measured values to the output buffer */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  batteryMonitor_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void batteryMonitor_Init(void)
{
  OutFlg = FALSE;
  ModeFlg = STOP;
  PE_Cnvrt_SetPro((byte)1, batteryMonitor_Interrupt);
  PE_Cnvrt_SetSpe((byte)0x03U, (byte)0x04U);
  HWEnDi();                            /* Enable/disable device according to the status flags */
}


/* END batteryMonitor. */


/*
** ###################################################################
**
**     This file was created by Processor Expert 10.0 [05.03]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
