/*
copyright (c) 2018 dongmu 
All rights reserved

文件 
	rn8209相关函数
文件名
	rn8209.c
@作者 
	张浩
@版本
	1.0.0
@日期	
	2018-07-02
*/
#include "rn8209.h"
#include "uart.h"
#include "alg.h"
#include "bt.h"

//uint8_t u8TxData[10] = {0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};//"HUADA MCU!";
//volatile uint8_t u8TxData[10] = {0x7f,0x45,0x4C,0x4C,0x4F,0x21,0x44,0x4F,0x4E,0x47};//"HUADA MCU!";
//uint8_t u8TxData[10] = {0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};

uint8_t u8TxCnt=0,u8RxCnt=0;
boolean_t  gbRxDoneFlag = FALSE;

static volatile  unsigned char ucRnRxCnt;
static volatile  unsigned char ucRnRXWriteIndex,ucRnRXReadIndex;
volatile unsigned char ucRnTxTime;
volatile uint32_t u32RnRxData[RNRXBUFSIZE];
#if MethodOne
void TxIntCallback(void)
{

    if(u8TxCnt<10)
    {
        M0P_UART1->SBUF = u32RxData[u8TxCnt];
        u8TxCnt++;
    }
    else //if(u8TxCnt>10)
    {
        u8TxCnt = 0;
        Uart_ClrStatus(UARTCH1,UartTxEmpty);   
        Uart_DisableIrq(UARTCH1,UartTxIrq);
        Uart_EnableIrq(UARTCH1,UartRxIrq);
    }
    
}
/*
8209接收中断
现在只实现接收三个字节的数据
*/
void RxIntCallback(void)
{
  //  Uart_ClrStatus(UARTCH1,UartRxFull); //不要忘了清中断
	uint8_t read_data = 0;
	u32RxData[u8RxCnt]=Uart_ReceiveData(UARTCH1);
//	read_data = Uart_ReceiveData(UARTCH1);
    u8RxCnt++;
    if(u8RxCnt>=4)
    {
        //Uart_DisableIrq(UARTCH1,UartRxIrq);
        //Uart_EnableIrq(UARTCH1,UartTxIrq);
		gbRxDoneFlag = TRUE;
        u8RxCnt = 0;
       // M0P_UART1->SBUF = u32RxData[0];
     // u8TxCnt = 1;
    }

}
#endif
#if MethodTwo

char RnRS232GetChar ( char *ch)
{
    char Byte;
    if (ucRnRxCnt)
    { // char still available
        Byte = (uint8_t)u32RnRxData[ucRnRXReadIndex++]; // get byte from buffer
        ucRnRXReadIndex &= RNRXBUFSIZE-1; // adjust index
       // RX_INT_DISABLE; // disable rx interrupt (IE2)
        ucRnRxCnt--; // one char read, dec count
      //  RX_INT_ENABLE; // done, enable int (IE2)
        *ch = Byte;
        return (1);
    }
    else
        return (0); // if there is no new char
}

unsigned char RnRS232RXBufferCount (void)
{ 
    return (ucRnRxCnt);
}
void TxIntCallback(void)
{

    if(u8TxCnt<10)
    {
        M0P_UART1->SBUF = u32RnRxData[u8TxCnt];
        u8TxCnt++;
    }
    else //if(u8TxCnt>10)
    {
        u8TxCnt = 0;
        Uart_ClrStatus(UARTCH0,UartTxEmpty);   
        Uart_DisableIrq(UARTCH0,UartTxIrq);
        Uart_EnableIrq(UARTCH0,UartRxIrq);
    }
    
}
/*
8209接收中断
现在只实现接收三个字节的数据
*/
void RxIntCallback(void)
{
  
	u32RnRxData[ucRnRXWriteIndex++] = Uart_ReceiveData(UARTCH0);
	ucRnRXWriteIndex &= RNRXBUFSIZE-1; 
	ucRnRxCnt++;
	ucRnTxTime = 5 * 10;
}
#endif
void ErrIntCallback(void)
{
	static uint32_t errCount = 0;
	errCount ++;
}

