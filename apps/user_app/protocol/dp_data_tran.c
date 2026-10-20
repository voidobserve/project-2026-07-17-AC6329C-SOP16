/*
适用：
产品ID: yxwh27s5
品类：幻彩户外串灯-蓝牙
协议：BLE
DP协议转置，提取DP点有效数据
*/
#include "cpu.h"
#include "led_strip_sys.h"
#include "paint_tool.h"
#include "dp_data_tran.h"
#include "led_strand_effect.h"
#include "ble_trans_profile.h"
#include "led_strand_effect.h"
#include "WS2812FX.H"
#include "btstack/le/att.h"
#include "ble_user.h"
#include "btstack/le/ble_api.h"
#include "led_strip_drive.h"
#include "user_ble_notify.h"     // BLE notify 反馈通道
#include "user_ble_notify_app.h" // 各状态 notify 接口
// #include "one_wire.h"
dp_data_header_t dp_data_header; //涂鸦DP数据头
dp_switch_led_t dp_switch_led;   //DPID_SWITCH_LED开关
dp_work_mode_t dp_work_mode;     //DPID_WORK_MODE工作模式
dp_countdown_t dp_countdown;     //DPID_COUNTDOWN倒计时
dp_music_data_t dp_music_data;   //DPID_MUSIC_DATA音乐灯

dp_draw_tool_t dp_draw_tool; //DPID_DRAW_TOOL 涂抹功能

/************************************************************************************
 *@  函数：string_hex_Byte
 *@  描述：把字符串转为HEX，支持小写abcdef,输出uint8、uint16、uint32类型的hex
 *@  形参1: str         <字符输入>
 *@  形参2: out_Byte    <输出的1、2、4字节>
 *@  返回：unsigned long
 ************************************************************************************/
unsigned long string_hex_Byte(char *str, unsigned char out_Byte)
{
    int i = 0;
    unsigned char temp = 0;
    unsigned long hex = 0;
    if ((out_Byte == 1) || (out_Byte == 2) || (out_Byte == 4)) {
        while (i < (out_Byte * 2)) {
            hex <<= 8;
            if (str[i] >= '0' && str[i] <= '9') {
                temp = (str[i] & 0x0f);
            } else if (str[i] >= 'a' && str[i] <= 'f') {
                temp = ((str[i] + 0x09) & 0x0f);
            }
            ++i;
            if (out_Byte != 1) {
                temp <<= 4;
                if (str[i] >= '0' && str[i] <= '9') {
                    temp |= (str[i] & 0x0f);
                } else if (str[i] >= 'a' && str[i] <= 'f') {
                    temp |= ((str[i] + 0x09) & 0x0f);
                }
            }
            ++i;
            hex |= temp;
        }
    } else
        return 0xffffffff; //错误类型
    return hex;
}

/*****************************************
 *@  函数：dp_extract_data_handle
 *@  描述：dp数据提取
 *@  形参: buff
 *@  返回：DP数据长度
 ****************************************/
