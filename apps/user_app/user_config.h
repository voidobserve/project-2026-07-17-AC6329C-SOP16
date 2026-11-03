#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

/*
	临时测量打印开关（动画错位问题排查用，详见 docs/动画错位问题分析.md）

	1：开启——主循环每秒打印一行 loop_stat（循环统计），
	   另有按键事件 key_event、写 flash 前后 save_begin/save_end 打点
	0：关闭——三处临时测量代码整体不参与编译，零运行开销

	当前 =0（2026-11-03 板测通过后关闭）；需要重新收日志时改回 1
*/
#define ENABLE_TEMP_MEASUREMENT 0

/*
	如果使能，会屏蔽 IO_PORT_DP 引脚对应的功能，专门用于打印输出

	REVIEW 修改后，注意检查 board_ac632n_demo_cfg.h -> TCFG_UART0_ENABLE
*/
#define USER_DEBUG_ENABLE 0



#endif