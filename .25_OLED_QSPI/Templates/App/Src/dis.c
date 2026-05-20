
#include "main.h"
#include "quadspi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "qspi.h"
//#include "image.h"
//#include "image1.h"
//#include "image2.h"
#include "dis.h"
#include "tianfeng_image.h"
#include "num_image.h"
//#include "font.h"
#include "date.h"
#include "draw.h"

#define dot 11
#define M_M 12
volatile S_dis dis;
MENU_STATE Menu = {1, 0, 1, 8};
extern uint8_t cdAA[5];
extern uint8_t cdA8[5];
extern uint8_t cdA9[5];
volatile uint32_t Random = 0;
extern volatile uint8_t qspi_tx_done;
extern void qspi_instruct(uint8_t *date, uint16_t length);


volatile int angle_degrees = 0;

#define angle_18_xs 97
#define angle_18_ys 90
#define angle_18_width 269
#define angle_18_height 438

#define angle_36_xs 50
#define angle_36_ys 94
#define angle_36_width 341
#define angle_36_height 413

#define angle_54_xs 21
#define angle_54_ys 119
#define angle_54_width 391
#define angle_54_height 350

#define angle_72_xs 14
#define angle_72_ys 165
#define angle_72_width 418
#define angle_72_height 260

// #define angle_90_xs 29
// #define angle_90_ys 225
// #define angle_90_width 421
// #define angle_90_height 191



// 设置显示亮度
void Dis_Bril_Set(uint8_t Level)
{
  switch (Level)
  {
  case 1:
    // dis.Bril = 0xFF;
    // cdAA[4] = 0X10;  // 寄存器
    dis.Bril = 0x01; // 像素点亮度
    cdAA[4] = 0X01;  // 寄存器
    break;
  case 2:
    // dis.Bril = 0xFF;
    dis.Bril = 0x05;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01; // 寄存器
    break;
  case 3:
    // dis.Bril = 0xFF;
    dis.Bril = 0x12;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01;
    break;
  case 4:
    // dis.Bril = 0xFF;
    dis.Bril = 0x24;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01;
    break;
  case 5:
    // dis.Bril = 0xFF;
    dis.Bril = 0x48;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01;
    break;
  case 6:
    // dis.Bril = 0xFF;
    dis.Bril = 0x80;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01;
    break;
  case 7:
    // dis.Bril = 0xFF;
    dis.Bril = 0xC0;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X01;
    break;
  case 8:
    // dis.Bril = 0xFF;
    dis.Bril = 0xA0;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X03;
    break;
  case 9:
    // dis.Bril = 0xFF;
    dis.Bril = 0x90;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X05;
    break;
  case 10:
    // dis.Bril = 0xFF;
    dis.Bril = 0xE0;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X05;
    break;
  case 11:
    // dis.Bril = 0xFF;
    dis.Bril = 0xD0;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X08;
    break;
  case 12:
    // dis.Bril = 0xFF;
    dis.Bril = 0xA0;
    // cdAA[4] = 0X10;
    cdAA[4] = 0X09;
    break;
  case 13:
    dis.Bril = 0xFF;
    cdAA[4] = 0X10;
    break;
  default:
    dis.Bril2 = 6;
  }
  // 重新绘制准星（应用新亮度）
  sight_replace();
  // 通过QSPI发送亮度控制指令
  qspi_instruct(cdA8, 5);
  while (qspi_tx_done == 0)
    ;
  qspi_tx_done = 0;
  HAL_Delay(10);

  qspi_instruct(cdAA, 5);
  while (qspi_tx_done == 0)
    ;
  qspi_tx_done = 0;
  HAL_Delay(10);

  qspi_instruct(cdA9, 5);
  while (qspi_tx_done == 0)
    ;
  qspi_tx_done = 0;
  HAL_Delay(10);
}

// void sight_replace() // 图标设置
//{

//  switch (dis.type)
//  {
//  case 1:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 2:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_2[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 3:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 4:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_4[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 5:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_5[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 6:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_6[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 7:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_7[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  case 8:
//    for (uint16_t i = 0; i < 19; i++)
//    {
//      for (uint16_t j = 0; j < 19; j++)
//      {
//        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_8[i][j] * dis.Bril; //[dis.type - 1]
//      }
//    }
//    break;
//  default:
//    break;
//  }
//}

