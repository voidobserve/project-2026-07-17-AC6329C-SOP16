

#include "system/includes.h"

#include "task.h"
#include "event.h"
#include "led_strip_sys.h"
#include "board_ac632n_demo_cfg.h"
#include "led_strand_effect.h"
#include "btstack/btstack_typedef.h"
#include "ble_multi_profile.h"
#include "att.h"
#include "asm/mcpwm.h"

#include "user_include.h"
#include "led_driver.h"
#include "rf24g_driver.h"
#include "rf24g_driver_4x7_28_keys.h"
#include "rf24g_driver_3x8_24_keys.h"

#include "led_strand_effect.h"
#include "user_ble_notify_app.h"
#if ENABLE_TEMP_MEASUREMENT
#include "ws2812_bsp.h"   // HAL_GetTick（临时测量用）
#include "system/timer.h" // sys_timer_get_ms（临时测量用）
#endif

void rf24g_key_handle(void)
{
#if USE_RF24G_REMOTE == RF24G_REMOTE_4x7_28_KEYS
    uint8_t rf24g_key_event = 0;
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = NULL;

    rf24g_key_event = rf24g_convert_to_key_event(rf24g_key.driver_key_val,
                                                 rf24g_key.driver_key_event);
    rf24g_key.driver_key_val = NO_KEY;

    if (rf24g_key_event == RF24G_TYPE_28KEY_EVENT_NONE) {
        return;
    }

    rf24g_key_handle_func_ptr =
        rf24g_key_type_28keys_handle_func_buff[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) {
        return;
    }

    rf24g_key_handle_func_ptr();
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
#elif USE_RF24G_REMOTE == RF24G_REMOTE_3x8_24_KEYS
    uint8_t rf24g_key_event = 0;
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = NULL;

    rf24g_key_event = rf24g_convert_to_key_event(rf24g_key.driver_key_val,
                                                 rf24g_key.driver_key_event);
    rf24g_key.driver_key_val = NO_KEY;

    if (rf24g_key_event == RF24G_TYPE_24KEY_EVENT_NONE) {
        return;
    }

#if ENABLE_TEMP_MEASUREMENT
    /* 临时测量（开关见 user_config.h 的 ENABLE_TEMP_MEASUREMENT）：
       记录按键事件被处理的时刻，用于和 flash 保存、动画错位的时间点在日志里对齐 */
    printf("key_event system_ms=%lu anim_ms=%lu event=%u\n",
           (unsigned long)sys_timer_get_ms(), (unsigned long)HAL_GetTick(),
           (u16)rf24g_key_event);
#endif

    rf24g_key_handle_func_ptr =
        rf24g_key_type_24keys_handle_func_buff[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) {
        return;
    }

    rf24g_key_handle_func_ptr();
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
#else
#error Unsupported USE_RF24G_REMOTE value
#endif
}