int rn_read(uint8_t addr,uint8_t *d,int len,int timeout)
{
//	uint8_t buf[5];
//	uint8_t chk = addr;
//	int rc;
	en_uart_mmdorck_t enTb8;

	if(len > 4)
		len = 4;
	
	Uart_ClrStatus(UARTCH0,UartTxEmpty); 
	
	if(Count_setbit(addr) % 2 == 0)
	{
		enTb8 = DataOrEven;
		Uart_SetMMDOrCk(UARTCH0,enTb8);
	}
	else
	{
		enTb8 = AddrOrOdd;
		Uart_SetMMDOrCk(UARTCH0,enTb8);
	}

	Uart_SendData(UARTCH0,addr);
/*		
	for(int i = 0; i < len + 1; i++)
	{
		buf[i]=Uart_ReceiveData(UARTCH1);
	}

	
	//tcflush(meteFd,TCIOFLUSH);
//	serial_write(meteFd,&addr,1);
//	rc = serial_read(meteFd,buf,(len+1),timeout);

	for (int i = 0; i < len+1; ++i) {
		chk += buf[i];
	}

//	chk += addr;

	if(chk != 0xff){
		printf("%d,err\n",__LINE__);
		return 0;
	}

	int j = 0;
	for (int i = len; i > 0; i--) {
		d[i - 1] = buf[j++]; //将接收到的数据保存到数组pBuf[]
	}
*/

	return len;
}
/*
rn8209 写入函数，一般用作读 其中的奇偶校验是要自己写入的哦
addr 写入的寄存器的地址
*d 写入的缓存地址
len 写入的字节长度
*/
void rn_write(uint8_t addr,void *d,int len)
{
	uint8_t buff[32];
	uint8_t chk = 0;
	uint8_t *dp = d;

	addr |= 0x80;
	buff[0] = addr;
	chk = buff[0];
	en_uart_mmdorck_t enTb8;
//	memcpy(&buff[1],dp,len);

	for (int i = 0; i < len; ++i){
		buff[i+1] = dp[i];
		chk += buff[i+1];
	}

	chk = ~chk;

	buff[len+1] = chk;
	
	for(int i = 0; i < len + 2; i++)
	{			
		if(Count_setbit(buff[i]) % 2 == 0)
		{
			enTb8 = DataOrEven;
			Uart_SetMMDOrCk(UARTCH0,enTb8);
		}
		else
		{
			enTb8 = AddrOrOdd;
			Uart_SetMMDOrCk(UARTCH0,enTb8);
		}
		Uart_SendData(UARTCH0,buff[i]);
		
		//delay1ms(60000);
	}

//	serial_write(meteFd,buff,len+2);
	
}

void write_enable()
{
	rn_write(0xea,"\xe5",1);
}
void write_disable()	
{
	rn_write(0xea,"\xdc",1);
}
void rn_reset()
{
	rn_write(0xea,"\xfa",1);
}

