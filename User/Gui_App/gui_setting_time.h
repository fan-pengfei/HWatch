#ifndef GUI_SETTING_TIME_H_INCLUDED
#define GUI_SETTING_TIME_H_INCLUDED

#include "gui_manager.h"

typedef struct{

	u8 year_index;
	u8 month_index:4;
	u8 day_index:3;
	u8 date_index;
	u8 hour_index;
	u8 min_index;
	u8 sec_index;
	
}TimeBtnsIndex;

void TimeSet_Init(lv_obj_t *tab, lv_group_t *group);
void TimeSet_FocusInit(void);

#endif // GUI_SETTING_TIME_H_INCLUDED
