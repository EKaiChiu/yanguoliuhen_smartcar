#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "zf_common_headfile.hpp"

#define PRINT_INTERVAL 1000           // 帧率打印间隔（毫秒），即每秒打印一次

#define BEEP_PATH        ZF_GPIO_BEEP
#define PWM_1_PATH        ZF_PWM_MOTOR_1
#define DIR_1_PATH        ZF_GPIO_MOTOR_1
#define PWM_2_PATH        ZF_PWM_MOTOR_2
#define DIR_2_PATH        ZF_GPIO_MOTOR_2
#define ENCODER_DIR_1_PATH           ZF_ENCODER_DIR_1
#define ENCODER_DIR_2_PATH           ZF_ENCODER_DIR_2

enum ImageType {
    GRAY,
    RGB
};

enum track_type_e {
    TRACK_LEFT,
    TRACK_RIGHT,
};

extern zf_device_ips200 ips200;        // 定义2寸IPS SPI屏幕的设备对象，用于屏幕的所有操作
extern zf_device_uvc uvc_dev;          // 定义UVC免驱摄像头的设备对象，用于摄像头的所有操作
extern zf_driver_gpio beep;            // 定义蜂鸣器GPIO驱动对象，用于蜂鸣器的控制
extern zf_driver_pit pit_timer;        // 定义PIT定时器对象，用于定时中断
extern uint8 Last_five_row_avg;        // 最后五行平均亮度值

extern zf_driver_encoder encoder_dir_1;
extern zf_driver_encoder encoder_dir_2;
extern int16 encoder_left;
extern int16 encoder_right;

extern struct pwm_info drv8701e_pwm_1_info;
extern struct pwm_info drv8701e_pwm_2_info;

extern zf_driver_gpio  drv8701e_dir_1;
extern zf_driver_gpio  drv8701e_dir_2;
extern zf_driver_pwm   drv8701e_pwm_1;
extern zf_driver_pwm   drv8701e_pwm_2;

extern int8 duty;

// 在设备树中，设置的10000。如果要修改，需要与设备树对应。
#define MOTOR1_PWM_DUTY_MAX    (drv8701e_pwm_1_info.duty_max)
// 在设备树中，设置的10000。如果要修改，需要与设备树对应。
#define MOTOR2_PWM_DUTY_MAX    (drv8701e_pwm_2_info.duty_max)

#define MAX_DUTY        (20)   // 最大 MAX_DUTY% 占空比

#endif // INTERACTION_HPP