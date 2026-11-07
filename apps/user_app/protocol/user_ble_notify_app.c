#include "user_ble_notify_app.h"
#include "user_ble_notify.h"

#include "led_strip_drive.h"
#include "led_strand_effect.h"
#include "motor.h"

#include "user_rtc.h"

// notify 设备信息
void user_ble_notify_dev_type(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x07;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 0x02; // 灯具类型：设备固定为 RGBW

    user_ble_notify_param_put(buf, buf_len);
}

// notify 灯光电源状态
void user_ble_notify_light_pwr_sta(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = fc_effect.on_off_flag;

    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_light_brightness(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x03;
    buf[buf_len++] = get_brightness_percent();

    user_ble_notify_param_put(buf, buf_len);
}

// notify 速度百分比值
void user_ble_notify_light_speed(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = fc_effect.speed_percent;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_light_len(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x08;
    buf[buf_len++] = fc_effect.led_num;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_sensitive(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x05;
    buf[buf_len++] = 100 - fc_effect.sound.sensitive;
    user_ble_notify_param_put(buf, buf_len);
}

/**
 * @brief notify 电机转速（每圈秒数）
 * 
 * @note 当前工程不返回电机转速，因为跟app中的电机转速对不上，上下限不同
 * 
 */
void user_ble_notify_motor_speed(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x07;
    buf[buf_len++] = fc_effect.motor_sec_per_round;
    // byte 3： 电机转速最小值
    buf[buf_len++] = MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND;
    // byte 4： 电机转速最大值
    buf[buf_len++] = MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND;
    user_ble_notify_param_put(buf, buf_len);
}

// notify 电机模式
void user_ble_notify_motor_mode(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x06;
    buf[buf_len++] = fc_effect.motor_mode;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_rgb_sequence(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x05;
    buf[buf_len++] = fc_effect.sequence;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_sound_ctl_type(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x06;
    buf[buf_len++] = 0x07;
    buf[buf_len++] = fc_effect.music.m_type;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_sound_ctl_mode(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x06;
    buf[buf_len++] = 0x06;
    buf[buf_len++] = fc_effect.music.m;
    user_ble_notify_param_put(buf, buf_len);
}

// notify 流星灯电源状态
void user_ble_notify_meteor_pwr_sta(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x02;
    buf[buf_len++] = fc_effect.metemor_on_off;

    user_ble_notify_param_put(buf, buf_len);
}

// 流星灯速度
void user_ble_notify_meteor_speed(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 110 - (fc_effect.speed / 3);

    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_meteor_period(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x03;
    buf[buf_len++] = fc_effect.meteor_period;

    user_ble_notify_param_put(buf, buf_len);
}

#if 1
// notify 闹钟（定时）数据
void user_ble_notify_alarm_info(uint8_t alarm_index, user_alarm_t *alarm_info)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    if (alarm_index > 2) {
        return;
    }

    buf[buf_len++] = 0x05;
    buf[buf_len++] = alarm_index;
    buf[buf_len++] = alarm_info->hour;
    buf[buf_len++] = alarm_info->min;
    buf[buf_len++] = alarm_info->enable << 7;
    buf[buf_len++] = (alarm_info->power_on << 7) | (alarm_info->weekday);

    user_ble_notify_param_put(buf, buf_len);
}
#endif

// 同步结束指令
void user_ble_notify_syn_end(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 0x03;
    buf[buf_len++] = 0x00;

    user_ble_notify_param_put(buf, buf_len);
}

static volatile user_alarm_t app_msg_sync_alarm_buf[3];
// 一次性下发设备全部状态（0x0103 与 app 同步时调用）
void user_ble_notify_dev_all_info(void)
{
    // 都取模块级缓冲区，不占用 任务的栈
    user_alarm_t *alarm_info = app_msg_sync_alarm_buf;

    user_ble_notify_dev_type();
    user_ble_notify_light_pwr_sta();
    user_ble_notify_light_brightness();
    user_ble_notify_light_speed();
    user_ble_notify_light_len();
    user_ble_notify_sensitive();
    user_ble_notify_motor_speed();
    user_ble_notify_motor_mode();
    user_ble_notify_rgb_sequence();
    user_ble_notify_sound_ctl_type();
    user_ble_notify_sound_ctl_mode();
    user_ble_notify_meteor_pwr_sta();
    user_ble_notify_meteor_speed();
    user_ble_notify_meteor_period();

#if 1
    // 获取闹钟数据
    user_rtc_get_alarm_info(0, &alarm_info[0]);
    user_rtc_get_alarm_info(1, &alarm_info[1]);
    user_rtc_get_alarm_info(2, &alarm_info[2]);

    user_ble_notify_alarm_info(0, &alarm_info[0]);
    user_ble_notify_alarm_info(1, &alarm_info[1]);
    user_ble_notify_alarm_info(2, &alarm_info[2]);
#endif
}
