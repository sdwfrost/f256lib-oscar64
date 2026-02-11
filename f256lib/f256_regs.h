#ifndef F256_REGS_H
#define F256_REGS_H

//
// F256 Hardware Register Definitions (merged)
// Integer constant style for use with PEEK/POKE macros.
// Merged from: f256jr.h, f256_dma.h, f256_intmath.h, f256_irq.h,
//   f256_rtc.h, f256_sprites.h, f256_tiles.h, f256_timers.h,
//   f256_via.h, f256_xymath.h
//

// ============================================================
// MMU Registers (from f256jr.h)
// ============================================================

#define MMU_MEM_CTRL    0x0000
#define MMU_IO_CTRL     0x0001
#define MMU_IO_PAGE_0   0x00
#define MMU_IO_PAGE_1   0x01
#define MMU_IO_TEXT     0x02
#define MMU_IO_COLOR    0x03
#define MMU_MEM_BANK_0  0x0008
#define MMU_MEM_BANK_1  0x0009
#define MMU_MEM_BANK_2  0x000A
#define MMU_MEM_BANK_3  0x000B
#define MMU_MEM_BANK_4  0x000C
#define MMU_MEM_BANK_5  0x000D
#define MMU_MEM_BANK_6  0x000E
#define MMU_MEM_BANK_7  0x000F

// ============================================================
// Vicky Registers (from f256jr.h)
// ============================================================

#define VKY_MSTR_CTRL_0   0xD000
#define VKY_MSTR_CTRL_1   0xD001

#define VKY_LAYER_CTRL_0  0xD002
#define VKY_LAYER_CTRL_1  0xD003

#define VKY_BRDR_CTRL     0xD004
#define VKY_BRDR_COL_B    0xD005
#define VKY_BRDR_COL_G    0xD006
#define VKY_BRDR_COL_R    0xD007
#define VKY_BRDR_VERT     0xD008
#define VKY_BRDR_HORI     0xD009

#define VKY_BKG_COL_B     0xD00D
#define VKY_BKG_COL_G     0xD00E
#define VKY_BKG_COL_R     0xD00F

#define VKY_CRSR_CTRL     0xD010
#define VKY_CRSR_CHAR     0xD012
#define VKY_CRSR_X_L      0xD014
#define VKY_CRSR_X_H      0xD015
#define VKY_CRSR_Y_L      0xD016
#define VKY_CRSR_Y_H      0xD017

#define VKY_LINE_CTRL     0xD018
#define VKY_LINE_ENABLE   0x01
#define VKY_LINE_NBR_L    0xD019
#define VKY_LINE_NBR_H    0xD01A

// ============================================================
// Bitmap Registers (from f256jr.h)
// ============================================================

#define VKY_BM0_CTRL      0xD100
#define VKY_BM0_ADDR_L    0xD101
#define VKY_BM0_ADDR_M    0xD102
#define VKY_BM0_ADDR_H    0xD103

#define VKY_BM1_CTRL      0xD108
#define VKY_BM1_ADDR_L    0xD109
#define VKY_BM1_ADDR_M    0xD10A
#define VKY_BM1_ADDR_H    0xD10B

#define VKY_BM2_CTRL      0xD110
#define VKY_BM2_ADDR_L    0xD111
#define VKY_BM2_ADDR_M    0xD112
#define VKY_BM2_ADDR_H    0xD113

#define VKY_TXT_FGLUT     0xD800
#define VKY_TXT_BGLUT     0xD840

// ============================================================
// Color Lookup Tables (I/O Page 1) (from f256jr.h)
// ============================================================

#define VKY_GR_CLUT_0     0xD000
#define VKY_GR_CLUT_1     0xD400
#define VKY_GR_CLUT_2     0xD800
#define VKY_GR_CLUT_3     0xDC00

// ============================================================
// System Control (from f256jr.h)
// ============================================================

