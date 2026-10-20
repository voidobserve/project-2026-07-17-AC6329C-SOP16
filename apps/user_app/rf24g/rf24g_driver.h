#ifndef __RF24G_DRIVER_H__
#define __RF24G_DRIVER_H__

#include "typedef.h"
#include "key_driver.h" // key_driver_para 结构体类型定义
#include "ble_api.h"    // adv_report_t（rf24g_driver_package_process 形参类型）

#define RF24G_KEY_EVENT_MAX                                                    \
    (5) // 按键事件种类个数，刚按下、短按、长按、持续hold、松开

#define RF24G_KEY_SCAN_TIME_MS (10) // 2.4G遥控器按键扫描频率，单位：ms
#define RF24G_KEY_LONG_TIME_MS (1000)
#define RF24G_KEY_HOLD_TIME_MS (500)
#define RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS                                     \
    (0) // 2.4G遥控器按键被抬起后等待连击延时数量
#define RF24G_KEY_SCAN_FILTER_TIME_MS (0) // 2.4G遥控器按键消抖延时

// 客户的28键遥控器数据头
enum
{
    RF24G_FIXVAL_1 = 0x55,
    RF24G_FIXVAL_2 = 0xAA,
};



typedef struct
{
    u8 is_update; // 遥控器按键键值 (.key_val) 是否有更新
    u8 key_val;   // 遥控器按键键值（从2.4G遥控器广播的数据包获得）

    // 遥控器按键键值转换成应用层对应的键值，由 key_driver_scan() 更新
    u8 driver_key_val;
    u8 driver_key_event; // 由 key_driver_scan() 更新
} rf24g_key_t;

typedef struct
{
    u8 fix_val_1;
    u8 fix_val_2;
    u8 fix_val_3;

    u8 rolling_code_1; // 滚动码
    u8 fix_val_4;
    u8 fix_val_5;
    u8 rolling_code_2;
    u8 fix_val_8;
    u8 fix_val_9;
    u8 fix_val_10;

    u8 key_val; // 键值
    u8 is_hold; // 是否长按未松手
} rf24g_remote_package_t;

// 给【指向遥控器按键事件处理函数的指针类型】起别名


extern volatile struct key_driver_para rf24g_driver_para;
extern volatile rf24g_key_t rf24g_key;

void rf24g_driver_package_process(adv_report_t *adv_report);
// void rf24g_driver_package_handle(void *ptr, u8 len);



#endif