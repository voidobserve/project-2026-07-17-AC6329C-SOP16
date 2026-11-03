#include "user_include.h"
#include "includes.h"

#include "motor_driver.h"
#include "led_driver.h"
#include "sound_control.h"
#include "save_flash.h"

#include "led_strand_effect.h"
#include "rf24g_app.h"
#include "user_config.h"
#include "user_ble_notify.h"

#include "dev_config.h"
#include "ws2812_bsp.h" // HAL_GetTick / run_tick_per_10ms
#if ENABLE_TEMP_MEASUREMENT
#include "system/timer.h" // sys_timer_get_ms（临时测量用）
#endif

void user_msg_handle_task(void *p);
void user_main(void *p);
void user_notify_task(void *p);

// 声控检测结果，1：检测到声音（麦克风 ADC 采样，按灵敏度判断）
extern u8 get_sound_result(void);

void user_init(void)
{
    extern void led_state_init(void);
    led_state_init(); // 初始化 LEDC（流星灯驱动引脚），必须在 full_color_init 之前
    led_driver_init();
#if !USE_TEST_BOARD
    motor_24byj48_init();
    motor_init();
#endif
    sound_control_init();

    user_data_init();

    full_color_init();

    sys_s_hi_timer_add(NULL, user_10ms_isr, 10);

    task_create(user_msg_handle_task, NULL, "msg_task");
    task_create(user_notify_task, NULL, "user_notify_task");
    task_create(user_main, NULL, "usr_main");

    printf("user_init\n");
}

// BLE notify 状态反馈线程：把 user_ble_notify 缓冲中的指令经 fff1 逐条发出
void user_notify_task(void *p)
{
    while (1) {
        user_ble_notify_param_handle();
        os_time_dly(1);
    }
}

void user_msg_handle_task(void *p)
{
    int msg[32] = {0};

    while (1) {
#if 1
        int ret = os_taskq_pend("msg_task", msg, 1);
        // printf("recv msg\n");
        // printf("ret %d\n", ret);
        if (OS_TASKQ != ret) // 类型不对
        {
            continue;
        }

        if (msg[0] != Q_USER) // 不是用户消息
        {
            continue;
        }

        // 打印接收到的消息
        // for (u8 i =0; i < ARRAY_SIZE(msg); i++)
        // {
        //     printf("msg [%u]: %d\n", (u16)i, msg[i]);
        // }

        switch (msg[1]) {
        case MSG_USER_SAVE_INFO: {
            user_data_save_enable();
        } break;
        }
#endif
    }
}

#if ENABLE_TEMP_MEASUREMENT
/* ---- 临时测量代码（开关见 user_config.h 的 ENABLE_TEMP_MEASUREMENT）----
    循环抖动 + 两套时钟发散，验证后整体删除
    system_time_ms   = sys_timer_get_ms()  系统时钟（os_time_dly 的时基）
    animation_time_ms = HAL_GetTick()      动画时钟（usr_timer 每 10ms 累加的 tick_ms）
    每 1000ms（按系统时钟）打印一行 loop_stat：
      clock_diff     = 系统时钟 - 动画时钟；持续增大说明动画钟在丢拍
      max_system_gap = 本打印周期内相邻两次循环的最大间隔；>50ms 说明出现任务级停顿
      max_anim_gap   = 同上，按动画时钟统计
    （第一行的最大间隔因静态变量还没记录过基准值会偏大，属正常） */
#define DEBUG_PRINT_PERIOD_MS 1000U

static void debug_print_loop_statistics(void)
{
    static u32 last_system_time_ms;
    static u32 last_animation_time_ms;
    static u32 max_system_interval_ms;
    static u32 max_animation_interval_ms;
    static u32 next_print_time_ms;
    u32 system_time_ms = sys_timer_get_ms();
    u32 animation_time_ms = (u32)HAL_GetTick();

    // 记录本打印周期内相邻两次循环的最大间隔
    if ((system_time_ms - last_system_time_ms) > max_system_interval_ms) {
        max_system_interval_ms = system_time_ms - last_system_time_ms;
    }
    if ((animation_time_ms - last_animation_time_ms) >
        max_animation_interval_ms) {
        max_animation_interval_ms =
            animation_time_ms - last_animation_time_ms;
    }
    last_system_time_ms = system_time_ms;
    last_animation_time_ms = animation_time_ms;

    // 到 1000ms 打印一次，并清零最大间隔统计
    if ((system_time_ms - next_print_time_ms) >= DEBUG_PRINT_PERIOD_MS) {
        next_print_time_ms = system_time_ms;
        printf("loop_stat system_ms=%lu anim_ms=%lu clock_diff=%ld "
               "max_system_gap=%lu max_anim_gap=%lu\n",
               (unsigned long)system_time_ms, (unsigned long)animation_time_ms,
               (long)(system_time_ms - animation_time_ms),
               (unsigned long)max_system_interval_ms,
               (unsigned long)max_animation_interval_ms);
        max_system_interval_ms = 0;
        max_animation_interval_ms = 0;
    }
}
#endif /* ENABLE_TEMP_MEASUREMENT */

void user_main(void *p)
{
    while (1) {
        // printf("user_main\n");
        // printf("fc_effect.rgb.w == %u\n", (u16)fc_effect.rgb.w);

        user_data_save_handle();

        /****添加 处理函数 start**/
        check_mic_sound();    // 采集声音并计算平均值
        music_static_sound(); // 声控，七彩灯定色转换

#if (0 == USE_TEST_BOARD)
        if (get_sound_result()) {
            motor_sound_control_trigger(); // 有声控信号，电机快速转动一段时间
        }
#endif

        meteor_period_sub(); // 流星周期控制（每10ms减一次）

        // 灯带长度固定 16（app 下发的长度值统一按 16 执行）
        // 正常时 led_num 恒为 16、条件不成立，零开销；被改动时立即纠正
        // if (fc_effect.led_num != 16) {
        //     extern void set_ls_lenght(u16 l);
        //     set_ls_lenght(16);
        // }

        rf24g_key_handle();
        WS2812FX_service();

#if ENABLE_TEMP_MEASUREMENT
        // 临时测量：每秒打印一次循环统计（开关见 user_config.h）
        debug_print_loop_statistics();
#endif

        // clr_wdt(); // 线程里只喂狗，不调用系统延时，还是会出现复位问题
        os_time_dly(1);
    }
}

void user_10ms_isr(void)
{
    run_tick_per_10ms();
    user_data_save_delay_add();
}

// 在定时器中断内调用
void user_125us_isr(void)
{
#if !USE_TEST_BOARD
    motor_handle_125us();
#endif
}
