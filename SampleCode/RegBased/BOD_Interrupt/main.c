/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"

BIT bodflag;

#if defined __C51__
void BOD_ISR (void) interrupt 8

#elif defined __ICC8051__
#pragma vector=0x43
__interrupt void BOD_ISR(void)

#elif defined __SDCC__
void BOD_ISR (void) __interrupt (8)           // Vector @  0x43
#endif
{
    PUSH_SFRS;

    clr_BODCON0_BOF;
    bodflag=1;

    POP_SFRS;
}


//----------------------------------------------------------------------------------------------//
void main (void)
{
/* As defaut all multi function define as GPIO */ 
    MODIFY_HIRC(HIRC_24);
    Enable_UART0_VCOM_printf_24M_115200();
    printf ("\n Test start ...");

    BOD_Open(Enable, VBOD_2_7, BOD_Reset_Disable);
    BOD_LowPower(LPBOD_Mode_3);
    BOD_Interrupt(Enable);
    ENABLE_GLOBAL_INTERRUPT;
    while(1)
    {
       if (bodflag)
       {
    	   bodflag = 0;
           printf ("\n BOD interrupt! ");

       }
  }
}


