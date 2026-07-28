
#include "ws2812_bsp.h"
#include "debug.h"
#include "my_effect.h"

#include "led_driver.h"

#include "led_strand_effect.h"

static unsigned long tick_ms;
void ws281x_init()
{

}
extern uint8_t           rOffset;    ///< Red index within each 3- or 4-byte pixel
extern uint8_t           gOffset;    ///< Index of green byte
extern uint8_t           bOffset;    ///< Index of blue byte
extern u8 send_rgbbuf_flag;
u8 buf[12 * 3];   //使用中断实现效果时，必须需要全局变量


/**
 * @brief 灯具的驱动集成，包括七彩的驱动，幻彩的驱动
 *
 * @param pixels_pattern   颜色
 * @param pattern_size     灯具的长度
 */
void ws281x_show(unsigned char* pixels_pattern, unsigned short pattern_size)
{
    // printf_buf(pixels_pattern, pattern_size);

    led_driver_set_rgbw_pwm_val(
        pixels_pattern[0],
        pixels_pattern[1],
        pixels_pattern[2],
        pixels_pattern[3]
    );
}

// 周期10ms
unsigned long HAL_GetTick(void)
{
    return tick_ms;
}

// 每10ms调用一次
void run_tick_per_10ms(void)
{
    tick_ms += 10;
}




