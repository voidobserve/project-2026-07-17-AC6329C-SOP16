#include "user_ble_notify_app.h"
#include "user_ble_notify.h"

#include "led_strip_drive.h"
#include "led_strand_effect.h"
#include "motor.h"

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
    buf[buf_len++] = fc_effect.b * 100 / 255;

    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_light_speed(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = fc_effect.report_speed;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_light_len(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x08;
    buf[buf_len++] = fc_effect.led_num >> 8;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_sensitive(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x05;
    buf[buf_len++] = fc_effect.sound.sensitive;
    user_ble_notify_param_put(buf, buf_len);
}

void user_ble_notify_light_color(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x04;
    buf[buf_len++] = 0x01;
    buf[buf_len++] = 0x1e;
    buf[buf_len++] = fc_effect.rgb.r;
    buf[buf_len++] = fc_effect.rgb.g;
    buf[buf_len++] = fc_effect.rgb.b;
    user_ble_notify_param_put(buf, buf_len);
}

// notify 电机转速（每圈秒数）
void user_ble_notify_motor_speed(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x07;
    buf[buf_len++] = fc_effect.motor_sec_per_round;
    user_ble_notify_param_put(buf, buf_len);
}

// notify 电机模式
void user_ble_notify_motor_mode(void)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    buf[buf_len++] = 0x2F;
    buf[buf_len++] = 0x06;
    switch (fc_effect.motor_mode) {
    case MOTOR_MODE_STOP:
        buf[buf_len++] = 0x00;
        break;
    case MOTOR_MODE_FORWARD_ROTATION:
        buf[buf_len++] = 0x02;
        break;
    case MOTOR_MODE_REVERSE_ROTATION:
        buf[buf_len++] = 0x01;
        break;
    case MOTOR_MODE_FORWARD_THEN_REVERSE_ROTATION:
        buf[buf_len++] = 0x03;
        break;
    case MOTOR_MODE_REVERSE_THEN_FORWARD_ROTATION:
        buf[buf_len++] = 0x04;
        break;
    case MOTOR_MODE_SOUND_CONTROL:
        buf[buf_len++] = 0x05;
        break;
    default:
        buf[buf_len++] = 0x00;
        break;
    }
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

void user_ble_notify_sound_mode(void)
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

// notify 闹钟（定时）数据
void user_ble_notify_alarm_data(unsigned char alarm_index)
{
    u8 buf[10] = {0};
    u8 buf_len = 0;
    if (alarm_index > 2) {
        return;
    }
    buf[buf_len++] = 0x05;
    buf[buf_len++] = alarm_index;
    buf[buf_len++] = alarm_clock[alarm_index].hour;
    buf[buf_len++] = alarm_clock[alarm_index].minute;
    buf[buf_len++] = alarm_clock[alarm_index].on_off;
    buf[buf_len++] = alarm_clock[alarm_index].mode;

    user_ble_notify_param_put(buf, buf_len);
}

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

// 一次性下发设备全部状态（0x0103 与 app 同步时调用）
void user_ble_notify_dev_all_info(void)
{
    user_ble_notify_dev_type();
    user_ble_notify_light_pwr_sta();
    user_ble_notify_light_brightness();
    user_ble_notify_light_speed();
    user_ble_notify_light_len();
    user_ble_notify_sensitive();
    user_ble_notify_light_color();
    user_ble_notify_motor_speed();
    user_ble_notify_motor_mode();
    user_ble_notify_rgb_sequence();
    user_ble_notify_sound_ctl_type();
    user_ble_notify_sound_mode();
    user_ble_notify_meteor_pwr_sta();
    user_ble_notify_meteor_speed();
    user_ble_notify_meteor_period();
    user_ble_notify_alarm_data(0);
    user_ble_notify_alarm_data(1);
    user_ble_notify_alarm_data(2);
    user_ble_notify_syn_end();
}
