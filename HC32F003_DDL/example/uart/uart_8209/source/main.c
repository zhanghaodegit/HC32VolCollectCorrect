/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
		20180705  1.1 优化O1为  zh  所有的中断和非中断共同变量使用volatile不然共同变量的值会不一样
			
 ******************************************************************************/
 
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "uart.h"
#include "bt.h"
#include "gpio.h"
#include "lpm.h"
#include "rn8209.h"
#include "alg.h"
#include "main.h"
#include "btimer.h"
#include "uart_host.h"
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/
extern uint16_t          	u16ArrData;//1MHz 50ms
extern uint16_t          	u16InitCntData ;
extern uint32_t 		 	u32gTimeOutFlag;
extern boolean_t  			gbRxDoneFlag;
extern uint8_t 				u8RxCnt;
extern uint32_t 			u32RxData[10];
uint32_t 					u32gIaVal;
uint32_t 					u32gIbVal;
uint32_t 					u32gVrVal;

volatile uint8_t 					RnRxFlag;   //must use volatile because the RnRxFlag used in interrupt and main
uint8_t 					u8RxBuf[RNRXBUFSIZE];

volatile uint8_t 					HostRxFlag;

extern  uint8_t ucRnTxTime;
uint32_t ch_accum[3];
RESULT adc_result;
volatile uint8_t u8TxData[10] = {0x7f,0x45,0x4C,0x4C,0x4F,0x21,0x44,0x4F,0x4E,0x47};//"HUADA MCU!";


/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/
typedef struct {
    uint8_t u8Port;
    uint8_t u8Pin;
}stc_gpio_list_t;
/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
const stc_gpio_list_t gGpiolist[] =
{
		{ 0, 3 }
}; 
//uint32_t u32RxData[10];
 
 
#define DELAY	50
/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/


/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
#if MethodTwo
int8_t host_receive_task(void)
{
	uint16_t i;
    uint8_t sum;
    uint16_t clc_value;
    uart_send_t *p_pack;
    i = 0;
    memset(u8RxBuf, 0, sizeof(u8RxBuf));
    while(HostRS232RXBufferCount())
    {
        HostRS232GetChar((char *)&u8RxBuf[i]);
        i++;
        i &= HOSTRXBUFSIZE - 1;
    }
    reset_host_rx_buf();
    for(i = 0; i< HOSTRXBUFSIZE; i++)        
    {
        if(u8RxBuf[i] == 0x68)
        {
            break;
        }
    }
    p_pack = (uart_send_t *)&u8RxBuf[i];
    
    switch(p_pack->CMD)
    {
    case 0x04:
        sum = ChkHostSum((uint8_t *)p_pack, 5);
        if(sum != 0){
            break;
        }
        clc_value = p_pack->DATA[1];
        clc_value <<= 8;
        clc_value += p_pack->DATA[0];
		/*
        if(clc_value == 10000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[0] = 10000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][0] = _gWorkSpace.raw_tmp[i];
            }
        }
        else if(clc_value == 22000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[1] = 22000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][1] = _gWorkSpace.raw_tmp[i]; 
            }
        }else if(clc_value == 38000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[2] = 38000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][2] = _gWorkSpace.raw_tmp[i];
            }
        }
        SaveData(); 
*/		
        break;
    default: break;
    }
    return 0;
}

static void CALCULATERPARAMETER(void)
{
	for(uint8_t i = 0 ;i < 3; i++)
	{
			ch_accum[i] &= 0xffffff;
			adc_result.rms[i]  = (uint16_t)(ch_accum[i] * 1.0 / Ki);
	}		
}
/*上层消息串口发送函数*/
void host_send_data(void)
{
    uart_send_t *p_pack;
    p_pack = (uart_send_t *)&adc_result;
    p_pack->HEAD = 0x68;
    p_pack->CMD = 0x81;
    p_pack->SUM = ~ChkHostSum((uint8_t *)p_pack, 8) + 1;

						//M0P_UART1->SBUF = u8TxData[i];
						//this will eheck ISR->TI although disabled tx interrupt
	host_rs232_send_bytes((uint8_t *)p_pack, 9 );
//	delay1ms(1000);     
}
/* rn8209 接收完成任务函数
	现在校验只处理读三个有效值
*/

