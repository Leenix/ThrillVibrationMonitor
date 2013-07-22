/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Cpu.h
**     Project   : VibrationSensor01
**     Processor : MC9S08JM60CLDE
**     Component : MC9S08JM60_44
**     Version   : Component 01.003, Driver 01.38, CPU db: 3.00.046
**     Datasheet : MC9S08JM60 Rev. 1 11/2007
**     Compiler  : CodeWarrior HCS08 C Compiler
**     Date/Time : 2/23/2011, 7:07 PM
**     Abstract  :
**         This component "MC9S08JM60_44" contains initialization 
**         of the CPU and provides basic methods and events for 
**         CPU core settings.
**     Settings  :
**
**     Contents  :
**         EnableInt  - void Cpu_EnableInt(void);
**         DisableInt - void Cpu_DisableInt(void);
**
**     Copyright : 1997 - 2010 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __Cpu
#define __Cpu

/* Active configuration define symbol */
#define PEcfg_S08JM60CLDE 1


/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* MODULE Cpu. */

#ifndef __BWUserType_tPowerDownModes
#define __BWUserType_tPowerDownModes
  typedef enum {                       /*  */
    PowerDown,
    PartialPowerDown,
    StandBy
  } tPowerDownModes;
#endif

#define CPU_BUS_CLK_HZ              0x016E3600UL /* Initial value of the bus clock frequency in Hz */

#define CPU_INSTR_CLK_HZ            0x016E3600UL /* Initial value of the instruction clock frequency in Hz */

#define CPU_EXT_CLK_HZ              0x00B71B00UL /* Value of the main clock frequency (crystal or external clock) in Hz */
#define CPU_INT_CLK_HZ              0x7A12UL /* Value of the internal oscillator clock frequency in Hz */

#define CPU_TICK_NS                 0x53LU /* CPU tick is a unit derived from the frequency of external clock source. If no external clock is enabled or available it is derived from the value of internal clock source. The value of this constant represents period of the clock source in ns. */

#define CPU_CORE_HCS08                 /* Specification of the core type of the selected cpu */
#define CPU_DERIVATIVE_MC9S08JM60      /* Name of the selected cpu derivative */
#define CPU_PARTNUM_MC9S08JM60CLDE     /* Part number of the selected cpu */


/* Global variables */
extern volatile byte CCR_reg;          /* Current CCR register */


__interrupt void Cpu_Interrupt(void);
/*
** ===================================================================
**     Method      :  Cpu_Interrupt (component MC9S08JM60_44)
**
**     Description :
**         The method services unhandled interrupt vectors.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/


#define   Cpu_DisableInt()  __DI()     /* Disable interrupts */
/*
** ===================================================================
**     Method      :  Cpu_DisableInt (component MC9S08JM60_44)
**
**     Description :
**         Disables maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define   Cpu_EnableInt()  __EI()      /* Enable interrupts */
/*
** ===================================================================
**     Method      :  Cpu_EnableInt (component MC9S08JM60_44)
**
**     Description :
**         Enables maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void PE_low_level_init(void);
/*
** ===================================================================
**     Method      :  PE_low_level_init (component MC9S08JM60_44)
**
**     Description :
**         Initializes components and provides common register 
**         initialization. The method is called automatically as a part 
**         of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

/* END Cpu. */

#endif /* ifndef __Cpu */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.09 [04.41]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/