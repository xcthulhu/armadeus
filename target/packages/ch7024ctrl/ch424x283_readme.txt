iMXL LCD controller parameters:
LPCR: 0XF8C80087
HCR:  0X78004401
VCR:  0X0C001414

LCD freq: 6000000.54063

SysClk has to be changed from internal to external:
CSCR = 0x03010003
SPCTL0 = 0x04011402
SPCTL1 = 0x00000040 

