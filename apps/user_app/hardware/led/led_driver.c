#include "led_driver.h"
#include "gpio.h"
#include "mcpwm.h"

// volatile led_driver_t led_driver;

void led_driver_init(void)
{
    struct pwm_platform_data mcpwm_arg;
    gpio_set_direction(LED_DRIVER_RED_PIN, 0); // 输出模式
    gpio_direction_output(LED_DRIVER_RED_PIN, 0);

    gpio_set_direction(LED_DRIVER_GREEN_PIN, 0); // 输出模式
    gpio_direction_output(LED_DRIVER_GREEN_PIN, 0);

    gpio_set_direction(LED_DRIVER_BLUE_PIN, 0); // 输出模式
    gpio_direction_output(LED_DRIVER_BLUE_PIN, 0);

    gpio_set_direction(LED_DRIVER_WHITE_PIN, 0); // 输出模式
    gpio_direction_output(LED_DRIVER_WHITE_PIN, 0);

    // R
    mcpwm_arg.pwm_aligned_mode = pwm_edge_aligned;     // 边沿对齐
    mcpwm_arg.pwm_ch_num = LED_DRIVER_PWM_RED_CHANNEL; //通道号
    mcpwm_arg.frequency = 1000;                        // 1KHz
    mcpwm_arg.duty = 0;                                // 上电输出0%占空比
    mcpwm_arg.h_pin = LED_DRIVER_RED_PIN;              // 任意引脚
    mcpwm_arg.l_pin = -1;                              // 任意引脚,不需要就填-1
    mcpwm_arg.complementary_en =
        0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_arg);
    // G
    mcpwm_arg.pwm_aligned_mode = pwm_edge_aligned;       // 边沿对齐
    mcpwm_arg.pwm_ch_num = LED_DRIVER_PWM_GREEN_CHANNEL; //通道号
    mcpwm_arg.frequency = 1000;                          // 1KHz
    mcpwm_arg.duty = 0;                                  // 上电输出0%占空比
    mcpwm_arg.h_pin = LED_DRIVER_GREEN_PIN;              // 任意引脚
    mcpwm_arg.l_pin = -1; // 任意引脚,不需要就填-1
    mcpwm_arg.complementary_en =
        0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_arg);
    // B
    mcpwm_arg.pwm_aligned_mode = pwm_edge_aligned;      // 边沿对齐
    mcpwm_arg.pwm_ch_num = LED_DRIVER_PWM_BLUE_CHANNEL; //通道号
    mcpwm_arg.frequency = 1000;                         // 1KHz
    mcpwm_arg.duty = 0;                                 // 上电输出0%占空比
    mcpwm_arg.h_pin = LED_DRIVER_BLUE_PIN;              // 任意引脚
    mcpwm_arg.l_pin = -1;                               // 任意引脚,不需要就填-1
    mcpwm_arg.complementary_en =
        0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_arg);

    // W
    mcpwm_arg.pwm_aligned_mode = pwm_edge_aligned;       // 边沿对齐
    mcpwm_arg.pwm_ch_num = LED_DRIVER_PWM_WHITE_CHANNEL; //通道号
    mcpwm_arg.frequency = 1000;                          // 1KHz
    mcpwm_arg.duty = 0;                                  // 上电输出0%占空比
    mcpwm_arg.h_pin = LED_DRIVER_WHITE_PIN;              // 任意引脚
    mcpwm_arg.l_pin = -1; // 任意引脚,不需要就填-1
    mcpwm_arg.complementary_en =
        0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_arg);
}

#if 0 
/**
 * @brief 根据RGB值，设置对应的pwm占空比值
 *
 */
void led_driver_set_rgb_pwm_val(u8 r, u8 g, u8 b)
{
	u32 red_pwm_duty;
	u32 green_pwm_duty;
	u32 blue_pwm_duty;

	red_pwm_duty = (u32)r * 10000 / 255;
	green_pwm_duty = (u32)g * 10000 / 255;
	blue_pwm_duty = (u32)b * 10000 / 255;

	mcpwm_set_duty(LED_DRIVER_PWM_RED_CHANNEL, red_pwm_duty);  // R
	mcpwm_set_duty(LED_DRIVER_PWM_GREEN_CHANNEL, green_pwm_duty);  // G   
	mcpwm_set_duty(LED_DRIVER_PWM_BLUE_CHANNEL, blue_pwm_duty);  // B
}
#endif

/**
 * @brief 根据RGB值，设置对应的pwm占空比值
 *
 */
void led_driver_set_rgbw_pwm_val(u8 r, u8 g, u8 b, u8 w)
{
    u32 red_pwm_duty;
    u32 green_pwm_duty;
    u32 blue_pwm_duty;
    u32 white_pwm_duty;

    red_pwm_duty = (u32)r * 10000 / 255;
    green_pwm_duty = (u32)g * 10000 / 255;
    blue_pwm_duty = (u32)b * 10000 / 255;
    white_pwm_duty = (u32)w * 10000 / 255;

    mcpwm_set_duty(LED_DRIVER_PWM_RED_CHANNEL, red_pwm_duty);     // R
    mcpwm_set_duty(LED_DRIVER_PWM_GREEN_CHANNEL, green_pwm_duty); // G
    mcpwm_set_duty(LED_DRIVER_PWM_BLUE_CHANNEL, blue_pwm_duty);   // B
    mcpwm_set_duty(LED_DRIVER_PWM_WHITE_CHANNEL, white_pwm_duty); // W
}
