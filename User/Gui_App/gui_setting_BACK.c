#include "gui_setting.h"

static const char* list_item_name[] ={
	HWATCH_SYMBOL_CLOCK,
	HWATCH_SYMBOL_LIGHT,
	HWATCH_SYMBOL_SOUND_ON,
	HWATCH_SYMBOL_ALARM_ON,
	HWATCH_SYMBOL_TASK,
};

static const char* timer_roller_label_txt[] = {
	"Year",
	"Month",
	"Day",
	"Hour",
	"Min",
	"Second",
	"Week"
};

static lv_obj_t *m_setting;

lv_obj_t *lv_title_setting_label;

lv_obj_t *lv_setting_tabview;
lv_obj_t *lv_tabs[SETTING_COUNT];

//������ڿؼ�
lv_obj_t * lv_light_slider;
lv_obj_t *lv_light_slider_label;

//�����л��ؼ�
lv_obj_t *lv_sound_switch;

//ʱ�����ÿؼ�
lv_obj_t *lv_time_rollers[TIME_ROLLERS_CNT];
lv_obj_t *lv_time_roller_label;
lv_task_t * time_set_task;

PageRegHandler setting_handler;
static short setting_index;
static short time_roller_index;

static void tab_item_focus(void);
static void tab_item_cancelfocus();
void timer_roller_change(void);

//void time_set_task_callback(lv_task_t *task)
//{
//		char buf[6];
//		lv_roller_get_selected_str(lv_time_rollers[time_roller_index], buf, sizeof(buf));
//	
//		u8 sec = 0;
//		u8 val = atoi(buf);
//	
//		//����DS1302����ʱ�䣬�Ȼ�ȡһ������
//		//�꣺0-99
//		//�£�1-12
//		//�գ�1-31
//		//ʱ��0-23
//		//�֣�0-59
//		//�룺0-59
//		//���ڣ�1-7 //1Ϊ������
//		
//		switch(time_roller_index)
//		{
//			case 0: DS1302_SetYear(val); break;
//			case 1: DS1302_SetMon(val); break;
//			case 2: DS1302_SetDay(val); break;
//			case 3: DS1302_SetHour(val); break;
//			case 4: DS1302_SetMin(val); break;
//			case 5: sec = val; DS1302_SetSec(sec); break;
//			case 6: DS1302_SetWeek(val); break;
//		}
//		
//		DS1302_SetSec(sec);
//		//��ʾ���óɹ�
//		GUI_Sound_Delay(50, STATE_ON);
//		
//		lv_label_set_text_fmt(lv_time_roller_label, "%s %s",timer_roller_label_txt[time_roller_index], buf);
//}

//static void event_roller_handler(lv_obj_t * obj, lv_event_t event)
//{
//	const u32 *key = lv_event_get_data();
//	
//	if(*key == LV_KEY_ENTER)
//	{
//		time_set_task = lv_task_create_basic();
//		lv_task_set_cb(time_set_task, time_set_task_callback);
//		lv_task_once(time_set_task);
//	}
//	
//	if(*key == LV_KEY_BACKSPACE)
//	{
//		tab_item_cancelfocus();
//	}else if(*key == LV_KEY_LEFT)	//���Ұ����л�������ʱ����ȹ�Ͳ����ʾ����
//	{
//		if(time_roller_index-- == 0){time_roller_index = 0;}
//		timer_roller_change();
//	}else if(*key == LV_KEY_RIGHT)
//	{
//		if(++time_roller_index == TIME_ROLLERS_CNT){time_roller_index = TIME_ROLLERS_CNT - 1;}
//		timer_roller_change();
//	}
//}

static void event_sound_handler(lv_obj_t * obj, lv_event_t event)
{
	const u32 *key = lv_event_get_data();
	if(event == LV_EVENT_VALUE_CHANGED)
	{
		u8 val = lv_switch_get_state(obj);
		hwatch_config.sound_state = val;
	}
	
	if(*key == LV_KEY_BACKSPACE)
	{
		tab_item_cancelfocus();
	}
}

static void event_light_slider_hanlder(lv_obj_t * obj, lv_event_t event)
{
	const u32 *key = lv_event_get_data();
	if(event == LV_EVENT_VALUE_CHANGED)
	{
		u8 val = lv_slider_get_value(obj);
		hwatch_config.blight_level = val;
		
		//���ñ���
		u16 light = MathMap(val, 0, 10, BLIGHT_MIN, BLIGHT_MAX);
		LCD_1IN28_SetBackLight(light);
		lv_label_set_text_fmt(lv_light_slider_label, "LV %d", val);
	}
	
	if(*key == LV_KEY_BACKSPACE)
	{
		tab_item_cancelfocus();
	}
}

