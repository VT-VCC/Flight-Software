#ifndef _COMMON_RFM_INTERNAL_H_
#define _COMMON_RFM_INTERNAL_H_

//TODO make sure this is accurate for our processor
#define FSTEP  61.03515625

// Register addresses
#define REG_FIFO          0x00
#define REG_OPMODE        0x01
#define REG_DATAMODUL     0x02
#define REG_BITRATEMSB    0x03
#define REG_BITRATELSB    0x04
#define REG_FDEVMSB       0x05
#define REG_FDEVLSB       0x06
#define REG_FRFMSB        0x07
#define REG_FRFMID        0x08
#define REG_FRFLSB        0x09
#define REG_OSC1          0x0A
#define REG_AFCCTRL       0x0B
#define REG_LOWBAT        0x0C
#define REG_LISTEN1       0x0D
#define REG_LISTEN2       0x0E
#define REG_LISTEN3       0x0F
#define REG_VERSION       0x10
#define REG_PALEVEL       0x11
#define REG_PARAMP        0x12
#define REG_OCP           0x13
#define REG_LNA           0x18
#define REG_RXBW          0x19
#define REG_AFCBW         0x1A
#define REG_OOKPEAK       0x1B
#define REG_OOKAVG        0x1C
#define REG_OOKFIX        0x1D
#define REG_AFCFEI        0x1E
#define REG_AFCMSB        0x1F
#define REG_AFCLSB        0x20
#define REG_FEIMSB        0x21
#define REG_FEILSB        0x22
#define REG_RSSICONFIG    0x23
#define REG_RSSIVALUE     0x24
#define REG_DIOMAPPING1   0x25
#define REG_DIOMAPPING2   0x26
#define REG_IRQFLAGS1     0x27
#define REG_IRQFLAGS2     0x28
#define REG_RSSITHRESH    0x29
#define REG_RXTIMEOUT1    0x2A
#define REG_RXTIMEOUT2    0x2B
#define REG_PREAMBLEMSB   0x2C
#define REG_PREAMBLELSB   0x2D
#define REG_SYNCCONFIG    0x2E
#define REG_SYNCVALUE1    0x2F
#define REG_SYNCVALUE2    0x30
#define REG_SYNCVALUE3    0x31
#define REG_SYNCVALUE4    0x32
#define REG_SYNCVALUE5    0x33
#define REG_SYNCVALUE6    0x34
#define REG_SYNCVALUE7    0x35
#define REG_SYNCVALUE8    0x36
#define REG_PACKETCONFIG1 0x37
#define REG_PAYLOADLENGTH 0x38
#define REG_NODEADRS      0x39
#define REG_BROADCASTADRS 0x3A
#define REG_AUTOMODES     0x3B
#define REG_FIFOTHRESH    0x3C
#define REG_PACKETCONFIG2 0x3D
#define REG_AESKEY1       0x3E
#define REG_AESKEY2       0x3F
#define REG_AESKEY3       0x40
#define REG_AESKEY4       0x41
#define REG_AESKEY5       0x42
#define REG_AESKEY6       0x43
#define REG_AESKEY7       0x44
#define REG_AESKEY8       0x45
#define REG_AESKEY9       0x46
#define REG_AESKEY10      0x47
#define REG_AESKEY11      0x48
#define REG_AESKEY12      0x49
#define REG_AESKEY13      0x4A
#define REG_AESKEY14      0x4B
#define REG_AESKEY15      0x4C
#define REG_AESKEY16      0x4D
#define REG_TEMP1         0x4E
#define REG_TEMP2         0x4F
#define REG_TESTLNA       0x58
#define REG_TESTPA1       0x5A
#define REG_TESTPA2       0x5C

#define DATAMODUL_DATAMODE_PACKET            0x00
#define DATAMODUL_DATAMODE_CONTINUOUS        0x40
#define DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC 0x60

#define DATAMODUL_MODULATIONTYPE_FSK         0x00 
#define DATAMODUL_MODULATIONTYPE_OOK         0x08

#define DATAMODUL_MODULATIONSHAPING_00       0x00
#define DATAMODUL_MODULATIONSHAPING_01       0x01
#define DATAMODUL_MODULATIONSHAPING_10       0x02
#define DATAMODUL_MODULATIONSHAPING_11       0x03

#define BITRATEMSB_1200            0x68
#define BITRATELSB_1200            0x2B
#define BITRATEMSB_2400            0x34
#define BITRATELSB_2400            0x15
#define BITRATEMSB_4800            0x1A
#define BITRATELSB_4800            0x0B
#define BITRATEMSB_9600            0x0D
#define BITRATELSB_9600            0x05
#define BITRATEMSB_19200           0x06
#define BITRATELSB_19200           0x83
#define BITRATEMSB_38400           0x03
#define BITRATELSB_38400           0x41

#define BITRATEMSB_38323           0x03
#define BITRATELSB_38323           0x43

#define BITRATEMSB_34482           0x03
#define BITRATELSB_34482           0xA0

#define BITRATEMSB_76800           0x01
#define BITRATELSB_76800           0xA1
#define BITRATEMSB_153600          0x00
#define BITRATELSB_153600          0xD0
#define BITRATEMSB_57600           0x02
#define BITRATELSB_57600           0x2C
#define BITRATEMSB_115200          0x01
#define BITRATELSB_115200          0x16
#define BITRATEMSB_12500           0x0A
#define BITRATELSB_12500           0x00
#define BITRATEMSB_25000           0x05
#define BITRATELSB_25000           0x00
#define BITRATEMSB_50000           0x02
#define BITRATELSB_50000           0x80
#define BITRATEMSB_100000          0x01
#define BITRATELSB_100000          0x40
#define BITRATEMSB_150000          0x00
#define BITRATELSB_150000          0xD5
#define BITRATEMSB_200000          0x00
#define BITRATELSB_200000          0xA0
#define BITRATEMSB_250000          0x00
#define BITRATELSB_250000          0x80
#define BITRATEMSB_300000          0x00
#define BITRATELSB_300000          0x6B
#define BITRATEMSB_32768           0x03
#define BITRATELSB_32768           0xD1

#define PALEVEL_PA0_ON     0x80
#define PALEVEL_PA0_OFF    0x00
#define PALEVEL_PA1_ON     0x40
#define PALEVEL_PA1_OFF    0x00
#define PALEVEL_PA2_ON     0x20
#define PALEVEL_PA2_OFF    0x00

#define IRQFLAGS1_MODEREADY            0x80
#define IRQFLAGS1_RXREADY              0x40
#define IRQFLAGS1_TXREADY              0x20
#define IRQFLAGS1_PLLLOCK              0x10
#define IRQFLAGS1_RSSI                 0x08
#define IRQFLAGS1_TIMEOUT              0x04
#define IRQFLAGS1_AUTOMODE             0x02
#define IRQFLAGS1_SYNCADDRESSMATCH     0x01

#define IRQFLAGS2_FIFOFULL             0x80
#define IRQFLAGS2_FIFONOTEMPTY         0x40
#define IRQFLAGS2_FIFOLEVEL            0x20
#define IRQFLAGS2_FIFOOVERRUN          0x10
#define IRQFLAGS2_PACKETSENT           0x08
#define IRQFLAGS2_PAYLOADREADY         0x04
#define IRQFLAGS2_CRCOK                0x02

#define TESTPA1_NORMAL 	0x55
#define TESTPA1_20DBM 	0x5D
#define TESTPA2_NORMAL	0x70
#define TESTPA2_20DBM	0x7C

#endif // _COMMON_RFM_INTERNAL_H_
