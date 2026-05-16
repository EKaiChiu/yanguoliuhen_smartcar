#include "Key.hpp"
#include "zf_common_headfile.hpp"

#define KEY_1_PATH        ZF_GPIO_KEY_1
#define KEY_2_PATH        ZF_GPIO_KEY_2
#define KEY_3_PATH        ZF_GPIO_KEY_3
#define KEY_4_PATH        ZF_GPIO_KEY_4

static zf_driver_gpio  key_1(KEY_1_PATH, O_RDWR);
static zf_driver_gpio  key_2(KEY_2_PATH, O_RDWR);
static zf_driver_gpio  key_3(KEY_3_PATH, O_RDWR);
static zf_driver_gpio  key_4(KEY_4_PATH, O_RDWR);

static int test_count = 0;

static bool anyKeyPressed(void)
{
    return (key_1.get_level() == 0) || (key_2.get_level() == 0) || (key_3.get_level() == 0) || (key_4.get_level() == 0);
}

void Key_Process(void)
{
    if (anyKeyPressed())
    {
        test_count++;
    }
}

bool Key_IsPressed(void)
{
    return anyKeyPressed();
}

int Key_GetPressCount(void)
{
    return test_count;
}