static void event_tabview_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{
				tab_item_focus();
			}else if(*key == LV_KEY_BACKSPACE){
				setting_handler(p_apps, app_null);
			}else if(*key == LV_KEY_LEFT){
				
				if(setting_index-- == 0){setting_index = 0;}
				lv_tabview_set_tab_act(lv_setting_tabview, setting_index, LV_ANIM_ON);
				lv_group_focus_obj(lv_tabs[setting_index]);
			
			}else if(*key == LV_KEY_RIGHT){
				
				if(++setting_index == SETTING_COUNT){setting_index = SETTING_COUNT - 1;}
				lv_tabview_set_tab_act(lv_setting_tabview, setting_index, LV_ANIM_ON);
				lv_group_focus_obj(lv_tabs[setting_index]);
				
			}
		}
				break;
    default:
        break;
    }
}

void GUI_Setting_Init(lv_obj_t *psetting, lv_group_t *group, PageRegHandler handler)
{
//	char options_year[301]={0};
//	char options_month[37]={0};
//	char options_date[94]={0};
//	char options_hour[73]={0};
//	char options_min[181]={0};
//	char options_sec[181]={0};
//	char options_week[22]={0};
	
	m_setting = psetting;
	setting_handler = handler;
	
	lv_obj_set_size(m_setting, WIN_W, WIN_H);
	lv_obj_add_style(m_setting, LV_OBJ_PART_MAIN, &page_style);
	lv_obj_set_event_cb( m_setting, event_tabview_handler );
	lv_group_add_obj( group, m_setting );
	
	lv_title_setting_label = lv_label_create(m_setting, NULL);
	lv_label_set_text(lv_title_setting_label,APP_SETTING);
	lv_obj_add_style(lv_title_setting_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(lv_title_setting_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
	
	//����ѡ�
	lv_setting_tabview = lv_tabview_create(m_setting, NULL);
	lv_obj_set_size(lv_setting_tabview, 180, 185);
	lv_obj_add_style(lv_setting_tabview, LV_OBJ_PART_MAIN, &page_style);
	lv_tabview_set_anim_time(lv_setting_tabview, ANIM_TIME);
	lv_tabview_set_btns_pos(lv_setting_tabview, LV_TABVIEW_TAB_POS_TOP);
	lv_obj_add_style(lv_setting_tabview, LV_TABVIEW_PART_TAB_BTN, &icons_style);
	lv_obj_align(lv_setting_tabview, lv_title_setting_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
	
	for(u8 i=0;i<SETTING_COUNT;i++)
	{
		lv_tabs[i] = lv_tabview_add_tab(lv_setting_tabview, list_item_name[i]);
		
		lv_group_add_obj( group, lv_tabs[i] );
		lv_obj_set_event_cb( lv_tabs[i], event_tabview_handler );
	}
	
	//ʱ�� TAB
	TimeSet_Init(lv_tabs[setting_time], group);
//	lv_time_roller_label = lv_label_create(lv_tabs[setting_time], NULL);
//	lv_label_set_long_mode(lv_time_roller_label, LV_LABEL_LONG_SROLL_CIRC );
//	lv_obj_set_size(lv_time_roller_label, 100, 20);
//	lv_obj_add_style(lv_time_roller_label, LV_LABEL_PART_MAIN, &font_style2);
//	lv_label_set_text(lv_time_roller_label, timer_roller_label_txt[time_roller_index]);
//	//lv_obj_set_auto_realign(lv_time_roller_label, true);
//	lv_label_set_align(lv_time_roller_label, LV_LABEL_ALIGN_CENTER);
//	lv_obj_align(lv_time_roller_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);
//	
//	
//	char a_c[3];
//	for(u8 i=0;i<100;i++)
//	{
//		u8 a = i << 1;
//		u8 b = (a + 1);
//		
//		memset(a_c, 0, 3);
//		sprintf(a_c, "%02d\n", i);
//		
//		for(u8 j=0;j<3;j++)
//		{
//			u16 k = i * 3 + j;
//			//������
//			options_year[k] = a_c[j];
//			
//			//����ʱ
//			if(i < 24) 
//			{
//				options_hour[k] = a_c[j];
//			}
//			//���÷֡���
//			if(i < 60)
//			{
//				options_min[k] = a_c[j];
//				options_sec[k] = a_c[j];
//			}
//		}
//		
//		memset(a_c, 0, 3);
//		sprintf(a_c, "%02d\n", (i+1));
//		
//		for(u8 k=0;k<3;k++)
//		{
//			u16 k1 = i * 3 + k;
//			
//			//������
//			if(i < 12) 
//			{
//				options_month[k1] = a_c[k];
//			}
//			
//			//������
//			if(i < 31)
//			{
//				options_date[k1] = a_c[k];
//			}
//			
//			//��������
//			if(i < 7)
//			{
//				options_week[k1] = a_c[k];
//			}
//		}
//	
//	}
//	
//	for(u8 n=0;n<TIME_ROLLERS_CNT;n++)
//	{
//		lv_time_rollers[n] = lv_roller_create(lv_tabs[setting_time], NULL);
//		lv_roller_set_visible_row_count(lv_time_rollers[n], 3);
//		lv_obj_set_width_fit(lv_time_rollers[n], 120);
//		lv_roller_set_selected(lv_time_rollers[n], 0, LV_ANIM_ON);
//		
//		lv_group_add_obj( group, lv_time_rollers[n]);
//		lv_obj_set_event_cb( lv_time_rollers[n], event_roller_handler );
//		
//		//��������
//		switch(n)
//		{
//			case 0:lv_roller_set_options(lv_time_rollers[n], options_year, LV_ROLLER_MODE_INFINITE);break;
//			case 1:lv_roller_set_options(lv_time_rollers[n], options_month, LV_ROLLER_MODE_INFINITE);break;
//			case 2:lv_roller_set_options(lv_time_rollers[n], options_date, LV_ROLLER_MODE_INFINITE);break;
//			case 3:lv_roller_set_options(lv_time_rollers[n], options_hour, LV_ROLLER_MODE_INFINITE);break;
//			case 4:lv_roller_set_options(lv_time_rollers[n], options_min, LV_ROLLER_MODE_INFINITE);break;
//			case 5:lv_roller_set_options(lv_time_rollers[n], options_sec, LV_ROLLER_MODE_INFINITE);break;		
//			case 6:lv_roller_set_options(lv_time_rollers[n], options_week, LV_ROLLER_MODE_INFINITE);break;		
//		}
//		
//		lv_obj_align(lv_time_rollers[n], NULL, LV_ALIGN_CENTER, 0, 5);
//	
//		if(n > 0){lv_obj_set_hidden(lv_time_rollers[n], true);}
//	}
	
	
	//���� TAB
	lv_light_slider = lv_slider_create(lv_tabs[setting_blight], NULL);
	lv_obj_set_size(lv_light_slider, 120, 10);
	lv_slider_set_anim_time(lv_light_slider, ANIM_TIME);
	lv_slider_set_range(lv_light_slider, 0, 10);
	lv_slider_set_value(lv_light_slider, hwatch_config.blight_level, LV_ANIM_ON);
	lv_obj_align(lv_light_slider, NULL, LV_ALIGN_CENTER, 0, 0);
	
	lv_light_slider_label = lv_label_create(lv_tabs[setting_blight], NULL);
	lv_obj_add_style(lv_title_setting_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_label_set_text_fmt(lv_light_slider_label, "LV %d", hwatch_config.blight_level);
	lv_obj_set_auto_realign(lv_light_slider_label, true);
	lv_obj_align(lv_light_slider_label, lv_light_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	lv_group_add_obj( group, lv_light_slider);
	lv_obj_set_event_cb( lv_light_slider, event_light_slider_hanlder );
	
	//���� TAB
	lv_sound_switch = lv_switch_create(lv_tabs[setting_sound], NULL);
	lv_obj_set_size(lv_sound_switch, 60, 24);
	lv_switch_set_anim_time(lv_sound_switch, ANIM_TIME);
	lv_obj_align(lv_sound_switch, NULL, LV_ALIGN_CENTER, 0, 0);
	
	lv_group_add_obj( group, lv_sound_switch);
	lv_obj_set_event_cb( lv_sound_switch, event_sound_handler );
	
	if(hwatch_config.sound_state)
	{
		lv_switch_on(lv_sound_switch, LV_ANIM_ON);
	}else
	{
		lv_switch_off(lv_sound_switch, LV_ANIM_OFF);
	}
	
	//���� TAB
	AlarmClock_Init(lv_tabs[setting_alarm], group);
	
	//���� TAB
//	label = lv_label_create(lv_tabs[setting_task], NULL);
//	lv_label_set_text(label, "5 tab");
}

void GUI_Setting_FocusInit()
{
	lv_group_focus_obj(lv_tabs[setting_index]);
	
}

static void tab_item_focus()
{
	switch(setting_index)
	{
		case setting_time:
			//timer_roller_change();
			TimeSet_FocusInit();
			break;
		case setting_blight:
			lv_group_focus_obj(lv_light_slider);
			break;
		case setting_sound:
			lv_group_focus_obj(lv_sound_switch);
			break;
		case setting_alarm:
			AlarmClock_FocusInit();
			break;
		case setting_task:
			break;
	}
}

static void tab_item_cancelfocus()
{
	u16 cur_tab_index = lv_tabview_get_tab_act(lv_setting_tabview);
	lv_group_focus_obj(lv_tabs[setting_index]);
}

void timer_roller_change()
{
	u8 i;
	
	for(i=0;i<TIME_ROLLERS_CNT;i++)
	{
		if(lv_obj_get_hidden(lv_time_rollers[i]) == false)
		{
			lv_obj_set_hidden(lv_time_rollers[i], true);
		}
	}
	
	lv_obj_set_hidden(lv_time_rollers[time_roller_index], false);
	lv_group_focus_obj(lv_time_rollers[time_roller_index]);
	lv_label_set_text(lv_time_roller_label, timer_roller_label_txt[time_roller_index]);
}




