#ifndef __MY_I2C1_H
#define __MY_I2C1_H
#include "ddl.h"

/**
 * \brief I2C 主机初始化配置
 */
void my_i2c1_init (void);    

/**
 * \brief I2C 主机写数据
 *
 * \param[in] p_buf  : 待写的数据缓存地址
 * \param[in] length : 待写的字节长度
 *
 * \retval  0 : 操作成功
 * \retval -1 : 操作失败
 */
int8_t my_i2c1_write (uint8_t *p_buf, uint16_t length);

/**
 * \brief I2C 主机读数据
 *
 * \param[in] p_buf  : 存放读取数据的缓存地址
 * \param[in] length : 待读的字节长度
 *
 * \retval  0 : 操作成功
 * \retval -1 : 操作失败
 */
int8_t my_i2c1_read (uint8_t *p_buf, uint16_t length);

#endif

