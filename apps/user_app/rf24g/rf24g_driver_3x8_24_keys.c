#include "rf24g_driver_3x8_24_keys.h"
#include "rf24g_driver.h"
#include <stdint.h>
#include "event.h" // key_driver 的按键事件定义
#include "key_driver.h"

#include "led_strand_effect.h"
#include "user_ble_notify_app.h"

#include "dev_config.h"

#if (USE_RF24G_REMOTE == RF24G_REMOTE_3x8_24_KEYS)

/*
    当r1c3按键触发时，设备是否处于开机状态

    用来修复的问题：设备处于关机状态时，如果长按r1c3，会先触发press事件进行开机，
    再触发long事件进行关机
*/
static uint8_t rf24g_r1c3_was_on_when_pressed;

const uint8_t rf24g_key_type_24keys_table[][RF24G_KEY_EVENT_MAX + 1] = {
    {RF24G_TYPE_24KEY_KEY_VAL_R1C1, RF24G_TYPE_24KEY_EVENT_R1C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R1C1_CLICK, RF24G_TYPE_24KEY_EVENT_R1C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R1C1_HOLD, RF24G_TYPE_24KEY_EVENT_R1C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R1C2, RF24G_TYPE_24KEY_EVENT_R1C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R1C2_CLICK, RF24G_TYPE_24KEY_EVENT_R1C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R1C2_HOLD, RF24G_TYPE_24KEY_EVENT_R1C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R1C3, RF24G_TYPE_24KEY_EVENT_R1C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R1C3_CLICK, RF24G_TYPE_24KEY_EVENT_R1C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R1C3_HOLD, RF24G_TYPE_24KEY_EVENT_R1C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R2C1, RF24G_TYPE_24KEY_EVENT_R2C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R2C1_CLICK, RF24G_TYPE_24KEY_EVENT_R2C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R2C1_HOLD, RF24G_TYPE_24KEY_EVENT_R2C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R2C2, RF24G_TYPE_24KEY_EVENT_R2C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R2C2_CLICK, RF24G_TYPE_24KEY_EVENT_R2C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R2C2_HOLD, RF24G_TYPE_24KEY_EVENT_R2C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R2C3, RF24G_TYPE_24KEY_EVENT_R2C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R2C3_CLICK, RF24G_TYPE_24KEY_EVENT_R2C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R2C3_HOLD, RF24G_TYPE_24KEY_EVENT_R2C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R3C1, RF24G_TYPE_24KEY_EVENT_R3C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R3C1_CLICK, RF24G_TYPE_24KEY_EVENT_R3C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R3C1_HOLD, RF24G_TYPE_24KEY_EVENT_R3C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R3C2, RF24G_TYPE_24KEY_EVENT_R3C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R3C2_CLICK, RF24G_TYPE_24KEY_EVENT_R3C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R3C2_HOLD, RF24G_TYPE_24KEY_EVENT_R3C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R3C3, RF24G_TYPE_24KEY_EVENT_R3C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R3C3_CLICK, RF24G_TYPE_24KEY_EVENT_R3C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R3C3_HOLD, RF24G_TYPE_24KEY_EVENT_R3C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R4C1, RF24G_TYPE_24KEY_EVENT_R4C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R4C1_CLICK, RF24G_TYPE_24KEY_EVENT_R4C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R4C1_HOLD, RF24G_TYPE_24KEY_EVENT_R4C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R4C2, RF24G_TYPE_24KEY_EVENT_R4C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R4C2_CLICK, RF24G_TYPE_24KEY_EVENT_R4C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R4C2_HOLD, RF24G_TYPE_24KEY_EVENT_R4C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R4C3, RF24G_TYPE_24KEY_EVENT_R4C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R4C3_CLICK, RF24G_TYPE_24KEY_EVENT_R4C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R4C3_HOLD, RF24G_TYPE_24KEY_EVENT_R4C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R5C1, RF24G_TYPE_24KEY_EVENT_R5C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R5C1_CLICK, RF24G_TYPE_24KEY_EVENT_R5C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R5C1_HOLD, RF24G_TYPE_24KEY_EVENT_R5C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R5C2, RF24G_TYPE_24KEY_EVENT_R5C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R5C2_CLICK, RF24G_TYPE_24KEY_EVENT_R5C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R5C2_HOLD, RF24G_TYPE_24KEY_EVENT_R5C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R5C3, RF24G_TYPE_24KEY_EVENT_R5C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R5C3_CLICK, RF24G_TYPE_24KEY_EVENT_R5C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R5C3_HOLD, RF24G_TYPE_24KEY_EVENT_R5C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R6C1, RF24G_TYPE_24KEY_EVENT_R6C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R6C1_CLICK, RF24G_TYPE_24KEY_EVENT_R6C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R6C1_HOLD, RF24G_TYPE_24KEY_EVENT_R6C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R6C2, RF24G_TYPE_24KEY_EVENT_R6C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R6C2_CLICK, RF24G_TYPE_24KEY_EVENT_R6C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R6C2_HOLD, RF24G_TYPE_24KEY_EVENT_R6C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R6C3, RF24G_TYPE_24KEY_EVENT_R6C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R6C3_CLICK, RF24G_TYPE_24KEY_EVENT_R6C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R6C3_HOLD, RF24G_TYPE_24KEY_EVENT_R6C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R7C1, RF24G_TYPE_24KEY_EVENT_R7C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R7C1_CLICK, RF24G_TYPE_24KEY_EVENT_R7C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R7C1_HOLD, RF24G_TYPE_24KEY_EVENT_R7C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R7C2, RF24G_TYPE_24KEY_EVENT_R7C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R7C2_CLICK, RF24G_TYPE_24KEY_EVENT_R7C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R7C2_HOLD, RF24G_TYPE_24KEY_EVENT_R7C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R7C3, RF24G_TYPE_24KEY_EVENT_R7C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R7C3_CLICK, RF24G_TYPE_24KEY_EVENT_R7C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R7C3_HOLD, RF24G_TYPE_24KEY_EVENT_R7C3_LOOSE},

    {RF24G_TYPE_24KEY_KEY_VAL_R8C1, RF24G_TYPE_24KEY_EVENT_R8C1_PRESS,
     RF24G_TYPE_24KEY_EVENT_R8C1_CLICK, RF24G_TYPE_24KEY_EVENT_R8C1_LONG,
     RF24G_TYPE_24KEY_EVENT_R8C1_HOLD, RF24G_TYPE_24KEY_EVENT_R8C1_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R8C2, RF24G_TYPE_24KEY_EVENT_R8C2_PRESS,
     RF24G_TYPE_24KEY_EVENT_R8C2_CLICK, RF24G_TYPE_24KEY_EVENT_R8C2_LONG,
     RF24G_TYPE_24KEY_EVENT_R8C2_HOLD, RF24G_TYPE_24KEY_EVENT_R8C2_LOOSE},
    {RF24G_TYPE_24KEY_KEY_VAL_R8C3, RF24G_TYPE_24KEY_EVENT_R8C3_PRESS,
     RF24G_TYPE_24KEY_EVENT_R8C3_CLICK, RF24G_TYPE_24KEY_EVENT_R8C3_LONG,
     RF24G_TYPE_24KEY_EVENT_R8C3_HOLD, RF24G_TYPE_24KEY_EVENT_R8C3_LOOSE},
};

