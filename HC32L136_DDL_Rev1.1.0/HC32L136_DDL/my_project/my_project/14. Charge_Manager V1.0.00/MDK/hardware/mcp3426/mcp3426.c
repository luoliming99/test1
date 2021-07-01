#include "mcp3426.h"
#include "my_i2c1.h"

/******************************************************************************/
void mcp3426_init (void)
{
    uint8_t read_buf[3] = {0};
    uint8_t config_reg  =  0;
    
    /* ��ȡ���üĴ�����ʼֵ */
    my_i2c1_read(read_buf, 3);
    config_reg = read_buf[2];
    
    /* ����ת��ģʽ */
    config_reg &= ~(1 << 4);    /* ����ת�� */
    
    /* ���ò������� */
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
     * ����ת�� 
     * 60 SPS��14λ��
     * ����x1
     */
    config_reg = 0x84;

    /* ѡ��ͨ�� */
    config_reg |= ((chan & 0x03) << 5);
    
    /* ����ת�� */
    my_i2c1_write(&config_reg, 1);
    
    /* ��ȡת����� */
    do {
        my_i2c1_read(read_buf, 3);
    } while (read_buf[2] & 0x80);
    
    result = ((uint16_t)read_buf[0] << 8) | read_buf[1];

    return result;
}

