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
        if (fc_effect.led_num != 16) {
            extern void set_ls_lenght(u16 l);
            set_ls_lenght(16);
        }

        WS2812FX_service(); // 注意，实际 这里约 20ms 才调用一次动画

        rf24g_key_handle();

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
