

#include "system/includes.h"

#include "task.h"
#include "event.h"
#include "rf24g.h"
#include "led_strip_sys.h"
#include "board_ac632n_demo_cfg.h"
#include "led_strand_effect.h"
// #include "tuya_ble_type.h"
// #include "one_wire.h"
#include "btstack/btstack_typedef.h"
#include "ble_multi_profile.h"
#include "att.h"
#include "asm/mcpwm.h"
// #if TCFG_RF24GKEY_ENABLE
// #include "ir_key_app.h"

#include "user_include.h"
#include "led_driver.h"
#include "rf24g_driver.h"

#include "led_strand_effect.h"
#include "user_report_app.h"

extern const rf24_key_handle_func_t
    rf24g_key_type_28keys_handle_func_buff[RF24G_TYPE_28KEY_EVENT_MAX]; // 变量声明

void rf24g_key_handle(void)
{
    u8 rf24g_key_event = 0;
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = NULL;

    rf24g_key_event = rf24g_convert_to_key_event(rf24g_key.driver_key_val,
                                                 rf24g_key.driver_key_event);
    rf24g_key.driver_key_val =
        NO_KEY; // 置为无效键值（由于扫描函数只更新，不会清除，在这里要清除）

    if (rf24g_key_event == RF24G_TYPE_28KEY_EVENT_NONE) {
        // 如果是无效的按键事件，直接返回
        return;
    }

    // printf("rf24g_key_event: %u\n", (u16)rf24g_key_event);

    // 调用按键事件对应的处理函数
    rf24g_key_handle_func_ptr =
        rf24g_key_type_28keys_handle_func_buff[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) {
        // 如果按键事件没有对应的处理函数，直接退出
        return;
    }

    // 直接调用对应的处理函数，这样需要每个处理函数内都要判断一下设备是否开机
    rf24g_key_handle_func_ptr();

    // 将变化的数据保存到flash
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
}

void rf24g_key_r1c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c1 press\n");
#endif

    // 静态模式下，调节亮度
    // 声控模式下，调节灵敏度

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_plus();
        user_report_brightness(fc_effect.b);
#if USER_DEBUG_ENABLE
        printf("fc_effect.b == %u\n", (u16)fc_effect.b);
#endif
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_plus();
        user_report_sound_control_sensitive(fc_effect.sound.sensitive);
#if USER_DEBUG_ENABLE
        printf("fc_effect.sound.sensitive == %u\n",
               (u16)fc_effect.sound.sensitive);
#endif
    }
}

void rf24g_key_r1c1_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c1 hold\n");
#endif

    // 静态模式下，调节亮度
    // 声控模式下，调节灵敏度

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_plus();
        user_report_brightness(fc_effect.b);
#if USER_DEBUG_ENABLE
        printf("fc_effect.b == %u\n", (u16)fc_effect.b);
#endif
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_plus();
        user_report_sound_control_sensitive(fc_effect.sound.sensitive);
#if USER_DEBUG_ENABLE
        printf("fc_effect.sound.sensitive == %u\n",
               (u16)fc_effect.sound.sensitive);
#endif
    }
}

void rf24g_key_r1c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c2 press\n");
#endif

    // 静态模式下，调节亮度
    // 声控模式下，调节灵敏度

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_sub();
        user_report_brightness(fc_effect.b);
#if USER_DEBUG_ENABLE
        printf("fc_effect.b == %u\n", (u16)fc_effect.b);
#endif
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_sub();
        user_report_sound_control_sensitive(fc_effect.sound.sensitive);
#if USER_DEBUG_ENABLE
        printf("fc_effect.sound.sensitive == %u\n",
               (u16)fc_effect.sound.sensitive);
#endif
    }
}

