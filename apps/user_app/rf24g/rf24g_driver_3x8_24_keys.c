#include "rf24g_driver_3x8_24_keys.h"
#include "rf24g_driver.h"
#include <stdint.h>
#include "event.h" // key_driver 的按键事件定义
#include "key_driver.h"

#include "led_strand_effect.h"
#include "user_ble_notify_app.h"

#if USE_RF24G_REMOTE == RF24G_REMOTE_3x8_24_KEYS

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
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_sub();
        user_ble_notify_light_brightness();
    } else if (fc_effect.Now_state == IS_light_scene) {
        // 动画速度 减
        ls_speed_sub();
        user_ble_notify_light_speed();
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_sub();
        user_ble_notify_sensitive();
    }
}

void rf24g_key_r1c2_press_handle(void)
{
    if (0 == fc_effect.on_off_flag) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_plus();
    } else if (fc_effect.Now_state == IS_light_scene) {
        // 动画速度 加
        ls_speed_plus();
        user_ble_notify_light_speed();
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_plus();
        user_ble_notify_sensitive();
    }
}

void rf24g_key_r1c3_press_handle(void)
{
    // 短按时间 进行开灯
    if (0 == fc_effect.on_off_flag) {
        soft_turn_on_the_light();
    }
}

void rf24g_key_r1c3_long_handle(void)
{
    // 长按事件 进行关灯
    if (fc_effect.on_off_flag == DEVICE_ON) {
        soft_rurn_off_lights();
    }
}

void rf24g_key_r2c1_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 红色
    led_colorful_light_set_static_color(RED);
}

void rf24g_key_r2c2_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 绿色
    led_colorful_light_set_static_color(GREEN);
}

void rf24g_key_r2c3_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 蓝色
    led_colorful_light_set_static_color(BLUE);
}

void rf24g_key_r3c1_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 黄色
    led_colorful_light_set_static_color(YELLOW);
}

void rf24g_key_r3c2_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // CYAN
    led_colorful_light_set_static_color(CYAN);
}

void rf24g_key_r3c3_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // MAGENTA ，特点是红色和蓝色分量为最大值
    led_colorful_light_set_static_color(MAGENTA);
}

void rf24g_key_r4c1_press_handle(void)
{
    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // PURE_WHITE
    led_colorful_light_set_static_color(PURE_WHITE);
}

void rf24g_key_r4c2_press_handle(void)
{
    uint32_t color =
        ((uint32_t)0xFF << 16) | ((uint32_t)180 << 8) | ((uint32_t)20);

    if (DEVICE_OFF == fc_effect.on_off_flag) {
        return;
    }

    // 模拟白光(日光？)
    led_colorful_light_set_static_color(color);
}

void rf24g_key_r4c3_press_handle(void)
{
    if (0 == fc_effect.on_off_flag) {
        return;
    }

    // USER_TO_DO 动态模式切换，每次短按换一种动态模式
}

void rf24g_key_r5c1_press_handle(void)
{
}

void rf24g_key_r5c2_press_handle(void)
{
}

void rf24g_key_r5c3_press_handle(void)
{
}
void rf24g_key_r6c1_press_handle(void)
{
}
void rf24g_key_r6c2_press_handle(void)
{
}
void rf24g_key_r6c3_press_handle(void)
{
}
void rf24g_key_r7c1_press_handle(void)
{
}
void rf24g_key_r7c2_press_handle(void)
{
}
void rf24g_key_r7c3_press_handle(void)
{
}
void rf24g_key_r8c1_press_handle(void)
{
}
void rf24g_key_r8c2_press_handle(void)
{
}
void rf24g_key_r8c3_press_handle(void)
{
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
