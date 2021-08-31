#ifndef GUI_MANAGER_H_INCLUDED
#define GUI_MANAGER_H_INCLUDED

#include "gui_inc.h"
#include "gui_main.h"
#include "gui_apps.h"
#include "gui_about.h"
#include "gui_setting.h"
#include "gui_sysinfo.h"
#include "gui_heart.h"
#include "gui_games.h"
#include "gui_calender.h"
#include "gui_calc.h"
#include "gui_game_player.h"
#include "gui_usb.h"

extern lv_style_t page_style;
extern lv_style_t lmeter_style;
extern lv_style_t font_style, font_style1, font_style2;
extern lv_style_t icons_style, icons_style1; /*icons_style2*/;
extern lv_style_t chart_style;;

extern u8 lvgl_task_flag;

void GUI_ManagerInit(void);
void GUI_TaskRun(void);
void GUI_TickRun(void);
void GUI_Key_Listen(void);

extern GUI_IDS gui_ids;

extern lv_mem_monitor_t hwatch_mem_monitor;

#endif // GUI_MANAGER_H_INCLUDED
