#ifndef __MY_SYSTICK_H
#define __MY_SYSTICK_H
#include "ddl.h"

extern uint32_t g_systick_cnt;  /**< �δ�ʱ������ֵ */

/**
 * \brief ϵͳʱ�ӳ�ʼ������
 */
void system_clock_init (void);

/**
 * \brief ϵͳ�δ�ʱ����ʼ��
 */
uint8_t my_systick_init (void);

/**
 * \brief ��ȡ��ǰ�δ����ֵ
 */
uint32_t my_tick_get (void);

/**
 * \brief �жϴӻ�ȡ�� t0 ʱ�������Ƿ�ʱ
 *
 * \param[in] t0       : ʱ��ο����ֵ
 * \param[in] overtime : ��ʱֵ
 *
 * \retval  TRUE    : �ѳ�ʱ
 * \retval  FALSE   : δ��ʱ
 */
boolean_t my_tick_over (uint32_t t0, uint32_t overtime);

#endif

