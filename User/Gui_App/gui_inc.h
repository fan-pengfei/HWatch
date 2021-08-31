#ifndef GUI_INC_H_INCLUDED
#define GUI_INC_H_INCLUDED

#include "system.h"
#include "stick.h"
#include "lcd_1inch28.h"
#include "GUI_Paint.h"
#include "image.h"

#include "drivers.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "hwatch_symbol_def.h"

#define APP_ABOUT "About"
#define APP_SETTING "Settings"
#define APP_SYSINFO	"SysInfo"
#define APP_HEART	"HeartBeat"
#define APP_GAMES "Games"
#define APP_CALENDER "Calendar"
#define APP_CALC	"Calculator"
#define APP_USB "Usb"

#define WIN_W LCD_1IN28_WIDTH
#define WIN_H LCD_1IN28_HEIGHT

#define PAGE_COUNT 2
#define APPS_COUNT 8
#define GAME_COUNT 6
#define SETTING_COUNT 5

#define ANIM_TIME 160

LV_FONT_DECLARE(hwatch_icons_font_16);
LV_FONT_DECLARE(hwatch_icons_font_26);
//LV_FONT_DECLARE(hwatch_icons_font_24);

typedef void(*PageRegHandler)(u8 pageId, short appId);
static void gui_update_delay(u32 t);

static const char * gui_number[] = {
	"00","01","02","03","04","05","06","07","08","09","10",
			 "11","12","13","14","15","16","17","18","19","20",
			 "21","22","23","24","25","26","27","28","29","30",
			 "31","32","33","34","35","36","37","38","39","40",
			 "41","42","43","44","45","46","47","48","49","50",
			 "51","52","53","54","55","56","57","58","59","60",
			 "61","62","63","64","65","66","67","68","69","70",
			 "71","72","73","74","75","76","77","78","79","80",
			 "81","82","83","84","85","86","87","88","89","90",
			 "91","92","93","94","95","96","97","98","99","100"
};

typedef enum{
	app_null = -1,//空值
	app_setting,	//设置页
	app_heart,		//心率监测页
	app_sys,			//系统信息页
	app_games,			//内置游戏页
	app_calc,			//计算器
	app_calendar,	//日历
	app_usb,			//PC&USB通信//显示图像//字库里没有合适的图标。。。
	app_about,		//关于页
}APP_ID;

typedef enum{
	game_null = -1,	//空值
	game_snake,			//贪吃蛇
	game_tank,
	game_tetris,
	game_2048,
	game_plane,
	game_block,
	
}GAME_ID;

typedef enum{
	setting_null = -1,
	setting_time,
	setting_blight,
	setting_sound,
	setting_alarm,
	setting_task,
}SETTING_ID;

typedef enum{
	p_main,		//主页
	p_apps,		//应用列表页
	p_games,	//游戏列表页
	p_settings,	//设置列表页
}PAGE_ID;

typedef struct{
	u8 page_id;
	short app_id;
}GUI_IDS;

#define GUI_Sound_Delay(t, state)\
{\
	if(g_config.hwatch_config.sound_state || state) beep = BEEP_ON;\
	gui_update_delay(t);\
	beep = BEEP_OFF;\
}

static void gui_update_delay(u32 t)
{
	while(t-->0)
	{
		lv_task_handler();
		Delay_Ms(1);
	}
}

static void lv_obj_add_anim(
	lv_obj_t* obj, lv_anim_t * a,
	lv_anim_exec_xcb_t exec_cb,
	int32_t start, int32_t end,
	uint16_t time
)
{
	lv_anim_init(a);
	
	lv_anim_set_exec_cb(a, exec_cb);
	lv_anim_set_var(a, obj);
	lv_anim_set_time(a, time);
	lv_anim_set_values(a, start, end);
	lv_anim_set_delay(a, -100);

	
	lv_anim_path_t path;
	lv_anim_path_init(&path);
	lv_anim_path_set_cb(&path, lv_anim_path_linear);
	
	lv_anim_set_path(a, &path);
	lv_anim_set_repeat_count(a, 1);

	lv_anim_start(a);
}

#define LV_OBJ_ADD_ANIM(obj, attr, target, time)\
do{\
	static lv_anim_t a;\
	lv_obj_add_anim(\
		(obj),&a,\
		(lv_anim_exec_xcb_t)lv_obj_set_##attr,\
		lv_obj_get_##attr(obj),\
		(target),\
		(time)\
	);\
}while(0)

static void set_page_style(lv_style_t *style, u8 radius, u8 opa, lv_color_t bg_color, lv_color_t border_color)
{
	lv_style_init(style);
	
	lv_style_set_radius(style, LV_STATE_DEFAULT, radius);	//页面圆角度数
	lv_style_set_bg_opa(style, LV_STATE_DEFAULT, opa);
	lv_style_set_bg_color(style, LV_STATE_DEFAULT, bg_color);			//背景色
	lv_style_set_border_color(style, LV_STATE_FOCUSED, border_color);	//页面聚焦边颜色
	lv_style_set_border_color(style, LV_STATE_DEFAULT, border_color);	//页面默认边颜色
}

static void set_font_style(lv_style_t *style, lv_color_t color, lv_font_t * font)
{
	lv_style_init(style);
	
	lv_style_set_text_font(style, LV_STATE_DEFAULT, font);
	lv_style_set_text_color(style, LV_STATE_DEFAULT, color);
}

static void obj_clear_task(void *obj)
{
	printf("clear state\n");
	lv_obj_clear_state(obj, LV_STATE_PRESSED);
}

#endif // GUI_INC_H_INCLUDED
