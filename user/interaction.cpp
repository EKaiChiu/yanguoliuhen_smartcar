#include "interaction.hpp"

zf_device_ips200 ips200;        // 定义2寸IPS SPI屏幕的设备对象，用于屏幕的所有操作
zf_device_uvc uvc_dev;          // 定义UVC免驱摄像头的设备对象，用于摄像头的所有操作
zf_driver_gpio beep(BEEP_PATH, O_RDWR);  // 定义蜂鸣器GPIO驱动对象，用于蜂鸣器的控制
zf_driver_pit pit_timer;         // 定义PIT定时器对象，用于定时中断
uint8 Last_five_row_avg = 0;     // 最后五行平均亮度值

zf_driver_encoder encoder_dir_1(ENCODER_DIR_1_PATH);
zf_driver_encoder encoder_dir_2(ENCODER_DIR_2_PATH);

int16 encoder_left;
int16 encoder_right;

struct pwm_info drv8701e_pwm_1_info;
struct pwm_info drv8701e_pwm_2_info;

zf_driver_gpio  drv8701e_dir_1(DIR_1_PATH, O_RDWR);
zf_driver_gpio  drv8701e_dir_2(DIR_2_PATH, O_RDWR);
zf_driver_pwm   drv8701e_pwm_1(PWM_1_PATH);
zf_driver_pwm   drv8701e_pwm_2(PWM_2_PATH);



//////////////////////////// 自定义函数以及宏定义 ///////////////////////////

int8 duty = 15;





