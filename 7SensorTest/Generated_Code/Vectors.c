/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : Vectors.c
**     Project     : ProcessorExpert
**     Processor   : MC9S08JM60CLHE
**     Version     : Component 01.004, Driver 01.40, CPU db: 3.00.050
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2013-07-24, 14:08, # CodeGen: 79
**     Abstract    :
**         This component "MC9S08JM60_64" contains initialization 
**         of the CPU and provides basic methods and events for 
**         CPU core settings.
**     Settings    :
**
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file Vectors.c                                                  
** @version 01.40
** @brief
**         This component "MC9S08JM60_64" contains initialization 
**         of the CPU and provides basic methods and events for 
**         CPU core settings.
*/         
/*!
**  @addtogroup Vectors_module Vectors module documentation
**  @{
*/         


#include "Cpu.h"
#include "AD1.h"
#include "sampleTimer.h"
#include "filterTimer.h"
#include "accelerometerSleep.h"

/*lint -save  -e950 Disable MISRA rule (1.1) checking. */
static void (* near const _vect[])(void) @0xFFC4 = { /* Interrupt vector table */
/*lint -restore Enable MISRA rule (1.1) checking. */
         sampleTimer_Interrupt,        /* Int.no. 29 Vrtc (at FFC4)                  Used */
         Cpu_Interrupt,                /* Int.no. 28 Viic (at FFC6)                  Unassigned */
         Cpu_Interrupt,                /* Int.no. 27 Vacmp (at FFC8)                 Unassigned */
         Cpu_Interrupt,                /* Int.no. 26 Vadc (at FFCA)                  Unassigned */
         Cpu_Interrupt,                /* Int.no. 25 Vkeyboard (at FFCC)             Unassigned */
         Cpu_Interrupt,                /* Int.no. 24 Vsci2tx (at FFCE)               Unassigned */
         Cpu_Interrupt,                /* Int.no. 23 Vsci2rx (at FFD0)               Unassigned */
         Cpu_Interrupt,                /* Int.no. 22 Vsci2err (at FFD2)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 21 Vsci1tx (at FFD4)               Unassigned */
         Cpu_Interrupt,                /* Int.no. 20 Vsci1rx (at FFD6)               Unassigned */
         Cpu_Interrupt,                /* Int.no. 19 Vsci1err (at FFD8)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 18 Vtpm2ovf (at FFDA)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 17 Vtpm2ch1 (at FFDC)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 16 Vtpm2ch0 (at FFDE)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 15 Vtpm1ovf (at FFE0)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 14 Vtpm1ch5 (at FFE2)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 13 Vtpm1ch4 (at FFE4)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 12 Vtpm1ch3 (at FFE6)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 11 Vtpm1ch2 (at FFE8)              Unassigned */
         Cpu_Interrupt,                /* Int.no. 10 Vtpm1ch1 (at FFEA)              Unassigned */
         Cpu_Interrupt,                /* Int.no.  9 Vtpm1ch0 (at FFEC)              Unassigned */
         Cpu_Interrupt,                /* Int.no.  8 VReserved (at FFEE)             Unassigned */
         Cpu_Interrupt,                /* Int.no.  7 Vusb (at FFF0)                  Unassigned */
         Cpu_Interrupt,                /* Int.no.  6 Vspi2 (at FFF2)                 Unassigned */
         Cpu_Interrupt,                /* Int.no.  5 Vspi1 (at FFF4)                 Unassigned */
         Cpu_Interrupt,                /* Int.no.  4 Vlol (at FFF6)                  Unassigned */
         Cpu_Interrupt,                /* Int.no.  3 Vlvd (at FFF8)                  Unassigned */
         Cpu_Interrupt,                /* Int.no.  2 Virq (at FFFA)                  Unassigned */
         Cpu_Interrupt,                /* Int.no.  1 Vswi (at FFFC)                  Unassigned */
         _EntryPoint                   /* Int.no.  0 Vreset (at FFFE)                Reset vector */
};
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.2 [05.07]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/

