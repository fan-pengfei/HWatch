#ifndef GUI_CALC_H_INCLUDED
#define GUI_CALC_H_INCLUDED

#include "gui_manager.h"
#include "calculator.h"

void GUI_Calc_Init(lv_obj_t *pcalc, lv_group_t *group, PageRegHandler handler);
void GUI_Calc_FocusInit(void);

#endif // GUI_CALC_H_INCLUDED