#define VKY_SYS0           0xD6A0
#define VKY_SYS1           0xD6A1

#define SYS_SID_ST         0x08
#define SYS_PSG_ST         0x04

#define VKY_RST0           0xD6A2
#define VKY_RST1           0xD6A3

#define VKY_SEEDL          0xD6A4
#define VKY_RNDL           0xD6A4
#define VKY_SEEDH          0xD6A5
#define VKY_RNDH           0xD6A5
#define VKY_RND_CTRL       0xD6A6
#define VKY_RND_STAT       0xD6A6

#define VKY_MID            0xD6A7
#define VKY_PCBID0         0xD6A8
#define VKY_PCBID1         0xD6A9
#define VKY_CHSV0          0xD6AA
#define VKY_CHSV1          0xD6AB
#define VKY_CHV0           0xD6AC
#define VKY_CHV1           0xD6AD
#define VKY_CHN0           0xD6AE
#define VKY_CHN1           0xD6AF
#define VKY_PCBMA          0xD6EB
#define VKY_PCBMI          0xD6EC
#define VKY_PCBD           0xD6ED
#define VKY_PCBM           0xD6EE
#define VKY_PCBY           0xD6EF

// ============================================================
// Sound Generators (from f256jr.h)
// ============================================================

#define VKY_PSG0           0xD600
#define VKY_PSG1           0xD610
#define VKY_PSG_BOTH       0xD608

#define CODEC_LO           0xD620
#define CODEC_HI           0xD621
#define CODEC_CTRL         0xD622

// ============================================================
// DMA Controller (from f256_dma.h)
// ============================================================

#define DMA_CTRL           0xDF00
#define DMA_CTRL_START     0x80
#define DMA_CTRL_INT_EN    0x08
#define DMA_CTRL_FILL      0x04
#define DMA_CTRL_2D        0x02
#define DMA_CTRL_ENABLE    0x01

#define DMA_STATUS         0xDF01
#define DMA_STAT_BUSY      0x80

#define DMA_FILL_VAL       0xDF01

#define DMA_SRC_ADDR       0xDF04
#define DMA_DST_ADDR       0xDF08
#define DMA_COUNT          0xDF0C

#define DMA_WIDTH          0xDF0C
#define DMA_HEIGHT         0xDF0E
#define DMA_STRIDE_SRC     0xDF10
#define DMA_STRIDE_DST     0xDF12

// ============================================================
// Integer Math Coprocessor (from f256_intmath.h)
// ============================================================

#define MULU_A_L           0xDE00
#define MULU_A_H           0xDE01
#define MULU_B_L           0xDE02
#define MULU_B_H           0xDE03

#define MULU_LL            0xDE10
#define MULU_LH            0xDE11
#define MULU_HL            0xDE12
#define MULU_HH            0xDE13

#define DIVU_DEN_L         0xDE04
#define DIVU_DEN_H         0xDE05
#define DIVU_NUM_L         0xDE06
#define DIVU_NUM_H         0xDE07

#define QUOU_LL            0xDE14
#define QUOU_LH            0xDE15
#define REMU_HL            0xDE16
#define REMU_HH            0xDE17

#define ADD_A_LL           0xDE08
#define ADD_A_LH           0xDE09
#define ADD_A_HL           0xDE0A
#define ADD_A_HH           0xDE0B

#define ADD_B_LL           0xDE0C
#define ADD_B_LH           0xDE0D
#define ADD_B_HL           0xDE0E
#define ADD_B_HH           0xDE0F

#define ADD_R_LL           0xDE18
#define ADD_R_LH           0xDE19
#define ADD_R_HL           0xDE1A
#define ADD_R_HH           0xDE1B

// ============================================================
// Interrupt Controller (from f256_irq.h)
// ============================================================

#define INT_PEND_0         0xD660
#define INT_PEND_1         0xD661
#define INT_MASK_0         0xD66C
#define INT_MASK_1         0xD66D

