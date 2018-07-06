#include "bt.h"
#include "btimer.h"
#include "rn8209.h"
/*******************************************************************************
 * BT1中断服务函数
 ******************************************************************************/
 #if  MethodTwo
extern unsigned char 			ucRnTxTime;
extern uint8_t 					RnRxFlag;

extern unsigned char 			ucHostTxTime;
extern uint8_t 					HostRxFlag;
void Bt2Int(void)
{
	static uint32_t led_val = 0;
	led_val ++;
	if(led_val == 1000)
	{
		M0P_GPIO->P0OUT_f.P03 = 1;
	}
	if(2000 == led_val)
	{
		M0P_GPIO->P0OUT_f.P03 = 0;
		led_val = 0;
	}
	//M0P_GPIO->P0OUT_f.P03 = led_val;
	if (TRUE == Bt_GetIntFlag(TIM2))
    {
        Bt_ClearIntFlag(TIM2);			   
    }
	if(ucRnTxTime != 0)
	{
			ucRnTxTime --;
			if(ucRnTxTime == 0)
			{
				RnRxFlag |= BIT0;
			}
	}
	if(ucHostTxTime != 0)
	{
			ucHostTxTime --;
			if(ucHostTxTime == 0)
			{
				HostRxFlag |= BIT0;
			}
	}
}
en_result_t BtTimerTest1ms(void)
{
	/*
	stc_clk_config_t stcClkCfg;
	Clk_SetRCHFreq(ClkFreq16Mhz);
    stcClkCfg.enClkSrc  = ClkRCH;
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
	Clk_Init(&stcClkCfg);
	*/
	
	stc_bt_config_t   stcConfig;
    en_result_t       enResult = Error;
	
    uint16_t          u16ArrData = 0xFC17;//1MHz  1ms (0xffff - 0x3e8)
    uint16_t          u16InitCntData = 0xFC17;

	//Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
	Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);
    
    EnableNvic(TIM2_IRQn, 3, TRUE);
    Bt_EnableIrq(TIM2);
    
    stcConfig.pfnTim2Cb = Bt2Int;
    //P25设置为门控使能IO
//    Gpio_SetFunc_GATE1_P25();
        
    stcConfig.enGateP = BtPositive; //no use Gate Control
//    stcConfig.enGate  = BtGateEnable;
	stcConfig.enGate  = BtGateDisable;
    stcConfig.enPRS   = BtPCLKDiv16;
    stcConfig.enTog   = BtTogDisable;
    stcConfig.enCT    = BtTimer;
    stcConfig.enMD    = BtMode2;
    //Bt初始化
    if (Ok != Bt_Init(TIM2, &stcConfig))
    {
        enResult = Error;
    }
    
    //设置重载值和计数值，启动计数
    Bt_ARRSet(TIM2, u16ArrData);
    Bt_Cnt16Set(TIM2, u16InitCntData);
    Bt_Run(TIM2);
	enResult = Ok;
	return enResult;
}
#endif
#if  MethodOne
uint32_t u32gTimeOutFlag = 0;
uint16_t          u16ArrData = 0xC350;//1MHz 50ms
uint16_t          u16InitCntData = 0xC350;

void Bt2Int(void)
{
    Bt_Stop(TIM2);
	if (TRUE == Bt_GetIntFlag(TIM2))
    {
        Bt_ClearIntFlag(TIM2);		
		u32gTimeOutFlag = 1;	
     //   u32BtTestFlag = 0x02;
    }
}
en_result_t BtTimerTest(void)
{
	/*
	stc_clk_config_t stcClkCfg;
	Clk_SetRCHFreq(ClkFreq16Mhz);
    stcClkCfg.enClkSrc  = ClkRCH;
    stcClkCfg.enHClkDiv = ClkDiv1;
    stcClkCfg.enPClkDiv = ClkDiv1;
	Clk_Init(&stcClkCfg);
	*/
	
	stc_bt_config_t   stcConfig;
    en_result_t       enResult = Error;
	
 //   uint16_t          u16ArrData = 0xC350;//1MHz 50ms
 //   uint16_t          u16InitCntData = 0xC350;

	//Clk_SetPeripheralGate(ClkPeripheralGpio, TRUE);
	Clk_SetPeripheralGate(ClkPeripheralBt, TRUE);
    
    EnableNvic(TIM2_IRQn, 3, TRUE);
    Bt_EnableIrq(TIM2);
    
    stcConfig.pfnTim2Cb = Bt2Int;
    //P25设置为门控使能IO
//    Gpio_SetFunc_GATE1_P25();
        
    stcConfig.enGateP = BtPositive;
//    stcConfig.enGate  = BtGateEnable;
	stcConfig.enGate  = BtGateDisable;
    stcConfig.enPRS   = BtPCLKDiv16;
    stcConfig.enTog   = BtTogDisable;
    stcConfig.enCT    = BtTimer;
    stcConfig.enMD    = BtMode2;
    //Bt初始化
    if (Ok != Bt_Init(TIM2, &stcConfig))
    {
        enResult = Error;
    }
    
    //设置重载值和计数值，启动计数
    Bt_ARRSet(TIM2, u16ArrData);
    Bt_Cnt16Set(TIM2, u16InitCntData);
 //   Bt_Run(TIM2);
}
#endif
