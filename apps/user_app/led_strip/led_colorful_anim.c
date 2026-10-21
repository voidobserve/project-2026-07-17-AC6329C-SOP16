#include "led_colorful_anim.h"
#include "WS2812FX.h"
#include "led_strand_effect.h" // 包含 fc_effect 定义
#include "ws2812fx_tool.h"
#include "Adafruit_NeoPixel.h" //
#include <math.h>

#include "led_anim_utils.h"
#include "led_anim_cfg.h"

#include "user_config.h"

/*
    28 键 2.4G 遥控器用到的七彩动画，速度只由 fc_effect.speed_percent（0 ~ 100）决定：
    每个动画在自己的函数上方用一对宏配置"时长区间"，速度 0% 取 SLOW，速度 100% 取 FAST，
    中间按百分比线性插值，所以每调节 10% 都会变快/变慢一档。

    APP 动态模式下的七彩动画也全部从这里取，不准再有第二套速度值：
    遥控的跳变/渐变和 APP 的跳变/渐变是同一个函数；APP 的单色呼吸就是 c_n = 1 的呼吸；
    APP 的频闪用下面的 led_colorful_anim_strobe()。速度区间只在本文件集中定义。

    动画统一按 LED_ANIM_FRAME_MS （20ms）请求下一帧，把"周期时长"换算成"每帧走多少步"再累加推进，
    慢速（一帧走不完一步）和快速（一帧走好几步）都能精确。

    换算用的是真实帧间隔 g_anim_frame_ms ，不是写死的 20ms：帧间隔被循环负载拉长时，
    动画速率仍然等于墙上时间，两台设备才不会跑出速度差。
*/

/* 七彩跳变：一种颜色停留的时间（ms），0% -> 4000ms，100% -> 200ms */
#define JUMP_COLOR_MS_SLOW (4000)
#define JUMP_COLOR_MS_FAST (200)

/* 连续渐变：一种颜色渐变到下一个颜色的时间（ms），0% -> 60000ms，100% -> 3000ms */
#define GRADUAL_COLOR_MS_SLOW (60000)
#define GRADUAL_COLOR_MS_FAST (3000)

/* 七彩呼吸：一次完整呼吸（渐亮 + 渐暗）的时间（ms），0% -> 12000ms，100% -> 1200ms */
#define BREATH_CYCLE_MS_SLOW (12000)
#define BREATH_CYCLE_MS_FAST (1200)

/*
    一次完整呼吸（0 -> 255 -> 0）的总步数。
    包络固定满量程，峰值亮度由 WS2812FX_setBrightness(fc_effect.b) 统一缩放一次；
    不能再用 fc_effect.b 当包络上限，否则 app 亮度会被缩放两次（峰值 b*b/255），
    比跳变/渐变/频闪暗一截。
*/
#define BREATH_TOTAL_STEPS (512)

/* 七彩频闪：一个完整亮灭周期（亮 + 灭）的时间（ms），0% -> 2000ms，100% -> 200ms */
#define STROBE_CYCLE_MS_SLOW (2000)
#define STROBE_CYCLE_MS_FAST (200)

/* 渐变带停顿：一种颜色渐变到下一个颜色的时间（ms），0% -> 20000ms，100% -> 4000ms */
#define PAUSE_COLOR_MS_SLOW (20000)
#define PAUSE_COLOR_MS_FAST (4000)

/* 渐变带停顿：一种颜色渐变完成后保持不动的时间（ms），0% -> 3000ms，100% -> 300ms */
#define PAUSE_HOLD_MS_SLOW (3000)
#define PAUSE_HOLD_MS_FAST (300)

/* 七色滑翔：转完一整圈（4 个色对，2π）的时间（ms），0% -> 50000ms，100% -> 5000ms */
#define SLIDE_CYCLE_MS_SLOW (50000)
#define SLIDE_CYCLE_MS_FAST (5000)

/* 一次渐变（0 -> 255 -> 0）的总步数 */
#define GRADUAL_TOTAL_STEPS (512)