// =========================================================================
// =========================================================================

typedef struct
{
    const uint32_t *colors;    // 本模式使用的颜色数组
    uint8_t color_count;       // 颜色数量
    change_type_e change_type; // 动态效果类型
} rf24g_scene_mode_t;

// 静态颜色模式，索引0~6依次对应红、蓝、绿、青、黄、紫、白
static const uint32_t rf24g_static_colors[] = {
    RED, BLUE, GREEN, CYAN, YELLOW, PURPLE, WHITE,
};

// 三色跳变/渐变和七色跳变/渐变使用的颜色顺序
static const uint32_t rf24g_three_colors[] = {
    BLUE,
    GREEN,
    RED,
};

static const uint32_t rf24g_seven_colors[] = {
    BLUE, GREEN, RED, WHITE, YELLOW, CYAN, PURPLE,
};

// 单色呼吸模式使用的颜色顺序
static const uint32_t rf24g_breath_colors[] = {
    RED, BLUE, GREEN, CYAN, YELLOW, PURPLE, WHITE, PURE_WHITE,
};

// 两色渐变模式的颜色组合
static const uint32_t rf24g_gradient_red_green[] = {RED, GREEN};
static const uint32_t rf24g_gradient_red_blue[] = {RED, BLUE};
static const uint32_t rf24g_gradient_green_blue[] = {GREEN, BLUE};

