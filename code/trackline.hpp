#ifndef TRACKLINE_HPP
#define TRACKLINE_HPP


void trackline_init(float p, float i, float d);

void trackline_set_camera_pd(float p, float d);
void trackline_set_turn_pd(float p, float d);
void trackline_set_gyro_param(float gyro_kp);
void trackline_set_gyro_z(float value);

void trackline_refresh_wheel_targets(int base_speed, int aim_y);

int trackline_wheel_target_right(void);
int trackline_wheel_target_left(void);

float trackline_get_error(void);
float trackline_get_turn_output(void);


#endif