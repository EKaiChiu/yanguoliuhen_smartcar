#include "trackline.hpp"
#include "image.hpp"


/*
 * Migrated from car_walk.c:
 *   Err_Sum()      -> calc_weighted_error()
 *   PD_Camera()    -> calc_turn_output()
 *   judge_inc()    -> trackline_refresh_wheel_targets()
 *
 * Sign convention:
 *   track_error > 0: center line is left of image center, turn left.
 *   track_error < 0: center line is right of image center, turn right.
 */


#define TRACK_DIR 1

/*
 * The original car_walk.c overwrites PWM_turn with u before returning.
 * Keep this 0 to use the full migrated camera + gyro PD output.
 * Set to 1 only if you want to reproduce that original behavior exactly.
 */
#define TRACK_RETURN_RAW_CAMERA_U 0


static int wheel_target_right = 0;
static int wheel_target_left = 0;

static float track_error = 0.0f;
static float turn_output = 0.0f;

static float camera_p = 0.41f;
static float camera_d = 0.01f;

static float target_turn_kp = 1.0f;
static float target_gyro_z_kp = 1.7f;

static float error_turn_kp = 12.0f;
static float error_turn_kd = 1.7f;

static float camera_error_last = 0.0f;
static float error_turn_last = 0.0f;
static float smooth_turn[4] = {0.0f, 0.0f, 0.0f, 0.0f};

static float gyro_z = 0.0f;


static int limit_int(int value, int min_value, int max_value)
{
    if(value < min_value) value = min_value;
    if(value > max_value) value = max_value;
    return value;
}


static float limit_float(float value, float min_value, float max_value)
{
    if(value < min_value) value = min_value;
    if(value > max_value) value = max_value;
    return value;
}


static int abs_int(int value)
{
    return value >= 0 ? value : -value;
}


static int get_half_road_width(int y)
{
    y = limit_int(y, 0, image_h - 1);

    int top_half = image_w / 6;
    int bottom_half = image_w / 3;

    return top_half + (bottom_half - top_half) * y / (image_h - 1);
}


static int get_center_by_row(int y)
{
    y = limit_int(y, 0, image_h - 1);

    int center = center_line[y];
    int left = l_border[y];
    int right = r_border[y];

    int left_valid = 1;
    int right_valid = 1;

    if(left <= border_min + 1)
    {
        left_valid = 0;
    }

    if(right >= border_max - 1)
    {
        right_valid = 0;
    }

    if(right <= left)
    {
        left_valid = 0;
        right_valid = 0;
    }

    if(center > border_min + 2 && center < border_max - 2)
    {
        return center;
    }

    int half_width = get_half_road_width(y);

    if(left_valid && !right_valid)
    {
        return limit_int(left + half_width, border_min, border_max);
    }

    if(!left_valid && right_valid)
    {
        return limit_int(right - half_width, border_min, border_max);
    }

    if(left_valid && right_valid)
    {
        return (left + right) / 2;
    }

    return image_w / 2;
}


/*
 * Replacement for the MT9V03X_H fixed Weight[] table.
 * It keeps the same idea: lower-middle rows get much higher weight,
 * top rows and very near rows stay light.
 */
static int get_row_weight(int y)
{
    int peak_y = image_h * 3 / 4;
    int distance = abs_int(y - peak_y);
    int span = image_h / 5;

    if(distance >= span)
    {
        return 1;
    }

    return 1 + (span - distance) * 20 / span;
}


static float calc_weighted_error(int scan_rows)
{
    int image_mid = image_w / 2;
    int valid_rows = limit_int(scan_rows, 1, image_h - 1);

    if(Search_Stop_Line > 0)
    {
        valid_rows = limit_int(Search_Stop_Line, 1, image_h - 1);
    }

    float err_sum = 0.0f;
    float weight_sum = 0.0f;

    int start_y = image_h - 1;
    int stop_y = image_h - valid_rows - 1;

    stop_y = limit_int(stop_y, 0, image_h - 1);

    for(int y = start_y; y >= stop_y; y--)
    {
        int center = get_center_by_row(y);
        int weight = get_row_weight(y);

        err_sum += (float)(image_mid - center) * (float)weight;
        weight_sum += (float)weight;
    }

    if(weight_sum <= 0.0f)
    {
        return 0.0f;
    }

    return err_sum / weight_sum;
}