// 单色频闪模式的颜色顺序
static const uint32_t rf24g_strobe_colors[] = {
    RED, BLUE, GREEN, CYAN, YELLOW, PURPLE, WHITE,
};

// 动态模式配置表，对应模式索引7~30，顺序需与按键循环索引保持一致
static const rf24g_scene_mode_t rf24g_scene_modes[] = {
    // 索引7~10：三色/七色跳变、三色/七色渐变
    {rf24g_three_colors, 3, MODE_JUMP},
    {rf24g_seven_colors, 7, MODE_JUMP},
    {rf24g_three_colors, 3, MODE_MUTIL_C_GRADUAL},
    {rf24g_seven_colors, 7, MODE_MUTIL_C_GRADUAL},
    // 索引11~18：红、蓝、绿、青、黄、紫、白、纯白呼吸
    {&rf24g_breath_colors[0], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[1], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[2], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[3], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[4], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[5], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[6], 1, MODE_COLORFUL_BREATH},
    {&rf24g_breath_colors[7], 1, MODE_COLORFUL_BREATH},
    // 索引19~21：红绿、红蓝、绿蓝渐变
    {rf24g_gradient_red_green, 2, MODE_MUTIL_C_GRADUAL},
    {rf24g_gradient_red_blue, 2, MODE_MUTIL_C_GRADUAL},
    {rf24g_gradient_green_blue, 2, MODE_MUTIL_C_GRADUAL},
    // 索引22~29：七色频闪及红、蓝、绿、青、黄、紫、白单色频闪
    {rf24g_seven_colors, 7, MODE_STROBE},
    {&rf24g_strobe_colors[0], 1, MODE_STROBE},
    {&rf24g_strobe_colors[1], 1, MODE_STROBE},
    {&rf24g_strobe_colors[2], 1, MODE_STROBE},
    {&rf24g_strobe_colors[3], 1, MODE_STROBE},
    {&rf24g_strobe_colors[4], 1, MODE_STROBE},
    {&rf24g_strobe_colors[5], 1, MODE_STROBE},
    {&rf24g_strobe_colors[6], 1, MODE_STROBE},
    // 索引30：白色呼吸
    {&rf24g_breath_colors[6], 1, MODE_COLORFUL_BREATH},
};
 
// =========================================================================
// =========================================================================

uint8_t rf24g_convert_to_key_event(uint8_t key_driver_value,
                                   uint8_t key_driver_event)
{
    uint8_t key_event_index = 0;
    uint8_t i = 0;

    switch (key_driver_event) {
    case KEY_EVENT_PRESS:
        key_event_index = 1;
        break;
    case KEY_EVENT_CLICK:
        key_event_index = 2;
        break;
    case KEY_EVENT_LONG:
        key_event_index = 3;
        break;
    case KEY_EVENT_HOLD:
        key_event_index = 4;
        break;
    case KEY_EVENT_UP:
        key_event_index = 5;
        break;
    default:
        break;
    }

    if (key_event_index == 0 || key_driver_value == NO_KEY) {
        return RF24G_TYPE_24KEY_EVENT_NONE;
    }

    for (i = 0; i < sizeof(rf24g_key_type_24keys_table) /
                        sizeof(rf24g_key_type_24keys_table[0]);
         i++) {
        if (key_driver_value == rf24g_key_type_24keys_table[i][0]) {
            return rf24g_key_type_24keys_table[i][key_event_index];
        }
    }

    return RF24G_TYPE_24KEY_EVENT_NONE;
}

