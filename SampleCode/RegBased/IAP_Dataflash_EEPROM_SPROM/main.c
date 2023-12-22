/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"

struct
{
    uint16_t a;
    uint32_t b;
    uint8_t  c;

} StructData;

uint8_t ArrayData[50];
uint8_t i;

/**
 * @brief       IAP program dataflash as EEPROM
 * @param       None
 * @return      None
 * @details     define dataflash need be modify address and value,
 */

void main(void)
{
    uint16_t system16highsite;
	
/* UART0 initial setting
  * include sys.c in Library for modify HIRC value to 24MHz
  * include uart.c in Library for UART initial setting
*/
    MODIFY_HIRC(HIRC_166);
    Enable_UART0_VCOM_printf_166M_115200();
    printf ("\n\r Toggle P05 Test start ...");

    /*loop here while P46 = 1; */
    GPIO_LED_QUASI_MODE;
    while (GPIO_LED);

    /** IAP program APROM as EEPROM way * include eeprom.c in Library       */
    for (i = 0; i < 50; i++)
    {
        ArrayData[i] = i;
    }

    StructData.a=0xA1A2;
    StructData.b=0xA3A4A5A6;
    StructData.c=0xA7;

    Write_SPROM_DATAFLASH_ARRAY(1, ArrayData, 50); //write 50 bytes
    Write_SPROM_DATAFLASH_ARRAY(0x10, (uint8_t *)&StructData, sizeof(StructData)); //write structure

    /*call read byte */
#if defined __C51__
    system16highsite = (Read_SPROM_BYTE((uint8_t code *)0x10) << 8)+ Read_SPROM_BYTE((uint8_t code *)0x20);
#elif defined __ICC8051__
    system16highsite = (Read_SPROM_BYTE((uint8_t __code *)0x10) << 8)+ Read_SPROM_BYTE((uint8_t __code *)0x20);
#elif defined __SDCC__
    system16highsite = (Read_SPROM_BYTE((uint8_t __code *)0x10) << 8)+ Read_SPROM_BYTE((uint8_t __code *)0x20);
#endif

    printf("\n\r SPROM 0x10+0x20 Value = 0x%X", system16highsite);

    while (1);
}



