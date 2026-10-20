#ifndef __DEV_CONFIG_H__
#define __DEV_CONFIG_H__

#include "user_config.h"

#if USER_DEBUG_ENABLE
// 使用测试板对应的引脚来测试
#define USE_TEST_BOARD 1
#else
#define USE_TEST_BORAD 0
#endif

// 设备使用的遥控器定义
#define RF24G_REMOTE_4x7_28_KEYS 0x00 // 4 * 8，28键2.4G遥控器
#define RF24G_REMOTE_3x8_24_KEYS 0x01 // 3 * 8，24键2.4G遥控器
// 选择当前设备使用的遥控器类型(只能有一个被选中)
#define USE_RF24G_REMOTE RF24G_REMOTE_4x7_28_KEYS
// #define USE_RF24G_REMOTE RF24G_REMOTE_3x8_24_KEYS

#endif
