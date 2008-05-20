iMXL LCD controller parameters:
LPCR: 0XF8C80085
HCR:  0X28000500
VCR:  0X0c001617

LCD freq: 7630000Hz

SysClk has to be changed from internal to external:
CSCR = 0x03010003
SPCTL0 = 0x04011402
SPCTL1 = 0x00000040 