// 重新绘制准星（应用新亮度）
//类型1-3：大型准星 (180×180像素)
//类型4-8：小型准星 (19×19像素)
//类型Logo模式 (193×423像素，中心透明)
void sight_replace() // 图标设置  设置准星
{
  int angle_t = angle_degrees % 90;

  switch (dis.type)
  {
  case 1:  
    for (uint16_t i = 0; i < 180; i++)
    {
      for (uint16_t j = 0; j < 180; j++)
      {
        // if(angle_degrees == 0 || angle_degrees == 90)
        // {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1[i][j] * dis.Bril; //[dis.type - 1]
        // }
        // else if(angle_degrees == 18)
        // {
        //   dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1_rotated_18[i][j] * dis.Bril; //[dis.type - 1]
        // }
        // else if(angle_degrees == 36)
        // {
        //   dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1_rotated_36[i][j] * dis.Bril; //[dis.type - 1]
        // }
        // else if(angle_degrees == 54)
        // {
        //   dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1_rotated_54[i][j] * dis.Bril; //[dis.type - 1]
        // }
        // else if(angle_degrees == 72)
        // {
        //   dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_1_rotated_72[i][j] * dis.Bril; //[dis.type - 1]
        // }
        
      }
    }
    break;
  case 2:  
    for (uint16_t i = 0; i < 180; i++)
    {
      for (uint16_t j = 0; j < 180; j++)
      {
         if(angle_t == 0)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_2[i][j] * dis.Bril; //[dis.type - 1]
        }
        else if(angle_t == 18)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_18[i][j] * dis.Bril; //[dis.type - 1]
          // clear_rect_area(310,230,329,249);
        }
        else if(angle_t == 36)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_36[i][j] * dis.Bril; //[dis.type - 1]
          // clear_rect_area(310,230,329,249);
        }
        else if(angle_t == 54)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_54[i][j] * dis.Bril; //[dis.type - 1]
          // clear_rect_area(310,230,329,249);
        }
        else if(angle_t == 72)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_72[i][j] * dis.Bril; //[dis.type - 1]
          // clear_rect_area(310,230,329,249);
        }
      }
    }

    
     if(angle_t != 0)
     {
        clear_rect_area(220,310,249,338);
     }
    break;
  case 3:  
    for (uint16_t i = 0; i < 180; i++)
    {
      for (uint16_t j = 0; j < 180; j++)
      {
         if(angle_t == 0)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3[i][j] * dis.Bril; //[dis.type - 1]
        }
        else if(angle_t == 18)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_18[i][j] * dis.Bril; //[dis.type - 1]
        }
        else if(angle_t == 36)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_36[i][j] * dis.Bril; //[dis.type - 1]
        }
        else if(angle_t == 54)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_54[i][j] * dis.Bril; //[dis.type - 1]
        }
        else if(angle_t == 72)
        {
          dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_3_rotated_72[i][j] * dis.Bril; //[dis.type - 1]
        }
      }
    }
    break;
  case 4: 
    for (uint16_t i = 0; i < 19; i++)
    {
      for (uint16_t j = 0; j < 19; j++)
      {
        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_4[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
    break;
  case 5:
    for (uint16_t i = 0; i < 19; i++)
    {
      for (uint16_t j = 0; j < 19; j++)
      {
        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_5[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
    break;
  case 6:
    for (uint16_t i = 0; i < 19; i++)
    {
      for (uint16_t j = 0; j < 19; j++)
      {
        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_6[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
    break;
  case 7:
    for (uint16_t i = 0; i < 19; i++)
    {
      for (uint16_t j = 0; j < 19; j++)
      {
        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_7[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
    break;
  case 8:
    for (uint16_t i = 0; i < 19; i++)
    {
      for (uint16_t j = 0; j < 19; j++)
      {
        dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = dis_type_8[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
    break;
  case 9:// Logo模式 193×423
    for (uint16_t i = 0; i < 193; i++)
    {
      for (uint16_t j = 0; j < 423; j++)
      {
        // 中心区域透明
        if ((i >= 66 && i <= 156) && (j >= 66 && j <= 366))
        {
          dis.gImage_img[i + 143][j + 108] = 0;
        }
        else
        {
          dis.gImage_img[i + 143][j + 108] = dis_logo[i][j] * dis.Bril; //[dis.type - 1]
        }
      }
    }
  default:
    break;
  }
}
/** 显示 Logo
 * @description: Logo数据处理
 * @param {uint16_t} xl x轴左
 * @param {uint16_t} xr x轴右
 * @param {uint16_t} yt y轴左
 * @param {uint16_t} yd y轴右
 * @return {*}
 */
void Logo_Data(uint16_t xl, uint16_t xr, uint16_t yt, uint16_t yd)
{
  for (uint16_t i = 0; i < 193; i++)
  {
    for (uint16_t j = 0; j < 423; j++)
    {
      if ((i > xl && i < xr) && (j > yt && j < yd))
      {
        dis.gImage_img[i + 143][j + 108] = 0;// 设置为透明
      }
      else
      {
        dis.gImage_img[i + 143][j + 108] = dis_logo[i][j] * dis.Bril; //[dis.type - 1]
      }
    }
  }
}

// void Logo_Data_rotate_angle(uint16_t xl, uint16_t xr, uint16_t yt, uint16_t yd, uint8_t angle)
// {
//   uint16_t xs = 0;
//   uint16_t ys = 0;
//   uint16_t width = 0;
//   uint16_t height = 0;
//   switch (angle)
//   {
//   case 18:
//     /* code */
//     xs = angle_18_xs;
//     ys = angle_18_ys;
//     width = angle_18_width;
//     height = angle_18_height;
//     break;
  
//   case 36:
//     /* code */
//     xs = angle_36_xs;
//     ys = angle_36_ys;
//     width = angle_36_width;
//     height = angle_36_height;
//     break;
//   case 54:
//     /* code */
//     xs = angle_54_xs;
//     ys = angle_54_ys;
//     width = angle_54_width;
//     height = angle_54_height;
//     break;
//   case 72:
//     /* code */
//     xs = angle_72_xs;
//     ys = angle_72_ys;
//     width = angle_72_width;
//     height = angle_72_height;
//     break;
//   // case 90:
//   //   /* code */
//   //   xs = angle_90_xs;
//   //   ys = angle_90_ys;
//   //   width = angle_90_width;
//   //   height = angle_90_height;
//   //   break;

//   default:
//     xs = 143;
//     ys = 108;
//     width = 193;
//     height = 423;
//     break;
//   }

//   for (uint16_t i = 0; i < width; i++)
//   {
//     for (uint16_t j = 0; j < height; j++)
//     {
//       if ((i > xl && i < xr) && (j > yt && j < yd))
//       {
//         dis.gImage_img[i + xs][j + ys] = 0;// 设置为透明
//       }
//       else
//       {
//         if(angle == 18)
//         {
//           dis.gImage_img[i + xs][j + ys] = dis_logo_rotated_18[i][j] * dis.Bril; //[dis.type - 1]
//         }
//         else if (angle == 36)
//         {
//           /* code */
//           dis.gImage_img[i + xs][j + ys] = dis_logo_rotated_36[i][j] * dis.Bril; //[dis.type - 1]
//         }
//         else if (angle == 54)
//         {
//           /* code */
//           dis.gImage_img[i + xs][j + ys] = dis_logo_rotated_54[i][j] * dis.Bril; //[dis.type - 1]
//         }
//         else if (angle == 72)
//         {
//           /* code */
//           dis.gImage_img[i + xs][j + ys] = dis_logo_rotated_72[i][j] * dis.Bril; //[dis.type - 1]
//         }
//         // else if (angle == 90)
//         // {
//         //   /* code */
//         //   dis.gImage_img[i + xs][j + ys] = dis_logo_rotated_90[i][j] * dis.Bril; //[dis.type - 1]
//         // }
//         else
//         {
//           dis.gImage_img[i + xs][j + ys] = dis_logo[i][j] * dis.Bril; //[dis.type - 1]
//         }
//       }
//     }
//   }
// }



void sight_clear() // 图标清零
{
  for (uint16_t i = 0; i < 180; i++)
  {
    for (uint16_t j = 0; j < 180; j++)
    {
      dis.gImage_img[i + dis.sight_X][j + dis.sight_Y] = 0;
    }
  }
}

void clear_rect_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  for (uint16_t i = y1; i <= y2; i++)
  {
    for (uint16_t j = x1; j <= x2; j++)
    {
      dis.gImage_img[j][i] = 0;
    }
  }
}

// 桌面清零
void gImage_clear(void) // 桌面清零
{
  for (uint16_t i = 0; i < 480; i++)
  {
    for (uint16_t j = 0; j < 640; j++)
    {
      if (dis.gImage_img[i][j])
      {
        dis.gImage_img[i][j] = 0;
      }
    }
  }
}
/**
 * @description: Logo动态显示
 * @return {*}
 */
void Logo_Show(void)
{
  uint16_t xl, xr, yt, yd;
  // uint8_t vx = 2, vy = 4;  // 收缩速度
  uint8_t vx = 0, vy = 0;  // 收缩速度

  // switch (angle_degrees)
  // {
  // case 18:
  //   /* code */
  //   xl = 0;
  //   xr = angle_18_width;
  //   yt = 0;
  //   yd = angle_18_height;
  //   vx = 2;
  //   vy = 4;
  //   break;
  // case 36:
  //   /* code */
  //   xl = 0;
  //   xr = angle_36_width;
  //   yt = 0;
  //   yd = angle_36_height;
  //   vx = 3;
  //   vy = 4;
  //   break;
  // case 54:
  //   /* code */
  //   xl = 0;
  //   xr = angle_54_width;
  //   yt = 0;
  //   yd = angle_54_height;
  //   vx = 4;
  //   vy = 3;
  //   break;
  // case 72:
  //   /* code */
  //   xl = 0;
  //   xr = angle_72_width;
  //   yt = 0;
  //   yd = angle_72_height;
  //   vx = 4;
  //   vy = 2;
  //   break;
  // // case 90:
  // //   /* code */
  // //   xl = 0;
  // //   xr = angle_90_width;
  // //   yt = 0;
  // //   yd = angle_90_height;
  // //   vx = 4;
  // //   vy = 2;
  // //   break;
  
  // default:
    xl = 0; // Logo左
    xr = 193; // Logo右
    yt = 0; // Logo上
    yd = 423; // Logo下
    vx = 2;
    vy = 4;
  //   break;
  // }

  gImage_clear(); // 桌面清零
  while (1)
  {
    Logo_Data(xl, xr, yt, yd);  // 绘制当前帧
    // if(angle_degrees == 0)
    // {
    //   Logo_Data(xl, xr, yt, yd);  // 绘制当前帧
    // }
    // else
    // {
    //   Logo_Data_rotate_angle(xl, xr, yt, yd, angle_degrees);
    // }
    Dis_Play(); // 显示到屏幕
    // 透明区域收缩
		if(xl < xr){
			xl += vx; // 左边界向右移动
			xr -= vx; // 右边界向左移动
		}
		if(yt < yd){
			yt += vy; // 上边界向下移动
			yd -= vy; // 下边界向上移动
		}
    // 防止边界交叉
    if (xl >= xr)
    {
      xl = xr;
    }
    if (yt >= yd)
    {
      yt = yd;
    }
    // 动画结束条件
    if (xl == xr && yt == yd)
    {
      break;
    }
  }
}

void Image_Init(void) // 初始化
{
  dis.type = 3;          // 默认显示
  sys.Range_Bril = 0X00; // 测距亮度
                         //  dis.sight_X = 240; // 准星X
                         //  dis.sight_Y = 310; // 准星Y
                         //	dis.sight_X = 150; // 准星X
                         //  dis.sight_Y = 240; // 准星Y

  //	dis.sight_X = 164; // 准星X
  //  dis.sight_Y = 230; // 准星Y
  dis.sight_X = 137; // 准星X
  dis.sight_Y = 231; // 准星Y
  gImage_clear();
  // sight_replace();
}
// 全屏设置亮度
void Bril_Set()
{
  for (uint16_t i = 0; i < 480; i++)
  {
    for (uint16_t j = 0; j < 640; j++)
    {
      if (dis.gImage_img[i][j])
      {
        dis.gImage_img[i][j] = dis.Bril;
      }
    }
  }
}
void Dis_Play(void)
{

  QSPI_send_image(dis.gImage_img); // 通过QSPI发送整个缓冲区
}
void Dis_Play_Logic(void) // 显示逻辑
{

  static uint8_t last_Bril = 0;     // 亮度
  static uint16_t last_sight_X = 0; // 准星X
  static uint16_t last_sight_Y = 0; // 准星Y
  static uint8_t last_type = 0;     // 显示类型
  static uint8_t last_range_data = 0; // 测距数据
  static uint8_t last_unit_switch = 0; // 单位切换
  static uint32_t last_Random = 0; // 随机数
  static uint8_t last_onetime = 0; // 上次一次性标志
  static int last_angle = 0; // 上次旋转角度

  //检查是否需要刷新（任一条件满足即刷新）
  if (
      last_Bril != dis.Bril ||       // 亮度变化
      last_sight_X != dis.sight_X || // 准星X变化
      last_sight_Y != dis.sight_Y || // 准星Y变化
      last_type != dis.type || // 显示类型变化
      last_range_data != sys.range_data || // 测距数据变化  
      
      Menu.last_pos != Menu.pos_arrow ||
      last_angle != angle_degrees
    ) // 菜单位置变化
  {
    //更新上次状态
    last_Bril = dis.Bril;
    last_sight_X = dis.sight_X;
    last_sight_Y = dis.sight_Y;
    last_type = dis.type;
    last_range_data = sys.range_data;
    Menu.last_pos = Menu.pos_arrow;
    last_angle = angle_degrees;

    
  
	  // display_rectange_rotated_fx(81, 198, 25, 267, angle_degrees);
    Dis_Play();
  }
}

// 关闭显示
void Image_off(void)
{

  for (uint16_t i = 0; i < 480; i++)
  {
    for (uint16_t j = 0; j < 640; j++)
    {
      dis.gImage_img[i][j] = 0;
    }
  }
}

// 数字显示
void Num_Add(uint16_t x, uint16_t y, uint8_t num,uint8_t t_flag)
{
  //  switch (num)
  //  {
  //  case 0:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_0[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 1:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_1[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 2:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_2[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 3:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_3[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 4:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_4[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 5:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_5[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 6:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_6[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 7:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_7[i][j] *dis.Bril;
  //      }
  //    }
  //    break;
  //  case 8:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_8[i][j] * dis.Bril;
  //      }
  //    }
  //    break;
  //  case 9:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = Num_9[i][j] * dis.Bril;// dis.Bril
  //      }
  //    }
  //    break;
  //  case 99:
  //    for (uint16_t i = 0; i < 25; i++)
  //    {
  //      for (uint16_t j = 0; j < 17; j++)
  //      {
  //        dis.gImage_img[x + i][y + j] = 0;
  //      }
  //    }
  //    break;
  //  default:
  //    return;
  //    break;
  //  }
  //}
  switch (num)
  {
  case 0:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_0[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_0[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_0[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 1:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_1[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_1[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_1[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 2:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_2[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_2[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_2[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 3:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_3[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_3[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_3[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 4:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_4[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_4[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_4[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 5:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_5[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_5[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_5[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 6:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_6[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_6[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_6[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 7:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_7[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_7[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_7[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 8:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_8[i][j] * dis.Bril;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_8[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_8[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 9:
    for (uint16_t i = 0; i < 25; i++)
    {
      for (uint16_t j = 0; j < 17; j++)
      {
        // dis.gImage_img[x + i][y + j] = Num_9[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Num_9[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Num_9[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 99:
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = 0;
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = 0;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = 0;
        }
      }
    }
    break;

  // 小数点
  case 11:
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = Dot[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = Dot[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = Dot[i][j] * dis.Bril;
        }
      }
    }
    break;

  case 12: // 符号
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = fuhao[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = fuhao[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = fuhao[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 13: // 空白
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = null[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = null[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = null[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 14: //摄氏度符号
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = sheshidu[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = sheshidu[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = sheshidu[i][j] * dis.Bril;
        }
      }
    }
    break;
  case 15:
    for (uint16_t i = 0; i < 20; i++)
    {
      for (uint16_t j = 0; j < 13; j++)
      {
        // dis.gImage_img[x + i][y + j] = fuhao1[i][j] * dis.Bril; // dis.Bril
        if (t_flag)
        {
          range_disp_buf[x + i][y + j] = fuhao1[i][j] * dis.Bril;
        }
        else
        {
          angle_disp_buf[x + i][y + j] = fuhao1[i][j] * dis.Bril;
        }
      }
    }
    break;

  default:
    return;
    break;
  }
}

// 测距显示  显示距离
void Range_dis()
{
  uint16_t range_data_copy = sys.range_data;
  //	uint16_t range_data_copy = 12345;
  //	if(sys.range_data>0)
  // 测距部分
  //  {
  //    sys.dis_range[0] = range_data_copy / 10000;
  //    range_data_copy %= 10000;

  //    // 提取千位
  //    sys.dis_range[1] = range_data_copy / 1000;
  //    range_data_copy %= 1000;

  //    // 提取百位
  //    sys.dis_range[2] = range_data_copy / 100;
  //    range_data_copy %= 100;

  //    // 提取十位
  //    sys.dis_range[3] = range_data_copy / 10;
  //    range_data_copy %= 10;

  //    // 提取个位
  //    sys.dis_range[5] = range_data_copy;
  //
  ////		sys.dis_range[5] = dot;
  //  }
  {
    // 提取万位  以厘米为单位
    sys.dis_range[0] = range_data_copy / 10000;
    range_data_copy %= 10000;

    // 提取千位
    sys.dis_range[1] = range_data_copy / 1000;
    range_data_copy %= 1000;

    // 提取百位
    sys.dis_range[2] = range_data_copy / 100;
    range_data_copy %= 100;

    // 提取十位
    sys.dis_range[3] = range_data_copy / 10;
    range_data_copy %= 10;

    //		sys.dis_range[4] = dot;
    // 提取个位
    sys.dis_range[5] = range_data_copy;

    sys.dis_range[6] = 0;
  }
  sys.dis_range[4] = dot;

  //  for (uint8_t k = 6; k > 0; k--)
  //  {
  ////    Num_Add(sys.Range_X, (sys.Range_Y + 30) + k * 17, sys.dis_range[k - 1]);
  //		Num_Add(sys.Range_X, (sys.Range_Y ) + k * 13, sys.dis_range[k - 1]);
  //  }
  
  // 显示7位数字
  for (uint8_t k = 0; k < 7; k++)
  {
    //    Num_Add(sys.Range_X, (sys.Range_Y + 30) + k * 17, sys.dis_range[k - 1]);
    Num_Add(0, 0 + k * 17, sys.dis_range[k],1);
  }

  // 显示"米"字
  for (uint16_t i = 0; i < 20; i++)
  {
    for (uint16_t j = 0; j < 17; j++)
    {
      // dis.gImage_img[sys.Range_X + i][sys.Range_Y + 5 + 7 * 17 + j] = MM[i][j] * dis.Bril;
       range_disp_buf[0 + i][0 + 5 + 7 * 17 + j] = MM[i][j] * dis.Bril;
    }
  }
}
// 显示距离(码)
void Range_disp_Y()
{
  uint16_t range_data_copy = sys.range_data;
  //	uint32_t range_data_copy = 10000;
  range_data_copy = range_data_copy * 11;   // 厘米转码 (1cm ≈ 0.11码)
  // range_data_copy = angle_degrees;
  //	if(sys.range_data>0)
  {
    // 提取万位  以厘米为单位
    sys.dis_range[0] = range_data_copy / 100000;
    range_data_copy %= 100000;

    // 提取千位
    sys.dis_range[1] = range_data_copy / 10000;
    range_data_copy %= 10000;

    // 提取百位
    sys.dis_range[2] = range_data_copy / 1000;
    range_data_copy %= 1000;

    // 提取十位
    sys.dis_range[3] = range_data_copy / 100;
    range_data_copy %= 100;

    //		sys.dis_range[4] = dot;
    // 提取个位
    sys.dis_range[5] = range_data_copy / 10;
    range_data_copy %= 10;

    sys.dis_range[6] = range_data_copy;
  }
  sys.dis_range[4] = dot;

  for (uint8_t k = 0; k < 7; k++)
  {
    // Num_Add(sys.Range_X, (sys.Range_Y) + k * 17, sys.dis_range[k]);
    Num_Add(0, 0 + k * 17, sys.dis_range[k],1);
  }

  // 显示"码"字
  for (uint16_t i = 0; i < 20; i++)
  {
    for (uint16_t j = 0; j < 17; j++)
    {
      // dis.gImage_img[sys.Range_X + i][sys.Range_Y + 5 + 7 * 17 + j] = YY[i][j] * dis.Bril;
       range_disp_buf[0 + i][0 + 5 + 7 * 17 + j] = YY[i][j] * dis.Bril;
    }
  }
}

// 显示角度
void Angle_dis()
{
  //  uint16_t range_data_copy = sys.Angle_data;
  //	Random=Random-9000;
  uint16_t range_data_copy = 0; // 测角度部分
  //	if(sys.range_data==0&&M_flag==0)
  //		range_data_copy=0;
  // if(sys.range_data>0)
  {
    //		sys.dis_Angle[0]=0;//高位
    //		sys.dis_Angle[1]=0;//低位
    //		sys.dis_Angle[2]=dot;
    //		sys.dis_Angle[3]=0;
    //		sys.dis_Angle[4]=0;

    // 正负号判断
    if (range_data_copy > 9000)
    {
      range_data_copy = range_data_copy - 9000;
      sys.dis_Angle[0] = 15;   // 负号
    }
    else
    {
      sys.dis_Angle[0] = 13;
    }


    sys.dis_Angle[1] = range_data_copy / 1000;
    range_data_copy %= 1000;

    // 提取千位
    sys.dis_Angle[2] = range_data_copy / 100;
    range_data_copy %= 100;

    sys.dis_Angle[3] = dot; // 小数点

    // 提取百位
    sys.dis_Angle[4] = range_data_copy / 10;
    range_data_copy %= 10;

    // 提取十位
    sys.dis_Angle[5] = range_data_copy;
  }
  //  if(Random>=9000)
  //	{
  //		Num_Add(sys.Angle_X, (sys.AngleY-13), 12);//负号
  //	}
  //	else
  //	{//符号清零
  //
  //	 for (uint16_t i = 0; i < 20; i++)
  //    {
  //      for (uint16_t j = 0; j < 13; j++)
  //      {
  //        dis.gImage_img[sys.Angle_X + i][sys.AngleY-13 + j] = 0x00;
  //      }
  //    }
  //	}

  //	sys.dis_Angle[2]=dot;
  
  // 显示6位数据
  for (uint8_t k = 0; k < 6; k++) // for (uint8_t k = 5; k > 0; k--)
  {
    //    Num_Add(sys.Range_X, (sys.Range_Y + 30) + k * 17, sys.dis_range[k - 1]);
    // Num_Add(sys.Angle_X, (sys.AngleY) + k * 17, sys.dis_Angle[k]); // Num_Add(sys.Angle_X, (sys.AngleY ) + k * 13, sys.dis_Angle[k - 1]);
    Num_Add(0, 0 + k * 17, sys.dis_Angle[k],0);
  }
  //  for (uint16_t i = 0; i < 32; i++)
  //  {
  //    for (uint16_t j = 0; j < 42; j++)
  //    {
  //      gImage_img[Angle_X + i][Angle_Y + j] = Img_Angle[i][j] * Bril;
  //    }
  //  }

  // 度数
  for (uint16_t i = 0; i < 20; i++)
  {
    for (uint16_t j = 0; j < 13; j++)
    {
      // dis.gImage_img[sys.Angle_X + i][sys.AngleY + 6 * 17 + j] = sheshidu[i][j] * dis.Bril;
       angle_disp_buf[0 + i][0 + 6 * 17 + j] = sheshidu[i][j] * dis.Bril;
    }
  }
}







/* USER CODE END Includes */