#define INT00_VKY_SOF      0x01
#define INT01_VKY_SOL      0x02
#define INT02_PS2_KBD      0x04
#define INT03_PS2_MOUSE    0x08
#define INT04_TIMER_0      0x10
#define INT05_TIMER_1      0x20
#define INT06_DMA          0x40
#define INT07_RESERVED     0x80

#define INT10_UART         0x01
#define INT11_VKY_2        0x02
#define INT12_VKY_3        0x04
#define INT13_VKY_4        0x08
#define INT14_RTC          0x10
#define INT15_VIA          0x20
#define INT16_IEC          0x40
#define INT17_SDC_INSERT   0x80

// ============================================================
// Real Time Clock (from f256_rtc.h)
// ============================================================

#define RTC_SECS           0xD690
#define RTC_SECS_ALRM      0xD691
#define RTC_MINS           0xD692
#define RTC_MINS_ALRM      0xD693
#define RTC_HOURS          0xD694
#define RTC_HOURS_ALRM     0xD695
#define RTC_DAY            0xD696
#define RTC_DAY_ALRM       0xD697
#define RTC_DAY_OF_WEEK    0xD698
#define RTC_MONTH          0xD699
#define RTC_YEAR           0xD69A

#define RTC_RATES          0xD69B
#define RTC_PI_0           0x00
#define RTC_PI_30us        0x01
#define RTC_PI_61us        0x02
#define RTC_PI_122us       0x03
#define RTC_PI_244us       0x04
#define RTC_PI_488us       0x05
#define RTC_PI_976us       0x06
#define RTC_PI_1ms         0x07
#define RTC_PI_3ms         0x08
#define RTC_PI_7ms         0x09
#define RTC_PI_15ms        0x0A
#define RTC_PI_31ms        0x0B
#define RTC_PI_62ms        0x0C
#define RTC_PI_125ms       0x0D
#define RTC_PI_250ms       0x0E
#define RTC_PI_500ms       0x0F

#define RTC_ENABLES        0xD69C
#define RTC_ABE            0x01
#define RTC_PWRIE          0x02
#define RTC_PIE            0x04
#define RTC_AIE            0x08

#define RTC_FLAGS          0xD69D
#define RTC_BVF            0x01
#define RTC_PWRF           0x02
#define RTC_PF             0x04
#define RTC_AF             0x08

#define RTC_CTRL           0xD96E
#define RTC_DSE            0x01
#define RTC_24HR           0x02
#define RTC_STOP           0x04
#define RTC_UTI            0x08

#define RTC_CENTURY        0xD69F

// ============================================================
// Sprite Registers (from f256_sprites.h)
// ============================================================

#define VKY_SP0_CTRL       0xD900
#define VKY_SP0_AD_L       0xD901
#define VKY_SP0_AD_M       0xD902
#define VKY_SP0_AD_H       0xD903
#define VKY_SP0_POS_X_L    0xD904
#define VKY_SP0_POS_X_H    0xD905
#define VKY_SP0_POS_Y_L    0xD906
#define VKY_SP0_POS_Y_H    0xD907

#define VKY_SP1_CTRL       0xD908
#define VKY_SP1_AD_L       0xD909
#define VKY_SP1_AD_M       0xD90A
#define VKY_SP1_AD_H       0xD90B
#define VKY_SP1_POS_X_L    0xD90C
#define VKY_SP1_POS_X_H    0xD90D
#define VKY_SP1_POS_Y_L    0xD90E
#define VKY_SP1_POS_Y_H    0xD90F

#define VKY_SP2_CTRL       0xD910
#define VKY_SP2_AD_L       0xD911
#define VKY_SP2_AD_M       0xD912
#define VKY_SP2_AD_H       0xD913
#define VKY_SP2_POS_X_L    0xD914
#define VKY_SP2_POS_X_H    0xD915
#define VKY_SP2_POS_Y_L    0xD916
#define VKY_SP2_POS_Y_H    0xD917

