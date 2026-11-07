#ifndef __USER_BLE_NOTIFY_APP_H__
#define __USER_BLE_NOTIFY_APP_H__

#include "user_rtc.h"

void user_ble_notify_dev_type(void);
void user_ble_notify_light_pwr_sta(void);
void user_ble_notify_light_brightness(void);
void user_ble_notify_light_speed(void);
void user_ble_notify_light_len(void);
void user_ble_notify_sensitive(void);
void user_ble_notify_motor_speed(void);
void user_ble_notify_motor_mode(void);
void user_ble_notify_rgb_sequence(void);
void user_ble_notify_sound_ctl_type(void);
void user_ble_notify_sound_ctl_mode(void);
void user_ble_notify_meteor_pwr_sta(void);
void user_ble_notify_meteor_speed(void);
void user_ble_notify_meteor_period(void);
#if 1
void user_ble_notify_alarm_info(uint8_t alarm_index, user_alarm_t *alarm_info);
#endif
void user_ble_notify_syn_end(void);

void user_ble_notify_dev_all_info(void);

#endif
