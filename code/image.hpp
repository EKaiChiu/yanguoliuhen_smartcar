#ifndef _IMAGE_HPP
#define _IMAGE_HPP

#include "zf_common_headfile.hpp"


// =====================================================
// 图像尺寸
// =====================================================
#define image_h UVC_HEIGHT
#define image_w UVC_WIDTH


// =====================================================
// 像素定义
// =====================================================
#define white_pixel 255
#define black_pixel 0


// =====================================================
// 边界范围
// =====================================================
#define border_min 1
#define border_max (image_w - 2)


// =====================================================
// 图像缓存
// =====================================================
extern uint8 bin_image[image_h][image_w];


// =====================================================
// 输出边界
// l_border[y]     第 y 行左边界 x 坐标
// r_border[y]     第 y 行右边界 x 坐标
// center_line[y]  第 y 行中线 x 坐标
// =====================================================
extern uint8 l_border[image_h];
extern uint8 r_border[image_h];
extern uint8 center_line[image_h];


// =====================================================
// 调试/状态量
// =====================================================
extern int Road_Wide[image_h];

extern int White_Column[image_w];

extern int Search_Stop_Line;

extern int Left_Lost_Time;
extern int Right_Lost_Time;
extern int Both_Lost_Time;

extern int Boundry_Start_Left;
extern int Boundry_Start_Right;

extern int Longest_White_Column_Left[2];
extern int Longest_White_Column_Right[2];

extern int Left_Lost_Flag[image_h];
extern int Right_Lost_Flag[image_h];


// =====================================================
// 主入口
// 注意：传入的是已经二值化后的图像指针
// main 中顺序应该是：
// otsu_threshold(gray_image);
// image_process_from_bin_ptr(gray_image);
// =====================================================
void image_process_from_bin_ptr(uint8 *bin_src);


// 兼容旧名字
void image_process_by_white_column(uint8 *bin_src);


#endif /* _IMAGE_HPP */