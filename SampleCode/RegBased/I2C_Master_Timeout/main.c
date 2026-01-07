/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"


#define I2C_SLAVE_ADDRESS         0xA4
#define I2C_WR                    0
#define I2C_RD                    1

#define LOOP_SIZE                 10

BIT   i2cErrorFlag;

#if defined __C51__
void Timer0_ISR (void) interrupt 1            // Vector @  0x0B

#elif defined __ICC8051__
#pragma vector=0x33
__interrupt void Pin_INT_ISR(void)

#elif defined __SDCC__
void Timer0_ISR (void) __interrupt (1)        // Vector @  0x0B
#endif
{
    SFRS_TMP = SFRS;              /* for SFRS page */
      SFRS = 0;
      i2cErrorFlag = 1;
      clr_TCON_TF0;
    if (SFRS_TMP)                 /* for SFRS page */
    {
      ENABLE_SFR_PAGE1;
    }
}
//========================================================================================================
void Init_I2C(void)
{
    P13_QUASI_MODE;          // Modify SCL pin to Open drain mode. don't forget the pull high resister in circuit
    P14_QUASI_MODE;          // Modify SDA pin to Open drain mode. don't forget the pull high resister in circuit

    P13_ST_ENABLE;               //Enable Schemitt trig mode
    P14_ST_ENABLE;

    /* Set I2C clock rate */
    I2C_Master_Open(16000000,300000);
}
//========================================================================================================

//--------------------------------------------------------------------------------------------
//----  Page Write----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Write_Process(UINT8 u8DAT)
{
    uint8_t  u8Count;

    /* Write Step1 */
    set_I2CON_STA;                              /* Send Start bit to I2C EEPROM */
    clr_I2CON_SI;
    while (!SI)                                 /*Check SI set or not  */
    {
      if (i2cErrorFlag)
        goto I2CWRSTOP;
    }
    if (I2STAT != 0x08)                         /*Check status value after every step   */
    {
        i2cErrorFlag=1;
        goto I2CWRSTOP;
    }
    Timer0_ReloadCounter();                     /*Clear time out counter  */

    /* Write Step2 */
    clr_I2CON_STA;                              /*STA=0*/
    I2DAT = (I2C_SLAVE_ADDRESS | I2C_WR);       /* Write command */
    clr_I2CON_SI;
    while (!SI);
    {
      if (i2cErrorFlag)
        goto I2CWRSTOP;
    }
    if (I2STAT != 0x18)
    {
        i2cErrorFlag=1;
        goto I2CWRSTOP;
    }
    Timer0_ReloadCounter();

    /* Write Step3 */
    for (u8Count = 0; u8Count < LOOP_SIZE; u8Count++)
    {
        I2DAT = u8DAT+u8Count;
        clr_I2CON_SI;
        while (!SI);                            /*Check SI set or not*/
        {
           if (i2cErrorFlag)
              goto I2CWRSTOP;
        }
        if (I2STAT != 0x28)
        {
            i2cErrorFlag=1;
            goto I2CWRSTOP;
        }
        Timer0_ReloadCounter();
    }


    I2CWRSTOP:
    if  (i2cErrorFlag)
    {
      I2C0_SI_Check();                          /* With error call SI check to help clear SI signal */
      printf ("\n\r I2C TimeOut!");
      printf ("\n I2C write error !");
//      printf ("\n\r I2C write STATUS, 0x%bX ",I2STAT);
//      printf ("\n\r I2C write I2DAT, 0x%bX ",I2DAT);
//      printf ("\n\r I2C write u8Count, 0x%bX ",u8Count);
      i2cErrorFlag = 0;
    }
    else
    {
          printf ("\n\r I2C write process...pass");
          i2cErrorFlag = 0;
    }
    
    u8Count = 0;
/* Write Step4 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */
}

