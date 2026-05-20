#ifndef __DRAW_H__
#define __DRAW_H__

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define M_PI 3.14159265358979323846 // pi

typedef enum
{
    RETOK = 0U,
    RETERROR = !RETOK
} ReturnStatus;

ReturnStatus draw_pixel(uint16_t addr_x, uint16_t addr_y, uint8_t bril);
ReturnStatus draw_rectangle(uint16_t addr_x, uint16_t addr_y, uint16_t width, uint16_t height, uint8_t bril);
void draw_hanzi_24x24(int x, int y, int index, uint8_t bril);
void display_hanzi_24x24_rotated(int x, int y, int index, float angle_degrees);
void screen_rotate(volatile uint8_t src[SCREEN_HEIGHT][SCREEN_WIDTH],uint8_t dst[SCREEN_HEIGHT][SCREEN_WIDTH],float angle_deg);
void display_hanzi_24x24_rotated_fx(int x, int y, int index, float angle_degrees);
void display_rectange_rotated_fx(int x, int y,int width,int height, float angle_degrees,uint8_t t_flag);


#define RANGE_ROTATE_BUF_W    141
#define RANGE_ROTATE_BUF_H    25
#define ANGLE_ROTATE_BUF_W    115
#define ANGLE_ROTATE_BUF_H    25
extern volatile uint8_t range_disp_buf[RANGE_ROTATE_BUF_H][RANGE_ROTATE_BUF_W];
extern volatile uint8_t angle_disp_buf[ANGLE_ROTATE_BUF_H][ANGLE_ROTATE_BUF_W];
#endif
