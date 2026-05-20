#include "stdint.h"
#include "draw.h"
#include "dis.h"
#include "hz24_font.h"
#include "math.h"

/**在屏幕任意位置画一个像素点
 * @description: Plot a single pixel anywhere on the screen
 * @param {uint16_t} addr_x
 * @param {uint16_t} addr_y
 * @param {uint8_t} bril  0x01 - 0xFF
 * @return {ReturnStatus} 0:SUCCESS 1:ERROR
 */
ReturnStatus draw_pixel(uint16_t addr_x, uint16_t addr_y, uint8_t bril)
{
    if (addr_x >= SCREEN_HEIGHT || addr_y >= SCREEN_WIDTH)
        return RETERROR;// 边界检查
    else
        dis.gImage_img[addr_x][addr_y] = bril;// 设置像素亮度
    return RETOK;
}
/** 矩形绘制
 * @description: Draw a rectangle anywhere on the screen
 * @param {uint16_t} addr_x
 * @param {uint16_t} addr_y
 * @param {uint16_t} width
 * @param {uint16_t} height
 * @param {uint8_t} bril 0x01 - 0xFF
 * @return {ReturnStatus} 0:SUCCESS 1:ERROR
 */
ReturnStatus draw_rectangle(uint16_t addr_x, uint16_t addr_y, uint16_t width, uint16_t height, uint8_t bril)
{
    if ((addr_x + width) >= SCREEN_HEIGHT || (addr_y + height) >= SCREEN_WIDTH)
        return RETERROR;
    else
    {
        for (uint16_t x = addr_x; x < (addr_x + width); x++)
        {
            for (uint16_t y = addr_y; y < (addr_y + height); y++)
            {
                dis.gImage_img[x][y] = bril;
            }
        }
    }
    return RETOK;
}

/**汉字显示 (24×24点阵)
 * @description: ��ʾһ��24x24����
 * @param {int} x
 * @param {int} y
 * @param {int} index
 * @param {uint8_t} bril
 * @return {*}
 */
void draw_hanzi_24x24(int x, int y, int index, uint8_t bril)
{
    const unsigned char *p = font_hz24[index]; // 选择汉字
    volatile uint32_t line;
    for (int row = 0; row < 24; row++)
    {
        // 3字节合成24位（每行24个像素）
        line = (p[row * 3] << 16) | (p[row * 3 + 1] << 8) | p[row * 3 + 2]; // 3�ֽںϳ�24λ
        for (int col = 0; col < 24; col++)
        {
            if (line & (0x800000 >> col))
            {                                       // 从高位开始判断
                draw_pixel(x + row, y + col, bril); // 画一个点
            }
        }
    }
}
/**旋转汉字显示
 * @description: ��ʾ��һ����Ļ���ĵ�ΪԲ����ת��ĺ���
 * @param {int} x
 * @param {int} y
 * @param {int} index
 * @param {float} angle_degrees ��ת�Ƕ�
 * @return {*}
 */
void display_hanzi_24x24_rotated(int x, int y, int index, float angle_degrees)
{
    const unsigned char *p = font_hz24[index];
    float cx = 240;  // 中心点X坐标
    float cy = 320;  // 中心点Y坐标
    float radians = angle_degrees * (M_PI / 180.0f);  // 角度转弧度

    for (int row = 0; row < 24; row++)
    {
        uint32_t line = (p[row * 3] << 16) | (p[row * 3 + 1] << 8) | p[row * 3 + 2];
        for (int col = 0; col < 24; col++)
        {
            if (line & (0x800000 >> col))
            {
								//汉字的原始地址
                float source_x = x + row;	//!!!!
                float source_y = y + col;	//!!!!

                // 旋转公式 计算旋转后的地址
                float rotated_x = cos(radians) * (source_x - cx) - sin(radians) * (source_y - cy) + cx;
                float rotated_y = sin(radians) * (source_x - cx) + cos(radians) * (source_y - cy) + cy;

                draw_pixel((int)rotated_x, (int)rotated_y, 0xFF); // 画旋转后的点
            }
        }
    }
}


/**
 * @brief  24x24汉字 反向映射旋转显示（无空洞、无锯齿）
 * @param  x: 汉字原始左上角X坐标
 * @param  y: 汉字原始左上角Y坐标
 * @param  index: 汉字索引
 * @param  angle_degrees: 旋转角度(°) 顺时针
 */
