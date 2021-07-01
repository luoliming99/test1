#include "i2c.h"
#include "gpio.h"
#include "my_i2c0.h"
#include "my_uart.h"

#define __DEMOI2C         I2C0
#define __I2C_BAUD        1000000
#define __I2C_SLAVEADDR   0x78       /**< OLED */

/******************************************************************************/
void my_i2c0_init (void)
{
    stc_gpio_config_t stcGpioCfg;
    stc_i2c_config_t stcI2cCfg;
    stc_sysctrl_clk_config_t stcCfg;
    
    DDL_ZERO_STRUCT(stcCfg);
    DDL_ZERO_STRUCT(stcI2cCfg);
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c0,TRUE);
    
    stcGpioCfg.enDir = GpioDirOut;
    stcGpioCfg.enOD = GpioOdEnable;
    stcGpioCfg.enPuPd = GpioNoPuPd;
    
    Gpio_Init(GpioPortB, GpioPin6,&stcGpioCfg);
    Gpio_Init(GpioPortB, GpioPin7,&stcGpioCfg);
    
    Gpio_SetAfMode(GpioPortB, GpioPin6,GpioAf1);//SCL
    Gpio_SetAfMode(GpioPortB, GpioPin7,GpioAf1);//SDA

    stcI2cCfg.enFunc = I2cBaud_En;
    
    uint32_t f_pclk = Sysctrl_GetPClkFreq();
    /*
     * I2C_BAUD = f_pclk / (8 * (1 + u8Tm))
     */
    stcI2cCfg.u8Tm = f_pclk / __I2C_BAUD / 8 - 1;
    stcI2cCfg.pfnI2c0Cb = NULL;
    stcI2cCfg.bTouchNvic = FALSE;
	if(TRUE == stcI2cCfg.bTouchNvic)
	{
		EnableNvic(I2C0_IRQn,IrqLevel3,TRUE);
	} 
    I2C_DeInit(__DEMOI2C); 
    I2C_Init(__DEMOI2C,&stcI2cCfg);//初始化模块
    I2C_SetFunc(__DEMOI2C,I2cMode_En);//模块使能
}

/******************************************************************************/
int8_t my_i2c0_write (uint8_t addr, uint8_t *p_buf, uint16_t length)
{
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
            if (tick > 100) {      /* 超时 */
                I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
                return -1;
            }
        }
		u8State = I2C_GetState(__DEMOI2C);
//        printf("%x\r\n", u8State);
		switch(u8State)
		{
			case 0x08:  /* 已发送起始条件，将发送SLA+W，接收ACK */
				I2C_ClearFunc(__DEMOI2C,I2cStart_En);
				I2C_WriteByte(__DEMOI2C,__I2C_SLAVEADDR);//从设备地址发送
				break;
			case 0x18:  /* 已发送SLA+W，已接受ACK，将发送数据 */
				I2C_WriteByte(__DEMOI2C,addr);//从设备内存地址发送
				break;
			case 0x28:	/* 已发送数据，已接受ACK，将继续发送数据 */
				I2C_WriteByte(__DEMOI2C,*(p_buf+(u16i++)));
				break;
			case 0x20:  /* 已发送SLA+W，已接受非ACK */
			case 0x38:  /* 在SLA+读写或写数据字节时丢失仲裁 */
			case 0x30:
				I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
				return -1;
			default:
				break;
		}			
		if(u16i>length)
		{
			I2C_SetFunc(__DEMOI2C,I2cStop_En);//此顺序不能调换，出停止条件
			I2C_ClearIrq(__DEMOI2C);
			break;
		}
		I2C_ClearIrq(__DEMOI2C);			
	}
    return 0;
}
    
/******************************************************************************/
int8_t my_i2c0_read (uint8_t addr, uint8_t *p_buf, uint16_t length)
{
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
            if (tick > 100) {      /* 超时 */
                I2C_SetFunc(__DEMOI2C,I2cStop_En);
                I2C_ClearIrq(__DEMOI2C);
                return -1;
            }
        }
		u8State = I2C_GetState(__DEMOI2C);
//        printf("%x\r\n", u8State);
		switch(u8State)
		{
			case 0x08:  /* 已发送起始条件，将发送SLA+W，接收ACK */
				I2C_ClearFunc(__DEMOI2C,I2cStart_En);
				I2C_WriteByte(__DEMOI2C,__I2C_SLAVEADDR);
				break;
			case 0x18:  /* 已发送SLA+W，已接受ACK，将发送读地址 */
				I2C_WriteByte(__DEMOI2C,addr);
				break;
			case 0x28:  /* 已发送读地址，已接受ACK，将发送重复起始条件 */
				I2C_SetFunc(__DEMOI2C,I2cStart_En);
				break;
			case 0x10:  /* 已发送重复起始条件，将发送SLA+R，接收ACK */
				I2C_ClearFunc(__DEMOI2C,I2cStart_En);
				I2C_WriteByte(__DEMOI2C,__I2C_SLAVEADDR|0x01);//从机地址发送OK
				break;
			case 0x40:  /* 已发送SLA+R，接收ACK，将接收数据，返回ACK或非ACK */
				if(length>1)
				{
					I2C_SetFunc(__DEMOI2C,I2cAck_En);
				} else {
                    I2C_ClearFunc(__DEMOI2C,I2cAck_En);
                }
				break;
			case 0x50:  /* 已接收数据字节，返回ACK，将继续接收数据 */
				*(p_buf + (u16i++)) = I2C_ReadByte(__DEMOI2C);
				if(u16i==length-1)
				{
					I2C_ClearFunc(__DEMOI2C,I2cAck_En);
				}
				break;
			case 0x58:  /* 已接收数据字节，返回非ACK，将发送停止条件 */
				*(p_buf + (u16i++)) = I2C_ReadByte(__DEMOI2C);
				I2C_SetFunc(__DEMOI2C,I2cStop_En);
				break;	
			case 0x38:  /* 在非ACK中丢失仲裁 */
			case 0x48:  /* 已发送SLA+R，已接收非ACK */
			default:    /* 其他错误状态 */
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
