
#include "ws2812_bsp.h"
#include "debug.h"
#include "my_effect.h"
#include <string.h>

#include "led_driver.h"

#include "led_strand_effect.h"

static unsigned long tick_ms;
void ws281x_init()
{
}
extern uint8_t rOffset; ///< Red index within each 3- or 4-byte pixel
extern uint8_t gOffset; ///< Index of green byte
extern uint8_t bOffset; ///< Index of blue byte
extern u8 send_rgbbuf_flag;
// 流星灯带 ledc 发送缓冲：最多 17 个像素(16流星+1)，白光字节流按 3 字节/灯位
// 发送需覆盖 ceil(16/3)=6 个灯位，留足余量并保证发送不越界
u8 buf[64] __attribute__((aligned(4))); //使用中断实现效果时，必须需要全局变量

/**
 * @brief 灯具的驱动集成，包括七彩的驱动，幻彩的驱动
 *
 * @param pixels_pattern   颜色
 * @param pattern_size     灯具的长度（字节数）
 */
void ws281x_show(unsigned char *pixels_pattern, unsigned short pattern_size)
{
    // printf_buf(pixels_pattern, pattern_size);

    // 第0个像素：RGBW 4字节，驱动 PWM 七彩灯
    led_driver_set_rgbw_pwm_val(pixels_pattern[0], pixels_pattern[1],
                                pixels_pattern[2], pixels_pattern[3]);

    // 第1个像素起：白光流星灯带，由 ledc 驱动（IO_PORT_DM）
    // 像素格式与 sequence 一致：RGBW=4字节/像素，RGB=3字节/像素
    // 流星为白光，取每个像素的 R 通道字节（参考工程同款处理）
    extern void ledc_send_rgbbuf_isr(u8 index, u8 * rgbbuf, u32 led_num,
                                     u16 again_cnt);
    unsigned short i, j = 0;
    unsigned short bpp, num_px, meteor_px, led_cnt;
    u8 r, g, b;

    bpp = (((fc_effect.sequence >> 6) & 0x03) ==
           ((fc_effect.sequence >> 4) & 0x03))
              ? 3
              : 4;
    num_px = pattern_size / bpp;           //总像素数（含第0个PWM灯）
    meteor_px = num_px ? (num_px - 1) : 0; //流星像素数
    if (meteor_px > sizeof(buf)) {
        meteor_px = sizeof(buf); //防止越界
    }

    memset(buf, 0, sizeof(buf)); //先清零，灯带尾部熄灭
    for (i = 0; i < meteor_px; i++) {
        //跳过第0个像素，取每个流星像素的 R(白光亮度) 字节，连续存放
        buf[i] = *(pixels_pattern + bpp + i * bpp + rOffset);
    }
    //数据重排：每3字节一组，按灯带通道序(rOffset/gOffset/bOffset)写回
    for (i = 0; i < meteor_px; i++) {
        if (j == 0) {
            r = *(buf + i);
        } else if (j == 1) {
            g = *(buf + i);
        } else if (j == 2) {
            b = *(buf + i);
        }
        j++;
        if (j == 3) {
            j = 0;
            *(buf + rOffset + (i - 2)) = r;
            *(buf + gOffset + (i - 2)) = g;
            *(buf + bOffset + (i - 2)) = b;
        }
    }
    //按 3 字节/灯位发送，灯位数 = ceil(流星像素数/3)
    led_cnt = (meteor_px + 2) / 3;
    if (led_cnt * 3 > sizeof(buf)) { //保证发送不越过 buf
        led_cnt = sizeof(buf) / 3;
    }
    ledc_send_rgbbuf_isr(0, buf, led_cnt, 0);
}

// 周期10ms
unsigned long HAL_GetTick(void)
{
    return tick_ms;
}

// 每10ms调用一次
void run_tick_per_10ms(void)
{
    tick_ms += 10;
}