void display_hanzi_24x24_rotated_fx(int x, int y, int index, float angle_degrees)
{
    const uint8_t *p = font_hz24[index];
    // 旋转中心（你可以随意改，屏幕内/外都支持）
    const float cx = 240.0f;  // 屏幕640中心X
    const float cy = 320.0f;  // 屏幕480中心Y

    float rad = angle_degrees * (M_PI / 180.0f);
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    // ==============================
    // 🔥 正确反向映射：
    // 遍历【原始汉字的所有像素】→ 反推目标坐标（小字库专用）
    // 既保留反向映射优点，又100%进入判断
    // ==============================
    for (int row = 0; row < 24; row++)   // 汉字行 Y
    {
        // 读取当前行的3字节点阵数据
        uint32_t line = (p[row*3] << 16) | (p[row*3+1] << 8) | p[row*3+2];

        for (int col = 0; col < 24; col++)  // 汉字列 X
        {
            // 只处理字库中为1的像素
            if (!(line & (0x800000 >> col))) continue;

            // ======================================
            // 1. 原始坐标：汉字在屏幕上的真实位置
            // ======================================
            float src_x = x + row;  //
            float src_y = y + col;  // 

            // ======================================
            // 2. 🔥 反向映射公式（正确！）
            // 计算：原始点 → 旋转后的目标点
            // ======================================
            float rx = cx + (src_x - cx) * cos_a + (src_y - cy) * sin_a;
            float ry = cy - (src_x - cx) * sin_a + (src_y - cy) * cos_a;

            // ======================================
            // 3. 屏幕边界保护（必加，防止花屏）
            // ======================================
            int x = (int)rx;
            int y = (int)ry;
            if(x >=0 && x < SCREEN_HEIGHT && y >=0 && y < SCREEN_WIDTH)
            {
                draw_pixel(x, y, 0xFF);
            }
        }
    }
}



volatile uint8_t range_disp_buf[RANGE_ROTATE_BUF_H][RANGE_ROTATE_BUF_W] = {0};
volatile uint8_t angle_disp_buf[ANGLE_ROTATE_BUF_H][ANGLE_ROTATE_BUF_W] = {0};
/**    矩阵旋转
 * @brief 旋转绘制长方形（严格适配你可用的旋转逻辑）
 * @param x: 长方形左上角 X 坐标
 * @param y: 长方形左上角 Y 坐标
 * @param width: 长方形宽度
 * @param height: 长方形高度
 * @param angle_degrees: 旋转角度
 * @param t_flag: 1 : range 0 : angle
 */
void display_rectange_rotated_fx(int x, int y, int width, int height, float angle_degrees,uint8_t t_flag)
{

    if (angle_degrees == 0)
    {
        for (int row = 0; row < width; row++)   
        {
            for (int col = 0; col < height; col++)  
            {
                int px = x + row;  
                int py = y + col;  
                if(px >= 0 && px < SCREEN_HEIGHT && py >= 0 && py < SCREEN_WIDTH)
                {
                    if (t_flag)
                    {
                        if(range_disp_buf[row][col] == 0) continue; 
                        draw_pixel(px, py, dis.Bril); 
                    }
                    else
                    {
                        if(angle_disp_buf[row][col] == 0) continue; 
                        draw_pixel(px, py, dis.Bril); 
                    }
                    
                }
            }
        }

        return;
    }


    // 旋转中心：和你汉字旋转完全一致（可改屏幕内外）
    const float cx = 240.0f;
    const float cy = 320.0f;
    // 

    float rad = angle_degrees * (M_PI / 180.0f);
    float cos_a = cosf(rad);
    float sin_a = sinf(rad);

    // // ====================== 步骤1：备份清空原始区域像素到数组 ======================
    // for (int row = 0; row < width; row++)   
    // {
    //     for (int col = 0; col < height; col++)  
    //     {
    //         int src_x = x + row;
    //         int src_y = y + col;
    //         if(src_x >=0 && src_x < SCREEN_HEIGHT && src_y >=0 && src_y < SCREEN_WIDTH)
    //         {
    //             backup_buf[row][col] = dis.gImage_img[src_x][src_y];
    //             dis.gImage_img[src_x][src_y] = 0;
    //         }
    //         else
    //         {
    //             backup_buf[row][col] = 0;
    //         }
    //     }
    // }


    // ==============================
    // 遍历长方形所有像素（替代汉字24x24遍历）
    // row：高度方向 0 ~ height-1
    // col：宽度方向 0 ~ width-1
    // ==============================
    for (int row = 0; row < width; row++)   
    {
        for (int col = 0; col < height; col++)  
        {
            // ======================================
            // 1. 原始坐标：完全照搬你能用的写法！
            // ======================================
            float src_x = x + row;  
            float src_y = y + col;  

            if(src_x < 0 || src_x >= SCREEN_HEIGHT || src_y < 0 || src_y >= SCREEN_WIDTH) continue;
            // if(dis.gImage_img[(int)src_x][(int)src_y] == 0) continue; //只处理非透明像素;
            // if(backup_buf[row][col] == 0) continue; 
            if (t_flag)
            {
                /* code */
                if(range_disp_buf[row][col] == 0) continue; 
            }
            else
            {
                /* code */
                if(angle_disp_buf[row][col] == 0) continue; 
            }
            

            // ======================================
            // 2. 旋转公式：和你汉字旋转一模一样！
            // ======================================
            float rx = cx + (src_x - cx) * cos_a + (src_y - cy) * sin_a;
            float ry = cy - (src_x - cx) * sin_a + (src_y - cy) * cos_a;

            // ======================================
            // 3. 屏幕边界保护 + 画点
            // ======================================
            int px = (int)rx;
            int py = (int)ry;
            if(px >= 0 && px < SCREEN_HEIGHT && py >= 0 && py < SCREEN_WIDTH)
            {
                draw_pixel(px, py, dis.Bril); 
            }
        }
    }
}

