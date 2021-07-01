#include "my_uart.h"
#include "uart.h"
#include "gpio.h"
#include "my_systick.h"

uint8_t g_uart_rx_buf[UART_REC_LEN_MAX];	/**< ���ջ��� */

/* bit15 : ������ɱ�־
 * bit14 : ���յ� 0x0D
 * bit[13:0] : ���յ�����Ч�ֽ���Ŀ
 */
uint16_t g_uart_rx_sta = 0;		            /**< ���ڽ���״̬�Ĵ��� */

/********************* �������´���,֧�� printf ���� **************************/     
/**
 * \brief �ض��� fputc ����
 */ 
int fputc(int ch, FILE *f)
{      
	Uart_SendData(UARTCH1, (uint8_t)ch);
	return ch;
}
/************************************ END *************************************/

/**
 * \brief UART �����жϻص�����
 */
static void __rx_int_callback(void)
{
    uint8_t ch = Uart_ReceiveData(UARTCH1);

    if ((g_uart_rx_sta & 0x8000) == 0) {	/* ����δ��� */
            
        if (g_uart_rx_sta & 0x4000) {		/* ���յ��� 0x0D */
 
            if (ch == 0x0A) {     /* ���յ��� 0x0A */                
                /* 
                 * ��һ�����յ����ַ� 0x0D �ǻس�����һ����! 
                 * ������ɡ�
                 */
                g_uart_rx_sta |= 0x8000; 
            } else if (ch != 0x0D){  /* �Է��ٴν��յ� 0x0D */                                           
                /* 
                 * ��һ�����յ����ַ� 0x0D ����Ч�ַ�!
                 * ������յ� 0x0D ��־λ��
                 */
                g_uart_rx_sta &= 0xBFFF;
            }
        } else {							/* ��û���յ� 0x0D */
        
            if (ch == 0x0D) {
 
                g_uart_rx_sta |= 0x4000;	
            }                                        
        }
    }

    /* ������Ч�ַ� */
    g_uart_rx_buf[g_uart_rx_sta & 0x3FFF] = ch;	  
    g_uart_rx_sta++;
    if ((g_uart_rx_sta & 0x3FFF) > (UART_REC_LEN_MAX - 1)) {
        printf("���ڽ����������!\r\n");
        g_uart_rx_sta = 0;		/* ��������̫�������½��� */
    }
    
    /* ��������� */
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
        Uart_EnableIrq(UARTCH1,UartRxIrq);//ʹ�ܴ����ж� 
	}

    stcConfig.enRunMode = UartMode1;//ģʽ1
    stcConfig.enStopBit = Uart1bit;//1λֹͣλ 
    enTb8 = UartDataOrAddr;//����żУ��
    Uart_SetMMDOrCk(UARTCH1,enTb8); 
    Uart_Init(UARTCH1, &stcConfig);//���ڳ�ʼ������
    
    Uart_SetClkDiv(UARTCH1,Uart8Or16Div);//������Ƶ
    stcBaud.u32Pclk = Sysctrl_GetPClkFreq();
    stcBaud.enRunMode = UartMode1;
    stcBaud.u32Baud = BAUDRATE;
    u16Scnt = Uart_CalScnt(UARTCH1,&stcBaud);//������ֵ����
    Uart_SetBaud(UARTCH1,u16Scnt);//����������
    
    Uart_ClrStatus(UARTCH1,UartRC);//��������� 
    Uart_EnableFunc(UARTCH1,UartRx);//ʹ���շ�
}

/**
 * \brief UART ���͵����ֽ�
 *
 * \param[in] data : �����͵�����
 *
 * \retval  0 : ���ͳɹ�
 * \retval -1 : ����ʧ�� 
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



