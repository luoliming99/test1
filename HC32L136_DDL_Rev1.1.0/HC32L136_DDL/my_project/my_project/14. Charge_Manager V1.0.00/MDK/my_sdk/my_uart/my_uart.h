#ifndef __MY_UART_H
#define __MY_UART_H
#include "ddl.h"

/**
 * \name UART ���ղ�������
 * @{
 */
 
#define BAUDRATE            115200

#define UART_REC_LEN_MAX	100		/**< �������ֽڳ��� */
#define UART_REC_OK			0x8000	/**< ���յ��س� */

/** @} */

extern uint8_t g_uart_rx_buf[];		/**< ���ջ��� */

/* bit15 : ������ɱ�־
 * bit14 : ���յ� 0x0D
 * bit[13:0] : ���յ�����Ч�ֽ���Ŀ
 */
extern uint16_t g_uart_rx_sta;		/**< ���ڽ���״̬�Ĵ��� */

/**
 * \brief UART ��ʼ������
 */
void my_uart_init (void);

/**
 * \brief UART ��������
 *
 * \param[in] p_buf : ���������ݵ�ַ
 * \param[in] len   : ���������ݳ���
 *
 * \retval  0 : ���ͳɹ�
 * \retval -1 : ����ʧ�� 
 */
int8_t uart_send_data (uint8_t *p_buf, uint8_t len);

/**
 * \brief UART �����ַ���
 *
 * \param[in] p_buf : �������ַ�����ַ
 *
 * \retval  0 : ���ͳɹ�
 * \retval -1 : ����ʧ�� 
 */
int8_t uart_send_string (char *p_buf);

#endif
