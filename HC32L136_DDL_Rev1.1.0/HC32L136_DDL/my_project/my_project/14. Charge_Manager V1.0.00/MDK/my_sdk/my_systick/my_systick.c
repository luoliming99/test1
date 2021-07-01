#include "sysctrl.h"
#include "flash.h"

/******************************************************************************/
void system_clock_init (void) 
{
    stc_sysctrl_clk_config_t stcCfg;
    stc_sysctrl_pll_config_t stcPLLCfg;
    
    ///< 开启FLASH外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);    
    
    ///< 因将要倍频的PLL作为系统时钟HCLK会达到48MHz：所以此处预先设置FLASH 读等待周期为1 cycle(默认值为0 cycle)
    Flash_WaitCycle(FlashWaitCycle1);
       
    ///< 时钟初始化前，优先设置要使用的时钟源
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);             //设置RCH    
    
    ///< 切换时钟前（根据外部高速晶振）设置XTH频率范围,配置晶振参数，使能目标时钟，此处为32MHz
    //Sysctrl_SetXTHFreq(SysctrlXthFreq20_32MHz);
    //Sysctrl_XTHDriverConfig(SysctrlXtalDriver3);
    //Sysctrl_SetXTHStableTime(SysctrlXthStableCycle16384);
    //Sysctrl_ClkSourceEnable(SysctrlClkXTH, TRUE);
    ///< 时钟切换
    //Sysctrl_SysClkSwitch(SysctrlClkXTH);
    ///< 根据需要选择是否关闭原时钟（此处关闭）
    //Sysctrl_ClkSourceEnable(SysctrlClkRCH, FALSE);
    
    ///< PLL配置 
    stcPLLCfg.enInFreq    = SysctrlPllInFreq4_6MHz;     //RCH 4MHz
    stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;  //PLL 输出48MHz
    stcPLLCfg.enPllClkSrc = SysctrlPllRch;              //输入时钟源选择RCH
    stcPLLCfg.enPllMul    = SysctrlPllMul12;            //4MHz x 12 = 24MHz
    Sysctrl_SetPLLFreq(&stcPLLCfg);     
    
    ///< 选择RCH作为HCLK时钟源;
    stcCfg.enClkSrc  = SysctrlClkPLL;
    ///< HCLK SYSCLK/1
    stcCfg.enHClkDiv = SysctrlHclkDiv1;
    ///< PCLK 为HCLK/1
    stcCfg.enPClkDiv = SysctrlPclkDiv1;
    ///< 系统时钟初始化
    Sysctrl_ClkInit(&stcCfg);
    /* 更新时钟变量 */
    SystemCoreClockUpdate();
}

uint32_t g_systick_cnt = 0;  /**< 滴答定时器计数值 */

void SysTick_Handler(void)
{
    g_systick_cnt++;
}

/******************************************************************************/
uint8_t my_systick_init (void)
{
    /* 初始化 SysTick Timer */
    return SysTick_Config(SystemCoreClock / 1000);    /* 定时 1ms */
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
     * 不用考虑 g_systick_cnt 溢出后小于 t0，为什么？
     */
    if (g_systick_cnt - t0 < overtime) {
        return FALSE;
    }    
    return TRUE;
}