void rf24g_key_r1c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c1 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_sub();
        user_ble_notify_light_brightness();
    } else if (fc_effect.Now_state == IS_light_scene) {
        /*
            原本是固定为一个 较慢 的速度值
            但现在的工程里，只改变速度百分比值，具体的速度值根据不同的动画内部来决定
        */
        fc_effect.speed_percent = 20;
        user_ble_notify_light_speed();
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_sub();
        user_ble_notify_sensitive();
    }
}

void rf24g_key_r1c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c2 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_plus();
    } else if (fc_effect.Now_state == IS_light_scene) {
        /*
            原本是固定为一个 较快 的速度值
            但现在的工程里，只改变速度百分比值，具体的速度值根据不同的动画内部来决定
        */
        fc_effect.speed_percent = 80;
        user_ble_notify_light_speed();
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_plus();
        user_ble_notify_sensitive();
    }
}

void rf24g_key_r1c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c3 press\n");
#endif

    rf24g_r1c3_was_on_when_pressed = (fc_effect.on_off_flag == DEVICE_ON);

    // 关机状态下按下时立即开灯
    if (rf24g_r1c3_was_on_when_pressed == 0) {
        // 设备原本处于 关闭状态 时，再执行开灯
        soft_turn_on_the_light();
        user_ble_notify_light_pwr_sta();
    }
}

void rf24g_key_r1c3_long_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c3 long\n");
#endif

    // 长按事件 进行关灯
    if (rf24g_r1c3_was_on_when_pressed != 0 &&
        fc_effect.on_off_flag == DEVICE_ON) {
        // 本次按下前已开启时，长按关灯
        soft_rurn_off_lights();
        user_ble_notify_light_pwr_sta();
    }
}

void rf24g_key_r2c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r2c1 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 红色
    led_colorful_light_set_static_color(RED);
}

void rf24g_key_r2c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r2c2 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 绿色
    led_colorful_light_set_static_color(GREEN);
}

void rf24g_key_r2c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r2c3 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 蓝色
    led_colorful_light_set_static_color(BLUE);
}

void rf24g_key_r3c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c1 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 黄色
    led_colorful_light_set_static_color(YELLOW);
}

void rf24g_key_r3c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c2 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // CYAN
    led_colorful_light_set_static_color(CYAN);
}

void rf24g_key_r3c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c3 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // MAGENTA ，特点是红色和蓝色分量为最大值
    led_colorful_light_set_static_color(MAGENTA);
}

void rf24g_key_r4c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c1 press\n");
#endif

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // PURE_WHITE
    led_colorful_light_set_static_color(PURE_WHITE);
}

void rf24g_key_r4c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c2 press\n");
#endif

    uint32_t color =
        ((uint32_t)0xFF << 16) | ((uint32_t)180 << 8) | ((uint32_t)20);

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 模拟白光(日光？)
    led_colorful_light_set_static_color(color);
}



