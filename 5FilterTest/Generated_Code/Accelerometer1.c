/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : Accelerometer1.c
**     Project     : ProcessorExpert
**     Processor   : MC9S08JM60CLHE
**     Component   : ADC
**     Version     : Component 01.685, Driver 01.30, CPU db: 3.00.050
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2013-05-28, 17:49, # CodeGen: 25
**     Abstract    :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings    :
**          Component name                                 : Accelerometer1
**          A/D converter                                  : ADC
**          Sharing                                        : Enabled
**          Interrupt service/event                        : Enabled
**            A/D interrupt                                : Vadc
**            A/D interrupt priority                       : medium priority
**          A/D channels                                   : 1
**            Channel0                                     : 
**              A/D channel (pin)                          : PTD1_ADP9_ACMPMINUS
**              A/D channel (pin) signal                   : 
**          A/D resolution                                 : 12 bits
**          Conversion time                                : 5.75 �s
**          Low-power mode                                 : Disabled
**          Sample time                                    : short
**          Internal trigger                               : Disabled
**          Number of conversions                          : 1
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Events enabled in init.                      : yes
**          CPU clock/speed selection                      : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**          High input limit                               : 1
**          Low input limit                                : 0
**          Get value directly                             : yes
**          Wait for result                                : yes
**     Contents    :
**         Enable       - byte Accelerometer1_Enable(void);
**         Disable      - byte Accelerometer1_Disable(void);
**         EnableEvent  - byte Accelerometer1_EnableEvent(void);
**         DisableEvent - byte Accelerometer1_DisableEvent(void);
**         Measure      - byte Accelerometer1_Measure(bool WaitForResult);
**         GetValue16   - byte Accelerometer1_GetValue16(word *Values);
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file Accelerometer1.c
** @version 01.30
** @brief
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
*/         
/*!
**  @addtogroup Accelerometer1_module Accelerometer1 module documentation
**  @{
*/         


/* MODULE Accelerometer1. */

#include "Events.h"
#include "Accelerometer1.h"

#pragma MESSAGE DISABLE C5703          /* Disable warning C5703 "Parameter is not referenced" */



#define STOP            0x00U          /* STOP state           */
#define MEASURE         0x01U          /* MESURE state         */
#define CONTINUOUS      0x02U          /* CONTINUOS state      */
#define SINGLE          0x03U          /* SINGLE state         */


static const  byte Channels = 0x49U;   /* Content for the device control register */

static bool EnUser;                    /* Enable/Disable device */
volatile bool Accelerometer1_EnEvent;  /* Enable/Disable events */
static volatile bool OutFlg;           /* Measurement finish flag */
static volatile byte ModeFlg;          /* Current state of device */

volatile word Accelerometer1_OutV;     /* Sum of measured values */





/*
** ===================================================================
**     Method      :  Accelerometer1_Interrupt (component ADC)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
ISR(Accelerometer1_Interrupt)
{
  /*lint -save  -e926 -e927 -e928 -e929 Disable MISRA rule (11.4) checking. */
  ((TWREG volatile*)(&Accelerometer1_OutV))->b.high = ADCRH; /* Save measured value */
  ((TWREG volatile*)(&Accelerometer1_OutV))->b.low = ADCRL; /* Save measured value */
  /*lint -restore Enable MISRA rule (11.4) checking. */
  OutFlg = TRUE;                       /* Measured values are available */
  if (Accelerometer1_EnEvent) {        /* Are events enables? */
    Accelerometer1_OnEnd();            /* If yes then invoke user event */
  }
  ModeFlg = STOP;                      /* Set the device to the stop mode */
}

/*
** ===================================================================
**     Method      :  Accelerometer1_HWEnDi (component ADC)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Accelerometer1_HWEnDi(void)
{
  if (EnUser) {                        /* Enable device? */
    if (ModeFlg) {                     /* Start or stop measurement? */
      OutFlg = FALSE;                  /* Output value isn't available */
      ADCSC1 = Channels;               /* If yes then start the conversion */
    }
  }
  else {
    ADCSC1 = 0x1FU;                    /* Disable the device */
  }
}

