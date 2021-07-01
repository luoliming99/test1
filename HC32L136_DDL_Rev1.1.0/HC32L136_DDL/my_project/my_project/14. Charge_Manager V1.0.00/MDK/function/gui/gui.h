#ifndef __GUI_H
#define __GUI_H
#include "ddl.h"

typedef struct {
    uint8_t     title_update;           /**< 标题更新标志 */
    uint8_t     title_content[4];       /**< 标题     */
    uint8_t     item_update[3];         /**< 项目更新标志 */
    uint8_t     item_content[3][4];     /**< 项目     */
    char        uint_content[3];        /**< 单位     */
    uint8_t     para_update[3];         /**< 参数更新标志 */
    int32_t     para_content[3];        /**< 参数     */
    uint8_t     button_update[2];       /**< 按钮更新标志 */
    uint8_t     button_content[2][2];   /**< 按钮     */
} itf_info_t;

extern itf_info_t g_itf_into;

void gui_refresh (itf_info_t *p_itf_info);

void init_interface (uint8_t cursor_pos, uint8_t sw, uint8_t reflash);

void charge_test_itf (int32_t volt, int32_t curr, int32_t power, uint8_t sw, uint8_t reflash);

void lure_power_itf (int32_t volt, int32_t curr, uint8_t sw, uint8_t reflash);

#endif