void rf24g_key_r1c2_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c2 hold\n");
#endif

    // 静态模式下，调节亮度
    // 声控模式下，调节灵敏度

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    if (fc_effect.Now_state == IS_STATIC) {
        bright_sub();
        user_report_brightness(fc_effect.b);
#if USER_DEBUG_ENABLE
        printf("fc_effect.b == %u\n", (u16)fc_effect.b);
#endif
    } else if (fc_effect.Now_state == IS_light_music) {
        ls_sensitive_sub();
        user_report_sound_control_sensitive(fc_effect.sound.sensitive);
#if USER_DEBUG_ENABLE
        printf("fc_effect.sound.sensitive == %u\n",
               (u16)fc_effect.sound.sensitive);
#endif
    }
}

void rf24g_key_r1c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c3 press\n");
#endif

    // 关机
    soft_rurn_off_lights();
}

void rf24g_key_r1c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r1c4 press\n");
#endif

    // 开机
    soft_turn_on_the_light();
}

void rf24g_key_r2c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r2c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
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

    if (fc_effect.on_off_flag == DEVICE_OFF) {
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

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 蓝色
    led_colorful_light_set_static_color(BLUE);
}

void rf24g_key_r2c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r2c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 纯白色
    led_colorful_light_set_static_color(PURE_WHITE);
}

void rf24g_key_r3c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
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

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 青色
    led_colorful_light_set_static_color(CYAN);
}

void rf24g_key_r3c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c3 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 紫色
    led_colorful_light_set_static_color(MAGENTA);
}

void rf24g_key_r3c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r3c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 纯白色灯、混白色灯，都只点亮RGB，构成混白色
    led_colorful_light_set_static_color(WHITE);
}

void rf24g_key_r4c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    /*
        AUTO 模式，在下面5个模式不断循环切换
        七色跳变 -> 七色滑翔 -> 七色呼吸 -> 连续渐变 -> 渐变带停顿 -> ...
    */
    ls_set_color(0, BLUE);
    ls_set_color(1, GREEN);
    ls_set_color(2, RED);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_COLORFUL_AUTO;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r4c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c2 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    /*
        七色跳变
    */
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

void rf24g_key_r4c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c3 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 连续渐变
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

void rf24g_key_r4c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 动画 速度加
    if (fc_effect.Now_state == IS_light_scene) {
        ls_speed_plus();
#if USER_DEBUG_ENABLE
        printf("fc_effect.dream_scene.speed == %u\n",
               fc_effect.dream_scene.speed);
        printf("fc_effect.report_speed == %u\n", (u16)fc_effect.report_speed);
#endif
        set_fc_effect();
        user_report_speed(fc_effect.report_speed);
    }
}

void rf24g_key_r4c4_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r4c4 hold\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 动画 速度加
    if (fc_effect.Now_state == IS_light_scene) {
        ls_speed_plus();
#if USER_DEBUG_ENABLE
        printf("fc_effect.dream_scene.speed == %u\n",
               fc_effect.dream_scene.speed);
        printf("fc_effect.report_speed == %u\n", (u16)fc_effect.report_speed);
#endif
        set_fc_effect();
        user_report_speed(fc_effect.report_speed);
    }
}

