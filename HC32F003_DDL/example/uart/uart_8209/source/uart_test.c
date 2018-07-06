#include "uart_test.h"

#if (P33_UART1_MODE3_TEST | P33_UART1_MODE1_TEST)
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
void RxIntCallback(void)
{
    u32RxData[u8RxCnt]=Uart_ReceiveData(UARTCH1);
    u8RxCnt++;
    if(u8RxCnt>=10)
    {
        //Uart_DisableIrq(UARTCH1,UartRxIrq);
        //Uart_EnableIrq(UARTCH1,UartTxIrq);
        u8RxCnt = 0;
       // M0P_UART1->SBUF = u32RxData[0];
       // u8TxCnt = 1;
    }

}
void ErrIntCallback(void)
{
	static uint32_t errCount = 0;
	errCount ++;
}
#endif
#if P35_UART1_MODE3_TEST
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
void RxIntCallback(void)
{
    u32RxData[u8RxCnt]=Uart_ReceiveData(UARTCH1);
    u8RxCnt++;
    if(u8RxCnt>=10)
    {
        //Uart_DisableIrq(UARTCH1,UartRxIrq);
        //Uart_EnableIrq(UARTCH1,UartTxIrq);
        u8RxCnt = 0;
       // M0P_UART1->SBUF = u32RxData[0];
       // u8TxCnt = 1;
    }

}
void ErrIntCallback(void)
{
	static uint32_t errCount = 0;
	errCount ++;
}
#endif
		#if  P33_UART1_MODE1_TEST
			uint16_t timer;
			uint32_t pclk;
			stc_clk_config_t stcCfg;
			stc_uart_config_t  stcConfig;
			stc_uart_irq_cb_t stcUartIrqCb;
			stc_uart_multimode_t stcMulti;
			stc_uart_baud_config_t stcBaud;
			stc_bt_config_t stcBtConfig;
			stc_lpm_config_t stcLpmCfg;
    
			en_uart_mmdorck_t enTb8;

			DDL_ZERO_STRUCT(stcConfig);
			DDL_ZERO_STRUCT(stcUartIrqCb);
			DDL_ZERO_STRUCT(stcMulti);
			DDL_ZERO_STRUCT(stcBaud);
			DDL_ZERO_STRUCT(stcBtConfig);
			DDL_ZERO_STRUCT(stcLpmCfg);
			
			stcLpmCfg.enSLEEPDEEP = SlpDpDisable;
			
			//时钟部分配置
			//stcCfg.enClkSrc = ClkXTH;
			stcCfg.enClkSrc = ClkRCH;
			stcCfg.enHClkDiv = ClkDiv1;
			stcCfg.enPClkDiv = ClkDiv1;
			Clk_Init(&stcCfg);
			
	//		Gpio_InitIO(T1_PORT,T1_PIN,GpioDirIn); 
	//		Gpio_InitIO(0,3,GpioDirOut);
	//		Gpio_SetIO(0,3,1);
			
	//		Gpio_InitIOExt(3,2,GpioDirOut,TRUE,FALSE,FALSE,FALSE);   
	//		Gpio_InitIOExt(3,3,GpioDirOut,TRUE,FALSE,FALSE,FALSE); 
	
			Gpio_InitIOExt(3,2,GpioDirOut,FALSE,FALSE,FALSE,FALSE);   
			Gpio_InitIOExt(3,3,GpioDirOut,FALSE,FALSE,FALSE,FALSE); 
			
			//通道端口配置
			Gpio_SetFunc_UART1TX_P32();
			Gpio_SetFunc_UART1RX_P33();

			//外设时钟使能
			Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);//模式0/2可以不使能
			Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);



			stcUartIrqCb.pfnRxIrqCb = RxIntCallback;
			stcUartIrqCb.pfnTxIrqCb = TxIntCallback;
			stcUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;
			stcConfig.pstcIrqCb = &stcUartIrqCb;
			stcConfig.bTouchNvic = TRUE;
		  

			stcConfig.enRunMode = UartMode1;//测试项，更改此处来转换4种模式测试
		   

			stcMulti.enMulti_mode = UartNormal;//测试项，更改此处来转换多主机模式，mode2/3才有多主机模式
			enTb8 = DataOrEven;
			Uart_SetMMDOrCk(UARTCH1,enTb8);
			stcConfig.pstcMultiMode = &stcMulti;
		#if 1  
			stcBaud.bDbaud = 0u;//双倍波特率功能
			stcBaud.u32Baud = 2400u;//更新波特率位置
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
			Uart_ClrStatus(UARTCH1,UartRxFull);
			Uart_EnableFunc(UARTCH1,UartRx);			
			Uart_DisableIrq(UARTCH1,UartTxIrq);
			while(1)
			{
					for(int i = 0; i < 1; i ++)
					{
						//M0P_UART1->SBUF = u8TxData[i];
						//this will eheck ISR->TI although disabled tx interrupt
						Uart_SendData(UARTCH1,u8TxData[i]);
						//delay1ms(1000);	
					}
					//delay1ms(250);					
			}
		#endif
		#if P33_UART1_MODE3_TEST
			uint32_t id;
			uint16_t timer,i=0;
			uint32_t pclk;
			stc_clk_config_t stcCfg;
			stc_uart_config_t  stcConfig;
			stc_uart_irq_cb_t stcUartIrqCb;
			stc_uart_multimode_t stcMulti;
			stc_uart_baud_config_t stcBaud;
			stc_bt_config_t stcBtConfig;

			en_uart_mmdorck_t enTb8;

			DDL_ZERO_STRUCT(stcConfig);
			DDL_ZERO_STRUCT(stcUartIrqCb);
			DDL_ZERO_STRUCT(stcMulti);
			DDL_ZERO_STRUCT(stcBaud);
			DDL_ZERO_STRUCT(stcBtConfig);
			//时钟部分配置
			//use RCH
			stcCfg.enClkSrc = ClkRCH;
			stcCfg.enHClkDiv = ClkDiv1;
			stcCfg.enPClkDiv = ClkDiv1;
			Clk_Init(&stcCfg);
			
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
			enTb8 = AddrOrOdd;
			Uart_SetMMDOrCk(UARTCH1,enTb8);
			stcConfig.pstcMultiMode = &stcMulti;
			
			stcBaud.bDbaud = 0u;
		   // stcBaud.u32Baud = 2400u;
			  stcBaud.u32Baud = 9600u;
			stcBaud.u8Mode = UartMode3; //计算波特率需要模式参数
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
			Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
			Uart_EnableFunc(UARTCH1,UartRx);   
		#if 1
		//    Uart_SendData(UARTCH1,0xC0);
			enTb8 = DataOrEven;
			Uart_SetMMDOrCk(UARTCH1,enTb8);
			Uart_ClrStatus(UARTCH1,UartTxEmpty);  
			#endif
			Uart_DisableIrq(UARTCH1,UartTxIrq);
			while(1)
			{
					for(int i = 0; i < 10; i ++)
					{
						//M0P_UART1->SBUF = u8TxData[i];
						//this will eheck ISR->TI although disabled tx interrupt
						if(Count_setbit(u8TxData[i]) % 2 == 0)
						{
							enTb8 = DataOrEven;
							Uart_SetMMDOrCk(UARTCH1,enTb8);
						}
						else
						{
							enTb8 = AddrOrOdd;
							Uart_SetMMDOrCk(UARTCH1,enTb8);
						}
						Uart_SendData(UARTCH1,u8TxData[i]);
						delay1ms(1000);	
					}
					//delay1ms(250);					
			}
			
			
		#endif
	#if P35_UART1_MODE3_TEST
			uint32_t id;
			uint16_t timer,i=0;
			uint32_t pclk;
			stc_clk_config_t stcCfg;
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
			Clk_SetRCHFreq(ClkFreq16Mhz);
			//Clk_SetRCHFreq(ClkFreq22_12Mhz);
			stcCfg.enClkSrc = ClkRCH;
			stcCfg.enHClkDiv = ClkDiv1;
			stcCfg.enPClkDiv = ClkDiv1;
			Clk_Init(&stcCfg);
			
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
			Gpio_SetFunc_UART1TX_P35();
			Gpio_SetFunc_UART1RX_P36();

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
			enTb8 = AddrOrOdd;
			Uart_SetMMDOrCk(UARTCH1,enTb8);
			stcConfig.pstcMultiMode = &stcMulti;
			
			stcBaud.bDbaud = 0u;
			stcBaud.u32Baud = 4800u;
			stcBaud.u8Mode = UartMode3; //计算波特率需要模式参数
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
			Uart_ClrStatus(UARTCH1,UartTxEmpty);//清所有中断请求
			Uart_EnableFunc(UARTCH1,UartRx);   
		#if 1
		//    Uart_SendData(UARTCH1,0xC0);
			Uart_ClrStatus(UARTCH1,UartTxEmpty);  
		#endif
			Uart_DisableIrq(UARTCH1,UartTxIrq);
			
			rn_reset();
			delay1ms(2000);


		//	write_enable();
		//	rn_write(EMUCON2,"\x00\x30",2);
		//	rn_write(SYSCON,"\x00\x40",2);
		//	rn_write(HFConst,"\x24\xf7",2);
		//	rn_write(IARMSOS,"\xFE\xA0",2);
		//	rn_write(IBRMSOS,"\xFE\xA0",2);
		//	write_disable();
		//	rn_read(SYSCON,&id,2,100);
		//	rn_read(IARMSOS,&id,2,100);
		//	rn_read(IARMS,&id,2,100);
		//rn_read(IBRMS,&id,2,100);
//		rn_read(URMS,&id,2,100);
		
		
			
			delay1ms(20000);
			
			while(1)
			{
				if(Count_setbit(u8TxData[0]) % 2 == 0)
				{
					enTb8 = DataOrEven;
					Uart_SetMMDOrCk(UARTCH1,enTb8);
				}
				else
				{
					enTb8 = AddrOrOdd;
					Uart_SetMMDOrCk(UARTCH1,enTb8);
				}
				//Uart_SendData(UARTCH1,u8TxData[0]);
				rn_read(0x7f,NULL, 0,0);
				delay1ms(1000);	
			}
			//while(1)
			//{
					//for(int i = 0; i < 1; i ++)
					//{
						//M0P_UART1->SBUF = u8TxData[i];
						//this will eheck ISR->TI although disabled tx interrupt
						//Uart_SendData(UARTCH1,u8TxData[i]);
						//delay1ms(1000);	
					//}
					//delay1ms(250);					
			//}
#endif
