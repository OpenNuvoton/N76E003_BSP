/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"

#if defined __C51__
uint8_t xdata sppage_buffer[127];
uint8_t xd_tmp[127];

#elif defined __ICC8051__
uint8_t __xdata sppage_buffer[127];
uint8_t __xdata xd_tmp[127];

#elif defined __SDCC__
uint8_t __xdata sppage_buffer[127];
uint8_t __xdata xd_tmp[127];
#endif


/**
 * @brief       Read Dataflash
 * @param       Dataflash start address
 * @return      Dataflash Value
 * @details     None
 */
#if defined __C51__
uint8_t Read_SPROM_BYTE(const uint8_t code *u8_addr)
#elif defined __ICC8051__
uint8_t Read_SPROM_BYTE(const uint8_t __code *u8_addr)
#elif defined __SDCC__
uint8_t Read_SPROM_BYTE(const uint8_t __code *u8_addr)
#endif
{
    uint8_t rdata;

    set_IAPUEN_SPMEN;
    rdata = *(0xFF80+u8_addr);
    return rdata;
}

//-------------------------------------------------------------------------
void WriteDataToSPOnePage(uint8_t u8_addr, uint8_t *pDat, uint16_t num)
{
#if defined __C51__
    uint8_t code *pCode;
#elif defined __ICC8051__
    uint8_t __code *pCode;
#elif defined __SDCC__
    uint8_t __code *pCode;
#endif
    uint8_t i;

#if defined __C51__
    pCode = (uint8_t code *)u8_addr;
#elif defined __ICC8051__
    pCode = (uint8_t __code *)u8_addr;
#elif defined __SDCC__
    pCode = (uint8_t __code *)u8_addr;
#endif

    for(i=0;i<127;i++)
    {
#if defined __C51__
        xd_tmp[i] = Read_SPROM_BYTE((uint8_t code *)i);
#elif defined __ICC8051__
        xd_tmp[i] = Read_SPROM_BYTE((uint8_t __code *)i);
#elif defined __SDCC__
        xd_tmp[i] = Read_SPROM_BYTE((uint8_t __code *)i);
#endif

    }
    for(i=0;i<num;i++)
    {
      xd_tmp[u8_addr+i] = pDat[i];
    }

    set_CHPCON_IAPEN;
    set_IAPUEN_SPMEN;
    set_IAPUEN_SPUEN;

      IAPCN = PAGE_ERASE_SPROM;
      IAPAL = 0x80;
      IAPAH = 0x01;

      IAPFD = 0xFF;
      set_IAPTRG_IAPGO; 
      IAPCN =BYTE_PROGRAM_SPROM;
      for(i=0;i<127;i++)
      {
        IAPFD = xd_tmp[i];
        set_IAPTRG_IAPGO;
        IAPAL++;
      }
      for(i=0;i<127;i++)
      {
        if(pCode[i]!=xd_tmp[i])break;
      }

    clr_IAPUEN_SPUEN;
    clr_CHPCON_IAPEN;
}

void Write_SPROM_DATAFLASH_ARRAY(uint8_t u8_addr, uint8_t *pDat, uint16_t num)
{
      WriteDataToSPOnePage(u8_addr,pDat,num);
}

//-------------------------------------------------------------------------
void Read_SPROM_DATAFLASH_ARRAY(uint16_t u8_addr, uint8_t *pDat, uint16_t num)
{
    uint16_t i;

    set_IAPUEN_SPMEN;
    for (i = 0; i < num; i++)
    {
#if defined __C51__
        pDat[i] = *(uint8_t code *)(u8_addr+0xFF80 + i);
#elif defined __ICC8051__
        pDat[i] = *(uint8_t __code *)(u8_addr+0xFF80 + i);
#elif defined __SDCC__
        pDat[i] = *(uint8_t __code *)(u8_addr+0xFF80 + i);
#endif
    }

}