unsigned short dp_extract_data_handle(unsigned char *buff)
{
    unsigned char num = 0;
    unsigned char max;
    unsigned char temp;
    /*提取涂鸦DP数据头*/
    dp_data_header.id = buff[0];
    dp_data_header.type = buff[1];
    dp_data_header.len = buff[2];
    dp_data_header.len <<= 8;
    dp_data_header.len |= buff[3];

    // m_hsv_to_rgb(&colour_R,&colour_G,&colour_B,colour_H,colour_S,colour_V);

    /*提取DP数据*/
    switch (dp_data_header.id) {
    case DPID_SWITCH_LED: //开关(可下发可上报)
        printf("\r\n DPID_SWITCH_LED");

        fc_effect.on_off_flag = buff[4];
        if (fc_effect.on_off_flag == DEVICE_ON) {
            soft_turn_on_the_light();
        } else {
            soft_rurn_off_lights();
        }
        break;

    case DPID_WORK_MODE: //工作模式(可下发可上报)
        dp_work_mode.mode = buff[4];

        printf("dp_work_mode.mode = %d\r\n", dp_work_mode.mode);
        printf("\r\n");

        break;

    case DPID_BRIGHT_VALUE: //白光亮度(可下发可上报)
        break;

    case DPID_COLOUR_DATA: //彩光(可下发可上报)
        break;
#if 0
    case DPID_COUNTDOWN://倒计时(可下发可上报)
        //DP协议参数
        dp_countdown.time = buff[4];
        dp_countdown.time <<= 8;
        dp_countdown.time |= buff[5];
        dp_countdown.time <<= 8;
        dp_countdown.time |= buff[6];
        dp_countdown.time <<= 8;
        dp_countdown.time |= buff[7];

        printf("dp_countdown.time = %d\r\n", dp_countdown.time);
        printf("\r\n");

        //效果参数
        fc_effect.countdown.time = dp_countdown.time;

        printf("fc_effect.countdown.time = %d\r\n", fc_effect.countdown.time);
        printf("\r\n");

        break;
#endif
    case DPID_MUSIC_DATA: //音乐律动(只下发)
        //DP协议参数
        dp_music_data.change_type = string_hex_Byte(&buff[4], 1);
        dp_music_data.colour.h_val = string_hex_Byte(&buff[5], 2);
        dp_music_data.colour.s_val = string_hex_Byte(&buff[9], 2);
        dp_music_data.colour.v_val = string_hex_Byte(&buff[13], 2);
        dp_music_data.white_b = string_hex_Byte(&buff[17], 2);
        dp_music_data.ct = string_hex_Byte(&buff[21], 2);

        break;

    case DPID_RGBIC_LINERLIGHT_SCENE: //炫彩情景(可下发可上报)
        fc_effect.Now_state = IS_light_scene;

        //变化类型、模式
        fc_effect.dream_scene.change_type = buff[4];
        // 方向
        fc_effect.dream_scene.direction = buff[5];
        // 段大小
        fc_effect.dream_scene.seg_size = buff[6];
        if (fc_effect.dream_scene.c_n > MAX_NUM_COLORS) {
            fc_effect.dream_scene.c_n = MAX_NUM_COLORS;
        }
        // 颜色数量
        fc_effect.dream_scene.c_n = buff[7];
        //清除rgb[0~n]数据
        memset(fc_effect.dream_scene.rgb, 0, sizeof(fc_effect.dream_scene.rgb));

        //数据循环传输
        for (num = 0; num < fc_effect.dream_scene.c_n; num++) {
            fc_effect.dream_scene.rgb[num].r = buff[8 + num * 3];
            fc_effect.dream_scene.rgb[num].g = buff[9 + num * 3];
            fc_effect.dream_scene.rgb[num].b = buff[10 + num * 3];
        }
        printf("\n fc_effect.dream_scene.c_n=%d", fc_effect.dream_scene.c_n);
        printf_buf(fc_effect.dream_scene.rgb,
                   fc_effect.dream_scene.c_n * sizeof(color_t));
        printf("\n fc_effect.dream_scene.direction=%d",
               fc_effect.dream_scene.direction);
        set_fc_effect();
        break;

    case DPID_DRAW_TOOL: //涂抹功能(可下发可上报)
        //DP协议参数
        dp_draw_tool.tool = buff[5];

        dp_draw_tool.colour.h_val = buff[7];
        dp_draw_tool.colour.h_val <<= 8;
        dp_draw_tool.colour.h_val |= buff[8];
        dp_draw_tool.colour.s_val = buff[9] * 10;
        dp_draw_tool.colour.v_val = buff[10] * 10;

        dp_draw_tool.white_b = buff[11] * 10;

        if (buff[13] == 0x81) {
            dp_draw_tool.led_place = buff[14];
            dp_draw_tool.led_place <<= 8;
            dp_draw_tool.led_place |= buff[15];
        }

        //效果参数
        if (dp_draw_tool.white_b != 0) {
            dp_draw_tool.colour.h_val = 0;
            dp_draw_tool.colour.s_val = 0;
            dp_draw_tool.colour.v_val = dp_draw_tool.white_b;
        }

        //effect_smear_adjust_updata((smear_tool_e )dp_draw_tool.tool, &(dp_draw_tool.colour), &dp_draw_tool.led_place);

        break;
    }
    return (dp_data_header.len + sizeof(dp_data_header_t));
}

