/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* SPDX-License-Identifier: Apache-2.0                                                                     */
/* Copyright(c) 2023 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

void Write_DATAFLASH_BYTE(uint16_t u16EPAddr, uint8_t u8EPData);
void Write_DATAFLASH_ARRAY(uint16_t u16_addr, uint8_t *pDat, uint16_t num);
void Read_DATAFLASH_ARRAY(uint16_t u16_addr, uint8_t *pDat, uint16_t num);
