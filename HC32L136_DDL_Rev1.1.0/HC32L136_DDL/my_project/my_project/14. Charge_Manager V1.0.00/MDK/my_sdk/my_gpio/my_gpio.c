#include "my_gpio.h"
#include "my_systick.h"

/******************************************************************************/
void all_gpio_init (void)
{
    stc_gpio_config_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   
    stcGpioCfg.enDir = GpioDirIn;
    stcGpioCfg.enPuPd = GpioPu;
    stcGpioCfg.enCtrlMode = GpioAHB;
    
    /* 按键 */
    Gpio_Init(GpioPortB, GpioPin11, &stcGpioCfg);
    Gpio_Init(GpioPortB, GpioPin1, &stcGpioCfg);
    
    /* 控制USB电平的4个GPIO */
    usb_dp_od_in_enable();
    usb_dn_od_in_enable();
}

/******************************************************************************/
void usb_dp_od_in_enable (void)
{
    stc_gpio_config_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     
    
    stcGpioCfg.enDir = GpioDirIn;                               
    stcGpioCfg.enPuPd = GpioNoPuPd;                             
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enCtrlMode = GpioAHB;
    
    Gpio_Init(GpioPortA, GpioPin15, &stcGpioCfg);   /* V1 */
    Gpio_Init(GpioPortB, GpioPin3, &stcGpioCfg);    /* V2 */
}   

/******************************************************************************/
void usb_dn_od_in_enable (void)
{
    stc_gpio_config_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     
    
    stcGpioCfg.enDir = GpioDirIn;                               
    stcGpioCfg.enPuPd = GpioNoPuPd;                             
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enCtrlMode = GpioAHB;
    
    Gpio_Init(GpioPortB, GpioPin4, &stcGpioCfg);    /* V3 */
    Gpio_Init(GpioPortB, GpioPin5, &stcGpioCfg);    /* V4 */
}  

/******************************************************************************/
void usb_dp_pp_out_enable (uint8_t v1, uint8_t v2)
{
    stc_gpio_config_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     
    
    stcGpioCfg.enDir = GpioDirOut; 
    stcGpioCfg.enPuPd = GpioNoPuPd;
    stcGpioCfg.enDrv = GpioDrvH;    
    stcGpioCfg.enCtrlMode = GpioAHB;
   
    /* 设置I/O模式及初始电平 */
    Gpio_Init(GpioPortA, GpioPin15, &stcGpioCfg);   /* V1 */
    Gpio_Init(GpioPortB, GpioPin3, &stcGpioCfg);    /* V2 */
    usb_dp(v1, v2);
}

/******************************************************************************/
void usb_dn_pp_out_enable (uint8_t v4, uint8_t v3)
{   
    stc_gpio_config_t stcGpioCfg;
        
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     
    
    stcGpioCfg.enDir = GpioDirOut; 
    stcGpioCfg.enPuPd = GpioNoPuPd;
    stcGpioCfg.enDrv = GpioDrvH;    
    stcGpioCfg.enCtrlMode = GpioAHB;

    /* 设置I/O模式及初始电平 */
    Gpio_Init(GpioPortB, GpioPin4, &stcGpioCfg);    /* V3 */
    Gpio_Init(GpioPortB, GpioPin5, &stcGpioCfg);    /* V4 */
    usb_dn(v4, v3);   
}

/******************************************************************************/
void usb_dp (uint8_t v1, uint8_t v2) 
{
    v1 == 0 ? Gpio_ClrIO(GpioPortA, GpioPin15) : Gpio_SetIO(GpioPortA, GpioPin15);
    v2 == 0 ? Gpio_ClrIO(GpioPortB, GpioPin3)  : Gpio_SetIO(GpioPortB, GpioPin3);
}

/******************************************************************************/
void usb_dn (uint8_t v4, uint8_t v3) 
{
    v4 == 0 ? Gpio_ClrIO(GpioPortB, GpioPin5) : Gpio_SetIO(GpioPortB, GpioPin5);
    v3 == 0 ? Gpio_ClrIO(GpioPortB, GpioPin4) : Gpio_SetIO(GpioPortB, GpioPin4);
}

/******************************************************************************/
uint8_t key_scan (void)
{
    uint32_t cnt = 0;
    
    if (K0_VAL == 0) {
        delay1ms(20);           /* 延时消抖 */
        if (K0_VAL == 0) {
            do {
                if (K0_VAL == 1) {
                    return K0_PRES;
                }
                delay1ms(10);
                cnt++;
            } while (cnt < 100);
            return K0_PRES_L;
        }
    } else if (K1_VAL == 0) {
        delay1ms(20);           /* 延时消抖 */
        if (K1_VAL == 0) {
            do {
                if (K1_VAL == 1) {
                    return K1_PRES;
                }
                delay1ms(10);
                cnt++;
            } while (cnt < 100);
            return K1_PRES_L;
        }
    } 
    return NO_KEY_PRES;
}