int16_t rn_uart_task(uint8_t reg,uint8_t channel)
{
    uint16_t i;
//    uint8_t sum;
//    uint16_t clc_value;
	uint8_t u8Check;
   // uart_send_t *p_pack;
    i = 0;
    memset(u8RxBuf, 0, sizeof(u8RxBuf));
    while(RnRS232RXBufferCount())
    {
        RnRS232GetChar((char *)&u8RxBuf[i]);
        i++;
        i &= RNRXBUFSIZE - 1;
    }
	
	u8Check = ~ChkRnSum(reg, u8RxBuf, 3);
	if(u8Check  == u8RxBuf[3])
	{
		ch_accum[channel] = u8RxBuf[0] << 16;
		ch_accum[channel] += u8RxBuf[1] << 8;
		ch_accum[channel] += u8RxBuf[2];	
	}
	else
	{
		ch_accum[channel] = 0;
		ch_accum[channel] += 0;
		ch_accum[channel] += 0;	
	}
	memset(u8RxBuf, 0, sizeof(u8RxBuf));
	
 //   reset_rxbuf();
	/*
    for(i = 0; i< RNRXBUFSIZE; i++)        
    {
        if(rx_buf[i] == 0x68)
        {
            break;
        }
    }
	
    p_pack = (uart_send_t *)&rx_buf[i];
    
    switch(p_pack->CMD)
    {
    case 0x04:
        sum = ChkSum((uint8_t *)p_pack, 5);
        if(sum != 0){
            break;
        }
        clc_value = p_pack->DATA[1];
        clc_value <<= 8;
        clc_value += p_pack->DATA[0];
        if(clc_value == 10000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[0] = 10000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][0] = _gWorkSpace.raw_tmp[i];
            }
        }
        else if(clc_value == 22000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[1] = 22000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][1] = _gWorkSpace.raw_tmp[i]; 
            }
        }else if(clc_value == 38000)
        {
            _gWorkSpace.ConfigAndSaved.clc_point[2] = 38000;
            for(uint16_t i=0; i<3; i++)
            {
                _gWorkSpace.ConfigAndSaved.raw[i][2] = _gWorkSpace.raw_tmp[i];
            }
        }
        SaveData();        
        break;
    default: break;
    }
	*/
    return 0;
}
#endif
int32_t main(void)
{  
	
	// P03 Led Init
	M0P_CLOCK->PERI_CLKEN_f.GPIO = 1;
	M0P_GPIO->P03_SEL_f.SEL = 0;
	M0P_GPIO->P0DIR_f.P03 = 0;
	//end P03 Led Init
	
	//时钟部分配置
	stc_clk_config_t stcCfg;
	//use RCH		
	//	Clk_SetRCHFreq(ClkFreq4Mhz);
	//	Clk_SetRCHFreq(ClkFreq8Mhz);
	//in system init is RCH 4Mhz
	Clk_SetRCHFreq(ClkFreq16Mhz);
	//Clk_SetRCHFreq(ClkFreq22_12Mhz);
	stcCfg.enClkSrc = ClkRCH;    //use RCH 
	stcCfg.enHClkDiv = ClkDiv1;
	stcCfg.enPClkDiv = ClkDiv1;
	Clk_Init(&stcCfg);
	//end Clk Init
	
	rn8209_init();
	
	
	#if  MethodTwo	
	BtTimerTest1ms(); //1ms timer bt2
	#endif	
	
	//uart1 to host Init
	uart_host_init();
	//end uart1 to host Init
	/*
	while(1)
	{
					for(int i = 0; i < 3; i ++)
					{
						//M0P_UART1->SBUF = u8TxData[i];
						//this will eheck ISR->TI although disabled tx interrupt
						Uart_SendData(UARTCH1,u8TxData[i]);
						delay1ms(1000);	
					}
	}
	*/
	
	#if MethodTwo
	while(1)
	{
		Uart_ClrStatus(UARTCH0,UartRxFull); //不要忘了清中断		
		rn_read(0x22,NULL,NULL,50);
		//memset(u8RnRxBuf, 0, sizeof(u8RnRxBuf));
		ucRnTxTime = 5 * 10;//防止接收中断没有产生
		while(!(RnRxFlag & BIT0))
		{
			//RnRxFlag &= ~BIT0;			
		}
		if(RnRxFlag & BIT0)
		{
			RnRxFlag &= ~BIT0;
			rn_uart_task(0x22,0);			
		}		
		Uart_ClrStatus(UARTCH0,UartRxFull); //不要忘了清中断
		rn_read(0x23,NULL,NULL,50);
		//memset(u8RnRxBuf, 0, sizeof(u8RnRxBuf));
		ucRnTxTime = 5 * 10;		
		while(!(RnRxFlag & BIT0))
		{
			//RnRxFlag &= ~BIT0;			
		}		
		if(RnRxFlag & BIT0)
		{
			RnRxFlag &= ~BIT0;
			rn_uart_task(0x23,1);			
		}
		Uart_ClrStatus(UARTCH0,UartRxFull); //不要忘了清中断
		rn_read(0x24,NULL,NULL,50);
		//memset(u8RnRxBuf, 0, sizeof(u8RnRxBuf));
		ucRnTxTime = 5 * 10;
		while(!(RnRxFlag & BIT0))
		{
			//RnRxFlag &= ~BIT0;			
		}
		if(RnRxFlag & BIT0)
		{
			RnRxFlag &= ~BIT0;
			rn_uart_task(0x24,2);
			
		}
		CALCULATERPARAMETER();
		//uart1 send to host
		host_send_data();
		//end uart1 send to host
		
		//if receive Host one Frame
		if(HostRxFlag & BIT0)
		{
			HostRxFlag &= ~BIT0;
			host_receive_task();
		}
		//end if receive Host one Frame
		
		//delay1ms(500);
		
	}
	
	#endif
	
#if  MethodOne	
	BtTimerTest(); //50ms timer bt2
#endif	

	
	
	
	//rn_read(0x22,NULL,NULL,3);
	//rn_write(0x7f,NULL,0);
	#if  MethodOne
	while(1)
	{

		Uart_ClrStatus(UARTCH1,UartRxFull); //不要忘了清中断
		
		rn_read(0x22,NULL,NULL,50);
		
		Bt_ARRSet(TIM2, u16ArrData);
		Bt_Cnt16Set(TIM2, u16InitCntData);
		u32gTimeOutFlag = 0;
		Bt_Run(TIM2);
		

		while(u32gTimeOutFlag == 0)
		{
			if(gbRxDoneFlag)
			{
				gbRxDoneFlag = FALSE;
				u32gIaVal = u32RxData[0]<<16;
				u32gIaVal += u32RxData[1]<<8;
				u32gIaVal += u32RxData[2];
				memset(u32RxData,0, 10);
				break;
			}
			else
			{	
				
			}
		}
		delay1ms(10);
		u8RxCnt= 0;
		
		
		
		
		//delay1ms(1000);
		rn_read(0x23,NULL,NULL,50);
		
		Bt_ARRSet(TIM2, u16ArrData);
		Bt_Cnt16Set(TIM2, u16InitCntData);
		u32gTimeOutFlag = 0;
		Bt_Run(TIM2);
		
		while(u32gTimeOutFlag == 0)
		{
			if(gbRxDoneFlag)
			{
				gbRxDoneFlag = FALSE;
				u32gIbVal = u32RxData[0] << 16;
				u32gIbVal += u32RxData[1] << 8;
				u32gIbVal += u32RxData[2];
				memset(u32RxData,0, 10);
				break;
			}
			else
			{		
				
			}
		}
		delay1ms(10);
		u8RxCnt= 0;
		
		
		
		//delay1ms(1000);
		rn_read(0x24,NULL,NULL,50);		
		Bt_ARRSet(TIM2, u16ArrData);
		Bt_Cnt16Set(TIM2, u16InitCntData);
		u32gTimeOutFlag = 0;
		Bt_Run(TIM2);
			
		while(u32gTimeOutFlag == 0)
		{
			if(gbRxDoneFlag)
			{
				gbRxDoneFlag = FALSE;
				u32gVrVal = u32RxData[0]<<16;
				u32gVrVal += u32RxData[1] << 8;
				u32gVrVal += u32RxData[2];
				memset(u32RxData,0, 10);
				break;
			}
			else
			{	
				
			}
		}		
		delay1ms(10);
		u8RxCnt= 0;
		
		

		delay1ms(10000);
		
	}
	#endif
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


