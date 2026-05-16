#include "zebra.hpp"
#include "image.hpp"


static int Zebra_Stripes_Flag = 0;
static int zebra_change_count = 0;
static int zebra_narrow_count = 0;


// 限幅函数
static int limit_int(int value, int min_value, int max_value)
{
    if(value < min_value)
    {
        value = min_value;
    }

    if(value > max_value)
    {
        value = max_value;
    }

    return value;
}



static int get_standard_road_width(int y)
{
    y = limit_int(y, 0, image_h - 1);

    /*
        简单线性估算：
        顶部约 image_w * 0.25
        底部约 image_w * 0.85

        如果你图像宽 160：
        顶部约 40
        底部约 136
    */
    int min_width = image_w / 4;
    int max_width = image_w * 85 / 100;

    int width = min_width + (max_width - min_width) * y / (image_h - 1);

    return width;
}


/*
    判断某一行左右边界是否可信
*/
static int border_is_valid(int y, uint8 *l_border, uint8 *r_border)
{
    if(y < 0 || y >= image_h)
    {
        return 0;
    }

    int left = l_border[y];
    int right = r_border[y];
    int width = right - left;

    if(left <= border_min + 2)
    {
        return 0;
    }

    if(right >= border_max - 2)
    {
        return 0;
    }

    if(width < image_w / 5)
    {
        return 0;
    }

    if(width > image_w - 5)
    {
        return 0;
    }

    return 1;
}


/*
    斑马线检测

    适配当前工程：
    bin_img   = bin_image
    l_border  = 左边界数组
    r_border  = 右边界数组

    检测逻辑：
    1. 在中下部区域找赛道宽度明显变窄的多行；
    2. 在变窄区域附近统计黑白跳变次数；
    3. 跳变次数足够多，认为是斑马线。
*/
void zebra_stripes_detect(uint8 (*bin_img)[image_w], uint8 *l_border, uint8 *r_border)
{
    Zebra_Stripes_Flag = 0;
    zebra_change_count = 0;
    zebra_narrow_count = 0;

    if(bin_img == NULL || l_border == NULL || r_border == NULL)
    {
        return;
    }

    int start_line = 0;
    int end_line = 0;

    /*
        原代码依赖：
        Search_Stop_Line >= 60
        Longest_White_Column 在中心
        Boundry_Start 靠下

        你当前八邻域没有这些变量。
        所以这里改成直接检查图像中下部区域。

        image_h = 120 时：
        y_start 大约 95
        y_end   大约 35
    */
    int y_start = image_h * 4 / 5;
    int y_end   = image_h / 3;

    y_start = limit_int(y_start, 0, image_h - 1);
    y_end   = limit_int(y_end,   0, image_h - 1);

    /*
        1. 找赛道明显变窄的行。
        斑马线区域因为有很多横向黑白条纹，会导致扫描出来的道路宽度变窄或不稳定。
    */
    for(int y = y_start; y >= y_end; y--)
    {
        if(!border_is_valid(y, l_border, r_border))
        {
            continue;
        }

        int road_width = r_border[y] - l_border[y];
        int standard_width = get_standard_road_width(y);

        /*
            原代码：
            if((Standard_Road_Wide[i] - Road_Wide[i]) > 10)

            这里保留这个思想。
            如果当前宽度比标准宽度小很多，认为这一行疑似斑马线。
        */
        if((standard_width - road_width) > image_w / 16)
        {
            zebra_narrow_count++;

            if(zebra_narrow_count >= 5)
            {
                start_line = y;
                break;
            }
        }
    }

    /*
        没有连续多行变窄，不认为是斑马线。
    */
    if(start_line == 0)
    {
        return;
    }

    /*
        2. 以变窄行为中心，划一个区域统计黑白跳变。
        原代码：
        start_line = start_line + 8;
        end_line = start_line - 15;
    */
    start_line = start_line + 8;
    end_line = start_line - 15;

    start_line = limit_int(start_line, 0, image_h - 1);
    end_line   = limit_int(end_line,   0, image_h - 1);

    if(start_line < end_line)
    {
        int temp = start_line;
        start_line = end_line;
        end_line = temp;
    }

    /*
        3. 在左右边界之间统计黑白跳变次数。
        斑马线的典型特征就是横向扫描时黑白跳变很多。
    */
    for(int y = start_line; y >= end_line; y--)
    {
        if(!border_is_valid(y, l_border, r_border))
        {
            continue;
        }

        int left = l_border[y];
        int right = r_border[y];

        left = limit_int(left, 0, image_w - 2);
        right = limit_int(right, 1, image_w - 1);

        if(right <= left + 2)
        {
            continue;
        }

        for(int x = left; x < right - 1; x++)
        {
            if(bin_img[y][x + 1] != bin_img[y][x])
            {
                zebra_change_count++;
            }
        }
    }

    /*
        原代码：
        if(change_count > 30)

        这里先保留 30。
        如果误识别多，就提高到 40 / 50。
        如果识别不到，就降低到 20。
    */
    if(zebra_change_count > 30)
    {
        Zebra_Stripes_Flag = 1;
    }
}


int zebra_stripes_is_found(void)
{
    return Zebra_Stripes_Flag;
}


int zebra_get_change_count(void)
{
    return zebra_change_count;
}


int zebra_get_narrow_count(void)
{
    return zebra_narrow_count;
}