/* 本帧是否跑完了一轮动画，自动模式靠它切换到下一个动画 */
static volatile u8 anim_cycle_done;

/* 按当前速度百分比，把 [slow_ms, fast_ms] 折算成实际时长 */
static u16 anim_ms(u16 slow_ms, u16 fast_ms)
{
    return led_anim_percent_to_ms(fc_effect.speed_percent, slow_ms, fast_ms);
}

/* 七彩跳变：每隔一段时间换一种颜色 */
u16 led_colorful_anim_jump(void)
{
    static u32 cycle_acc; // 累计时间（ms），够一个换色周期就换一种颜色
    u16 cycle_ms;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该动画
        _seg_rt->aux_param = 1;
        _seg_rt->counter_mode_step = 0;
        cycle_acc = 0;

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_jump: ms == %u\n",
               anim_ms(JUMP_COLOR_MS_SLOW, JUMP_COLOR_MS_FAST));
#endif
    }

    Adafruit_NeoPixel_fill(_seg->colors[_seg_rt->counter_mode_step],
                           _seg->start, _seg_len);

    /*
        这个动画一帧就是一步，直接用毫秒累加判断：
        按定点步长累加要每帧取整，慢速档（4s 换一种颜色）会有千分之几的速率误差
    */
    cycle_ms = anim_ms(JUMP_COLOR_MS_SLOW, JUMP_COLOR_MS_FAST);
    cycle_acc += g_anim_frame_ms;
    while (cycle_acc >= cycle_ms) {
        cycle_acc -= cycle_ms;

        _seg_rt->counter_mode_step++;
        _seg_rt->counter_mode_step %= _seg->c_n;
        if ((0 == _seg_rt->counter_mode_step) && (_seg->c_n > 1)) {
            anim_cycle_done = 1;
        }
    }

    return LED_ANIM_FRAME_MS;
}

/* 七彩灯的渐变动画，支持多种颜色切换 */
u16 led_colorful_anim_gradual(void)
{
    static u8 index;
    static u32 c0, c1;
    static u32 step_acc;
    u32 color;
    u16 lum;
    u16 step;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该函数
        _seg_rt->aux_param = 1;
        _seg_rt->counter_mode_step = 0;
        step_acc = 0;
        index = 0;
        c1 = _seg->colors[index];
        index++;
        c0 = _seg->colors[index];

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_gradual: ms == %u\n",
               anim_ms(GRADUAL_COLOR_MS_SLOW, GRADUAL_COLOR_MS_FAST));
#endif
    }

    step = led_anim_step_per_frame(
        GRADUAL_TOTAL_STEPS,
        anim_ms(GRADUAL_COLOR_MS_SLOW, GRADUAL_COLOR_MS_FAST));

    step_acc += step;
    while (step_acc >= LED_ANIM_STEP_SCALE) {
        step_acc -= LED_ANIM_STEP_SCALE;

        // c1：当前颜色，c0：目标颜色
        if (256 == _seg_rt->counter_mode_step) {
            index++;
            index %= _seg->c_n;
            c1 = _seg->colors[index];
        }

        _seg_rt->counter_mode_step++;
        if (_seg_rt->counter_mode_step > 511) {
            _seg_rt->counter_mode_step = 0;
            index++;
            index %= _seg->c_n;
            c0 = _seg->colors[index];
            if ((0 == index) && (_seg->c_n > 1)) {
                anim_cycle_done = 1;
            }
            SET_CYCLE;
        }
    }

    // lum：0 -> 255 -> 0
    lum = (u16)_seg_rt->counter_mode_step;
    if (lum > 255) {
        lum = 511 - lum;
    }

    color = WS2812FX_color_blend(c1, c0, (u8)lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    return LED_ANIM_FRAME_MS;
}

/**
 * @brief 七彩灯的呼吸动画
 *        一次完整呼吸 = 0 -> 255 -> 0（满量程包络），
 *        峰值亮度由 app 亮度经 WS2812FX_setBrightness 统一缩放，与跳变/渐变/频闪一致
 */