#define VKY_SP3_CTRL       0xD918
#define VKY_SP3_AD_L       0xD919
#define VKY_SP3_AD_M       0xD91A
#define VKY_SP3_AD_H       0xD91B
#define VKY_SP3_POS_X_L    0xD91C
#define VKY_SP3_POS_X_H    0xD91D
#define VKY_SP3_POS_Y_L    0xD91E
#define VKY_SP3_POS_Y_H    0xD91F

// ============================================================
// Tile Registers (from f256_tiles.h)
// ============================================================

#define VKY_TM0_CTRL       0xD200
#define VKY_TM0_ADDR_L     0xD201
#define VKY_TM0_ADDR_M     0xD202
#define VKY_TM0_ADDR_H     0xD203
#define VKY_TM0_SIZE_X     0xD204
#define VKY_TM0_SIZE_Y     0xD206
#define VKY_TM0_POS_X_L    0xD208
#define VKY_TM0_POS_X_H    0xD209
#define VKY_TM0_POS_Y_L    0xD20A
#define VKY_TM0_POS_Y_H    0xD20B

#define VKY_TM1_CTRL       0xD20C
#define VKY_TM1_ADDR_L     0xD20D
#define VKY_TM1_ADDR_M     0xD20E
#define VKY_TM1_ADDR_H     0xD20F
#define VKY_TM1_SIZE_X     0xD210
#define VKY_TM1_SIZE_Y     0xD212
#define VKY_TM1_POS_X_L    0xD214
#define VKY_TM1_POS_X_H    0xD215
#define VKY_TM1_POS_Y_L    0xD216
#define VKY_TM1_POS_Y_H    0xD217

#define VKY_TM2_CTRL       0xD218
#define VKY_TM2_ADDR_L     0xD219
#define VKY_TM2_ADDR_M     0xD21A
#define VKY_TM2_ADDR_H     0xD21B
#define VKY_TM2_SIZE_X     0xD21C
#define VKY_TM2_SIZE_Y     0xD21E
#define VKY_TM2_POS_X_L    0xD220
#define VKY_TM2_POS_X_H    0xD221
#define VKY_TM2_POS_Y_L    0xD222
#define VKY_TM2_POS_Y_H    0xD223

#define VKY_TS0_ADDR_L     0xD280
#define VKY_TS0_ADDR_M     0xD281
#define VKY_TS0_ADDR_H     0xD282

#define VKY_TS1_ADDR_L     0xD284
#define VKY_TS1_ADDR_M     0xD285
#define VKY_TS1_ADDR_H     0xD286

#define VKY_TS2_ADDR_L     0xD288
#define VKY_TS2_ADDR_M     0xD289
#define VKY_TS2_ADDR_H     0xD28A

#define VKY_TS3_ADDR_L     0xD28C
#define VKY_TS3_ADDR_M     0xD28D
#define VKY_TS3_ADDR_H     0xD28E

#define VKY_TS4_ADDR_L     0xD290
#define VKY_TS4_ADDR_M     0xD291
#define VKY_TS4_ADDR_H     0xD292

#define VKY_TS5_ADDR_L     0xD294
#define VKY_TS5_ADDR_M     0xD295
#define VKY_TS5_ADDR_H     0xD296

#define VKY_TS6_ADDR_L     0xD298
#define VKY_TS6_ADDR_M     0xD299
#define VKY_TS6_ADDR_H     0xD29A

#define VKY_TS7_ADDR_L     0xD29C
#define VKY_TS7_ADDR_M     0xD29D
#define VKY_TS7_ADDR_H     0xD29E

