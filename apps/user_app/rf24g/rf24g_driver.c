#include "rf24g_driver.h"
#include "event.h"   // key_driver 的按键事件定义
#include "ble_api.h" // adv_report_t

volatile rf24g_key_t rf24g_key;

// 作用域为内部的函数声明
static u8 __rf24g_get_key_val__(void);

volatile struct key_driver_para rf24g_driver_para = {
    .scan_time = RF24G_KEY_SCAN_TIME_MS, // 按键扫描频率, 单位: ms
    .last_key = NO_KEY, // 上一次get_value按键值, 初始化为NO_KEY;
    .filter_time = RF24G_KEY_SCAN_FILTER_TIME_MS, // 按键消抖延时;
    .long_time =
        RF24G_KEY_LONG_TIME_MS / RF24G_KEY_SCAN_TIME_MS, // 按键判定长按数量
    .hold_time = (RF24G_KEY_LONG_TIME_MS + RF24G_KEY_HOLD_TIME_MS) /
                 RF24G_KEY_SCAN_TIME_MS, // 按键判定HOLD数量
    .click_delay_time =
        RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS, // 按键被抬起后等待连击延时数量
    .key_type = KEY_DRIVER_TYPE_RF24GKEY,
    .get_value = __rf24g_get_key_val__,
};



static u8 __rf24g_get_key_val__(void)
{
    // 加入超时，防止丢包（超时时间与按键扫描时间有关）
    static volatile u8 timeout_cnt = 0;
    static u8 last_key_val =
        0; // 上一次按键键值，在超时时间内返回上一次按键键值

    if (rf24g_key.is_update) {
        rf24g_key.is_update = 0;
        /*
			2.4G接收可能会丢失100~200ms的数据包（响应会慢一些）
			值 == 20，10ms调用一次该函数，这里填充200ms的超时值
		*/
        timeout_cnt = 20;
        last_key_val = rf24g_key.key_val;
        return rf24g_key.key_val;
    }

    if (timeout_cnt) {
        // 如果还没有超时，返回上次检测到的键值
        timeout_cnt--;
        return last_key_val;
    }

    // 如果没有检测到按键，并且已经超时，返回 NO_KEY
    return NO_KEY;
}

void rf24g_driver_package_process(adv_report_t *adv_report)
{
    if (adv_report->length < 13) {
        // 长度不对，直接返回
        return;
    }

    if (!(adv_report->data[2] == RF24G_FIXVAL_1 &&
          adv_report->data[3] == RF24G_FIXVAL_2)) {
        // 固定值不对，直接返回
        return;
    }

    // TEST ONLY
    // if (adv_report->rssi >= -60) {
    //     put_buf(adv_report->data, adv_report->length);
    // }

    // 获取键值
    rf24g_key.key_val = (u8)adv_report->data[12];
    rf24g_key.is_update = 1;
}

/**
 * @brief 处理2.4G遥控器数据包
 *
 * 	在 le_gatt_client.c -> __resolve_adv_report() 中调用
 *
 */
void rf24g_driver_package_handle(void *ptr, u8 len)
{
    // printf_buf(ptr, len);

    if (len >= 10 &&
        ((rf24g_remote_package_t *)ptr)->fix_val_1 ==
            RF24G_FIXVAL_1 &&
        ((rf24g_remote_package_t *)ptr)->fix_val_2 ==
            RF24G_FIXVAL_2) {
        // printf_buf(ptr, len);
        // printf("key == %02x\n", (u16)((rf24g_remote_package_t*)ptr)->key_val);
        // printf("is_hold == %02x\n", (u16)((rf24g_remote_package_t*)ptr)->is_hold);

        rf24g_key.key_val = (u8)((rf24g_remote_package_t *)ptr)->key_val;
        rf24g_key.is_update = 1;
    }
}
