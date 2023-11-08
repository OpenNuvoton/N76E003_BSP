/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"


struct
{
  unsigned int a;
  unsigned long b;
  unsigned char  c;
  
}StructData;

unsigned char ArrayData[50];
unsigned char i;


/******************************
  * @brief     Main loop.
  * @param[in] None.
  * @return    None.
******************************/
void main (void) 
{
    uint16_t system16highsite;
  
    MODIFY_HIRC(HIRC_24);
    Enable_UART0_VCOM_printf_24M_115200();
    printf ("\n\r Toggle P05 to low to start...");

    GPIO_LED_QUASI_MODE;
    while(GPIO_LED);


    Write_DATAFLASH_BYTE (0x3882,0x34);

    for(i=0;i<50;i++)
    {
      ArrayData[i]=i;
    }

     StructData.a=0xA1A2;
     StructData.b=0xA3A4A5A6;
     StructData.c=0xA7;
  
    Write_DATAFLASH_ARRAY(0x38E0,ArrayData,50);//write 50 bytes
    Write_DATAFLASH_ARRAY(0x38FE,(unsigned char *)&StructData,sizeof(StructData));//write structure
  
  
#if defined __C51__
    system16highsite = (Read_APROM_BYTE(0x38FD)<<8);
    system16highsite |= Read_APROM_BYTE(0x38FE);

#elif defined __ICC8051__
    system16highsite = Read_APROM_BYTE((unsigned int __code *)0x38FD) << 8 ;
    system16highsite |= Read_APROM_BYTE ((unsigned int __code *)0x38FE) ;

#elif defined __SDCC__
    system16highsite = ((Read_APROM_BYTE((uint16_t __code*)0x38FD)<<8)+Read_APROM_BYTE((uint16_t __code*)0x38FE));
#endif

    printf ("\n\r system16highsite = 0x%X", system16highsite);

	while(1);
}



