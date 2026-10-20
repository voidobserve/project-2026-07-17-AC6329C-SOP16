#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "typedef.h"

enum
{
    MOTOR_MODE_STOP,             // 停止

    // 先正转，再反转
    MOTOR_MODE_FORWARD_THEN_REVERSE_ROTATION,
    // 先反转，再正转
    MOTOR_MODE_REVERSE_THEN_FORWARD_ROTATION,

    MOTOR_MODE_REVERSE_ROTATION, // 电机模式 反转
    MOTOR_MODE_FORWARD_ROTATION, // 电机模式 正转    

    MOTOR_MODE_SOUND_CONTROL, // 电机模式 声控
};
typedef u8 motor_mode_t;

typedef struct
{
    motor_mode_t mode;
    u8 speed_sec_per_round; // 转速，单位：秒/圈
    u8 phase;               // 在正反转期间使用
    u16 step;               // 控制整个电机的步进 0 ~ 4095

    // 声控模式使用
    u8 sound_trigger_flag;   // 声控信号触发标志，声控检测时置位，125us 处理函数消耗
    u32 sound_fast_hold_cnt; // 快速转动的剩余时间，单位：125us
} motor_t;

extern volatile motor_t motor;

/*
	调节电机转速时，每次调节的单位，单位：秒/圈
*/
#define MOTOR_ADJUST_UNIT 10

/*
	调节电机转速时，限制的最快转速

	由于客户选的电机质量比较差，不能达到理论的4秒转一圈，
	这里要实际测试客户的电机可以转多快，再给个保守值
*/
#define MOTOR_THE_FASTEST_SPEED_SEC_PER_ROUND 10

/*
	调节电机转速时，限制的最慢转速
*/
#define MOTOR_THE_SLOWEST_SPEED_SEC_PER_ROUND 70

/*
	声控模式下，检测到声控信号时快速转动的转速，单位：秒/圈
*/
#define MOTOR_SOUND_CONTROL_FAST_SPEED_SEC_PER_ROUND 8

/*
	声控模式下，没有声控信号时慢速转动的转速，单位：秒/圈
*/
#define MOTOR_SOUND_CONTROL_SLOW_SPEED_SEC_PER_ROUND 35

/*
	声控模式下，检测到声控信号后快速转动持续的时间，单位：ms

	期间持续检测到声控信号会不断重新计时，时间到了就回到慢速转动
*/
#define MOTOR_SOUND_CONTROL_FAST_HOLD_MS 4000

void motor_handle_125us(void);

// 设置电机模式
void motor_set_mode(motor_mode_t mode);
// 根据传参设置电机转速
void motor_set_speed_sec_per_round(u8 sec_per_round);
// 声控信号触发，检测到声音时调用，声控模式下会快速转动一段时间
void motor_sound_control_trigger(void);

#endif