static float calc_turn_output(float expect_val, float err)
{
    float error_current = err - expect_val;
    float camera_u = camera_p * error_current
                   + camera_d * (error_current - camera_error_last);

    camera_error_last = error_current;

    float error_turn = camera_u * target_turn_kp
                     - gyro_z * target_gyro_z_kp;

    float pwm_turn = error_turn * error_turn_kp
                   + (error_turn - error_turn_last) * error_turn_kd;

    error_turn_last = error_turn;

    smooth_turn[3] = smooth_turn[2];
    smooth_turn[2] = smooth_turn[1];
    smooth_turn[1] = smooth_turn[0];
    smooth_turn[0] = pwm_turn;

    pwm_turn = smooth_turn[0] * 0.80f
             + smooth_turn[1] * 0.10f
             + smooth_turn[2] * 0.06f
             + smooth_turn[3] * 0.04f;

#if TRACK_RETURN_RAW_CAMERA_U
    return camera_u;
#else
    return pwm_turn;
#endif
}


void trackline_init(float p, float i, float d)
{
    (void)i;

    camera_p = p;
    camera_d = d;

    camera_error_last = 0.0f;
    error_turn_last = 0.0f;

    smooth_turn[0] = 0.0f;
    smooth_turn[1] = 0.0f;
    smooth_turn[2] = 0.0f;
    smooth_turn[3] = 0.0f;

    gyro_z = 0.0f;

    track_error = 0.0f;
    turn_output = 0.0f;

    wheel_target_right = 0;
    wheel_target_left = 0;
}


void trackline_set_camera_pd(float p, float d)
{
    camera_p = p;
    camera_d = d;
}


void trackline_set_turn_pd(float p, float d)
{
    error_turn_kp = p;
    error_turn_kd = d;
}


void trackline_set_gyro_param(float gyro_kp)
{
    target_gyro_z_kp = gyro_kp;
}


void trackline_set_gyro_z(float value)
{
    gyro_z = value;
}


void trackline_refresh_wheel_targets(int base_speed, int aim_y)
{
    int scan_rows = image_h - aim_y;

    scan_rows = limit_int(scan_rows, 1, image_h - 1);

    track_error = calc_weighted_error(scan_rows);

    if(track_error > -2.0f && track_error < 2.0f)
    {
        track_error = 0.0f;
    }

    turn_output = calc_turn_output(0.0f, track_error);
    turn_output = limit_float(turn_output * TRACK_DIR, -320.0f, 320.0f);

    int abs_error = abs_int((int)track_error);
    int run_speed = base_speed;

    if(abs_error > 26 || Left_Lost_Time > image_h / 3 || Right_Lost_Time > image_h / 3)
    {
        int corner_speed = limit_int(base_speed, 80, 160);

        if(track_error * TRACK_DIR > 0.0f)
        {
            wheel_target_left = 0;
            wheel_target_right = corner_speed;
        }
        else if(track_error * TRACK_DIR < 0.0f)
        {
            wheel_target_left = corner_speed;
            wheel_target_right = 0;
        }
        else
        {
            wheel_target_left = corner_speed;
            wheel_target_right = corner_speed;
        }

        return;
    }

    if(abs_error > 22)
    {
        run_speed = base_speed * 55 / 100;
    }
    else if(abs_error > 14)
    {
        run_speed = base_speed * 70 / 100;
    }

    run_speed = limit_int(run_speed, 0, 700);

    int increase = (int)(-turn_output);

    /*
     * car_walk.c:
     *   motor_A = Speed - increase
     *   motor_B = Speed + increase
     *
     * Here:
     *   left  = Speed - increase
     *   right = Speed + increase
     */
    wheel_target_left = run_speed - increase;
    wheel_target_right = run_speed + increase;

    wheel_target_left = limit_int(wheel_target_left, 0, 700);
    wheel_target_right = limit_int(wheel_target_right, 0, 700);
}


int trackline_wheel_target_right(void)
{
    return wheel_target_right;
}


int trackline_wheel_target_left(void)
{
    return wheel_target_left;
}


float trackline_get_error(void)
{
    return track_error;
}


float trackline_get_turn_output(void)
{
    return turn_output;
}
