#ifndef __MY_UART_H
#define __MY_UART_H
#include "ddl.h"

/**
 * \name UART 接收参数设置
 * @{
 */
 
#define BAUDRATE            115200

#define UART_REC_LEN_MAX	100		/**< 最大接收字节长度 */
#define UART_REC_OK			0x8000	/**< 接收到回车 */

/** @} */

extern uint8_t g_uart_rx_buf[];		/**< 接收缓存 */

/* bit15 : 接收完成标志
 * bit14 : 接收到 0x0D
 * bit[13:0] : 接收到的有效字节数目
 */
extern uint16_t g_uart_rx_sta;		/**< 串口接收状态寄存器 */

/**
 * \brief UART 初始化函数
 */
void my_uart_init (void);

/**
 * \brief UART 发送数据
 *
 * \param[in] p_buf : 待发送数据地址
 * \param[in] len   : 待发送数据长度
 *
 * \retval  0 : 发送成功
 * \retval -1 : 发送失败 
 */
int8_t uart_send_data (uint8_t *p_buf, uint8_t len);

/**
 * \brief UART 发送字符串
 *
 * \param[in] p_buf : 待发送字符串地址
 *
 * \retval  0 : 发送成功
 * \retval -1 : 发送失败 
 */
int8_t uart_send_string (char *p_buf);

#endif
