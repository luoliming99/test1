#include "gui.h"
#include "oled.h"

itf_info_t g_itf_into;

/******************************************************************************/
void gui_refresh (itf_info_t *p_itf_info)
{
    /* 写标题 */
    if (p_itf_info->title_update == 1) {
        p_itf_info->title_update = 0;
        OLED_ShowChinese(32,0,p_itf_info->title_content[0],16);  
        OLED_ShowChinese(48,0,p_itf_info->title_content[1],16);   
        OLED_ShowChinese(64,0,p_itf_info->title_content[2],16);  
        OLED_ShowChinese(80,0,p_itf_info->title_content[3],16); 
    }

    /* 写项目、单位 */
    if (p_itf_info->item_update[0] == 1) {
        p_itf_info->item_update[0] = 0;
        for (int i = 0; i < 4; i++) {
            if (p_itf_info->item_content[0][i] != 0) {
                OLED_ShowChinese(16*i,16,p_itf_info->item_content[0][i],16);
            } else {
                OLED_ShowChar(16*i,16,':',16);
                break;
            }                
        }       
        OLED_ShowChar(112,16,p_itf_info->uint_content[0],16);  
    }
    
    if (p_itf_info->item_update[1] == 1) {
        p_itf_info->item_update[1] = 0;
        for (int i = 0; i < 4; i++) {
            if (p_itf_info->item_content[1][i] != 0) {
                OLED_ShowChinese(16*i,32,p_itf_info->item_content[1][i],16);
            } else {
                OLED_ShowChar(16*i,32,':',16);
                break;
            }                
        }       
        OLED_ShowChar(112,32,p_itf_info->uint_content[1],16);  
    }
    
    if (p_itf_info->item_update[2] == 1) {
        p_itf_info->item_update[2] = 0;
        for (int i = 0; i < 4; i++) {
            if (p_itf_info->item_content[2][i] != 0) {
                OLED_ShowChinese(16*i,48,p_itf_info->item_content[2][i],16);
            } else {
                OLED_ShowChar(16*i,48,':',16);
                break;
            }                
        }       
        OLED_ShowChar(112,48,p_itf_info->uint_content[2],16);  
    }
    
    /* 写参数 */
    if (p_itf_info->para_update[0] == 1) {
        p_itf_info->para_update[0] = 0;
        if (p_itf_info->para_content[0] < 0) {
            OLED_ShowChar(40,16,'-',16);
        }
        OLED_ShowNum(48,16,p_itf_info->para_content[0]/1000,2,16);
        OLED_ShowChar(64,16,'.',16);
        OLED_ShowNum(72,16,p_itf_info->para_content[0]%1000/100,1,16);
        OLED_ShowNum(80,16,p_itf_info->para_content[0]%100/10,1,16);
        OLED_ShowNum(88,16,p_itf_info->para_content[0]%10,1,16);
    }
    
    if (p_itf_info->para_update[1] == 1) {
        p_itf_info->para_update[1] = 0;
        if (p_itf_info->para_content[1] < 0) {
            OLED_ShowChar(40,32,'-',16);
        }
        OLED_ShowNum(48,32,p_itf_info->para_content[1]/1000,2,16);
        OLED_ShowChar(64,32,'.',16);
        OLED_ShowNum(72,32,p_itf_info->para_content[1]%1000/100,1,16);
        OLED_ShowNum(80,32,p_itf_info->para_content[1]%100/10,1,16);
        OLED_ShowNum(88,32,p_itf_info->para_content[1]%10,1,16);
    }
    
    if (p_itf_info->para_update[2] == 1) {
        p_itf_info->para_update[2] = 0;
        if (p_itf_info->para_content[2] < 0) {
            OLED_ShowChar(40,48,'-',16);
        }
        OLED_ShowNum(48,48,p_itf_info->para_content[2]/1000,2,16);
        OLED_ShowChar(64,48,'.',16);
        OLED_ShowNum(72,48,p_itf_info->para_content[2]%1000/100,1,16);
        OLED_ShowNum(80,48,p_itf_info->para_content[2]%100/10,1,16);
        OLED_ShowNum(88,48,p_itf_info->para_content[2]%10,1,16);
    }
    
    /* 写按钮 */
    if (p_itf_info->button_update[0] == 1) {
        p_itf_info->button_update[0] = 0;
        OLED_ShowChinese(0,48,p_itf_info->button_content[0][0],16); 
        OLED_ShowChinese(16,48,p_itf_info->button_content[0][1],16);
    }
    
    if (p_itf_info->button_update[1] == 1) {
        p_itf_info->button_update[1] = 0;
        OLED_ShowChinese(96,48,p_itf_info->button_content[1][0],16); 
        OLED_ShowChinese(112,48,p_itf_info->button_content[1][1],16);
    }
    
    OLED_Refresh();
}

