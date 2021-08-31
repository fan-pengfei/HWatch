#include "gui_usb.h"

static lv_obj_t *m_usb;

//lv_obj_t *lv_title_usb_label;

PageRegHandler usb_handler;

lv_task_t * stop_task;

void draw_test(void);

void lvgl_stop_task(lv_task_t *task)
{
	lvgl_task_flag = FALSE;
	
	LCD_1IN28_Clear(BLACK);
	//draw_test();
	u16 offset_x = (WIN_W - 128) / 2;
	u16 offset_y = (WIN_H - 64) / 2;
	LCD_Disp.disp_play = 1;
	
	for(;;)
	{
		if(LCD_Disp.disp_flush)
		{
			LCD_1IN28_DisplayWindows(offset_x + 0,offset_y + LCD_Disp.disp_row,offset_x + 127, offset_y + LCD_Disp.disp_row+31, LCD_Disp.disp_buf);
			while(LCD_Disp.disp_flush){gui_update_delay(1);};
		}
		
		if(KEY_RIGHT == KEY_Scan(0))
		{
			lvgl_task_flag = TRUE;
			LCD_Disp.disp_play = 0;
			usb_handler(p_apps, app_null);
			break;
		}
		
	}
}

void GUI_Usb_Init(lv_obj_t *pusb, lv_group_t *group, PageRegHandler handler)
{
	m_usb = pusb;
	usb_handler = handler;
	
	lv_obj_set_size(m_usb, WIN_W, WIN_H);
	lv_obj_clean_style_list(m_usb, LV_OBJ_PART_MAIN);
	lv_obj_add_style(m_usb, LV_OBJ_PART_MAIN, &page_style);
	//lv_obj_set_style_local_bg_color(m_usb, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_group_add_obj( group, m_usb );

	
}

void GUI_Usb_FocusInit()
{
	lv_group_focus_obj(m_usb);
	
	stop_task = lv_task_create_basic();
	lv_task_set_cb(stop_task, lvgl_stop_task);
	lv_task_once(stop_task);
}

void draw_test()
{

	LCD_Disp.disp_flush = 1;
	for(size_t i=0;i<LCD_Disp.disp_size;i++)
	{
		LCD_Disp.disp_buf[i] = lv_color_to16(lv_color_make(0x00, 0xFF, 0x00));
	}
	LCD_1IN28_DisplayWindows(0,20,WIN_W-1, 39, LCD_Disp.disp_buf);
	while(LCD_Disp.disp_flush){gui_update_delay(1);};
	LCD_Disp.disp_flush = 1;
	for(int i=0;i<LCD_Disp.disp_size;i++)
	{
		LCD_Disp.disp_buf[i] = lv_color_to16(LV_COLOR_RED);
	}
	LCD_1IN28_DisplayWindows(0,40,WIN_W-1, 59, LCD_Disp.disp_buf);
	while(LCD_Disp.disp_flush){gui_update_delay(1);};
	LCD_Disp.disp_flush = 1;
	for(int i=0;i<LCD_Disp.disp_size;i++)
	{
		LCD_Disp.disp_buf[i] = lv_color_to16(LV_COLOR_BLUE);
	}
	LCD_1IN28_DisplayWindows(0,60,WIN_W-1, 79, LCD_Disp.disp_buf);
	
}