// /**    矩阵旋转 但是会有重复现象
//  * @brief 旋转绘制长方形（严格适配你可用的旋转逻辑）
//  * @param x: 长方形左上角 X 坐标
//  * @param y: 长方形左上角 Y 坐标
//  * @param width: 长方形宽度
//  * @param height: 长方形高度
//  * @param angle_degrees: 旋转角度
//  */
// void display_rectange_rotated_fx(int x, int y, int width, int height, float angle_degrees)
// {
//     // 旋转中心：和你汉字旋转完全一致（可改屏幕内外）
//     const float cx = 240.0f;
//     const float cy = 320.0f;

//     float rad = angle_degrees * (M_PI / 180.0f);
//     float cos_a = cosf(rad);
//     float sin_a = sinf(rad);

//     // ==============================
//     // 遍历长方形所有像素（替代汉字24x24遍历）
//     // row：高度方向 0 ~ height-1
//     // col：宽度方向 0 ~ width-1
//     // ==============================
//     for (int row = 0; row < width; row++)   
//     {
//         for (int col = 0; col < height; col++)  
//         {
//             // ======================================
//             // 1. 原始坐标：完全照搬你能用的写法！
//             // ======================================
//             float src_x = x + row;  
//             float src_y = y + col;  

//             if(src_x < 0 || src_x >= SCREEN_HEIGHT || src_y < 0 || src_y >= SCREEN_WIDTH) continue;
//             if(dis.gImage_img[(int)src_x][(int)src_y] == 0) continue; //只处理非透明像素;

//             // ======================================
//             // 2. 旋转公式：和你汉字旋转一模一样！
//             // ======================================
//             float rx = cx + (src_x - cx) * cos_a + (src_y - cy) * sin_a;
//             float ry = cy - (src_x - cx) * sin_a + (src_y - cy) * cos_a;

//             // ======================================
//             // 3. 屏幕边界保护 + 画点
//             // ======================================
//             int px = (int)rx;
//             int py = (int)ry;
//             if(px >= 0 && px < SCREEN_HEIGHT && py >= 0 && py < SCREEN_WIDTH)
//             {
//                 draw_pixel(px, py, dis.Bril); 
//             }
//         }
//     }
// }

// /**
//  * @brief  全屏画面旋转（反向映射，无孔洞、无失真）
//  * @param  src: 原始画面缓冲区 dis.gImage_img
//  * @param  dst: 旋转后输出缓冲区 gImage_rotated
//  * @param  angle_deg: 旋转角度（传感器计算值，正数顺时针）
//  * @return 无
//  */
// void screen_rotate(volatile uint8_t src[SCREEN_HEIGHT][SCREEN_WIDTH], 
//                    uint8_t dst[SCREEN_HEIGHT][SCREEN_WIDTH], 
//                    float angle_deg)
// {
//     float CENTER_X = 320.0f;
//     float CENTER_Y = 240.0f;
//     float radians = angle_deg * (M_PI / 180.0f);
//     float cos_ang = cosf(radians);
//     float sin_ang = sinf(radians);

//     // 1. 清空目标缓冲区（背景色：0x00全黑，根据你的屏修改）
//     for (int x = 0; x < SCREEN_HEIGHT; x++)
//     {
//         for (int y = 0; y < SCREEN_WIDTH; y++)
//         {
//             dst[x][y] = 0x00; 
//         }
//     }

//     // 2. 反向遍历：目标坐标 → 计算原始坐标（核心算法）
//     for (int x_dst = 0; x_dst < SCREEN_HEIGHT; x_dst++)
//     {
//         for (int y_dst = 0; y_dst < SCREEN_WIDTH; y_dst++)
//         {
//             // 反向旋转公式：求当前目标点对应的原始坐标
//             float x_rel = x_dst - CENTER_X;
//             float y_rel = y_dst - CENTER_Y;

//             float x_src = x_rel * cos_ang + y_rel * sin_ang + CENTER_X;
//             float y_src = -x_rel * sin_ang + y_rel * cos_ang + CENTER_Y;

//             // 3. 边界检查：原始坐标必须在屏幕范围内
//             if (x_src >= 0 && x_src < SCREEN_HEIGHT && 
//                 y_src >= 0 && y_src < SCREEN_WIDTH)
//             {
//                 // 4. 复制原始像素到旋转后的位置
//                 dst[x_dst][y_dst] = src[(int)x_src][(int)y_src];
//             }
//         }
//     }
// }

