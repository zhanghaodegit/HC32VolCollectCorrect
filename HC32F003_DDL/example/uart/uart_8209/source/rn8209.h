
#include <stdint.h>
typedef enum {
	SYSCON=0x00,
	EMUCON,
	HFConst,
	PStart,
	DStart,
	GPQA,
	GPQB,
	PhsA,
	PhsB,
	QPhsCal,
	APOSA,
	APOSB,
	RPOSA,
	RPOSB,
	IARMSOS,
	IBRMSOS,
	IBGain,
	D2FPL,
	D2FPH,
	DCIAH,
	DCIBH,
	DCUH,
	DCL,
	EMUCON2,

	PFCnt = 0x20,
	DFcnt,
	IARMS,		// 22 	閫氶亾A鐢垫祦鐨勬湁鏁堝�?
	IBRMS,		// 23	閫氶亾B鐢垫祦鐨勬湁鏁堝�?
	URMS,		// 24	鐢靛帇鏈夋晥鍊?
	UFreq,
	PowerPA,	// 26 	鏈夊姛鍔熺巼A
	PowerPB,	// 27	鏈夊姛鍔熺巼B
	PowerQ,
	EnergyP,	// 29	鏈夊姛鑳介噺
	EnergyP2,	// 2A
	EnergyD,	// 2B	鏃犲姛鑳介噺鎴栬嚜瀹氫箟鑳介噺
	EnergyD2,	// 2C
	EMUStatus,
	SPL_IA,
	SPL_IB,
	SPL_U,
	UFreq2,

	IE = 0x40,
	IF,
	RIF ,
	SysStatus ,
	RData ,
	WData ,

	DeviceID = 0x7f
}Rn8209Reg;


#define		MethodOne				0  /* 方法1是通过循环读然后超时等待接收中断标志 ，该标志是通过收到固定的个数来置位，缺点是只能接收固定的字符个数没有可变性。*/
#define		MethodTwo				1
#define BIT0                	(0x0001u)
#define 	RNRXBUFSIZE			16
int rn_read(uint8_t addr,uint8_t *d,int len,int timeout);
void rn_write(uint8_t addr,void *d,int len);

void write_enable(void );	
void write_disable(void );	
void rn_reset(void );

uint8_t rn8209_init(void );

char RnRS232GetChar ( char *ch);
unsigned char RnRS232RXBufferCount (void);