u16 led_colorful_anim_breath(void)
{
    static u8 color_index;
    static u32 step_acc;
    u16 step;
    u16 brightness;
    u32 color;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该函数
        _seg_rt->aux_param = 1;
        _seg_rt->counter_mode_step = 0;
        step_acc = 0;
        color_index = 0;

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_breath: ms == %u\n",
               anim_ms(BREATH_CYCLE_MS_SLOW, BREATH_CYCLE_MS_FAST));
#endif
    }

    if (_seg_rt->counter_mode_step >= BREATH_TOTAL_STEPS) {
        _seg_rt->counter_mode_step = 0;
    }

    step = led_anim_step_per_frame(
        BREATH_TOTAL_STEPS,
        anim_ms(BREATH_CYCLE_MS_SLOW, BREATH_CYCLE_MS_FAST));

    step_acc += step;
    while (step_acc >= LED_ANIM_STEP_SCALE) {
        step_acc -= LED_ANIM_STEP_SCALE;

        _seg_rt->counter_mode_step++;
        if (_seg_rt->counter_mode_step >= BREATH_TOTAL_STEPS) {
            _seg_rt->counter_mode_step = 0;

            // 一轮呼吸完成后换下一种颜色
            color_index++;
            if (color_index >= _seg->c_n) {
                color_index = 0;
                anim_cycle_done = 1;
            }
            SET_CYCLE;
        }
    }

    // 0 -> 255 -> 0
    brightness = (u16)_seg_rt->counter_mode_step;
    if (brightness > 255) {
        brightness = 511 - brightness;
    }

    color = WS2812FX_color_blend(BLACK, _seg->colors[color_index],
                                 (u8)brightness);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    return LED_ANIM_FRAME_MS;
}

/*
    七彩频闪：颜色池里的颜色逐个亮一次，每个颜色后面跟一段全黑（灭场）。
    一个完整亮灭周期（亮 + 灭）按速度百分比取 STROBE_CYCLE_MS_*，亮场和灭场各占半个周期。
*/
u16 led_colorful_anim_strobe(void)
{
    static u8 color_index; // 当前亮场显示的颜色
    static u8 phase_on;    // 1：亮场，0：灭场
    static u32 phase_acc;  // 当前相位已经累计的时间（ms）
    u16 half_ms;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该动画
        _seg_rt->aux_param = 1;
        _seg_rt->counter_mode_step = 0;
        phase_acc = 0;
        color_index = 0;
        phase_on = 1;

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_strobe: ms == %u\n",
               anim_ms(STROBE_CYCLE_MS_SLOW, STROBE_CYCLE_MS_FAST));
#endif
    }

    // 亮场和灭场各占半个周期
    half_ms = (u16)(anim_ms(STROBE_CYCLE_MS_SLOW, STROBE_CYCLE_MS_FAST) / 2);

    if (phase_on) {
        Adafruit_NeoPixel_fill(_seg->colors[color_index], _seg->start,
                               _seg_len);
    } else {
        Adafruit_NeoPixel_fill(BLACK, _seg->start, _seg_len);
    }

    /*
        相位按真实帧间隔推进（不用固定帧数），帧间隔被循环负载拉长时节奏仍然等于墙上时间；
        一帧跨过多次相位切换时用 while 补齐，保证不丢相位。
    */
    phase_acc += g_anim_frame_ms;
    while (phase_acc >= half_ms) {
        phase_acc -= half_ms;
        phase_on = !phase_on;

        if (phase_on) {
            // 一个颜色的亮场 + 灭场走完，换颜色池里的下一种颜色
            color_index++;
            if (color_index >= _seg->c_n) {
                // 颜色池用完，从头开始循环
                color_index = 0;
                anim_cycle_done = 1;
            }
            SET_CYCLE;
        }
    }

    return LED_ANIM_FRAME_MS;
}

