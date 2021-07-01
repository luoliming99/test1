#include "manager.h"
#include "my_gpio.h"

/******************************************************************************/
void charge_mode_select (charge_mode_t mode)
{
    usb_dp_pp_out_enable(0, 1);     /* VD+ = 0.6V */  
    
    for (int i = 0; i < 700; i++) { /* 延时1.5s，等待D-电压下降 */
        for (int j = 0; j < 10000; j++) {;}
    }        

    usb_dn_pp_out_enable(1, 1);     /* VD- = 3.3V, 进入QC3.0模式 */

    /*
     * out_5v:  usb_dp(0, 1); usb_dn(0, 0); VD+ = 0.6V  VD- = 0V   
     * out_9v:  usb_dp(1, 1); usb_dn(0, 1); VD+ = 3.3V  VD- = 0.6V 
     * out_12v: usb_dp(0, 1); usb_dn(0, 1); VD+ = 0.6V  VD- = 0.6V 
     * out_20v: usb_dp(1, 1); usb_dn(1, 1); VD+ = 3.3V  VD- = 3.3V 
     * qc3_0:   usb_dp(0, 1); usb_dn(1, 1); VD+ = 0.6V  VD- = 3.3V  
     */   
}

/******************************************************************************/
void volt_inc_200mv (void) 
{
    usb_dp(1, 1);       /* VD+ = 3.3V */
    delay10us(25);
    usb_dp(0, 1);       /* VD+ = 0.6V */
    delay100us(27);
}

/******************************************************************************/
void volt_dec_200mv (void)
{
    usb_dn(0, 1);       /* VD- = 0.6V */
    delay10us(25);
    usb_dn(1, 1);       /* VD- = 3.3V */
    delay100us(27);
}