// Tile set square config (byte after ADDR_H for each)
#define VKY_TS0_SQUARE     (VKY_TS0_ADDR_H+1)
#define VKY_TS1_SQUARE     (VKY_TS1_ADDR_H+1)
#define VKY_TS2_SQUARE     (VKY_TS2_ADDR_H+1)
#define VKY_TS3_SQUARE     (VKY_TS3_ADDR_H+1)
#define VKY_TS4_SQUARE     (VKY_TS4_ADDR_H+1)
#define VKY_TS5_SQUARE     (VKY_TS5_ADDR_H+1)
#define VKY_TS6_SQUARE     (VKY_TS6_ADDR_H+1)
#define VKY_TS7_SQUARE     (VKY_TS7_ADDR_H+1)

// ============================================================
// Timers (from f256_timers.h)
// ============================================================

#define TM0_CTRL           0xD650
#define TM_CTRL_ENABLE     0x01
#define TM_CTRL_CLEAR      0x02
#define TM_CTRL_LOAD       0x04
#define TM_CTRL_UP_DOWN    0x08
#define TM_CTRL_RECLEAR    0x10
#define TM_CTRL_RELOAD     0x20
#define TM_CTRL_INTEN      0x80

#define TM0_STAT           0xD650
#define TM_STAT_EQUAL      0x01

#define TM0_VALUE_L        0xD651
#define TM0_VALUE_M        0xD652
#define TM0_VALUE_H        0xD653

#define TM0_CMP_CTRL       0xD654
#define TM_CMP_CTRL_CLR    0x01
#define TM_CMP_CTRL_LOAD   0x02

#define TM0_CMP_L          0xD655
#define TM0_CMP_M          0xD656
#define TM0_CMP_H          0xD657

#define TM1_CTRL           0xD658
#define TM1_STAT           0xD658
#define TM1_VALUE_L        0xD659
#define TM1_VALUE_M        0xD65A
#define TM1_VALUE_H        0xD65B
#define TM1_CMP_CTRL       0xD65C
#define TM1_CMP_L          0xD65D
#define TM1_CMP_M          0xD65E
#define TM1_CMP_H          0xD65F

// ============================================================
// VIA (from f256_via.h)
// ============================================================

#define VIA_IORB           0xDC00
#define VIA_IORA           0xDC01
#define VIA_DDRB           0xDC02
#define VIA_DDRA           0xDC03

// ============================================================
// Bitmap Coordinate Math (from f256_xymath.h)
// ============================================================

#define XY_BASE            0xD301
#define XY_POS_X           0xD304
#define XY_POS_Y           0xD306
#define XY_OFFSET          0xD308
#define XY_BANK            0xD30A
#define XY_ADDRESS         0xD30B

// ============================================================
// PS/2 Interface
// ============================================================

#define PS2_CTRL           0xD640
#define PS2_OUT            0xD641
#define KBD_IN             0xD642
#define MS_IN              0xD643
#define PS2_STAT           0xD644

// PS2_CTRL bits
#define PS2_K_WR           0x01
#define PS2_M_WR           0x02
#define PS2_KCLR           0x04
#define PS2_MCLR           0x08

// PS2_STAT bits
#define PS2_KEMP           0x01   // Keyboard FIFO empty
#define PS2_MEMP           0x02   // Mouse FIFO empty

// PS/2 Mouse position registers
#define PS2_M_MODE_EN      0xD6E0
#define PS2_M_X_LO         0xD6E2
#define PS2_M_X_HI         0xD6E3
#define PS2_M_Y_LO         0xD6E4
#define PS2_M_Y_HI         0xD6E5

// ============================================================
// VKY Master Control 0 bits
// ============================================================

#define VKY_TEXT           0x01
#define VKY_OVRLY          0x02
#define VKY_GRAPH          0x04
#define VKY_BITMAP         0x08
#define VKY_TILE           0x10
#define VKY_SPRITE         0x20
#define VKY_GAMMA          0x40

// ============================================================
// VKY Master Control 1 bits
// ============================================================