void rf24g_key_r5c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    if (fc_effect.Now_state == IS_light_scene) {
        // 从其他动态模式进入到这里时，是七色呼吸
        // 七色呼吸
        ls_set_color(0, BLUE);
        ls_set_color(1, GREEN);
        ls_set_color(2, RED);
        ls_set_color(3, WHITE);
        ls_set_color(4, YELLOW);
        ls_set_color(5, CYAN);
        ls_set_color(6, PURPLE);
        fc_effect.dream_scene.change_type = MODE_COLORFUL_BREATH;
        fc_effect.dream_scene.c_n = 7;
    } else {
        // 从非动态模式进入到这里时，是单色呼吸
        // 将静态模式赋值过的颜色放到动态模式对应的参数中
        fc_effect.dream_scene.rgb[0].r = fc_effect.rgb.r;
        fc_effect.dream_scene.rgb[0].g = fc_effect.rgb.g;
        fc_effect.dream_scene.rgb[0].b = fc_effect.rgb.b;
        fc_effect.dream_scene.rgb[0].w = fc_effect.rgb.w;

        fc_effect.dream_scene.change_type = MODE_COLORFUL_BREATH;
        fc_effect.dream_scene.c_n = 1;
    }

    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r5c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c2 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 七色滑翔
    ls_set_color(0, BLUE);
    ls_set_color(1, GREEN);
    ls_set_color(2, RED);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_COLORFUL_SLIDE;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r5c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c3 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 七色 渐变带停顿
    ls_set_color(0, BLUE);
    ls_set_color(1, GREEN);
    ls_set_color(2, RED);
    ls_set_color(3, WHITE);
    ls_set_color(4, YELLOW);
    ls_set_color(5, CYAN);
    ls_set_color(6, PURPLE);
    fc_effect.dream_scene.change_type = MODE_COLORFUL_GRADUAL_BY_PAUSE;
    fc_effect.dream_scene.c_n = 7;
    fc_effect.Now_state = IS_light_scene;
    set_fc_effect();
}

void rf24g_key_r5c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 动画速度减
    if (fc_effect.Now_state == IS_light_scene) {
        ls_speed_sub();
#if USER_DEBUG_ENABLE
        printf("fc_effect.dream_scene.speed == %u\n",
               fc_effect.dream_scene.speed);
        printf("fc_effect.report_speed == %u\n", (u16)fc_effect.report_speed);
#endif
        set_fc_effect();
        user_report_speed(fc_effect.report_speed);
    }
}

void rf24g_key_r5c4_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r5c4 hold\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 动画速度减
    if (fc_effect.Now_state == IS_light_scene) {
        ls_speed_sub();
#if USER_DEBUG_ENABLE
        printf("fc_effect.dream_scene.speed == %u\n",
               fc_effect.dream_scene.speed);
        printf("fc_effect.report_speed == %u\n", (u16)fc_effect.report_speed);
#endif
        set_fc_effect();
        user_report_speed(fc_effect.report_speed);
    }
}

void rf24g_key_r6c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 声控模式，没有声音时灭灯
    fc_effect.music.m = 3;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_report_sound_control_mode(fc_effect.music.m);
}

void rf24g_key_r6c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c2 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 声控模式，没有声音的时候渐变
    fc_effect.music.m = 0;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_report_sound_control_mode(fc_effect.music.m);
}

void rf24g_key_r6c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c3 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 声控模式，没有声音的时候跳变

    // 实际是有声音的时候跳变
    fc_effect.music.m = 2;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_report_sound_control_mode(fc_effect.music.m);
}

void rf24g_key_r6c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r6c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 声控模式，没有声音的时候呼吸
    fc_effect.music.m = 1;
    fc_effect.Now_state = IS_light_music;
    set_fc_effect();
    user_report_sound_control_mode(fc_effect.music.m);
}

void rf24g_key_r7c1_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c1 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // fc_effect.motor_sec_per_round = 70;
    // motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);

    // 电机速度 减（数值越大，转得越慢）
    if (fc_effect.motor_sec_per_round <
        (MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND - MOTOR_ADJUST_UNIT)) {
        fc_effect.motor_sec_per_round += MOTOR_ADJUST_UNIT;
    } else {
        fc_effect.motor_sec_per_round = MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND;
    }

    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);

#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

void rf24g_key_r7c1_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c1 hold\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 电机速度 减 （ 数值越大，转得越慢 ）
    if (fc_effect.motor_sec_per_round <
        MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND - MOTOR_ADJUST_UNIT) {
        fc_effect.motor_sec_per_round += MOTOR_ADJUST_UNIT;
    } else {
        fc_effect.motor_sec_per_round = MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND;
    }

    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);

#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

void rf24g_key_r7c2_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c2 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    fc_effect.motor_sec_per_round = 42;
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

void rf24g_key_r7c3_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c3 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    fc_effect.motor_sec_per_round = 30;
    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

