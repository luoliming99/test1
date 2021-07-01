#include "i2c.h"
#include "gpio.h"
#include "my_i2c1.h"
#include "my_uart.h"

#define __DEMOI2C         I2C1
#define __I2C_BAUD        1000000
#define __I2C_SLAVEADDR   0xD0       /**< MCP3426 */

/******************************************************************************/
void my_i2c1_init (void)
{
    stc_gpio_config_t stcGpioCfg;
    stc_i2c_config_t stcI2cCfg;
    stc_sysctrl_clk_config_t stcCfg;
    
    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c1,TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enPuPd = GpioNoPuPd;
    
    Gpio_Init(GpioPortA, GpioPin11,&stcGpioCfg);
    Gpio_Init(GpioPortA, GpioPin12,&stcGpioCfg);
    
    Gpio_SetAfMode(GpioPortA, GpioPin11,GpioAf3);//SCL
    Gpio_SetAfMode(GpioPortA, GpioPin12,GpioAf3);//SDA

    stcI2cCfg.enFunc = I2cBaud_En;
    
    uint32_t f_pclk = Sysctrl_GetPClkFreq();
    /*
     * I2C_BAUD = f_pclk / (8 * (1 + u8Tm))
     */
    stcI2cCfg.u8Tm = f_pclk / __I2C_BAUD / 8 - 1;
    stcI2cCfg.pfnI2c1Cb = NULL;
    stcI2cCfg.bTouchNvic = FALSE;
	if(TRUE == stcI2cCfg.bTouchNvic)
	{
		EnableNvic(I2C1_IRQn,IrqLevel3,TRUE);
	} 
    I2C_DeInit(__DEMOI2C); 
    I2C_Init(__DEMOI2C,&stcI2cCfg);//��ʼ��ģ��
    I2C_SetFunc(__DEMOI2C,I2cMode_En);//ģ��ʹ��
}

/******************************************************************************/
int8_t my_i2c1_write (uint8_t *p_buf, uint16_t length)
{
    /*
     * ���������ǳ�����I2C����ʱ�򣬽��������MCP3426
     */
    uint16_t u16i = 0;
    uint8_t u8State;
    uint8_t tick = 0;
    
    I2C_SetFunc(__DEMOI2C,I2cStart_En);
	while(1)
	{
        tick = 0;
		while(0 == I2C_GetIrq(__DEMOI2C))
		{
            tick++;
            if (tick > 100) {      /* ��ʱ */
                I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
                return -1;
            }
        }
		u8State = I2C_GetState(__DEMOI2C);
//        printf("%x\r\n", u8State);
		switch(u8State)
		{
			case 0x08:  /* �ѷ�����ʼ������������SLA+W������ACK */
				I2C_ClearFunc(__DEMOI2C,I2cStart_En);
				I2C_WriteByte(__DEMOI2C,__I2C_SLAVEADDR);//���豸��ַ����
				break;
			case 0x18:  /* �ѷ���SLA+W���ѽ���ACK������������ */
			case 0x28:	/* �ѷ������ݣ��ѽ���ACK���������������� */
				I2C_WriteByte(__DEMOI2C,*(p_buf+(u16i++)));
				break;
			case 0x20:  /* �ѷ���SLA+W���ѽ��ܷ�ACK */
			case 0x38:  /* ��SLA+��д��д�����ֽ�ʱ��ʧ�ٲ� */
			case 0x30:
				I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
				return -1;
			default:
				break;
		}			
		if(u16i>length)
		{
			I2C_SetFunc(__DEMOI2C,I2cStop_En);//��˳���ܵ�������ֹͣ����
			I2C_ClearIrq(__DEMOI2C);
			break;
		}
		I2C_ClearIrq(__DEMOI2C);			
	}
    return 0;
}
    
/******************************************************************************/
int8_t my_i2c1_read (uint8_t *p_buf, uint16_t length)
{
    /*
     * ���������ǳ�����I2C����ʱ�򣬽��������MCP3426
     */
    uint16_t u16i=0;
    uint8_t u8State;
    uint8_t tick = 0;
    
    I2C_SetFunc(__DEMOI2C,I2cStart_En);   
	while(1)
	{
		tick = 0;
		while(0 == I2C_GetIrq(__DEMOI2C))
		{
            tick++;
            if (tick > 100) {      /* ��ʱ */
                I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
                return -1;
            }
        }
		u8State = I2C_GetState(__DEMOI2C);
//        printf("%x\r\n", u8State);
		switch(u8State)
		{
			case 0x08:  /* �ѷ�����ʼ������������SLA+R������ACK */
				I2C_ClearFunc(__DEMOI2C,I2cStart_En);
				I2C_WriteByte(__DEMOI2C,__I2C_SLAVEADDR|0x01);
				break;
			case 0x40:  /* �ѷ���SLA+R������ACK�����������ݣ�����ACK���ACK */
				if(length>1)
				{
					I2C_SetFunc(__DEMOI2C,I2cAck_En);
				} else {
                    I2C_ClearFunc(__DEMOI2C,I2cAck_En);
                }
				break;
			case 0x50:  /* �ѽ��������ֽڣ�����ACK���������������� */
				*(p_buf + (u16i++)) = I2C_ReadByte(__DEMOI2C);
				if(u16i==length-1)
				{
					I2C_ClearFunc(__DEMOI2C,I2cAck_En);
				}
				break;
			case 0x58:  /* �ѽ��������ֽڣ����ط�ACK��������ֹͣ���� */
				*(p_buf + (u16i++)) = I2C_ReadByte(__DEMOI2C);
				I2C_SetFunc(__DEMOI2C,I2cStop_En);
				break;	
			case 0x38:  /* �ڷ�ACK�ж�ʧ�ٲ� */
			case 0x48:  /* �ѷ���SLA+R���ѽ��շ�ACK */
			default:    /* ��������״̬ */
				I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
				return -1;
		}
		I2C_ClearIrq(__DEMOI2C);
		if(u16i==length)
		{
			break;
		}
	}
	return 0;
}
