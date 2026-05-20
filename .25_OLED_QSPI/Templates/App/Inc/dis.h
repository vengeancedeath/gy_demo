#ifndef __DIS_H
#define __DIS_H
#include <stdint.h>
void sight_clear();      // 图标清零
void sight_replace();    // 图标设置
void clear_rect_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // 清除指定矩形区域
void gImage_clear(void); // 桌面清零
void Dis_Bril_Set(uint8_t Level);
void Dis_Play(void);
void Dis_Play_Logic(void); // 显示逻辑
void Image_Init(void);     // 初始化
void Num_Add(uint16_t x, uint16_t y, uint8_t num,uint8_t t_flag);
void Range_dis(void);
void Angle_dis(void);
void Range_disp_Y();
void Logo_Show(void);
void Logo_Data(uint16_t xl, uint16_t xr, uint16_t yt, uint16_t yd);
void dis_menu(void);
void dis_arrow(void);
void clear_arrow(void);
typedef struct
{
    uint8_t gImage_img[480][640]; // 桌面
    uint8_t Bril;                 // 亮度
    uint8_t Bril2;                // 档位亮度
    uint16_t sight_X;             // 准星X
    uint16_t sight_Y;             // 准星Y
    uint8_t type;                 // 显示类型

} S_dis;

typedef struct
{
    uint8_t pos_arrow;
    uint8_t last_pos;
    uint8_t pos_min;
    uint8_t pos_max;
} MENU_STATE;

volatile extern S_dis dis;
extern volatile uint32_t Random;
#endif