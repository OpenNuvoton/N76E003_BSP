/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "numicro_8051.h"


#define I2C_SLAVE_ADDRESS         0xA4
#define LOOP_SIZE                 10 

uint8_t data_received[10], DataRevCounter = 0, DataSendCounter = 0 ;
BIT i2cReceiveOverflag;

/**
  * @brief     I2C interrupt vector.
  * @param[in] Vector 0x38 (6).
  * @return    None.
*/
#if defined __C51__
void I2C0_ISR(void) interrupt 6
#elif defined __ICC8051__
#pragma vector=0x33
__interrupt void I2C_ISR(void)
#elif defined __SDCC__
void I2C0_ISR(void) __interrupt (6)
#endif
{
   SFRS_TMP = SFRS;              /* for SFRS page */

  SFRS = 0;
  switch (I2STAT)
    {
        case 0x00:
            set_I2CON_STO;
            while(STO);
        break;

        case 0x60:
            set_I2CON_AA;
        break;
        
        case 0x68:
            clr_I2CON_AA;
        break;

        case 0x80:
            data_received[DataRevCounter] = I2DAT;
            DataRevCounter++;
            if (DataRevCounter == 10) 
            {
                clr_I2CON_AA;
            }
            else
                set_I2CON_AA;
        break;

        case 0x88:
            data_received[DataRevCounter] = I2DAT;
            DataRevCounter = 0;
            clr_I2CON_AA;
        break;

        case 0xA0:
            if (DataRevCounter==10)
            {
               i2cReceiveOverflag = 1 ;
            }
            DataRevCounter =0;
            set_I2CON_AA;
        break;

        case 0xA8:
            I2DAT = data_received[DataSendCounter];
            DataSendCounter++;
            set_I2CON_AA;
        break;

        case 0xB8: 
            I2DAT = data_received[DataSendCounter];
            DataSendCounter++;
            set_I2CON_AA;
        break;

        case 0xC0:
            DataSendCounter = 0;
            set_I2CON_AA;
        break; 

        case 0xC8:
            set_I2CON_AA;
        break;
    }

    I2C0_SI_Check();

    if (SFRS_TMP)                 /* for SFRS page */
    {
      ENABLE_SFR_PAGE1;
    }
}

//========================================================================================================
void Init_I2C_Slave_Interrupt(void)
{
    P13_QUASI_MODE;          /* External pull high resister in circuit */
    P14_QUASI_MODE;          /* External pull high resister in circuit */

    P13_ST_ENABLE;               /* Setting schmit tigger mode */
    P14_ST_ENABLE;               /* Setting schmit tigger mode */
  
    P13 = 1;                     /* set SDA and SCL pins high */
    P14 = 1;
  
    I2C_Slave_Open(I2C_SLAVE_ADDRESS);
    I2C_Interrupt(ENABLE);
}


/******************************
  * @brief     Main loop.
  * @param[in] None.
  * @return    None.
******************************/
void main(void)
{

    Enable_UART0_VCOM_printf_166M_115200();
    printf ("\n I2C slave start.");

  /* Initial I2C function */
    i2cReceiveOverflag = 0;
    Init_I2C_Slave_Interrupt();                                 //initial I2C circuit
    ENABLE_GLOBAL_INTERRUPT;

    while (1)
    {
         if (i2cReceiveOverflag)
        {
           /* After receive storage in dataflash */
           Write_DATAFLASH_ARRAY(0x3000, data_received, 10);
           i2cReceiveOverflag = 0;
           printf ("\n I2C slave receive data write in 0x3000...DONE.");
        }
     }

}