/*
** ===================================================================
**     Method      :  Accelerometer1_Enable (component ADC)
**     Description :
**         Enables A/D converter component. <Events> may be generated
**         (<DisableEvent>/<EnableEvent>). If possible, this method
**         switches on A/D converter device, voltage reference, etc.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte Accelerometer1_Enable(void)
{
  if (EnUser) {                        /* Is the device enabled by user? */
    return ERR_OK;                     /* If yes then set the flag "device enabled" */
  }
  EnUser = TRUE;                       /* Set the flag "device enabled" */
  Accelerometer1_HWEnDi();             /* Enable the device */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  Accelerometer1_Disable (component ADC)
**     Description :
**         Disables A/D converter component. No <events> will be
**         generated. If possible, this method switches off A/D
**         converter device, voltage reference, etc.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte Accelerometer1_Disable(void)
{
  if (!EnUser) {                       /* Is the device disabled by user? */
    return ERR_OK;                     /* If yes then OK */
  }
  EnUser = FALSE;                      /* If yes then set the flag "device disabled" */
  Accelerometer1_HWEnDi();             /* Enable the device */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  Accelerometer1_EnableEvent (component ADC)
**     Description :
**         Enables the <events>.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
/*
byte Accelerometer1_EnableEvent(void)

**      This method is implemented as a macro. See header module. **
*/

/*
** ===================================================================
**     Method      :  Accelerometer1_DisableEvent (component ADC)
**     Description :
**         Disables the <events>.
**     Parameters  : None
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
/*
byte Accelerometer1_DisableEvent(void)

**      This method is implemented as a macro. See header module. **
*/

/*
** ===================================================================
**     Method      :  Accelerometer1_Measure (component ADC)
**     Description :
**         This method performs one measurement on all channels that
**         are set in the component inspector. (Note: If the <number of
**         conversions> is more than one the conversion of A/D channels
**         is performed specified number of times.)
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for a result of a
**                           conversion. If <interrupt service> is
**                           disabled, A/D peripheral doesn't support
**                           measuring all channels at once or Autoscan
**                           mode property isn't enabled and at the same
**                           time the <number of channels> is greater
**                           than 1, then the WaitForResult parameter is
**                           ignored and the method waits for each
**                           result every time. If the <interrupt
**                           service> is disabled and a <number of
**                           conversions> is greater than 1, the
**                           parameter is ignored and the method also
**                           waits for each result every time.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
** ===================================================================
*/
#pragma MESSAGE DISABLE C5703 /* WARNING C5703: Parameter declared but not referenced */
byte Accelerometer1_Measure(bool WaitForResult)
{
  if (!EnUser) {                       /* Is the device disabled by user? */
    return ERR_DISABLED;               /* If yes then error */
  }
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = MEASURE;                   /* Set state of device to the measure mode */
  Accelerometer1_HWEnDi();             /* Enable the device */
  if (WaitForResult) {                 /* Is WaitForResult TRUE? */
    while (ModeFlg == MEASURE) {}      /* If yes then wait for end of measurement */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  Accelerometer1_GetValue16 (component ADC)
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
byte Accelerometer1_GetValue16(word *Values)
{
  if (OutFlg == 0U) {                  /* Is output flag set? */
    return ERR_NOTAVAIL;               /* If no then error */
  }
  *Values = (word)((Accelerometer1_OutV) << 4); /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  Accelerometer1_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Accelerometer1_Init(void)
{
  /* ADCSC1: COCO=0,AIEN=0,ADCO=0,ADCH4=1,ADCH3=1,ADCH2=1,ADCH1=1,ADCH0=1 */
  setReg8(ADCSC1, 0x1FU);              /* Disable the module */ 
  /* ADCSC2: ADACT=0,ADTRG=0,ACFE=0,ACFGT=0,??=0,??=0,??=0,??=0 */
  setReg8(ADCSC2, 0x00U);              /* Disable HW trigger and autocompare */ 
  EnUser = TRUE;                       /* Enable device */
  Accelerometer1_EnEvent = TRUE;       /* Enable events */
  OutFlg = FALSE;                      /* No measured value */
  ModeFlg = STOP;                      /* Device isn't running */
  /* ADCCFG: ADLPC=0,ADIV1=0,ADIV0=1,ADLSMP=0,MODE1=0,MODE0=1,ADICLK1=0,ADICLK0=0 */
  setReg8(ADCCFG, 0x24U);              /* Set prescaler bits */ 
  Accelerometer1_HWEnDi();             /* Enable/disable device according to the status flags */
}


/* END Accelerometer1. */

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
