#include "circle.hpp"

// trend:
//  0  不要求单调方向，只要求连续
//  1  要求从下往上 x 逐渐增大
// -1  要求从下往上 x 逐渐减小
//
// 返回值：
// 1 连续
// 0 不连续
static int border_is_continuous(
    uint8 *border,
    int start_y,
    int end_y,
    int trend
)
{
    int last_x = -1;
    int valid_count = 0;
    int bad_count = 0;

    const int max_jump = 8;       // 相邻两行最大允许跳变，后面可调
    const int tolerance = 2;      // 单调允许 2 像素抖动
    const int max_bad = 3;        // 最多允许 3 次异常
    const int min_valid = 15;     // 至少需要 15 个有效点

    if(start_y > image_h - 1)
    {
        start_y = image_h - 1;
    }

    if(end_y < 0)
    {
        end_y = 0;
    }

    // 从下往上扫描：y 从大到小
    for(int y = start_y; y >= end_y; y--)
    {
        int x = border[y];

        // 贴边一般说明丢线，不参与连续性判断
        if(x <= border_min + 1 || x >= border_max - 1)
        {
            continue;
        }

        if(last_x >= 0)
        {
            int diff = x - last_x;

            // 1. 判断是否跳变太大
            if(diff > max_jump || diff < -max_jump)
            {
                bad_count++;
            }

            // 2. 判断单调趋势
            if(trend == 1)
            {
                // 要求从下往上 x 增大
                if(diff < -tolerance)
                {
                    bad_count++;
                }
            }
            else if(trend == -1)
            {
                // 要求从下往上 x 减小
                if(diff > tolerance)
                {
                    bad_count++;
                }
            }
        }

        last_x = x;
        valid_count++;
    }

    if(valid_count < min_valid)
    {
        return 0;
    }

    if(bad_count > max_bad)
    {
        return 0;
    }

    return 1;
}
//左边界是否连续
static int left_border_is_continuous(uint8 *l_border)
{
    return border_is_continuous(
        l_border,
        image_h - 2,
        image_h / 3,
        0
    );
}
//右边界是否连续
static int right_border_is_continuous(uint8 *r_border)
{
    return border_is_continuous(
        r_border,
        image_h - 2,
        image_h / 3,
        0
    );
}
static int abs_int(int value)
{
    return value >= 0 ? value : -value;
}

// 判断右边界这一行是否有效
static int right_border_valid(uint8 *r_border, int y)
{
    if(y < 0 || y >= image_h)
    {
        return 0;
    }

    int x = r_border[y];

    // 贴最右边，说明右边线可能丢失
    if(x >= border_max - 2)
    {
        return 0;
    }

    // 右边界一般应该在图像右半边
    if(x <= image_w / 2)
    {
        return 0;
    }

    return 1;
}

static int find_right_break_y(uint8 *r_border, int *break_y)
{
    int valid_count = 0;
    int last_x = -1;

    const int min_valid_count = 6;
    const int max_jump = 12;

    for(int y = image_h - 2; y >= image_h / 4; y--)
    {
        int valid = right_border_valid(r_border, y);

        if(valid)
        {
            int x = r_border[y];

            // 如果右边界突然跳变，也认为这里附近出现断裂
            if(last_x >= 0 && abs_int(x - last_x) > max_jump && valid_count >= min_valid_count)
            {
                *break_y = y;
                return 1;
            }

            last_x = x;
            valid_count++;
        }
        else
        {
            // 下方已经连续有效，突然无效，认为这里是断裂处
            if(valid_count >= min_valid_count)
            {
                *break_y = y;
                return 1;
            }
        }
    }

    return 0;
}