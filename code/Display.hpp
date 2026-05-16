#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "zf_common_headfile.hpp"
#include "../user/interaction.hpp"
#include "camera.hpp"
#include "pid.hpp"

void display_image(ImageType type);

// 显示 PID 参数
void display_pid_data(void);

// 显示灰度/二值图 + PID 参数
void display_gray_with_pid(uint8 *image);

#endif // DISPLAY_HPP