/* 解析中道数据，主要是静态模式，和动态效果的“基本”效果 */
void parse_zd_data(unsigned char *LedCommand)
{
    if (LedCommand[0] == 0x01 && LedCommand[1] == 0x03) {
        // APP的同步指令，与APP同步数据
        user_ble_notify_dev_all_info();
    }

    //---------------------------------接收到开关灯命令-----------------------------------
    if (LedCommand[0] == 0x01 && LedCommand[1] == 0x01) {
        extern void set_on_off_led(u8 on_off);
        set_on_off_led(LedCommand[2]);
        user_ble_notify_light_pwr_sta(); //
    }

    //--------------------------下面所有的操作都需要在开灯状态下操作-----------------------------------
    if (DEVICE_OFF == get_on_off_state()) {
        return;
    }

    //---------------------------------更新RGB 色盘-----------------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x01 &&
        LedCommand[2] == 0x1e) {
        extern void set_static_mode(u8 r, u8 g, u8 b);

        set_static_mode(
            LedCommand[3], LedCommand[4],
            LedCommand[5]); //配好颜色，在service中会调回PWM驱动修改颜色
    }
    //---------------------------------静态（模式）任务处理-----------------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x02 && LedCommand[2] >= 0 &&
        LedCommand[2] < 0x07) {
        switch (LedCommand[2]) {
        case 0:
            led_colorful_light_set_static_color(RED);
            break;

        case 1:
            led_colorful_light_set_static_color(BLUE);
            break;

        case 2:
            led_colorful_light_set_static_color(GREEN);
            break;

        case 3:
            led_colorful_light_set_static_color(CYAN);
            break;

        case 4:
            led_colorful_light_set_static_color(YELLOW);
            break;

        case 5:
            led_colorful_light_set_static_color(PURPLE);
            break;

        case 6:
            led_colorful_light_set_static_color(PURE_WHITE);
            break;
        }
    }

    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x02 &&
        LedCommand[2] >= 0x07 && LedCommand[2] <= 0xFF) {
        // 动态模式
        fc_dynamic_effect(LedCommand[2]);
    }
    //---------------------------------调节亮度0-100-----------------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x03) {
#if USER_DEBUG_ENABLE
        // uint8_t brightness_percent = 0;
#endif
        set_bright(LedCommand[2]);
        user_ble_notify_light_brightness(); // 亮度

#if USER_DEBUG_ENABLE
        // brightness_percent = (uint16_t)fc_effect.b * 100 / 255;
        // printf("app brightness_percent == %u\n", (uint16_t)LedCommand[2]);
        // printf("brightness_percent == %u\n", (uint16_t)brightness_percent);
        // printf("fc_effect.b == %u\n", fc_effect.b);        