/* 七彩灯动画：渐变带停顿 */
u16 led_colorful_anim_gradual_by_pause(void)
{
    static u8 index;
    static u32 c0, c1;
    static u32 step_acc;
    static u16 hold_ms; // 渐变成一种颜色后，保持不动的时间
    u32 color;
    u16 lum, step, cycle_ms;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该函数，初始化参数
        _seg_rt->aux_param = 1;
        _seg_rt->counter_mode_step = 0;
        step_acc = 0;
        hold_ms = 0;
        index = 0;
        c1 = _seg->colors[index];
        index++;
        c0 = _seg->colors[index];

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_gradual_by_pause: ms == %u\n",
               anim_ms(PAUSE_COLOR_MS_SLOW, PAUSE_COLOR_MS_FAST));
#endif
    }

    if (hold_ms > 0) {
        // 停顿期间画面保持不动，按真实帧间隔扣，否则帧间隔变长时停顿会跟着变久
        hold_ms = (hold_ms > g_anim_frame_ms)
                      ? (u16)(hold_ms - g_anim_frame_ms)
                      : 0;
        return LED_ANIM_FRAME_MS;
    }

    cycle_ms = anim_ms(PAUSE_COLOR_MS_SLOW, PAUSE_COLOR_MS_FAST);
    step = led_anim_step_per_frame(GRADUAL_TOTAL_STEPS, cycle_ms);

    step_acc += step;
    while (step_acc >= LED_ANIM_STEP_SCALE) {
        step_acc -= LED_ANIM_STEP_SCALE;

        if (256 == _seg_rt->counter_mode_step) {
            index++;
            index %= _seg->c_n;
            c1 = _seg->colors[index];
        }

        _seg_rt->counter_mode_step++;
        if (_seg_rt->counter_mode_step > 511) {
            _seg_rt->counter_mode_step = 0;
            index++;
            index %= _seg->c_n;
            c0 = _seg->colors[index];
            if ((0 == index) && (_seg->c_n > 1)) {
                anim_cycle_done = 1;
            }
            // 变化完成一个颜色后，停顿一段时间，再变换下一个颜色
            hold_ms = anim_ms(PAUSE_HOLD_MS_SLOW, PAUSE_HOLD_MS_FAST);
            break;
        }
    }

    lum = (u16)_seg_rt->counter_mode_step;
    if (lum > 255) {
        lum = 511 - lum;
    }

    color = WS2812FX_color_blend(c1, c0, (u8)lum);
    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    return LED_ANIM_FRAME_MS;
}

/* 七彩灯的七色滑翔动画 */
u16 led_colorful_anim_slide(void)
{
    static u8 next_idx;     // 下一个颜色索引
    static float radian;    // 正弦角度（弧度值），范围 [0, 2PI)
    static float radian_cnt;
    static u32 color_0;
    static u32 color_1;
    static u8 dir; // 亮度变化方向，0：渐亮，1：渐暗

    const float TWO_PI = 6.28318530717958647692f;

    u32 color;
    u16 blend;
    u16 cycle_ms;
    float delta;
    float t;

    if (0 == _seg_rt->aux_param) {
        // 刚进入该动画
        _seg_rt->aux_param = 1;
        next_idx = (_seg->c_n > 1) ? 1 : 0;
        radian = 0.0f;
        radian_cnt = 0.0f;
        dir = 0;
        color_0 = _seg->colors[0];
        color_1 = _seg->colors[next_idx % _seg->c_n];

#if USER_DEBUG_ENABLE
        printf("led_colorful_anim_slide: ms == %u\n",
               anim_ms(SLIDE_CYCLE_MS_SLOW, SLIDE_CYCLE_MS_FAST));
#endif
    }

    cycle_ms = anim_ms(SLIDE_CYCLE_MS_SLOW, SLIDE_CYCLE_MS_FAST);
    // 一整圈 2π 分 cycle_ms / 实际帧间隔 步走完
    delta = TWO_PI / ((float)cycle_ms / (float)g_anim_frame_ms);

    /* 取当前两色并按正弦插值计算混合比例（0..255） */
    t = sinf(radian);
    t = (t > 0) ? t : -t;
    blend = (u16)(t * 255.0f); // t 从 0 -> 1 -> 0时，blend 从 0 -> 255 -> 0

    if (0 == dir) {
        // blend 从 0 -> 255
        color = WS2812FX_color_blend(color_0, color_1, (u8)blend);
    } else {
        // blend 从 255 -> 0，颜色 传参顺序要反过来
        color = WS2812FX_color_blend(color_1, color_0, (u8)blend);
    }

    Adafruit_NeoPixel_fill(color, _seg->start, _seg_len);

    /* 前进角度，完成 1/4 圈后切换到下一对颜色 */
    radian += delta;
    radian_cnt += delta;

    if ((radian_cnt >= (TWO_PI / 4)) || ((0 == dir) && (255 == blend)) ||
        ((1 == dir) && (0 == blend))) {
        radian_cnt = 0;
        dir = !dir;

        next_idx = (next_idx + 1) % _seg->c_n;
        color_0 = color; // 从当前颜色开始往下一个颜色变化，防止颜色骤变
        color_1 = _seg->colors[next_idx];

        if ((0 == next_idx) && (_seg->c_n > 1)) {
            anim_cycle_done = 1;
        }
    }

    if (radian >= TWO_PI) {
        radian = 0;
        radian_cnt = 0;
    }

    return LED_ANIM_FRAME_MS;
}

