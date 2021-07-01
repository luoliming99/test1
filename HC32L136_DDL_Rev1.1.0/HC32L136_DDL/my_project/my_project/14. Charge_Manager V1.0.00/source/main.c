#include "my_systick.h"
#include "my_gpio.h"
#include "my_uart.h"
#include "my_i2c0.h"
#include "my_i2c1.h"
#include "mcp3426.h"
#include "manager.h"
#include "oled.h"
#include "gui.h"

static void __system_init (void) 
{   
    system_clock_init();    /* 配置系统时钟 */   
    my_systick_init();      /* 配置滴答定时器 */   
    my_uart_init();         /* 配置基本外设 */
    
    printf("CLK_SYSCLK:%dHz\r\n", SystemCoreClock);
    printf("CLK_PCLK:%dHz\r\n", Sysctrl_GetPClkFreq());
}

#define AVER_CNT    15  

int32_t main(void)
{  
    uint8_t  keyval   =  0;
    int32_t  adc_v    =  0;
    int32_t  adc_i    =  0;
    int32_t  volt     =  0;
    int32_t  curr     =  0;
    int32_t  power    =  0;      
    uint8_t  itf_idx  =  0;   /**< 界面索引，0-初始界面，1-充电检测，2-诱骗取电 */
    uint8_t  cur_pos  =  0;   /**< 初始界面光标位置 */
    uint8_t  itf_sw   =  1;   /**< 界面切换标志 */
    uint8_t  itf_ref  =  1;   /**< 界面刷新标志 */
    uint16_t cnt      =  0;  
    
    __system_init();
    
    /* 外设初始化 */    
    all_gpio_init();
    my_i2c0_init();
    my_i2c1_init();
    
    OLED_Init();
    
    while (1) {
        
        cnt++;
        if (cnt == 10000) {
            cnt = 0;
        }
        
        //printf("%d \r\n", cnt);
        
        if ((cnt % AVER_CNT) == 0) {
            
            /*
             * 电压(mV) = adc_val(mV) * 10  
             */
            adc_v = adc_v / AVER_CNT;
            volt = ((adc_v * 10) << 11) >> 13;  /* 电压值，单位0.1mV */
            volt = volt * 10;                   /* 乘以电阻分压系数 */          
            
            volt = volt - volt / 185;           /* 校准(随输入电压变化) */
            
            volt = (volt + 5) / 10;             /* 四舍五入，单位mV */                     
            adc_v = 0;
            
            /*
             * 电流(mA) = adc_val(mV) / 50 / 0.01 
             *          = adc_val(mV) * 2 
             */
            adc_i = adc_i / AVER_CNT;
            curr = ((adc_i * 10) << 11) >> 13;  /* 电压值，单位0.1mV */
            curr = curr * 2;                    /* 电流值，单位0.1mA */ 
            
            curr = curr - volt / 416 + 2;       /* 校准(随输入电压变化) */
            if (curr > 4) {                  
                curr = curr - curr / 77 + 130;  /* 校准(随输出电流变化）*/
            }            
            //printf("curr:%d\t", curr); 
            
            curr = (curr + 5) / 10;             /* 四舍五入，单位mA */ 
            adc_i = 0;

            /*
             * 功率(mW) = 电压(mV) * 电流(mA) / 1000
             */
            power = volt * curr / 1000;
            
            itf_ref = 1;                        /* 刷新屏幕 */
        }
                
        adc_v += mcp3426_read_adc(0);        /* 读取电压通道转换值 */
        adc_i += mcp3426_read_adc(1);        /* 读取电流通道转换值 */

        keyval = key_scan();
        if (keyval == K0_PRES) {  
            switch (itf_idx) {
                case 0: 
                    cur_pos = 1 - cur_pos;
                    itf_ref = 1;            /* 刷新屏幕 */               
                    break;
                case 1:
                    break;
                case 2:
                    volt_inc_200mv();   
                    break;                    
            }
        } else if (keyval == K1_PRES) {
            switch (itf_idx) {
                case 0:
                    if (cur_pos == 0) {     /* 充电检测 */
                        usb_dp_od_in_enable();
                        usb_dn_od_in_enable();
                        itf_idx = 1;        /* 切换界面 */
                    } else {                /* 诱骗取电 */
                        charge_mode_select(qc3_0);
                        itf_idx = 2;        /* 切换界面 */
                    }                   
                    itf_sw = 1;
                    itf_ref = 1;                    
                    break;
                case 1:
                    break;
                case 2:
                    volt_dec_200mv();
                    break;                   
            }
        } else if (keyval == K0_PRES_L) {
            switch (itf_idx) {
                case 0:
                    break;
                case 1:
                    itf_idx = 0;        /* 返回初始界面 */
                    itf_sw = 1; 
                    itf_ref = 1;
                    break;
                case 2:
                    usb_dp_od_in_enable();
                    usb_dn_od_in_enable();
                    itf_idx = 0;        /* 返回初始界面 */
                    itf_sw = 1; 
                    itf_ref = 1;
                    break;
            }
        }
        
        /* GUI刷新 */
        switch (itf_idx) {
            case 0: /* 初始界面 */
                init_interface(cur_pos, itf_sw, itf_ref);
                itf_sw = 0;
                itf_ref = 0;
                break;
            case 1: /* 充电检测 */
                charge_test_itf(volt, curr, power, itf_sw, itf_ref);
                itf_sw = 0;
                itf_ref = 0;
                break;
            case 2: /* 诱骗取电 */
                lure_power_itf(volt, curr, itf_sw, itf_ref);
                itf_sw = 0;
                itf_ref = 0;
                break;            
        }
    }
}