#endif
    }
    //---------------------------------调节速度0-100-----------------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x04) {
        // 范围0-100
        ls_set_speed(LedCommand[2]);
        user_ble_notify_light_speed(); // 速度
    }
    //---------------------------------更改RGB接口-----------------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x05) {
        extern void set_rgb_sequence(u8 s);
        set_rgb_sequence(LedCommand[2]);
        user_ble_notify_rgb_sequence(); // RGB接口顺序
    }
    //---------------------------------W（灰度调节）控制----------------------------
    if (LedCommand[0] == 0x04 && LedCommand[1] == 0x06) {
        // app下发的是0-100，需要转换成0-255
        u32 color = 0;
        // 结合app下发的数据，限制颜色为纯白色，app下发的数据只改变白色分量
        color = (((u32)LedCommand[2] * 255 / 100) << 24) & PURE_WHITE;
        led_colorful_light_set_static_color(color);
    }
    //---------------------------------手机音乐律动 手机麦克风-----------------------------------
    if (LedCommand[0] == 0x06 && LedCommand[1] == 0x04) {
        extern void set_static_mode(u8 r, u8 g, u8 b);

        if (fc_effect.music.m_type == 0x00) //手机麦模式
        {
            set_bright(LedCommand[5]);
            set_static_mode(LedCommand[2], LedCommand[3], LedCommand[4]);
        }
    }
    //---------------------------------外麦声控模式-----------------------------------
    if (LedCommand[0] == 0x06 && LedCommand[1] == 0x06) {
        extern void set_music_mode(u8 m);
        if (fc_effect.music.m_type == 0x01) //外模模式
        {
            set_music_mode(LedCommand[2]);
        }
    }
    //---------------------------------设备手机麦或者外麦-----------------------------------
    if (LedCommand[0] == 0x06 && LedCommand[1] == 0x07) {
        extern void set_music_type(u8 ty);
        set_music_type(LedCommand[2]);
        user_ble_notify_sound_ctl_type(); // 声控类型
    }

    //-------------------------------- 流星开关------------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x02) {
        extern void set_on_off_meteor(u8 on_off);
        set_on_off_meteor(LedCommand[2]);
        user_ble_notify_meteor_pwr_sta(); // 流星开关
        return;
    }

    // --------------------------------流星模式-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x00 &&
        fc_effect.metemor_on_off == 0x01) {
        extern void set_mereor_mode(u8 m);
        set_mereor_mode(LedCommand[2]);
    }
    //-------------------------------- 流星速度-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x01 &&
        fc_effect.metemor_on_off == 0x01) {
        extern void set_mereor_speed(u8 s);
        set_mereor_speed(LedCommand[2]);
        user_ble_notify_meteor_speed(); // 流星速度
    }
    // --------------------------------流星灯时间间隔-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x03 &&
        fc_effect.metemor_on_off == 0x01) {
        extern void set_meteor_p(u8 p);
        set_meteor_p(LedCommand[2]);
        user_ble_notify_meteor_period(); // 流星周期
    }
    //---------------------------------设置麦克风灵，电机，流星敏度-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x05) {
        set_sensitive(100 - LedCommand[2]);
        user_ble_notify_sensitive(); // 灵敏度
    }
    // ---------------------------------设置电机模式-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x06) {

        fc_effect.motor_mode = LedCommand[2];
        motor_set_mode(fc_effect.motor_mode);
        user_ble_notify_motor_mode(); // 电机模式

#if USER_DEBUG_ENABLE
        printf("app set motor mode == %u\n", LedCommand[2]);
        printf("fc_effect.motor_mode == %u\n", (u16)fc_effect.motor_mode);
#endif
    }

    // --------------------------------设置电机转速-----------------------------------
    if (LedCommand[0] == 0x2F && LedCommand[1] == 0x07) {
        fc_effect.motor_sec_per_round = LedCommand[2];
        motor_set_speed_sec_per_round(fc_effect.motor_sec_per_round);
        // user_ble_notify_motor_speed(); // 电机转速
    }
}

void tuya_fb_sw_state(void);

void respond_led_strip_dp_query(void)
{
    int res;
    u8 buf[] = {DPID_WORK_MODE, 4, 0, 1, 1}; //涂抹模式
}

/* APP数据解析入口函数 */
void parse_led_strip_data(u8 *pBuf, u8 len)
{
    // 重复解析协议此时，避免卡死
    u8 retry = 0;
    led_strip_t strip;

    u16 handler_len;

    unsigned char num = 0;

    /* 涂鸦DP协议解析 */
    handler_len = dp_extract_data_handle(pBuf);
    /* 中道孔明灯协议解析 */
    /* 为兼容全彩的协议 */

    save_user_data_area3();
}

void tuya_fb_sw_state(void)
{
    dp_data_header_t *p_dp;
    u8 dp_data[4 + 1];
    p_dp = (dp_data_header_t *)dp_data;
    p_dp->id = DPID_SWITCH_LED;
    p_dp->type = DP_TYPE_BOOL;
    p_dp->len = __SWP16(1);
    // dp_data[4] = fc_effect.on_off_flag;
    dp_data[4] = 1; //默认开机
}

/* -------------------------------------DPID_CONTROL_DATA 调节模式----------------------------- */

//调节(只下发)
//备注:类型：字符串;
//Value: 011112222333344445555  ;
//0：   变化方式，0表示直接输出，1表示渐变;
//1111：H（色度：0-360，0X0000-0X0168）;
//2222：S (饱和：0-1000, 0X0000-0X03E8);
//3333：V (明度：0-1000，0X0000-0X03E8);
//4444：白光亮度（0-1000）;
//5555：色温值（0-1000）
// typedef struct
// {
//     uint8_t change_type;        //变化方式，0表示直接输出，1表示渐变;
//     hsv_t c;
//     uint16_t white_b;        //白光亮度
//     uint16_t ct;             //色温
// }dp_control_t; //DPID_CONTROL_DATA

