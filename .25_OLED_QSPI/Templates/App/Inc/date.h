#ifndef __DATE_H__
#define __DATE_H__
#include "main.h"

typedef enum
{
  Power_OFF,
  Power_ON,
  Power_SLEEP,

} Power_Struct;

typedef struct
{
  Power_Struct power;

  uint16_t Angle_data;
  uint16_t Angle_Bril;
  uint16_t Angle_run_time;
  uint16_t Angle_X;
  uint16_t AngleY;
  uint8_t dis_Angle[6];

  uint16_t range_data;
  uint16_t Range_Bril;
  uint16_t range_run_time;
  uint16_t Range_X;
  uint16_t Range_Y;
  uint8_t dis_range[7];

  uint8_t dis_type;
  uint16_t yuan;

  uint32_t opt3001_data;
  uint8_t opt3001_en;
  uint16_t opt3001_run_time;

  uint32_t Key_run_time;
  uint32_t Ec11_run_time;

  uint8_t Bat_Dis;
  uint16_t Bat_AD;
  uint16_t Bat_AD_Num;
  uint8_t Bat_Num[3];

  _Bool Blue_EN;
  uint16_t Blue_not_connect_time;
  uint32_t Blue_get_time;
  _Bool Blue_get_ing;
  _Bool Blue_get_over;
  uint16_t img_x_cnt;
  uint16_t img_y_cnt;

  uint32_t silent_time;
} S_sys;
extern S_sys sys;

typedef struct
{
  uint16_t cnt_ms;
  uint16_t cnt_s;
  uint16_t cnt_m;
  uint16_t cnt_h;
} Timer;

#endif
