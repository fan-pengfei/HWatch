#include "gui_main.h"

static lv_obj_t *m_main;

lv_obj_t *lv_lmeter;
lv_obj_t *lv_time_label;
lv_obj_t *lv_date_label;
lv_obj_t *lv_temp_label;
lv_obj_t *lv_hum_label;
lv_obj_t *lv_week_label;
lv_obj_t *lv_title_main_label;

lv_obj_t *lv_alarm_ico;
lv_obj_t *lv_sound_ico;

lv_obj_t *lv_mcu_t_label;
lv_obj_t * lv_mcu_t_bar;
lv_obj_t *lv_mcu_mem_label;
lv_obj_t *lv_mcu_mem_bar;

lv_task_t *time_task;
lv_task_t *alarm_task;

PageRegHandler main_handler;

char tempBuff[8];
char humBuff[8];
u8 screen_lock_t_num = 0;
u8 is_clock_run = 0;

static void main_alarm_task(lv_task_t *task)
{
	static u8 i = 0;
	if(i > 8) i=0;
	
	switch(i)
	{
		case 0:
			beep = STATE_OFF;
			PULSESENSOR_EN_PIN = STATE_OFF;
			beep = STATE_ON;	//100
			PULSESENSOR_EN_PIN = STATE_ON;
			break;
		case 1:
			beep = STATE_OFF;	//100
			PULSESENSOR_EN_PIN = STATE_OFF;
			break;
		case 2:
			beep = STATE_ON;	//200
			PULSESENSOR_EN_PIN = STATE_ON;
			break;
		case 4:
			beep = STATE_OFF;	//100
			PULSESENSOR_EN_PIN = STATE_OFF;
			break;
		case 5:
			beep = STATE_ON;	//300
			PULSESENSOR_EN_PIN = STATE_ON;
			break;
		case 8:
			beep = STATE_OFF;
			PULSESENSOR_EN_PIN = STATE_OFF;
			break;
	}
	
	i++;
}

static void main_task(lv_task_t *task)
{
	u8 i;
	char clock[7];
	float temp, humi, mem;
	u8 sec = 0;
	DS1302_GetTime();
	sec = HWatch_Time.sec;
	
	//printf("main task running...\n");
	
	//设置秒
	lv_linemeter_set_value(lv_lmeter, sec);
	
	//设置时间
	lv_label_set_text_fmt(lv_time_label, "%02d : %02d : %02d", HWatch_Time.hour, HWatch_Time.min, HWatch_Time.sec);
	
	//设置日期
	lv_label_set_text_fmt(lv_date_label,"%02d/%02d/%02d", HWatch_Time.year, HWatch_Time.month, HWatch_Time.date);
	
	//定时关闭屏幕背光
	if(g_config.hwatch_config.screen_state && ++screen_lock_t_num == g_config.hwatch_config.home_screen_lock_t)
	{
		//g_config.hwatch_config.screen_state = STATE_OFF;
		//LCD_1IN28_SetBackLight(STATE_OFF);
	}
	
	//5s更新一次温湿度等信息
	if(sec % 2 == 0)
	{
		//设置星期
		lv_label_set_text(lv_week_label, weeks[HWatch_Time.week-1]); //1-7
		
		//设置温湿度
		HDC1080_ReadTempHumidity(&temp, &humi);
		sprintf(tempBuff, "%.1f *C", temp);
		sprintf(humBuff, "%.1f %%", humi);
		lv_label_set_text(lv_temp_label, tempBuff);
		lv_label_set_text(lv_hum_label, humBuff);
		
		//设置MCU温度
		char mcu_t_buff[8];
		sprintf(mcu_t_buff, "MCU %.1f *C", HWatchSystemp_Get());
		lv_label_set_text(lv_mcu_t_label, mcu_t_buff);
		lv_bar_set_value(lv_mcu_t_bar, (HWatchSystemp_Get() * 10), LV_ANIM_ON);
		
		//设置lvgl内存使用率
		lv_mem_monitor(&hwatch_mem_monitor);
		mem = (hwatch_mem_monitor.total_size - hwatch_mem_monitor.free_size) * 1.0f / hwatch_mem_monitor.total_size * 100.0f;
		//printf("total: %d, free: %d\n", hwatch_mem_monitor.total_size, hwatch_mem_monitor.free_size);
		char mcu_mem_buff[8];
		sprintf(mcu_mem_buff, "MEM %.1f %%", mem);
		lv_label_set_text(lv_mcu_mem_label, mcu_mem_buff);
		lv_bar_set_value(lv_mcu_mem_bar, mem, LV_ANIM_ON);
	}else if(sec == 1)
	{
		//检测闹钟
		sprintf(clock, "%02d%02d%02d",HWatch_Time.week, HWatch_Time.hour, HWatch_Time.min);
		for(i=0;i<ALARM_CLOCK_CNT_MAX;i++)
		{
			//到达闹钟设定时间，闹钟状态开启，闹钟未响铃，则开启闹钟
			if(atoi(clock) == g_config.hwatch_config.alarm_clock[i] && 
					g_config.hwatch_config.alarm_state &&
					is_clock_run == 0)
			{
				printf("clock start\n");
				
				//打开背光
				g_config.hwatch_config.screen_state = STATE_ON;
				LCD_1IN28_SetBackLight(MathMap(g_config.hwatch_config.blight_level, 0, 10, BLIGHT_MIN, BLIGHT_MAX));
				
				is_clock_run = 1;
				alarm_task = lv_task_create(main_alarm_task, 100, LV_TASK_PRIO_MID, NULL);	//创建闹钟任务
				break;
			}
		}
	}

}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{
				//判断屏幕亮灭状态
				if(!g_config.hwatch_config.screen_state)
				{
					//点亮屏幕
					u16 light = MathMap(g_config.hwatch_config.blight_level, 0, 10, BLIGHT_MIN, BLIGHT_MAX);
					LCD_1IN28_SetBackLight(light);
					g_config.hwatch_config.screen_state = STATE_ON;
					
					screen_lock_t_num = 0;
				}else
				{		
					lv_task_del(alarm_task);
					lv_task_del(time_task);
					main_handler(p_apps, app_null);
				}
			}else if(*key == LV_KEY_BACKSPACE){
				is_clock_run = 0; //按返回键关闭闹钟
				PULSESENSOR_EN_PIN = STATE_OFF;
				lv_task_del(alarm_task); //删除闹钟任务
			}else if(*key == LV_KEY_UP)
			{	
				//测试
				//alarm_task = lv_task_create(main_alarm_task, 100, LV_TASK_PRIO_MID, NULL);
			}else if(*key == LV_KEY_DOWN)
			{
				//HWatchDevice_ConfigSave();
			}
		}
				break;
    default:
        break;
    }
}

