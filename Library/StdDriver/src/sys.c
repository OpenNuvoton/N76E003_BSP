/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"

  /**
  * @brief This API configures modify system HIRC value
  * @param[in] u8FsysMode . Valid values are:
  *                       - \ref HIRC_16                 :Internal HIRC 16MHz.
  *                       - \ref HIRC_166                :Internal HIRC 16.6MHz.
  * @note      None.
  * @exmaple : MODIFY_HIRC(HIRC_166);
  */
 void MODIFY_HIRC(uint8_t u8HIRCSEL)
{
#if defined __C51__
    uint8_t data hircmap0,hircmap1, DIDhighbyte, DIDlowbyte;
#elif defined __ICC8051__
    uint8_t hircmap0, hircmap1, offset,judge;
#elif defined __SDCC__
    uint8_t __data hircmap0,hircmap1, offset,judge;
#endif
    uint8_t trimvalue16bit;

    BYTE_TMP = IE;
    EA = 0 ;

    set_CHPCON_IAPEN;
     /*check ID for adjust value */
    IAPAL = 0;
    IAPAH = 0;
    IAPCN = READ_DID;
    set_IAPTRG_IAPGO;
    DIDlowbyte = IAPFD;
    IAPAL++;
    set_IAPTRG_IAPGO;
    DIDhighbyte = IAPFD;
    /* Reload 16M trim value */
    IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPTRG_IAPGO;
    hircmap0 = IAPFD;
    IAPAL++;
    set_IAPTRG_IAPGO;
    hircmap1 = IAPFD;

    switch (u8HIRCSEL)
    { 
      case HIRC_166:
        trimvalue16bit = ((hircmap0 << 1) + (hircmap1 & 0x01));
        if ((DIDhighbyte==0x67)&(DIDlowbyte==0x50))
            trimvalue16bit -= 16;                            /*N76S003 process */
        else if((DIDhighbyte==0x36)&(DIDlowbyte==0x50))
            trimvalue16bit -= 14;                            /*N76E003 process */
        hircmap1 = trimvalue16bit&0x01;
        hircmap0 = trimvalue16bit>>1;
        set_IAPTRG_IAPGO;
        break;
      default: break;                                       /*for 16M process */
    }

    TA = 0xAA;
    TA = 0x55;
    RCTRIM0 = hircmap0;
    TA = 0xAA;
    TA = 0x55;
    RCTRIM1 = hircmap1;
    clr_CHPCON_IAPEN;
    IE = BYTE_TMP;
}


  /**
  * @brief This API configures system clock source
  * @param[in] u8FsysMode . Valid values are:
  *                       - \ref FSYS_HIRC                :Internal HIRC (16MHz as default).
  *                       - \ref FSYS_LIRC                :Internal LIRC.
  *                       - \ref FSYS_OSCIN_P00           :External clock input from P0.0.
  * @note      None.
  * @exmaple : FsysSelect(FSYS_LIRC);
  */
void FsysSelect(uint8_t u8FsysMode)
{
  switch (u8FsysMode)
  {
        //***** HIRC enable part *****  
    case FSYS_HIRC:
        ClockEnable(FSYS_HIRC);                 //Enable HIRC
        ClockSwitch(FSYS_HIRC);                 //Switching system clock HIRC
    break;
    
    //***** LIRC enable part*****
    case FSYS_LIRC:
        ClockSwitch(FSYS_LIRC);                 // LIRC always on switch system to LIRC
        clr_CKEN_HIRCEN;                        // Disable HIRC if needed 
    break;
    
    /***** ECLK enable part clock in with P3.0 *****/ 
    case FSYS_OSCIN_P30:
        ClockEnable(FSYS_HIRC);                 //Enable and system clock to HIRC
        ClockSwitch(FSYS_HIRC);
        ClockEnable(FSYS_OSCIN_P30);            //Enable External clock source
        ClockSwitch(FSYS_OSCIN_P30);            //Switching system clock to OSCIN 
        clr_CKEN_HIRCEN;                        //step5: disable HIRC if needed 
    break;
  }
}

void ClockEnable(uint8_t u8FsysMode)
{
  switch (u8FsysMode)
  {
    /***** HIRC enable part ******/
    case FSYS_HIRC:
        set_CKEN_HIRCEN;                        //step1: Enable extnal clock source.
        while(!(CKSWT&SET_BIT5));               //step2: check clock source status and wait for ready
    break;
    /***** ECLK P30 enable part ******/
    case FSYS_OSCIN_P30:
        TA=0xAA;TA=0x55;CKEN|=0xC0;             //step1: Enable extnal clock source.
        while(!(CKSWT&SET_BIT3));               //step2: check clock source status and wait for ready
    break;
  }
}

void ClockDisable(uint8_t u8FsysMode)
{
  SFRS = 0;
  switch (u8FsysMode)
  {
     /***** HIRC Disable part ******/
    case FSYS_HIRC:
        clr_CKEN_HIRCEN;
    break;
    //***** ECLK from P3.0 Disable part ******/
    case FSYS_OSCIN_P30:
       TA=0xAA;TA=0x55;CKEN&=0x3F;
    break;
  }
}

void ClockSwitch(uint8_t u8FsysMode)
{
  SFRS = 0 ;
  BIT_TMP=EA;EA=0;
  switch (u8FsysMode)
  {
    /***** HIRC Disable part ******/
    case FSYS_HIRC:
        clr_CKSWT_OSC1;
        clr_CKSWT_OSC0;
    break;
    /***** LIRC Disable part******/
    case FSYS_LIRC:
        set_CKSWT_OSC1;
        clr_CKSWT_OSC0;
    break;
    /***** ECLK P30 Disable part ******/
    case FSYS_OSCIN_P30:
        set_CKSWT_ECLKST;
    break;
  }
  EA = BIT_TMP;
}

