#include "target.hpp"

static red_target_t red_target_result = {0};


static void rgb565_to_rgb888(uint16 pixel, int *r, int *g, int *b)
{
    int r5 = (pixel >> 11) & 0x1F;
    int g6 = (pixel >> 5)  & 0x3F;
    int b5 = pixel & 0x1F;

    *r = r5 * 255 / 31;
    *g = g6 * 255 / 63;
    *b = b5 * 255 / 31;
}


static int is_red_pixel(uint16 pixel)
{
    int r, g, b;

    rgb565_to_rgb888(pixel, &r, &g, &b);

    if(r > 120 &&
       g < 110 &&
       b < 110 &&
       r > g + 40 &&
       r > b + 40)
    {
        return 1;
    }

    return 0;
}


void red_target_detect_rgb565(uint16 *rgb_image)
{
    if(rgb_image == NULL)
    {
        red_target_result.found = 0;
        return;
    }

    int min_x = UVC_WIDTH - 1;
    int max_x = 0;
    int min_y = UVC_HEIGHT - 1;
    int max_y = 0;
    int red_count = 0;

    /*
        可以先全图搜索。
        如果后面误识别多，再限制 ROI。
        例如只检测图像上半部分或中间区域。
    */
    for(int y = 0; y < UVC_HEIGHT; y++)
    {
        for(int x = 0; x < UVC_WIDTH; x++)
        {
            uint16 pixel = rgb_image[y * UVC_WIDTH + x];

            if(is_red_pixel(pixel))
            {
                red_count++;

                if(x < min_x) min_x = x;
                if(x > max_x) max_x = x;
                if(y < min_y) min_y = y;
                if(y > max_y) max_y = y;
            }
        }
    }

    /*
        面积阈值：
        太小可能是噪声。
        这个值需要实车调。
    */
    if(red_count > 30)
    {
        red_target_result.found = 1;
        red_target_result.min_x = min_x;
        red_target_result.max_x = max_x;
        red_target_result.min_y = min_y;
        red_target_result.max_y = max_y;
        red_target_result.center_x = (min_x + max_x) / 2;
        red_target_result.center_y = (min_y + max_y) / 2;
        red_target_result.area = red_count;
    }
    else
    {
        red_target_result.found = 0;
        red_target_result.center_x = 0;
        red_target_result.center_y = 0;
        red_target_result.min_x = 0;
        red_target_result.max_x = 0;
        red_target_result.min_y = 0;
        red_target_result.max_y = 0;
        red_target_result.area = 0;
    }
}


red_target_t red_target_get_result(void)
{
    return red_target_result;
}