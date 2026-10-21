#include "led_anim_utils.h"
#include "led_anim_cfg.h"

/**
 * @brief 速度百分比换算成动画时长
 *        速度 0% -> slow_ms，速度 100% -> fast_ms，中间线性插值，
 *        所以每调节 10% 都会得到一个新的时长
 *
 * @param speed_percent 速度百分比（0 ~ 100）
 * @param slow_ms 速度 0% 时的时长（ms）
 * @param fast_ms 速度 100% 时的时长（ms）
 */
uint16_t led_anim_percent_to_ms(uint8_t speed_percent, uint16_t slow_ms,
                                uint16_t fast_ms)
{
    uint32_t speed = speed_percent;
    uint32_t ms;

    // 无符号运算，slow_ms < fast_ms 时会下溢成一个很大的数，这里先兜底
    if (slow_ms <= fast_ms) {
        return fast_ms;
    }

    if (speed > 100) {
        speed = 100;
    }

    ms = (uint32_t)slow_ms - (uint32_t)(slow_ms - fast_ms) * speed / 100;
    if (ms < LED_ANIM_MIN_FRAME_MS) {
        ms = LED_ANIM_MIN_FRAME_MS;
    }

    return (uint16_t)ms;
}

/**
 * @brief 一个周期共 total_steps 步、时长 cycle_ms 时，每帧要走多少步
 *        返回值放大了 LED_ANIM_STEP_SCALE 倍，调用方用累加器判断够不够走一步，
 *        这样慢速动画（一帧走不完一步）和快速动画（一帧走好几步）都能精确
 *
 *        注意：这里用的是真实帧间隔 g_anim_frame_ms，不是 LED_ANIM_FRAME_MS。
 *        帧间隔会被循环负载拉长，用固定值折算会让动画整体变慢，
 *        而且两台设备负载不同就会跑出不同速度，时间一长就错位。
 *
 * @param total_steps 一个周期总共多少步
 * @param cycle_ms 一个周期的时长（ms）
 */
uint16_t led_anim_step_per_frame(uint16_t total_steps, uint32_t cycle_ms)
{
    uint32_t step;

    if (0 == cycle_ms) {
        cycle_ms = 1;
    }

    step = (uint32_t)total_steps * g_anim_frame_ms * LED_ANIM_STEP_SCALE /
           cycle_ms;
    if (step < 1) {
        step = 1;
    }

    return (uint16_t)step;
}
