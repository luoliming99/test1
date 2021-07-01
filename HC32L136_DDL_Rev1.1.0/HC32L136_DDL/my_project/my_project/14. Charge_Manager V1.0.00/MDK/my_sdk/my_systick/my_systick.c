#include "sysctrl.h"
#include "flash.h"

/******************************************************************************/
void system_clock_init (void) 
{
    stc_sysctrl_clk_config_t stcCfg;
    stc_sysctrl_pll_config_t stcPLLCfg;
    
    ///< ����FLASH����ʱ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);    
    
    ///< ��Ҫ��Ƶ��PLL��Ϊϵͳʱ��HCLK��ﵽ48MHz�����Դ˴�Ԥ������FLASH ���ȴ�����Ϊ1 cycle(Ĭ��ֵΪ0 cycle)
    Flash_WaitCycle(FlashWaitCycle1);
       
    ///< ʱ�ӳ�ʼ��ǰ����������Ҫʹ�õ�ʱ��Դ
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);             //����RCH    
    
    ///< �л�ʱ��ǰ�������ⲿ���پ�������XTHƵ�ʷ�Χ,���þ��������ʹ��Ŀ��ʱ�ӣ��˴�Ϊ32MHz
    //Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);
    //Sysctrl_XTHDriverConfig(SysctrlXtalDriver3);
    //Sysctrl_SetXTHStableTime(SysctrlXthStableCycle16384);
    //Sysctrl_ClkSourceEnable(SysctrlClkXTH, TRUE);
    ///< ʱ���л�
    //Sysctrl_SysClkSwitch(SysctrlClkXTH);
    ///< ������Ҫѡ���Ƿ�ر�ԭʱ�ӣ��˴��رգ�
    //Sysctrl_ClkSourceEnable(SysctrlClkRCH, FALSE);
    
    ///< PLL���� 
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL ���48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //����ʱ��Դѡ��RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 24MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);     
    
    ///< ѡ��RCH��ΪHCLKʱ��Դ;
    stcCfg.enClkSrc  = SysctrlClkPLL;
    ///< HCLK SYSCLK/1
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    ///< PCLK ΪHCLK/1
    stcCfg.enPClkDiv = SysctrlPclkDiv1;
    ///< ϵͳʱ�ӳ�ʼ��
    Sysctrl_ClkInit(&stcCfg);
    /* ����ʱ�ӱ��� */
    SystemCoreClockUpdate();
}

uint32_t g_systick_cnt = 0;  /**< �δ�ʱ������ֵ */

void SysTick_Handler(void)
{
    g_systick_cnt++;
}

/******************************************************************************/
uint8_t my_systick_init (void)
{
    /* ��ʼ�� SysTick Timer */
    return SysTick_Config(SystemCoreClock / 1000);    /* ��ʱ 1ms */
}

/******************************************************************************/
uint32_t my_tick_get (void)
{
    return g_systick_cnt;
}

/******************************************************************************/
boolean_t my_tick_over (uint32_t t0, uint32_t overtime)
{
    /*
     * ���ÿ��� g_systick_cnt �����С�� t0��Ϊʲô��
     */
    if (g_systick_cnt - t0 < overtime) {
        return FALSE;
    }    
    return TRUE;
}