/*
rn8209 初始化
@return 0 成功
*/
uint8_t rn8209_init()
{
//			uint32_t id;
			uint16_t timer=0;
			uint32_t pclk;
		//	stc_clk_config_t stcCfg;
			stc_uart_config_t  stcConfig;
			stc_uart_irq_cb_t stcUartIrqCb;
			stc_uart_multimode_t stcMulti;
			stc_uart_baud_config_t stcBaud;
			stc_bt_config_t stcBtConfig;

			en_uart_mmdorck_t enTb8;
			volatile uint32_t u32Val = 0;

			DDL_ZERO_STRUCT(stcConfig);
			DDL_ZERO_STRUCT(stcUartIrqCb);
			DDL_ZERO_STRUCT(stcMulti);
			DDL_ZERO_STRUCT(stcBaud);
			DDL_ZERO_STRUCT(stcBtConfig);
			//时钟部分配置
			//use RCH
			
		//	Clk_SetRCHFreq(ClkFreq4Mhz);
		//	Clk_SetRCHFreq(ClkFreq8Mhz);
			//in system init is RCH 4Mhz
		//	Clk_SetRCHFreq(ClkFreq16Mhz);
			//Clk_SetRCHFreq(ClkFreq22_12Mhz);
		//	stcCfg.enClkSrc = ClkRCH;
		//	stcCfg.enHClkDiv = ClkDiv1;
		//	stcCfg.enPClkDiv = ClkDiv1;
		//	Clk_Init(&stcCfg);
			
			/*
			stcCfg.enClkSrc = ClkRCH;
			stcCfg.enHClkDiv = ClkDiv1;
			stcCfg.enPClkDiv = ClkDiv1;
			Clk_Init(&stcCfg);	
			u32Val = Clk_GetHClkFreq();	
			Clk_SwitchTo(ClkRCL);
			u32Val = Clk_GetHClkFreq();	
			
			//Clk_SetRCHFreq(ClkFreq22_12Mhz);
			M0P_CLOCK->RCH_CR_f.TRIM = (*((volatile uint16_t*) (0x00100C02ul)));
			
			Clk_SwitchTo(ClkRCH);
			
			u32Val = Clk_GetHClkFreq();	
			*/
			/*
			u32Val = Clk_GetHClkFreq();	
			
			Clk_SwitchTo(ClkRCL);
			u32Val = Clk_GetHClkFreq();	
			
			Clk_SetRCLFreq(ClkFreq32_8K);
			u32Val = Clk_GetHClkFreq();
			
			
			Clk_SetRCHFreq(ClkFreq22_12Mhz);
			
			Clk_SwitchTo(ClkRCH);
			u32Val = Clk_GetHClkFreq();	
			*/
			
			
			
			
			
			//add to set sysclock fre to 22 not confirm
		//	 Clk_SetRCHFreq(ClkFreq22_12Mhz);
			
			Gpio_InitIOExt(3,5,GpioDirOut,FALSE,FALSE,FALSE,FALSE); 
			Gpio_InitIOExt(3,6,GpioDirOut,FALSE,FALSE,FALSE,FALSE);
			
			//通道端口配置
			/*
			Gpio_SetFunc_UART1TX_P35();
			Gpio_SetFunc_UART1RX_P36();

			//外设时钟使能
			Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
			Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);

			*/
			Gpio_SetFunc_UART0TX_P35();
			Gpio_SetFunc_UART0RX_P36();

			//外设时钟使能
			Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
			Clk_SetPeripheralGate(ClkPeripheralUart0,TRUE);
/*
			stcUartIrqCb.pfnRxIrqCb = NULL;
			stcUartIrqCb.pfnTxIrqCb = NULL;
			stcUartIrqCb.pfnRxErrIrqCb = NULL;
			stcConfig.pstcIrqCb = &stcUartIrqCb;
			stcConfig.bTouchNvic = FALSE;
*/
			stcUartIrqCb.pfnRxIrqCb = RxIntCallback;
			stcUartIrqCb.pfnTxIrqCb = TxIntCallback;
			stcUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;
			stcConfig.pstcIrqCb = &stcUartIrqCb;
			stcConfig.bTouchNvic = TRUE;
		#if 1//多主机模式测试
			stcConfig.enRunMode = UartMode3;
			//stcMulti.enMulti_mode = UartMulti;
			stcMulti.enMulti_mode = UartNormal;
				//don't know rb8 shoud be set???
				//don't know just DataOrEven is right???
			//enTb8 = DataOrEven;
			
			enTb8 = AddrOrOdd; //如果用作奇偶校验那么这一位要自己填写
			
			//Uart_SetMMDOrCk(UARTCH1,enTb8);
			Uart_SetMMDOrCk(UARTCH0,enTb8);
			stcConfig.pstcMultiMode = &stcMulti;
			
			stcBaud.bDbaud = 0u;
			stcBaud.u32Baud = 4800u;
			stcBaud.u8Mode = UartMode3; //计算波特率需要模式参数
			pclk = Clk_GetPClkFreq();
			//timer=Uart_SetBaudRate(UARTCH1,pclk,&stcBaud);
			timer=Uart_SetBaudRate(UARTCH0,pclk,&stcBaud);
		  
			stcBtConfig.enMD = BtMode2;
			stcBtConfig.enCT = BtTimer;
			/*
			Bt_Init(TIM1, &stcBtConfig);//调用basetimer1设置函数产生波特率
			Bt_ARRSet(TIM1,timer);
			Bt_Cnt16Set(TIM1,timer);
			Bt_Run(TIM1);
			*/
			Bt_Init(TIM0, &stcBtConfig);//调用basetimer1设置函数产生波特率
			Bt_ARRSet(TIM0,timer);
			Bt_Cnt16Set(TIM0,timer);
			Bt_Run(TIM0);
		#endif
			/*
			Uart_Init(UARTCH1, &stcConfig);
			Uart_EnableIrq(UARTCH1,UartRxIrq);
		//	Uart_EnableIrq(UARTCH1,UartTxIrq);
			//Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
			Uart_EnableFunc(UARTCH1,UartRx);   //通道发送或接受使能 非mode0模式代表RX and TX ,mode0模式代表RX 

		//    Uart_SendData(UARTCH1,0xC0);
		//	Uart_ClrStatus(UARTCH1,UartTxEmpty);  
		//	Uart_DisableIrq(UARTCH1,UartTxIrq);
			*/
			Uart_Init(UARTCH0, &stcConfig);
			Uart_EnableIrq(UARTCH0,UartRxIrq);
		//	Uart_EnableIrq(UARTCH1,UartTxIrq);
			//Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
			Uart_EnableFunc(UARTCH0,UartRx);   //通道发送或接受使能 非mode0模式代表RX and TX ,mode0模式代表RX 

		//    Uart_SendData(UARTCH1,0xC0);
		//	Uart_ClrStatus(UARTCH1,UartTxEmpty);  
		//	Uart_DisableIrq(UARTCH1,UartTxIrq);
			rn_reset();
			delay1ms(200);
			
			return 0;
	
}