void rf24g_key_r7c4_press_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c4 press\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 电机速度 加（数值越小，转得越快，但不能小于一定值）
    if (fc_effect.motor_sec_per_round >
        MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND + MOTOR_ADJUST_UNIT) {
        fc_effect.motor_sec_per_round -= MOTOR_ADJUST_UNIT;
    } else {
        fc_effect.motor_sec_per_round = MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND;
    }

    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);

#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

void rf24g_key_r7c4_hold_handle(void)
{
#if USER_DEBUG_ENABLE
    printf("r7c4 hold\n");
#endif

    if (fc_effect.on_off_flag == DEVICE_OFF) {
        return;
    }

    // 电机速度 加（数值越小，转得越快，但不能小于一定值）
    if (fc_effect.motor_sec_per_round >
        MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND + MOTOR_ADJUST_UNIT) {
        fc_effect.motor_sec_per_round -= MOTOR_ADJUST_UNIT;
    } else {
        fc_effect.motor_sec_per_round = MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND;
    }

    motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);

#if USER_DEBUG_ENABLE
    printf("fc_effect.motor_sec_per_round == %u\n",
           (u16)fc_effect.motor_sec_per_round);
#endif
}

const rf24_key_handle_func_t
    rf24g_key_type_28keys_handle_func_buff[RF24G_TYPE_28KEY_EVENT_MAX] = {
        [RF24G_TYPE_28KEY_EVENT_R1C1_PRESS] = rf24g_key_r1c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R1C2_PRESS] = rf24g_key_r1c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R1C3_PRESS] = rf24g_key_r1c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R1C4_PRESS] = rf24g_key_r1c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R1C1_HOLD] = rf24g_key_r1c1_hold_handle,
        [RF24G_TYPE_28KEY_EVENT_R1C2_HOLD] = rf24g_key_r1c2_hold_handle,

        [RF24G_TYPE_28KEY_EVENT_R2C1_PRESS] = rf24g_key_r2c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R2C2_PRESS] = rf24g_key_r2c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R2C3_PRESS] = rf24g_key_r2c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R2C4_PRESS] = rf24g_key_r2c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R3C1_PRESS] = rf24g_key_r3c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R3C2_PRESS] = rf24g_key_r3c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R3C3_PRESS] = rf24g_key_r3c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R3C4_PRESS] = rf24g_key_r3c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R4C1_PRESS] = rf24g_key_r4c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R4C2_PRESS] = rf24g_key_r4c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R4C3_PRESS] = rf24g_key_r4c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R4C4_PRESS] = rf24g_key_r4c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R4C4_HOLD] = rf24g_key_r4c4_hold_handle,

        [RF24G_TYPE_28KEY_EVENT_R5C1_PRESS] = rf24g_key_r5c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R5C2_PRESS] = rf24g_key_r5c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R5C3_PRESS] = rf24g_key_r5c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R5C4_PRESS] = rf24g_key_r5c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R5C4_HOLD] = rf24g_key_r5c4_hold_handle,

        [RF24G_TYPE_28KEY_EVENT_R6C1_PRESS] = rf24g_key_r6c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R6C2_PRESS] = rf24g_key_r6c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R6C3_PRESS] = rf24g_key_r6c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R6C4_PRESS] = rf24g_key_r6c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R7C1_PRESS] = rf24g_key_r7c1_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R7C2_PRESS] = rf24g_key_r7c2_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R7C3_PRESS] = rf24g_key_r7c3_press_handle,
        [RF24G_TYPE_28KEY_EVENT_R7C4_PRESS] = rf24g_key_r7c4_press_handle,

        [RF24G_TYPE_28KEY_EVENT_R7C1_HOLD] = rf24g_key_r7c1_hold_handle,
        [RF24G_TYPE_28KEY_EVENT_R7C4_HOLD] = rf24g_key_r7c4_hold_handle,

};