//--------------------------------------------------------------------------------------------
//----  Page Read ----------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void I2C_Read_Process(UINT8 u8DAT)
{
    uint8_t  u8Count;

    Timer0_ReloadCounter();

    /* Read Step1 */
    set_I2CON_STA;
    clr_I2CON_SI;
    while (!SI);
    {
      if (i2cErrorFlag)
        goto I2CRDSTOP;
    }
    if (I2STAT != 0x08)                         //Check status value after every step
    {
        i2cErrorFlag=1;
        goto I2CRDSTOP;
    }
    Timer0_ReloadCounter();

    /* Step13 */
    clr_I2CON_STA;                              //STA needs to be cleared after START codition is generated
    I2DAT = (I2C_SLAVE_ADDRESS | I2C_RD);
    clr_I2CON_SI;
    while (!SI);                                //Check SI set or not
    {
        if (i2cErrorFlag)
          goto I2CRDSTOP;
    }
    if (I2STAT != 0x40)
    {
        i2cErrorFlag=1;
          goto I2CRDSTOP;
    }
    Timer0_ReloadCounter();

    /* Step14 */
    for (u8Count = 0; u8Count <LOOP_SIZE; u8Count++)
    {
        set_I2CON_AA;
        clr_I2CON_SI;
        while (!SI);                            //Check SI set or not
        {
           if (i2cErrorFlag)
             goto I2CRDSTOP;
        }
        if (I2STAT != 0x50)
        {
            i2cErrorFlag=1;
            goto I2CRDSTOP;
        }
        if (I2DAT != (u8DAT+u8Count))
        {
            i2cErrorFlag=1;
            goto I2CRDSTOP;
        }
        Timer0_ReloadCounter();
    } 

    /* Step15 */
    clr_I2CON_AA;
    clr_I2CON_SI;
    while (!SI);                                //Check SI set or not
    if (I2STAT != 0x58)
    {
      i2cErrorFlag=1;
      goto I2CRDSTOP;
    }
    Timer0_ReloadCounter();

    I2CRDSTOP:
    if  (i2cErrorFlag)
    {
      I2C0_SI_Check();
      printf ("\n\r I2C TimeOut!");
      printf ("\n I2C read error !");
//      printf ("\n\r I2C act STATUS, 0x%bX ",I2STAT);
//      printf ("\n\r I2C I2DAT, 0x%bX ",I2DAT);
//      printf ("\n\r I2C u8Count, 0x%bX ",u8Count);
      i2cErrorFlag = 0;
    }
    else
    {
       printf ("\n\r I2C read test...pass");
    }

    /* Step16 */
    set_I2CON_STO;
    clr_I2CON_SI;
    while (STO);                                /* Check STOP signal */
}
//========================================================================================================
void main(void)
{
    /* UART0 settting for printf function */

    Enable_UART0_VCOM_printf_166M_115200();
    printf ("\n Test start ...");

    Init_I2C();                                  /* initial I2C circuit  */
    printf ("\n\r I2C Write process start.");
    Timer0_AutoReload_Interrupt_Initial(16,450); /* I2C time out counter about 50ms . Always clear counter in transmit */
    ENABLE_GLOBAL_INTERRUPT;

    I2C_Write_Process(0x10);                     /* I2C Master will send 10 byte from 0x10 */
    Timer0_Interrupt_Disable();                  /* After write process disable i2c time out counter */

    printf ("\n\r Delay 500ms ");
    Timer2_Delay(24000000,128,500,1000);         /* Delay demo application other process */

    printf ("\n\r I2C Read process start.");
    Timer0_AutoReload_Interrupt_Initial(16,450); /* Enable I2C time out counter for read process */
    I2C_Read_Process(0x10);                      /* I2C Master receive from slave check with byte from 0x10 */
    Timer0_Interrupt_Disable();                  /* After read process disable i2c time out counter */

    printf ("\n\r I2C Write/Read process finish.");
    printf ("\n\r");
    while (1);
/* =================== */
}