/* -------------------------------------DPID_SCENE_DATA 场景----------------------------- */

//场景(可下发可上报)
//备注:Value: 0011223344445555666677778888
//00：情景号
//11：单元切换间隔时间（0-100）
//22：单元变化时间（0-100）
//33：单元变化模式（0 静态 1 跳变 2 渐变）
//4444：H（色度：0-360，0X0000-0X0168）
//5555：S (饱和：0-1000, 0X0000-0X03E8)
//6666：V (明度：0-1000，0X0000-0X03E8)
//7777：白光亮度（0-1000）
//8888：色温值（0-1000）
//注：数字 1-8 的标号对应有多少单元就有多少组
// #pragma pack (1)
// typedef struct
// {
//     uint8_t sw_time;            //单元切换间隔时间
//     uint8_t chg_time;            //单元变化时间
//     uint8_t change_type;        //单元变化模式（0 静态 1 跳变 2 渐变）
//     hsv_t c;
//     uint16_t white_b;           //白光亮度
//     uint16_t ct;                //色温
// }dp_secene_data_t;                 //场景数据

// typedef struct
// {
//     uint8_t secene_n;           //情景号
//     dp_secene_data_t  *data;        //多组
// }dp_secene_t;
// #pragma pack ()
/*
typedef struct
{
    color_t c[MAX_CORLOR_N];               //颜色池
    uint8_t s;                              //速度
    uint8_t n;                             //当前颜色数量
    uint8_t change_type;                   //变化模式（0 静态 1 跳变 2 渐变）
}fc_effect_t;       //全彩效果

 */
// void dp_secene_data_handle(u8 *pIn)
// {
//     dp_data_header_t *header = (dp_secene_t*) pIn;
//     uint8_t len, i = 0;
//     if(header->type != DP_TYPE_STRING)
//     {
//         #ifdef MY_DEBUG
//         printf("\n dp_secene_data_handle type err");
//         #endif
//         return;
//     }

//     /* 提取涂鸦效果数据 */
//     uint8_t secene_data[ sizeof(dp_data_header_t) * MAX_CORLOR_N + 1];

//     len = string2hex(pIn + sizeof(dp_data_header_t), &secene_data, __SWP16(header->len));

//     #ifdef MY_DEBUG
//     printf("\n secene_data =");
//     printf_buf(secene_data, len);
//     #endif

//     /* 提取颜色数量 */
//     fc_effect.n = (len - 1) / sizeof(dp_secene_data_t);
//     #ifdef MY_DEBUG
//     printf("\n dp_secene_data_t =%d",sizeof(dp_secene_data_t));
//     #endif
//     /* 提取颜色 */
//     dp_secene_data_t *pdata;
//     pdata = (dp_secene_data_t *) (secene_data + 1); //第1个byte是情景号
//     m_hsv_to_rgb(   &fc_effect.c[i].r, &fc_effect.c[i].g, &fc_effect.c[i].b, \
 //                     __SWP16(pdata->c.h_val), \
 //                     __SWP16(pdata->c.s_val), \
 //                     __SWP16(pdata->c.v_val));
//     /* 提取速度 */
//     fc_effect.s = pdata->chg_time;
//     fc_effect.change_type = pdata->change_type;

//     #ifdef MY_DEBUG
//     printf("\n rgb =");
//     printf_buf(fc_effect.c[i], 3);
//     printf("\n fc_effect.n = %d",fc_effect.n);

//     #endif
//     i++;
//     while(i < fc_effect.n)
//     {

//         pdata = (dp_secene_data_t *) (secene_data + 1 + sizeof(dp_secene_data_t) * i); //第1个byte是情景号
//         m_hsv_to_rgb(   &fc_effect.c[i].r, &fc_effect.c[i].g, &fc_effect.c[i].b, \
 //                     __SWP16(pdata->c.h_val), \
 //                     __SWP16(pdata->c.s_val), \
 //                     __SWP16(pdata->c.v_val));

//         #ifdef MY_DEBUG
//         printf("\n rgb =");
//         printf_buf(fc_effect.c[i], 3);
//         #endif
//         i++;
//     }
// }