#define VKY_CLK_70         0x01
#define VKY_DBL_X          0x02
#define VKY_DBL_Y          0x04
#define VKY_MON_SLP        0x08
#define VKY_FON_OVLY       0x10
#define VKY_FON_SET        0x20

// ============================================================
// UART (16750-compatible)
// ============================================================

#define UART_RXD           0xD630
#define UART_TXR           0xD630
#define UART_IER           0xD631
#define UART_ISR           0xD632
#define UART_FCR           0xD632
#define UART_LCR           0xD633
#define UART_MCR           0xD634
#define UART_LSR           0xD635
#define UART_MSR           0xD636
#define UART_SPR           0xD637
#define UART_DLL           0xD630
#define UART_DLH           0xD631

// UART LSR bits
#define UART_LSR_DR        0x01
#define UART_LSR_THRE      0x20

// ============================================================
// VIA 1 (F256K keyboard matrix)
// ============================================================

#define VIA1_IORB          0xDB00
#define VIA1_IORA          0xDB01
#define VIA1_DDRB          0xDB02
#define VIA1_DDRA          0xDB03
#define VIA1_T1C_L         0xDB04
#define VIA1_T1C_H         0xDB05
#define VIA1_T1L_L         0xDB06
#define VIA1_T1L_H         0xDB07
#define VIA1_T2C_L         0xDB08
#define VIA1_T2C_H         0xDB09
#define VIA1_SDR           0xDB0A
#define VIA1_ACR           0xDB0B
#define VIA1_PCR           0xDB0C
#define VIA1_IFR           0xDB0D
#define VIA1_IER           0xDB0E
#define VIA1_IORA2         0xDB0F

// ============================================================
// RNG control bits
// ============================================================

#define VKY_RNG_EN         0x01
#define VKY_RNG_SEED       0x02

// ============================================================
// System control bits
// ============================================================

#define VKY_SYS_PWR_LED    0x01
#define VKY_SYS_SD_LED     0x02
#define VKY_SYS_L0         0x04
#define VKY_SYS_L1         0x08
#define VKY_SYS_BUZZ       0x10
#define VKY_SYS_LCK        0x20
#define VKY_SYS_NET        0x40
#define VKY_SYS_RESET      0x80

// LED color registers (active when manual LED control is enabled via VKY_SYS0)
#define LED_PWR_B          0xD6A7
#define LED_PWR_G          0xD6A8
#define LED_PWR_R          0xD6A9
#define LED_SD_B           0xD6AA
#define LED_SD_G           0xD6AB
#define LED_SD_R           0xD6AC
#define LED_LCK_B          0xD6AD
#define LED_LCK_G          0xD6AE
#define LED_LCK_R          0xD6AF
#define LED_NET_B          0xD6B3
#define LED_NET_G          0xD6B4
#define LED_NET_R          0xD6B5

// ============================================================
// SID Registers
// ============================================================

#define SID_SYS1           0xD6A1  // bit2: 0=mono, 1=stereo

#define SID1               0xD400
#define SID2               0xD500

// Voice offsets from SID base
#define SID_VOICE1         0x00
#define SID_VOICE2         0x07
#define SID_VOICE3         0x0E

// Register offsets from voice base
#define SID_LO_B           0x00    // Frequency low byte
#define SID_HI_B           0x01    // Frequency high byte
#define SID_LO_PWDC        0x02    // Pulse wave duty cycle low byte
#define SID_HI_PWDC        0x03    // Pulse wave duty cycle high byte (low nibble)
#define SID_CTRL           0x04    // Noise|Pulse|Saw|Tri|Test|Ring|Sync|Gate
#define SID_ATK_DEC        0x05    // Hi-nibble=attack, lo-nibble=decay
#define SID_SUS_REL        0x06    // Hi-nibble=sustain, lo-nibble=release

// SID-wide register offsets
#define SID_LO_FCF         0x14    // Filter cutoff freq low byte
#define SID_HI_FCF         0x15    // Filter cutoff freq high byte
#define SID_FRR            0x17    // Filter resonance and routing
#define SID_FM_VC          0x18    // Hi-nibble=mode (3ChOff|HP|BP|LP), lo-nibble=volume

