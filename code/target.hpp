#ifndef TARGET_HPP
#define TARGET_HPP

#include "zf_common_headfile.hpp"

typedef struct
{
    int found;      // 是否找到红色目标
    int center_x;   // 红色目标中心 x
    int center_y;   // 红色目标中心 y
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int area;       // 红色像素数量
} red_target_t;

void red_target_detect_rgb565(uint16 *rgb_image);
red_target_t red_target_get_result(void);

#endif