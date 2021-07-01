#ifndef __MY_SYSTICK_H
#define __MY_SYSTICK_H
#include "ddl.h"

extern uint32_t g_systick_cnt;  /**< 滴答定时器计数值 */

/**
 * \brief 系统时钟初始化函数
 */
void system_clock_init (void);

/**
 * \brief 系统滴答定时器初始化
 */
uint8_t my_systick_init (void);

/**
 * \brief 获取当前滴答计数值
 */
uint32_t my_tick_get (void);

/**
 * \brief 判断从获取到 t0 时到现在是否超时
 *
 * \param[in] t0       : 时间参考起点值
 * \param[in] overtime : 超时值
 *
 * \retval  TRUE    : 已超时
 * \retval  FALSE   : 未超时
 */
boolean_t my_tick_over (uint32_t t0, uint32_t overtime);

#endif

