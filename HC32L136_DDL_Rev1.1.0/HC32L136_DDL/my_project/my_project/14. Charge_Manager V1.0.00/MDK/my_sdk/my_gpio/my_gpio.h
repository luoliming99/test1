#ifndef __MY_GPIO_H
#define __MY_GPIO_H
#include "gpio.h"

#define K0_VAL          Gpio_GetInputIO(GpioPortB, GpioPin11)  
#define K1_VAL          Gpio_GetInputIO(GpioPortB, GpioPin1)
                        
#define K0_PRES         1   /**< \brief K0 ���� */
#define K0_PRES_L       2   /**< \bried K0����  */
#define K1_PRES         3   /**< \brief K1 ���� */ 
#define K1_PRES_L       4   /**< \brief K1����  */
#define NO_KEY_PRES     0   /**< \brief �ް������� */

/**
 * \brief GPIO��ʼ������
 */
void all_gpio_init (void);

void usb_dp_od_in_enable (void);

void usb_dn_od_in_enable (void);

void usb_dp_pp_out_enable (uint8_t v1, uint8_t v2);

void usb_dn_pp_out_enable (uint8_t v4, uint8_t v3);

void usb_dp (uint8_t v1, uint8_t v2);

void usb_dn (uint8_t v4, uint8_t v3);

/**
 * \brief ����ɨ��
 *
 * \param[in] mode : 0 - ��֧������; 1 - ֧������
 *
 * \return  ����ֵ
 */
uint8_t key_scan (void);

#endif
