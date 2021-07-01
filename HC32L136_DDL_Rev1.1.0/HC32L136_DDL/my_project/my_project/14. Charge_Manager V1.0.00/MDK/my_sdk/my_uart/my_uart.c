#include "my_uart.h"
#include "uart.h"
#include "gpio.h"
#include "my_systick.h"

uint8_t g_uart_rx_buf[UART_REC_LEN_MAX];	/**< 接收缓存 */

/* bit15 : 接收完成标志
 * bit14 : 接收到 0x0D
 * bit[13:0] : 接收到的有效字节数目
 */
uint16_t g_uart_rx_sta = 0;		            /**< 串口接收状态寄存器 */

/********************* 加入以下代码,支持 printf 函数 **************************/     
/**
 * \brief 重定义 fputc 函数
 */ 
int fputc(int ch, FILE *f)
{      
	Uart_SendData(UARTCH1, (uint8_t)ch);
	return ch;
}
/************************************ END *************************************/

/**
 * \brief UART 接收中断回调函数
 */
static void __rx_int_callback(void)
{
    uint8_t ch = Uart_ReceiveData(UARTCH1);

    if ((g_uart_rx_sta & 0x8000) == 0) {	/* 接收未完成 */
            
        if (g_uart_rx_sta & 0x4000) {		/* 接收到了 0x0D */
 
            if (ch == 0x0A) {     /* 接收到了 0x0A */                
                /* 
                 * 上一个接收到的字符 0x0D 是回车符的一部分! 
                 * 接收完成。
                 */
                g_uart_rx_sta |= 0x8000; 
            } else if (ch != 0x0D){  /* 以防再次接收到 0x0D */                                           
                /* 
                 * 上一个接收到的字符 0x0D 是有效字符!
                 * 清除接收到 0x0D 标志位。
                 */
                g_uart_rx_sta &= 0xBFFF;
            }
        } else {							/* 还没接收到 0x0D */
        
            if (ch == 0x0D) {
 
                g_uart_rx_sta |= 0x4000;	
            }                                        
        }
    }

    /* 接收有效字符 */
    g_uart_rx_buf[g_uart_rx_sta & 0x3FFF] = ch;	  
    g_uart_rx_sta++;
    if ((g_uart_rx_sta & 0x3FFF) > (UART_REC_LEN_MAX - 1)) {
        printf("串口接收数据溢出!\r\n");
        g_uart_rx_sta = 0;		/* 接收数据太长，重新接收 */
    }
    
    /* 加入结束符 */
    if (g_uart_rx_sta & UART_REC_OK) {
            
        g_uart_rx_buf[(g_uart_rx_sta & 0x3FFF)] = '\0';
    }
}

/******************************************************************************/
void my_uart_init (void)
{
    uint16_t u16Scnt = 0;
    stc_gpio_config_t stcGpioCfg;    
    stc_uart_config_t  stcConfig;
    stc_uart_irq_cb_t stcUartIrqCb;
    stc_uart_baud_t stcBaud;
    en_uart_mmdorck_t enTb8;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcBaud);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortD,GpioPin0,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortD,GpioPin0,GpioAf3);//TX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortD,GpioPin1,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortD,GpioPin1,GpioAf3);//RX
    
    stcUartIrqCb.pfnRxIrqCb   = __rx_int_callback;
    stcUartIrqCb.pfnTxIrqCb   = NULL;
    stcUartIrqCb.pfnRxFEIrqCb = NULL;
    stcUartIrqCb.pfnPEIrqCb   = NULL;
    stcUartIrqCb.pfnCtsIrqCb  = NULL;
    stcConfig.pstcIrqCb = &stcUartIrqCb;
    stcConfig.bTouchNvic = TRUE;
  	if(TRUE == stcConfig.bTouchNvic)
	{
		EnableNvic(UART1_IRQn,IrqLevel3,TRUE);
        Uart_EnableIrq(UARTCH1,UartRxIrq);//使能串口中断 
	}

    stcConfig.enRunMode = UartMode1;//模式1
    stcConfig.enStopBit = Uart1bit;//1位停止位 
    enTb8 = UartDataOrAddr;//无奇偶校验
    Uart_SetMMDOrCk(UARTCH1,enTb8); 
    Uart_Init(UARTCH1, &stcConfig);//串口初始化函数
    
    Uart_SetClkDiv(UARTCH1,Uart8Or16Div);//采样分频
    stcBaud.u32Pclk = Sysctrl_GetPClkFreq();
    stcBaud.enRunMode = UartMode1;
    stcBaud.u32Baud = BAUDRATE;
    u16Scnt = Uart_CalScnt(UARTCH1,&stcBaud);//波特率值计算
    Uart_SetBaud(UARTCH1,u16Scnt);//波特率设置
    
    Uart_ClrStatus(UARTCH1,UartRC);//清接收请求 
    Uart_EnableFunc(UARTCH1,UartRx);//使能收发
}

/**
 * \brief UART 发送单个字节
 *
 * \param[in] data : 待发送的数据
 *
 * \retval  0 : 发送成功
 * \retval -1 : 发送失败 
 */
static int8_t __uart_send_one_byte (uint8_t data)
{
    if (Ok != Uart_SendData(UARTCH1, data)) {
        return -1;
    }
    return 0;
}

/******************************************************************************/
int8_t uart_send_data (uint8_t *p_buf, uint8_t len)
{
    uint8_t  i    = 0;
    uint32_t tick = 0;
    
    for (i = 0; i < len; i++) {
        
        tick = my_tick_get();
        while (1) {
            if (__uart_send_one_byte(*(p_buf + i)) == 0) {
                break;
            }
            if (my_tick_over(tick, 10)) {
                printf("__uart_send_one_byte tmo!\r\n");
                return -1;
            }
        }   
    }
    return 0;
}

/******************************************************************************/
int8_t uart_send_string (char *p_buf)
{
	uint8_t  i    = 0;
    uint32_t tick = 0;
	
	while (*(p_buf + i) != '\0') {
	
		tick = my_tick_get();
        while (1) {
            if (__uart_send_one_byte(*(p_buf + i)) == 0) {
                break;
            }
            if (my_tick_over(tick, 10)) {
                printf("__uart_send_one_byte tmo!\r\n");
                return -1;
            }
        } 
		i++;
	}
	return 0;
}



