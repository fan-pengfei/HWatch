#ifndef GUI_ABOUT_H_INCLUDED
#define GUI_ABOUT_H_INCLUDED

#include "gui_manager.h"

#define ABOUT_CONTENT ("\
HWatch\r\n\
www.wiyixiao4.com\r\n\
moyanyan\r\n\
")\

void GUI_About_Init(lv_obj_t *pmain, lv_group_t *group, PageRegHandler handler);
void GUI_About_FocusInit(void);

#endif // GUI_ABOUT_H_INCLUDED
