#include "mcp3426.h"
#include "my_i2c1.h"

/******************************************************************************/
void mcp3426_init (void)
{
    uint8_t read_buf[3] = {0};
    uint8_t config_reg  =  0;
    
    /* 获取配置寄存器初始值 */
    my_i2c1_read(read_buf, 3);
    config_reg = read_buf[2];
    
    /* 设置转换模式 */
    config_reg &= ~(1 << 4);    /* 单次转换 */
    
    /* 设置采样速率 */
    config_reg |= (2 << 2);     /* */
    
    my_i2c1_write(&config_reg, 1);
    
//    printf("%x %x %x \r\n", read_buf[0], read_buf[1], read_buf[2]);
}

/******************************************************************************/
int16_t mcp3426_read_adc (uint8_t chan)
{
    uint8_t read_buf[3] = {0};
    uint8_t config_reg  =  0;
    int16_t result      =  0;
    
    /*
     * 单次转换 
     * 60 SPS（14位）
     * 增益x1
     */
    config_reg = 0x84;

    /* 选择通道 */
    config_reg |= ((chan & 0x03) << 5);
    
    /* 启动转换 */
    my_i2c1_write(&config_reg, 1);
    
    /* 读取转换结果 */
    do {
        my_i2c1_read(read_buf, 3);
    } while (read_buf[2] & 0x80);
    
    result = ((uint16_t)read_buf[0] << 8) | read_buf[1];

    return result;
}