// 按配置写入颜色，并更新动态效果参数和当前场景状态
static void rf24g_apply_scene_mode(uint8_t mode_idx)
{
    const rf24g_scene_mode_t *mode = &rf24g_scene_modes[mode_idx - 7];

    for (uint8_t i = 0; i < mode->color_count; i++) {
        ls_set_color(i, mode->colors[i]);
    }

    fc_effect.dream_scene.change_type = mode->change_type;
    fc_effect.dream_scene.c_n = mode->color_count;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r4c3_press_handle(void)
{
    static u8 mode_idx;

#if USER_DEBUG_ENABLE
    printf("r4c3 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 动态模式切换，每次短按换一种动态模式
    // 索引值每次上电不带记忆，每次切换其他模式后不清空索引
    mode_idx++;
    // if (mode_idx >= 0x1C) {
    if (mode_idx > 30) {
        mode_idx = 0;
    }

#if USER_DEBUG_ENABLE
    printf("mode_idx == %u\n", (u16)mode_idx);
#endif

    if (mode_idx <
        sizeof(rf24g_static_colors) / sizeof(rf24g_static_colors[0])) {
        led_colorful_light_set_static_color(rf24g_static_colors[mode_idx]);
    } else if (mode_idx <= 30) {
        rf24g_apply_scene_mode(mode_idx);
    } else {
        mode_idx = 0;
        led_colorful_light_set_static_color(rf24g_static_colors[mode_idx]);
    }
}

void rf24g_key_r5c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c1 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 七色渐变
    ls_set_color(0, BLUE);
    ls_set_color(1, GREEN);
    ls_set_color(2, RED);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r5c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c2 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 七色循环呼吸
    ls_set_color(0, RED);
    ls_set_color(1, GREEN);
    ls_set_color(2, BLUE);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_COLORFUL_BREATH;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r5c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c3 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 七色跳变
    ls_set_color(0, BLUE);
    ls_set_color(1, GREEN);
    ls_set_color(2, RED);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_JUMP;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r6c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c1 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 只关闭电机，不影响下次上电的状态
    motor_set_mode(MOTOR_MODE_STOP);
}

void rf24g_key_r6c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c2 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 声控模式索引递减，索引头尾不循环
    if (fc_effect.music.m > 0) {
        fc_effect.music.m--;
    }

    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_ble_notify_sound_ctl_mode();
}

void rf24g_key_r6c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c3 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // 声控模式索引递增，索引头尾不循环
    // 0 -> 1 -> 2 -> 3 -> 3(最大只能到3)
    if (fc_effect.music.m < 3) {
        fc_effect.music.m++;
    }

    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_ble_notify_sound_ctl_mode();
}

void rf24g_key_r7c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c1 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 8;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

void rf24g_key_r7c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c2 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 13;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

void rf24g_key_r7c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c3 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 18;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

void rf24g_key_r8c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r8c1 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 21;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

void rf24g_key_r8c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r8c2 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 26;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

void rf24g_key_r8c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r8c3 press\n");
#endif

    if (0 == fc_effect.on_off_flag) {
        return;
    }

    fc_effect.motor_mode = MOTOR_MODE_FORWARD_ROTATION;
    fc_effect.motor_sec_per_round = 35;
    motor_set_mode(fc_effect.motor_mode);
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
}

const rf24_key_handle_func_t
    rf24g_key_type_24keys_handle_func_buff[RF24G_TYPE_24KEY_EVENT_MAX - 1] = {
        [RF24G_TYPE_24KEY_EVENT_R1C1_PRESS] = rf24g_key_r1c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R1C2_PRESS] = rf24g_key_r1c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R1C3_PRESS] = rf24g_key_r1c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R1C3_LONG] = rf24g_key_r1c3_long_handle,

        [RF24G_TYPE_24KEY_EVENT_R2C1_PRESS] = rf24g_key_r2c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R2C2_PRESS] = rf24g_key_r2c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R2C3_PRESS] = rf24g_key_r2c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R3C1_PRESS] = rf24g_key_r3c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R3C2_PRESS] = rf24g_key_r3c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R3C3_PRESS] = rf24g_key_r3c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R4C1_PRESS] = rf24g_key_r4c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R4C2_PRESS] = rf24g_key_r4c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R4C3_PRESS] = rf24g_key_r4c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R5C1_PRESS] = rf24g_key_r5c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R5C2_PRESS] = rf24g_key_r5c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R5C3_PRESS] = rf24g_key_r5c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R6C1_PRESS] = rf24g_key_r6c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R6C2_PRESS] = rf24g_key_r6c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R6C3_PRESS] = rf24g_key_r6c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R7C1_PRESS] = rf24g_key_r7c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R7C2_PRESS] = rf24g_key_r7c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R7C3_PRESS] = rf24g_key_r7c3_press_handle,

        [RF24G_TYPE_24KEY_EVENT_R8C1_PRESS] = rf24g_key_r8c1_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R8C2_PRESS] = rf24g_key_r8c2_press_handle,
        [RF24G_TYPE_24KEY_EVENT_R8C3_PRESS] = rf24g_key_r8c3_press_handle,
};

#endif
