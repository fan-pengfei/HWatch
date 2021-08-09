#ifndef GUI_SETTING_H_INCLUDED
#define GUI_SETTING_H_INCLUDED

#include "gui_manager.h"
#include "gui_setting_alarmclock.h"
#include "gui_setting_task.h"

//年、月、日、时、分、秒、星期
#define TIME_ROLLERS_CNT 7

void GUI_Setting_Init(lv_obj_t *psetting, lv_group_t *group, PageRegHandler handler);
void GUI_Setting_FocusInit(void);

#endif // GUI_SETTING_H_INCLUDED
