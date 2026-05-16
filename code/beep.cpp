#include "beep.hpp"
#include "zf_common_headfile.hpp"
#include "../user/interaction.hpp"


void beep_toggle(void)
{
    beep.set_level(1);
    system_delay_ms(50);

    beep.set_level(0);
    system_delay_ms(50);
}

void beep_on(void)
{
    beep.set_level(1);
}

void beep_off(void)
{
    beep.set_level(0);
}
