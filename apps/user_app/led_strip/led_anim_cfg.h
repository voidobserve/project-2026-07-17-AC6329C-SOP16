#ifndef __LED_ANIM_CFG_H__
#define __LED_ANIM_CFG_H__

#include <stdint.h>

/*
    请求的最小帧间隔（ms）
    动画函数把它作为返回值，只是告诉框架"下一帧不要早于 20ms"，它并不等于真实帧间隔：
    ws2812fx_service() 由 user_main() 的循环调用，循环周期会被 BLE、2.4G 按键扫描、
    电机中断、声卡 ADC、printf 等负载拉长，真实帧间隔是 20ms 的若干倍不等。

    所以换算步长时必须用 ws2812fx2.c 测出来的真实帧间隔 g_anim_frame_ms，
    不能用这个常量：否则帧间隔一变长，动画就整体变慢，两台设备负载不同就会错位。
*/
#define LED_ANIM_FRAME_MS ((uint16_t)20)

/*
    真实帧间隔的合理上限（ms）
    超过这个值认为中间发生了长时间停顿（如写 flash），落回 LED_ANIM_FRAME_MS，
    避免一帧走掉一大截动画。
*/
#define LED_ANIM_FRAME_MAX_MS ((uint16_t)200)

/* 动画时长下限（ms），比一帧还短的时长没有意义 */
#define LED_ANIM_MIN_FRAME_MS ((uint16_t)20)

/*
    每帧步长的定点放大倍数。
    越慢的动画每帧走的步数越小，倍数太小会把相邻两档（10%）算成同一个值，
    4096 下最慢的档位（跳变 4s 一种颜色）每帧步长还有 20，10% 的差别也能区分出来。
*/
#define LED_ANIM_STEP_SCALE (4096)

/*
    仍使用 _seg->speed 的动画（app 端效果）统一使用的时长区间：
    速度 0% -> 5000ms，速度 100% -> 100ms（与原来的速度值区间一致）
*/
#define LED_ANIM_GENERIC_SLOW_MS ((uint16_t)5000)
#define LED_ANIM_GENERIC_FAST_MS ((uint16_t)100)

#endif