// ============================================================
// OPL3 (YMF262) Registers
// ============================================================

#define OPL_ADDR_L         0xD580  // Address pointer for ports 0x000-0x0FF
#define OPL_DATA           0xD581  // Data register for all ports
#define OPL_ADDR_H         0xD582  // Address pointer for ports 0x100-0x1FF

// Internal YMF262 registers
#define OPL_EN             0x01    // Chip enable (set to 0x20)
#define OPL_T1             0x02    // Timer 1
#define OPL_T2             0x03    // Timer 2
#define OPL_IRQ            0x04    // IRQ reset/mask/start
#define OPL_FOE            0x104   // Four-operator enable (OPL3 only)
#define OPL_OPL3           0x105   // OPL3 enable (set to 0x01)
#define OPL_CSW            0x08    // Composite sine wave / note select
#define OPL_PERC           0xBD    // Percussion mode control

// OPL3 channel-scope registers
#define OPL_CH_F_LO        0xA0   // Channel frequency low byte base
#define OPL_CH_KBF_HI      0xB0   // Channel key-on/block/freq-hi base
#define OPL_CH_FEED         0xC0   // Channel feedback/synthesis base

// OPL3 operator-scope registers
#define OPL_OP_TVSKF       0x20   // Tremolo/vibrato/sustain/KSR/freq-mult base
#define OPL_OP_KSLVOL      0x40   // Key scale level / output volume base
#define OPL_OP_AD          0x60   // Attack / decay base
#define OPL_OP_SR          0x80   // Sustain / release base
#define OPL_OP_WAV         0xE0   // Waveform select base

// ============================================================
// SAM2695 MIDI Registers
// ============================================================

#define MIDI_CTRL          0xDDA0
#define MIDI_FIFO          0xDDA1
#define MIDI_RXD           0xDDA2
#define MIDI_RXD_COUNT     0xDDA3
#define MIDI_TXD           0xDDA4
#define MIDI_TXD_COUNT     0xDDA5

// ============================================================
// VS1053b MIDI Alternate Registers
// ============================================================

#define MIDI_CTRL_ALT      0xDDB0
#define MIDI_FIFO_ALT      0xDDB1
#define MIDI_RXD_ALT       0xDDB2
#define MIDI_RXD_COUNT_ALT 0xDDB3
#define MIDI_TXD_ALT       0xDDB4
#define MIDI_TXD_COUNT_ALT 0xDDB5

// ============================================================
// VS1053b Serial Bus (SCI)
// ============================================================

#define VS_SCI_CTRL        0xD700
#define VS_SCI_ADDR        0xD701
#define VS_SCI_DATA        0xD702  // 2 bytes
#define VS_FIFO_STAT       0xD704  // 2 bytes
#define VS_FIFO_DATA       0xD707

// ============================================================
// Game Pad Registers
// ============================================================

#define PAD_CTRL           0xD880
#define PAD_STAT           0xD880

// PAD_CTRL bits
#define PAD_TRIG           0x80
#define PAD_TRIG_OFF       0x00
#define PAD_DONE           0x40

// Mode selection (OR with trigger)
#define PAD_MODE_NES       0x01
#define PAD_MODE_SNES      0x05

// Pad data registers
#define PAD0               0xD884
#define PAD0_S             0xD885  // SNES only
#define PAD1               0xD886
#define PAD1_S             0xD887  // SNES only
#define PAD2               0xD888
#define PAD2_S             0xD889  // SNES only
#define PAD3               0xD88A
#define PAD3_S             0xD88B  // SNES only

// NES button masks (0 = pressed)
#define NES_A              0x80
#define NES_B              0x40
#define NES_SELECT         0x20
#define NES_START          0x10
#define NES_UP             0x08
#define NES_DOWN           0x04
#define NES_LEFT           0x02
#define NES_RIGHT          0x01

