#ifndef __LED_ANIM_UTILS_H__
#define __LED_ANIM_UTILS_H__

#include <stdint.h>

/*
    最近一次 WS2812FX_show() 的真实帧间隔（ms），由 ws2812fx2.c 维护。
    动画步长按它换算，才能和真实时间一一对应，不受循环周期/帧率变化影响。
*/
extern volatile uint16_t g_anim_frame_ms;

uint16_t led_anim_percent_to_ms(uint8_t speed_percent, uint16_t slow_ms,
                                uint16_t fast_ms);
uint16_t led_anim_step_per_frame(uint16_t total_steps, uint32_t cycle_ms);

#endif
