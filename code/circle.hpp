#ifndef _CIRCLE_HPP
#define _CIRCLE_HPP

#include "image.hpp"

extern int circle_state;
extern int circle_type;

// circle_type
#define CIRCLE_NONE   0
#define CIRCLE_RIGHT  3
#define CIRCLE_LEFT   4

// 返回 1 表示正在处理环岛，返回 0 表示没有环岛
int circle_process(
    uint8 (*image)[image_w],
    uint8 *l_border,
    uint8 *r_border,
    uint8 hightest
);

#endif