enum
{
    ANIM_INDEX_JUMP = 0,         // 跳变
    ANIM_INDEX_SLIDE,            // 滑翔
    ANIM_INDEX_BREATH,           // 呼吸
    ANIM_INDEX_GRADUAL,          // 渐变
    ANIM_INDEX_GRADUAL_BY_PAUSE, // 带停顿的渐变
    ANIM_INDEX_MAX,
};
typedef u8 anim_index_e;

/**
 * @brief 七彩灯的自动模式
 *        跳变 -> 滑翔 -> 呼吸 -> 渐变 -> 渐变带停顿 -> ... 循环，
 *        每个动画跑完一轮就换下一个，速度沿用各动画自己的配置
 */
u16 led_colorful_anim_auto(void)
{
    static anim_index_e anim_index;
    u16 delay;

    if (0 == _seg_rt->counter_mode_call) {
        // 刚进入该动画
        anim_index = ANIM_INDEX_JUMP;
        _seg_rt->aux_param = 0;
        _seg_rt->counter_mode_step = 0;

#if USER_DEBUG_ENABLE
        printf("anim begin\n");
#endif
    }

    anim_cycle_done = 0;

    if (_seg->c_n < 2) {
        // 只有一种颜色时，跳变/滑翔没有意义，直接按呼吸跑
        return led_colorful_anim_breath();
    }

    switch (anim_index) {
    case ANIM_INDEX_JUMP:
        delay = led_colorful_anim_jump();
        break;
    case ANIM_INDEX_SLIDE:
        delay = led_colorful_anim_slide();
        break;
    case ANIM_INDEX_BREATH:
        delay = led_colorful_anim_breath();
        break;
    case ANIM_INDEX_GRADUAL:
        delay = led_colorful_anim_gradual();
        break;
    case ANIM_INDEX_GRADUAL_BY_PAUSE:
        delay = led_colorful_anim_gradual_by_pause();
        break;
    default:
        anim_index = ANIM_INDEX_JUMP;
        delay = led_colorful_anim_jump();
        break;
    }

    if (anim_cycle_done) {
        // 当前动画跑完一轮，切换到下一个动画
        anim_cycle_done = 0;
        anim_index++;
        if (anim_index >= ANIM_INDEX_MAX) {
            anim_index = ANIM_INDEX_JUMP;
        }

        // 让下一个动画重新初始化自己的参数
        _seg_rt->aux_param = 0;
        _seg_rt->counter_mode_step = 0;
    }

    return delay;
}
