#if defined __C51__
extern unsigned char xdata IAPSPDataBuf[127];
#elif defined __ICC8051__

#elif defined __SDCC__
extern unsigned char __xdata IAPSPDataBuf[127];
#endif

void Erase_SPROM(void);
void Erase_Verify_SPROM(unsigned int u16IAPDataSize);
void Program_SPROM(unsigned int u16IAPDataSize);
void Read_Verify_SPROM( unsigned int u16IAPDataSize);
