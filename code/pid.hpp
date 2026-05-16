#ifndef PID_HPP
#define PID_HPP

#include "zf_common_headfile.hpp"
#include "interaction.hpp"


typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float target;
    float Error;
    float LastError;
    float PrevError;
    float Error_all;

    float P_out;
    float I_out;
    float D_out;

    float output;
    float output_limit;
    float integral_limit;
} pid_class;


extern pid_class RSPID;
extern pid_class LSPID;

extern int16 encoder_right;
extern int16 encoder_left;


// 基础 PID
void pid_init(pid_class *pid, float Kp, float Ki, float Kd);
void pid_compute(pid_class *pid, int target, int measurement);


// 速度闭环接口
void pid_speed_init(float kp, float ki, float kd, int target_right, int target_left);
void pid_speed_set_target(int target_right, int target_left);
void pid_speed_start(uint32 period_ms);
void pid_speed_stop(void);

int pid_get_target_right(void);
int pid_get_target_left(void);


// 兼容旧接口，如果其他地方还调用 Motor_set_speed_R/L，不会链接失败
void Motor_set_speed_R(float right_pwm);
void Motor_set_speed_L(float left_pwm);

#endif