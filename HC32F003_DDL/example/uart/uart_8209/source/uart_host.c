#include "uart.h"
#include "bt.h"
#include "uart_host.h"
//static uint32_t u32RxData[10];
//static uint8_t u8HostRxCnt;

volatile uint32_t u32HostRxData[HOSTRXBUFSIZE];
static volatile  unsigned char ucHostRxCnt;
static volatile  unsigned char ucHostRXWriteIndex,ucHostRXReadIndex;
volatile unsigned char 				ucHostTxTime;


void reset_host_rx_buf(void)
{
	memset((void *)u32HostRxData, 0, sizeof(u32HostRxData));
}
char HostRS232GetChar ( char *ch)
{
    char Byte;
    if (ucHostRxCnt)
    { // char still available
        Byte = (uint8_t)u32HostRxData[ucHostRXReadIndex++]; // get byte from buffer
        ucHostRXReadIndex &= HOSTRXBUFSIZE-1; // adjust index
       // RX_INT_DISABLE; // disable rx interrupt (IE2)
        ucHostRxCnt--; // one char read, dec count
      //  RX_INT_ENABLE; // done, enable int (IE2)
        *ch = Byte;
        return (1);
    }
    else
        return (0); // if there is no new char
}

unsigned char HostRS232RXBufferCount (void)
{ 
    return (ucHostRxCnt);
}



int host_rs232_send_bytes(uint8_t *bytes,int len)
{
    unsigned int i;
    for(i = 0;i < len;i++)
    {
        Uart_SendData(UARTCH1,bytes[i]);
    }
    return(i);
} 
void HostTxIntCallback(void)
{
/*
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
*/
    
}
void HostRxIntCallback(void)
{	
	u32HostRxData[ucHostRXWriteIndex++] = Uart_ReceiveData(UARTCH1);
	ucHostRXWriteIndex &= HOSTRXBUFSIZE-1; 
	ucHostRxCnt++;
	ucHostTxTime = 5 * 10;
}
void HostErrIntCallback(void)
{
	static uint32_t errCount = 0;
	errCount ++;
}
void uart_host_init(void)
{
//			uint32_t id;
			uint16_t timer;
			uint32_t pclk;
//			stc_clk_config_t stcCfg;
			stc_uart_config_t  stcConfig;
			stc_uart_irq_cb_t stcUartIrqCb;
		//	stc_uart_multimode_t stcMulti;
			stc_uart_baud_config_t stcBaud;
			stc_bt_config_t stcBtConfig;

		//	en_uart_mmdorck_t enTb8;

			DDL_ZERO_STRUCT(stcConfig);
			DDL_ZERO_STRUCT(stcUartIrqCb);
		//	DDL_ZERO_STRUCT(stcMulti);
			DDL_ZERO_STRUCT(stcBaud);
			DDL_ZERO_STRUCT(stcBtConfig);
			//时钟部分配置
			//use RCH
		/*
			stcCfg.enClkSrc = ClkRCH;
			stcCfg.enHClkDiv = ClkDiv1;
			stcCfg.enPClkDiv = ClkDiv1;
			Clk_Init(&stcCfg);
		*/	
			Gpio_InitIOExt(3,2,GpioDirOut,FALSE,FALSE,FALSE,FALSE); 
			Gpio_InitIOExt(3,3,GpioDirOut,FALSE,FALSE,FALSE,FALSE);
			
			//通道端口配置
			Gpio_SetFunc_UART1TX_P32();
			Gpio_SetFunc_UART1RX_P33();

			//外设时钟使能
			Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);
			Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);


/*
			stcUartIrqCb.pfnRxIrqCb = NULL;
			stcUartIrqCb.pfnTxIrqCb = NULL;
			stcUartIrqCb.pfnRxErrIrqCb = NULL;
			stcConfig.pstcIrqCb = &stcUartIrqCb;
			stcConfig.bTouchNvic = FALSE;
*/
			stcUartIrqCb.pfnRxIrqCb = HostRxIntCallback;
			stcUartIrqCb.pfnTxIrqCb = HostTxIntCallback;
			stcUartIrqCb.pfnRxErrIrqCb = HostErrIntCallback;
			stcConfig.pstcIrqCb = &stcUartIrqCb;
			stcConfig.bTouchNvic = TRUE;
		#if 1
			stcConfig.enRunMode = UartMode1;
			//stcMulti.enMulti_mode = UartMulti;
		//		stcMulti.enMulti_mode = UartNormal;
		//		//don't know rb8 shoud be set???
		//	enTb8 = AddrOrOdd;
		//	Uart_SetMMDOrCk(UARTCH1,enTb8);
		//	stcConfig.pstcMultiMode = &stcMulti;
			
			stcBaud.bDbaud = 0u;
		   // stcBaud.u32Baud = 2400u;
			  stcBaud.u32Baud = 9600u;
			stcBaud.u8Mode = UartMode1; //计算波特率需要模式参数
			pclk = Clk_GetPClkFreq();
			timer=Uart_SetBaudRate(UARTCH1,pclk,&stcBaud);
		  
			stcBtConfig.enMD = BtMode2;
			stcBtConfig.enCT = BtTimer;
			Bt_Init(TIM1, &stcBtConfig);//调用basetimer1设置函数产生波特率
			Bt_ARRSet(TIM1,timer);
			Bt_Cnt16Set(TIM1,timer);
			Bt_Run(TIM1);
		#endif
			Uart_Init(UARTCH1, &stcConfig);
			Uart_EnableIrq(UARTCH1,UartRxIrq);
		//	Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
			Uart_EnableFunc(UARTCH1,UartRx);   
		#if 1
		//    Uart_SendData(UARTCH1,0xC0);
	//		enTb8 = DataOrEven;
		//	Uart_SetMMDOrCk(UARTCH1,enTb8);
	//		Uart_ClrStatus(UARTCH1,UartTxEmpty);  
			#endif
			Uart_DisableIrq(UARTCH1,UartTxIrq);
	
}
