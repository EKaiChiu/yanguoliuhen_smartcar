#include "crossing.hpp"

#define SIDE_LEFT   0
#define SIDE_RIGHT  1

static int border_point_valid(uint8 *border, int y, int side)
{
    if(y < 0 || y >= image_h)
    {
        return 0;
    }

    int x = border[y];

    if(side == SIDE_LEFT)
    {
        if(x <= border_min + 2)
        {
            return 0;
        }
    }
    else
    {
        if(x >= border_max - 2)
        {
            return 0;
        }
    }

    return 1;
}

static int find_border_break_pair(
    uint8 *border,
    int side,
    int start_y,
    int end_y,
    int *P1_x,
    int *P1_y,
    int *P2_x,
    int *P2_y
)
{
    const int max_jump = 12;
    const int min_lost_len = 3;

    int last_valid_x = -1;
    int last_valid_y = -1;

    int found_P1 = 0;
    int lost_count = 0;

    if(start_y > image_h - 2)
    {
        start_y = image_h - 2;
    }

    if(end_y < 1)
    {
        end_y = 1;
    }

    for(int y = start_y; y >= end_y; y--)
    {
        int valid = border_point_valid(border, y, side);
        int x = border[y];

        if(!found_P1)
        {
            if(valid)
            {
                if(last_valid_x >= 0)
                {
                    int diff = x - last_valid_x;

                    if(diff > max_jump || diff < -max_jump)
                    {
                        *P1_x = last_valid_x;
                        *P1_y = last_valid_y;

                        *P2_x = x;
                        *P2_y = y;

                        return 1;
                    }
                }

                last_valid_x = x;
                last_valid_y = y;
                lost_count = 0;
            }
            else
            {
                if(last_valid_x >= 0)
                {
                    lost_count++;

                    if(lost_count >= min_lost_len)
                    {
                        *P1_x = last_valid_x;
                        *P1_y = last_valid_y;
                        found_P1 = 1;
                    }
                }
            }
        }
        else
        {
            if(valid)
            {
                *P2_x = x;
                *P2_y = y;
                return 1;
            }
        }
    }

    return 0;
}

int find_cross_ABCD(
    uint8 *l_border,
    uint8 *r_border,
    int *A_x,
    int *A_y,
    int *B_x,
    int *B_y,
    int *C_x,
    int *C_y,
    int *D_x,
    int *D_y
)
{
    int left_found = find_border_break_pair(
        l_border,
        SIDE_LEFT,
        image_h - 2,
        image_h / 4,
        A_x,
        A_y,
        B_x,
        B_y
    );

    int right_found = find_border_break_pair(
        r_border,
        SIDE_RIGHT,
        image_h - 2,
        image_h / 4,
        C_x,
        C_y,
        D_x,
        D_y
    );

    if(left_found && right_found)
    {
        return 1;
    }

    return 0;
}
static void fill_border_by_two_points(
    uint8 *border,
    int x1,
    int y1,
    int x2,
    int y2
)
{
    if(y1 == y2)
    {
        return;
    }

    // 边界数组是 border[y] = x
    // 所以公式是 x = k * y + b
    float k = (float)(x2 - x1) / (float)(y2 - y1);
    float b = x1 - k * y1;

    int y_start = y1;
    int y_end = y2;

    if(y_start > y_end)
    {
        int temp = y_start;
        y_start = y_end;
        y_end = temp;
    }

    if(y_start < 0)
    {
        y_start = 0;
    }

    if(y_end > image_h - 1)
    {
        y_end = image_h - 1;
    }

    for(int y = y_start; y <= y_end; y++)
    {
        int x = (int)(k * y + b);

        if(x < border_min)
        {
            x = border_min;
        }

        if(x > border_max)
        {
            x = border_max;
        }

        border[y] = (uint8)x;
    }
}

int cross_fill_by_ABCD(
    uint8 *l_border,
    uint8 *r_border
)
{
    int A_x = 0, A_y = 0;
    int B_x = 0, B_y = 0;
    int C_x = 0, C_y = 0;
    int D_x = 0, D_y = 0;

    int found = find_cross_ABCD(
        l_border,
        r_border,
        &A_x,
        &A_y,
        &B_x,
        &B_y,
        &C_x,
        &C_y,
        &D_x,
        &D_y
    );

    if(!found)
    {
        return 0;
    }

    // 左边界：连接 A-B
    fill_border_by_two_points(
        l_border,
        A_x,
        A_y,
        B_x,
        B_y
    );

    // 右边界：连接 C-D
    fill_border_by_two_points(
        r_border,
        C_x,
        C_y,
        D_x,
        D_y
    );

    return 1;
}