/******************************************************************************/
void init_interface (uint8_t cursor_pos, uint8_t sw, uint8_t reflash)
{
    if (reflash == 0) {
        return;
    }
    
    if (sw) {
        OLED_Clear();
    }
    
    g_itf_into.title_content[0] = 1;      /* 功 */
    g_itf_into.title_content[1] = 2;      /* 能 */
    g_itf_into.title_content[2] = 3;      /* 选 */
    g_itf_into.title_content[3] = 4;      /* 择 */
    g_itf_into.title_update     = sw;  
    
    if (cursor_pos == 0) {
        g_itf_into.uint_content[0]    = '<';  /* 光标 */
        g_itf_into.uint_content[1]    = ' ';
    } else if (cursor_pos == 1) {
        g_itf_into.uint_content[0]    = ' ';  
        g_itf_into.uint_content[1]    = '<';  /* 光标 */
    }    
    g_itf_into.item_content[0][0] = 5;    /* 电 */
    g_itf_into.item_content[0][1] = 6;    /* 压 */
    g_itf_into.item_content[0][2] = 7;    /* 检 */
    g_itf_into.item_content[0][3] = 8;    /* 测 */
    g_itf_into.item_update[0]     = 1;
    
    g_itf_into.item_content[1][0] = 9;    /* 诱 */
    g_itf_into.item_content[1][1] = 10;   /* 骗 */
    g_itf_into.item_content[1][2] = 11;   /* 取 */
    g_itf_into.item_content[1][3] = 12;   /* 电 */
    g_itf_into.item_update[1]     = 1;
    
    g_itf_into.button_content[0][0] = 19; /* 选 */
    g_itf_into.button_content[0][1] = 20; /* 择 */
    g_itf_into.button_update[0]     = sw;
    g_itf_into.button_content[1][0] = 21; /* 进 */
    g_itf_into.button_content[1][1] = 22; /* 入 */
    g_itf_into.button_update[1]     = sw;
    
    gui_refresh(&g_itf_into);
}

/******************************************************************************/
void charge_test_itf (int32_t volt, int32_t curr, int32_t power, uint8_t sw, uint8_t reflash)
{
    if (reflash == 0) {
        return;
    }
    
    if (sw) {
        OLED_Clear();
    }
    
    g_itf_into.title_content[0] = 5;      /* 充 */
    g_itf_into.title_content[1] = 6;      /* 电 */
    g_itf_into.title_content[2] = 7;      /* 检 */
    g_itf_into.title_content[3] = 8;      /* 测 */
    g_itf_into.title_update     = sw;  
    
    g_itf_into.item_content[0][0] = 13;   /* 电 */
    g_itf_into.item_content[0][1] = 14;   /* 压 */
    g_itf_into.item_content[0][2] = 0;
    g_itf_into.item_content[0][3] = 0;
    g_itf_into.uint_content[0]    = 'V';  /* V  */
    g_itf_into.item_update[0]     = sw;
    
    g_itf_into.item_content[1][0] = 15;   /* 电 */
    g_itf_into.item_content[1][1] = 16;   /* 流 */
    g_itf_into.item_content[1][2] = 0;
    g_itf_into.item_content[1][3] = 0;
    g_itf_into.uint_content[1]    = 'A';  /* A  */
    g_itf_into.item_update[1]     = sw;
    
    g_itf_into.item_content[2][0] = 17;   /* 功 */
    g_itf_into.item_content[2][1] = 18;   /* 率 */
    g_itf_into.item_content[2][2] = 0;
    g_itf_into.item_content[2][3] = 0;
    g_itf_into.uint_content[2]    = 'W';  /* W  */
    g_itf_into.item_update[2]     = sw;
    
    g_itf_into.para_content[0] = volt;
    g_itf_into.para_update[0]  = 1;
    
    g_itf_into.para_content[1] = curr;
    g_itf_into.para_update[1]  = 1;
    
    g_itf_into.para_content[2] = power;
    g_itf_into.para_update[2]  = 1;
    
    gui_refresh(&g_itf_into);
}

/******************************************************************************/
void lure_power_itf (int32_t volt, int32_t curr, uint8_t sw, uint8_t reflash)
{
    if (reflash == 0) {
        return;
    }
    
    if (sw) {
        OLED_Clear();
    }
    
    g_itf_into.title_content[0] = 9;      /* 诱 */
    g_itf_into.title_content[1] = 10;     /* 骗 */
    g_itf_into.title_content[2] = 11;     /* 取 */
    g_itf_into.title_content[3] = 12;     /* 电 */
    g_itf_into.title_update     = sw;  
    
    g_itf_into.item_content[0][0] = 13;   /* 电 */
    g_itf_into.item_content[0][1] = 14;   /* 压 */
    g_itf_into.item_content[0][2] = 0;
    g_itf_into.item_content[0][3] = 0;
    g_itf_into.uint_content[0]    = 'V';  /* V  */
    g_itf_into.item_update[0]     = sw;
    
    g_itf_into.item_content[1][0] = 15;   /* 电 */
    g_itf_into.item_content[1][1] = 16;   /* 流 */
    g_itf_into.item_content[1][2] = 0;
    g_itf_into.item_content[1][3] = 0;
    g_itf_into.uint_content[1]    = 'A';  /* A  */
    g_itf_into.item_update[1]     = sw;
    
    g_itf_into.button_content[0][0] = 23; /* 加 */
    g_itf_into.button_content[0][1] = 0;  /*    */
    g_itf_into.button_update[0]     = sw;
    g_itf_into.button_content[1][0] = 0;  /*    */
    g_itf_into.button_content[1][1] = 24; /* 减 */
    g_itf_into.button_update[1]     = sw;
    
    g_itf_into.para_content[0] = volt;
    g_itf_into.para_update[0]  = 1;
    
    g_itf_into.para_content[1] = curr;
    g_itf_into.para_update[1]  = 1;
   
    gui_refresh(&g_itf_into);
}

