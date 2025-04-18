/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"

/* PLEASE first setting CONFIG P2.0 as notmal GPIO not reset pin*/ 
void main(void)
{
  P11_PUSHPULL_MODE;
  P12_PUSHPULL_MODE;
  set_CKCON_CLOEN;
/* clock output divide 100 */
  CKDIV = 50;     /* clock output 1.6MHz */
  while (P20)
  {
    MODIFY_HIRC(HIRC_16);
    P12 = 1;
    Timer0_Delay(16000000,5,1000);
    P12 = 0;
    Timer0_Delay(16000000,5,1000);
  }
/* HIRC modify to about 16.6MHz  */
  MODIFY_HIRC(HIRC_166);        /* clock output 1.66MHz */
  while(1);
  
}
