#ifndef GUI_USB_H_INCLUDED
#define GUI_USB_H_INCLUDED

#include "gui_manager.h"

void GUI_Usb_Init(lv_obj_t *pusb, lv_group_t *group, PageRegHandler handler);
void GUI_Usb_FocusInit(void);

#endif // GUI_USB_H_INCLUDED