// SNES button masks - byte 0 (0 = pressed)
#define SNES_B             0x80
#define SNES_Y             0x40
#define SNES_SELECT        0x20
#define SNES_START         0x10
#define SNES_UP            0x08
#define SNES_DOWN          0x04
#define SNES_LEFT          0x02
#define SNES_RIGHT         0x01

// SNES button masks - byte 1 (PADx_S, 0 = pressed)
#define SNES_A             0x08
#define SNES_X             0x04
#define SNES_L             0x02
#define SNES_R             0x01

// ============================================================
// PSG Address Aliases
// ============================================================

#define PSG_LEFT           VKY_PSG0   // 0xD600
#define PSG_RIGHT          VKY_PSG1   // 0xD610
#define PSG_BOTH           VKY_PSG_BOTH // 0xD608

// ============================================================
// Interrupt Controller (extended from f256_irq.h)
// ============================================================

// Aliases for INT_PEND_x (some modules use PENDING spelling)
#define INT_PENDING_0      INT_PEND_0
#define INT_PENDING_1      INT_PEND_1
#define INT_PENDING_2      INT_PEND_2

#define INT_PEND_2         0xD662

#define INT_POLARITY_0     0xD664
#define INT_POLARITY_1     0xD665
#define INT_POLARITY_2     0xD666

#define INT_EDGE_0         0xD668
#define INT_EDGE_1         0xD669
#define INT_EDGE_2         0xD66A

#define INT_MASK_2         0xD66E

// Interrupt bit masks
#define INT_VKY_SOF        0x01
#define INT_VKY_SOL        0x02
#define INT_PS2_KBD        0x04
#define INT_PS2_MOUSE      0x08
#define INT_TIMER_0        0x10
#define INT_TIMER_1        0x20
#define INT_CART           0x80

// ============================================================
// LCD Controller (F256K2 case display)
// ============================================================

#define LCD_CMD_CMD        0xDD40  // Write command register
#define LCD_CMD_DTA        0xDD41  // Write data (for command)
#define LCD_PIX_LO         0xDD42  // Pixel data low {G[2:0], B[4:0]}
#define LCD_PIX_HI         0xDD43  // Pixel data high {R[4:0], G[5:3]}
#define LCD_CTRL_REG       0xDD44  // LCD control register

// LCD command codes
#define LCD_WIN_X          0x2A    // Window X command
#define LCD_WIN_Y          0x2B    // Window Y command
#define LCD_WRI            0x2C    // Write command
#define LCD_RD             0x2E    // Read command
#define LCD_MAD            0x36    // MADCTL control register

// LCD control bits
#define LCD_RST            0x10    // 0 to Reset (RSTn)
#define LCD_BL             0x20    // 1 = ON, 0 = OFF
#define LCD_TE             0x40    // Tear Enable

// LCD color constants (R5G6B5 format)
#define LCD_PURE_RED       0xF800
#define LCD_PURE_GRN       0x07E0
#define LCD_PURE_BLU       0x001F
#define LCD_PURE_WHI       0xFFFF
#define LCD_PURE_BLK       0x0000

// ============================================================
// VS1053b SCI Register Indices
// ============================================================

#define VS_SCI_ADDR_MODE       0x00
#define VS_SCI_ADDR_STATUS     0x01
#define VS_SCI_ADDR_BASS       0x02
#define VS_SCI_ADDR_CLOCKF     0x03
#define VS_SCI_ADDR_WRAM       0x06
#define VS_SCI_ADDR_WRAMADDR   0x07
#define VS_SCI_ADDR_VOL        0x0B

// VS1053b SCI CTRL bit masks
#define VS_CTRL_START      0x01
#define VS_CTRL_RWN        0x02
#define VS_CTRL_BUSY       0x80

#endif // F256_REGS_H
