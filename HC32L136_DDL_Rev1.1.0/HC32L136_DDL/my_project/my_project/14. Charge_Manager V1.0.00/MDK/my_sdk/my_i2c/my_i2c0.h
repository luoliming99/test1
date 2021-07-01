#ifndef __MY_I2C0_H
#define __MY_I2C0_H
#include "ddl.h"

/**
 * \brief I2C ������ʼ������
 */
void my_i2c0_init (void);    

/**
 * \brief I2C ����д����
 *
 * \param[in] addr   : �ӻ��ļĴ�����ַ
 * \param[in] p_buf  : ��д�����ݻ����ַ
 * \param[in] length : ��д���ֽڳ���
 *
 * \retval  0 : �����ɹ�
 * \retval -1 : ����ʧ��
 */
int8_t my_i2c0_write (uint8_t addr, uint8_t *p_buf, uint16_t length);

/**
 * \brief I2C ����������
 *
 * \param[in] addr   : �ӻ��ļĴ�����ַ
 * \param[in] p_buf  : ��Ŷ�ȡ���ݵĻ����ַ
 * \param[in] length : �������ֽڳ���
 *
 * \retval  0 : �����ɹ�
 * \retval -1 : ����ʧ��
 */
int8_t my_i2c0_read (uint8_t addr, uint8_t *p_buf, uint16_t length);

#endif
