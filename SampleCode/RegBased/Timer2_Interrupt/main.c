/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"


#if defined __C51__
void Timer2_ISR(void)  interrupt 5
#elif defined __ICC8051__
#pragma vector=0x2B                             /* interrupt 5 */
__interrupt void Timer2_ISR(void)
#elif defined __SDCC__
void Timer2_ISR(void)  __interrupt (5)
#endif
{
    SFRS_TMP = SFRS;              /* for SFRS page */
    clr_T2CON_TF2;
    GPIO_LED ^= 1;
    if (SFRS_TMP)                 /* for SFRS page */
    {
      ENABLE_SFR_PAGE1;
    }
}
/************************************************************************************************************/
/* FUNCTION_PURPOSE: Main Loop                                                                              */
/************************************************************************************************************/
void main (void)
{
    GPIO_LED_QUASI_MODE;

    Timer2_AutoReload_Interrupt_Initial(16,640000);
    ENABLE_GLOBAL_INTERRUPT;

    while(1);
}

