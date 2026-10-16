
#ifndef __LED_STRIP_DRIVE_H__
#define __LED_STRIP_DRIVE_H__

// #include "board_ac632n_demo_cfg.h"
#include "asm/ledc.h"
#include "asm/gpio.h"

#include "user_config.h"
#include "dev_config.h"

#define MIC_PIN IO_PORTA_08
// 流星灯控制信号输出引脚：
#if !USE_TEST_BOARD
#define LEDC_PIN IO_PORT_DM
#else
#define LEDC_PIN IO_PORTB_07
#endif

void led_state_init(void);
void mic_gpio_init();
void led_gpio_init(void);
void led_pwm_init(void);
void ledc_init(const struct ledc_platform_data *arg);
// void fc_driver(u8 r,u8 g ,u8 b);
#endif