void GUI_Main_Init(lv_obj_t *pmain, lv_group_t *group, PageRegHandler handler)
{
	m_main = pmain;
	main_handler = handler;
	
	lv_obj_set_size(m_main, WIN_W, WIN_H);
	//lv_obj_align(m_main, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(m_main, LV_OBJ_PART_MAIN, &page_style);
	lv_obj_set_event_cb( m_main, event_handler );
	lv_group_add_obj( group, m_main );
	
	//创建表盘
	lv_lmeter = lv_linemeter_create(m_main, NULL);
	lv_linemeter_set_range(lv_lmeter, 0, 60);
	lv_linemeter_set_value(lv_lmeter, 0);
	lv_linemeter_set_scale(lv_lmeter, 360, 61);
	lv_linemeter_set_angle_offset(lv_lmeter, 180);
	lv_obj_set_size(lv_lmeter, LCD_1IN28_WIDTH-18, LCD_1IN28_HEIGHT-18);
	lv_obj_align(lv_lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(lv_lmeter, LV_LINEMETER_PART_MAIN, &lmeter_style);
	
	//时间标签
	lv_time_label = lv_label_create(m_main, NULL);
//	lv_label_set_long_mode(lv_time_label, LV_LABEL_LONG_BREAK);
//	lv_obj_set_width(lv_time_label, lv_page_get_width_fit(m_main));
	lv_label_set_long_mode(lv_time_label, LV_LABEL_LONG_SROLL_CIRC );
	lv_obj_set_size(lv_time_label, 200, 36);
	lv_label_set_text(lv_time_label,"23 : 59 : 59");
	lv_label_set_align(lv_time_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(lv_time_label, LV_LABEL_PART_MAIN, &font_style);
	lv_obj_align(lv_time_label, NULL, LV_ALIGN_CENTER, 0, -18);
	
	//日期标签
	lv_date_label = lv_label_create(m_main, NULL);
	lv_label_set_long_mode(lv_date_label, LV_LABEL_LONG_SROLL_CIRC );
	lv_obj_set_size(lv_date_label, 200, 24);
	lv_label_set_text(lv_date_label,"21/01/01");
	lv_label_set_align(lv_date_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(lv_date_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_date_label, lv_time_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
	
	//温湿度标签
	lv_obj_t *th_cont = lv_cont_create(m_main, NULL);
	lv_obj_set_size(th_cont, 140, 24);
	lv_obj_add_style(th_cont, LV_OBJ_PART_MAIN, &page_style);
	lv_cont_set_layout(th_cont, LV_LAYOUT_PRETTY_MID );
	lv_temp_label = lv_label_create(th_cont, NULL);
	lv_hum_label = lv_label_create(th_cont, NULL);
	lv_label_set_text(lv_temp_label,"22.2 *C");
	lv_label_set_text(lv_hum_label, "66.6 %");
	lv_obj_add_style(lv_temp_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_add_style(lv_hum_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(th_cont, lv_date_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
	lv_obj_align(lv_temp_label, th_cont, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
	lv_obj_align(lv_hum_label, th_cont, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
	
	//星期标签
	lv_week_label = lv_label_create(m_main, NULL);
	lv_label_set_text(lv_week_label,"SUN.");
	lv_obj_add_style(lv_week_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_week_label, th_cont, LV_ALIGN_OUT_TOP_RIGHT, 2, 0);
	
	//MCU温度指示
//	static lv_style_t angles_style;
//	lv_style_init(&angles_style);
//	lv_style_set_line_color(&angles_style, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_mcu_t_label = lv_label_create(m_main, NULL);
	lv_label_set_long_mode(lv_mcu_t_label, LV_LABEL_LONG_SROLL_CIRC );
	lv_obj_set_size(lv_mcu_t_label, 130, 20);
	lv_label_set_text(lv_mcu_t_label,"0 *C");
	lv_label_set_align(lv_mcu_t_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(lv_mcu_t_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_mcu_t_label, th_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

	lv_mcu_t_bar = lv_bar_create(m_main, NULL);
	lv_obj_set_size(lv_mcu_t_bar, 52, 8);
	lv_bar_set_anim_time(lv_mcu_t_bar, 50);
	lv_bar_set_range(lv_mcu_t_bar, 0, 1000);
	lv_bar_set_value(lv_mcu_t_bar, 0, LV_ANIM_ON);
	lv_obj_align(lv_mcu_t_bar, lv_mcu_t_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	
	//闹钟图标
	lv_alarm_ico = lv_label_create(m_main, NULL);
	lv_obj_add_style(lv_alarm_ico, LV_LABEL_PART_MAIN, &icons_style);
	lv_label_set_text(lv_alarm_ico, HWATCH_SYMBOL_ALARM_OFF);
	lv_obj_align(lv_alarm_ico, lv_time_label, LV_ALIGN_OUT_TOP_RIGHT, -24, -6);
	
	//声音图标
	lv_sound_ico = lv_label_create(m_main, NULL);
	lv_obj_add_style(lv_sound_ico, LV_LABEL_PART_MAIN, &icons_style);
	lv_label_set_text(lv_sound_ico, HWATCH_SYMBOL_SOUND_OFF);
	lv_obj_align(lv_sound_ico, lv_time_label, LV_ALIGN_OUT_TOP_LEFT, 24, -6);
	
	//标题
	lv_title_main_label = lv_label_create(m_main, NULL);
	lv_label_set_text(lv_title_main_label,APP_NAME);
	lv_obj_add_style(lv_title_main_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(lv_title_main_label, lv_time_label, LV_ALIGN_OUT_TOP_MID, 0, -4);
	
	//内存使用率
	lv_mcu_mem_label = lv_label_create(m_main, NULL);
	lv_label_set_long_mode(lv_mcu_mem_label, LV_LABEL_LONG_SROLL_CIRC );
	lv_obj_set_size(lv_mcu_mem_label, 130, 20);
	lv_label_set_text(lv_mcu_mem_label,"MEM 0.0 %%");
	lv_label_set_align(lv_mcu_mem_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(lv_mcu_mem_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_mcu_mem_label, lv_title_main_label, LV_ALIGN_OUT_TOP_MID, 0, 0);

	lv_mcu_mem_bar = lv_bar_create(m_main, NULL);
	lv_obj_set_size(lv_mcu_mem_bar, 52, 8);
	lv_bar_set_anim_time(lv_mcu_mem_bar, 50);
	lv_bar_set_range(lv_mcu_mem_bar, 0, 100);
	lv_bar_set_value(lv_mcu_mem_bar, 0, LV_ANIM_ON);
	lv_obj_align(lv_mcu_mem_bar, lv_mcu_mem_label, LV_ALIGN_OUT_TOP_MID, 0, 0);
	
	GUI_Main_FocusInit();
}

void GUI_Main_FocusInit()
{
	lv_group_focus_obj(m_main);
	//创建主页更新任务，优先级设置为高
	time_task = lv_task_create(main_task, 1000, LV_TASK_PRIO_HIGHEST, NULL);
	
	//输出lvgl内存使用状态
	lv_mem_monitor(&hwatch_mem_monitor);
	printf("LVGL Memory State: total %d, free %d\n", hwatch_mem_monitor.total_size, hwatch_mem_monitor.free_size);
	
	//设置声音和闹钟图标状态
	if(g_config.hwatch_config.alarm_state)
	{
		lv_label_set_text(lv_alarm_ico, HWATCH_SYMBOL_ALARM_ON);
	}else
	{
		lv_label_set_text(lv_alarm_ico, HWATCH_SYMBOL_ALARM_OFF);
	}
	
	if(g_config.hwatch_config.sound_state)
	{
		lv_label_set_text(lv_sound_ico, HWATCH_SYMBOL_SOUND_ON);
	}else
	{
		lv_label_set_text(lv_sound_ico, HWATCH_SYMBOL_SOUND_OFF);
	}
	
	screen_lock_t_num = 0;
	//HWatchDevice_Printf();
}


