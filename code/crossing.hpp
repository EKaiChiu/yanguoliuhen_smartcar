#ifndef _CROSSING_HPP
#define _CROSSING_HPP

#include "image.hpp"

float Slope_Calculate(uint8 begin, uint8 end, uint8 *border);

void calculate_s_i(
    uint8 start,
    uint8 end,
    uint8 *border,
    float *slope_rate,
    float *intercept
);

void cross_fill(
    uint8 (*image)[image_w],
    uint8 *l_border,
    uint8 *r_border,
    uint16 total_num_l,
    uint16 total_num_r,
    uint16 *dir_l,
    uint16 *dir_r,
    uint16 (*points_l)[2],
    uint16 (*points_r)[2]
);
int cross_fill_by_ABCD(
    uint8 *l_border,
    uint8 *r_border
);
#endif