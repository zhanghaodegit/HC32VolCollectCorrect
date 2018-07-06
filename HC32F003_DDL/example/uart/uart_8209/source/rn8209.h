
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
	IARMS,		// 22 	通道A电流的有效�?
	IBRMS,		// 23	通道B电流的有效�?
	URMS,		// 24	电压有效�?
	UFreq,
	PowerPA,	// 26 	有功功率A
	PowerPB,	// 27	有功功率B
	PowerQ,
	EnergyP,	// 29	有功能量
	EnergyP2,	// 2A
	EnergyD,	// 2B	无功能量或自定义能量
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


#define		MethodOne				0  /* ����1��ͨ��ѭ����Ȼ��ʱ�ȴ������жϱ�־ ���ñ�־��ͨ���յ��̶��ĸ�������λ��ȱ����ֻ�ܽ��չ̶����ַ�����û�пɱ��ԡ�*/
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
