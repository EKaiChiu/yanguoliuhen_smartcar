#ifndef ZEBRA_HPP
#define ZEBRA_HPP

#include "zf_common_headfile.hpp"
#include "image.hpp"

void zebra_stripes_detect(uint8 (*bin_img)[image_w], uint8 *l_border, uint8 *r_border);

int zebra_stripes_is_found(void);
int zebra_get_change_count(void);
int zebra_get_narrow_count(void);

#endif /* ZEBRA_HPP */