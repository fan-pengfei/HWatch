#include "gui_setting_alarmclock.h"

static const char * alarm_btn_map[] = {	
	"SUN.", "0", "0", "\n",
	"Add", "Del" ,"Off", "",
};

//使用按钮组实现
lv_obj_t *m_tab;
//时、分、秒、星期设置按钮组
lv_obj_t *lv_alarm_btnm;
lv_obj_t *lv_alarm_info_label;
static u8 day_index = 0;
static u8 hour_index = 0;
static u8 min_index = 0;
static u8 alarm_set_type = 0; //添加：0；删除：1
//闹钟列表
lv_obj_t *alarm_clock_list;

void alarm_state_change(void);
static void btnm_key_exec(void);

static void btnm_map_change(u16 index, const char * txt);

static void event_ac_list_handler(lv_obj_t * obj, lv_event_t event)
{
	const u32 *key = lv_event_get_data();
	if(*key == LV_KEY_ENTER)
	{
		lv_obj_t *select = lv_list_get_btn_selected(obj);
		//lv_obj_t *prev;
		
		if(select != NULL)
		{
			//prev = lv_list_get_prev_btn(obj, select);
			u32 select_index = lv_list_get_btn_index(obj, select);
			
			//修改所选闹钟列表项
			//printf("Alarm clock del: %d, %s\n", select_index, lv_list_get_btn_text(select));
			//修改配置
			char ac_str[7];
			if(alarm_set_type)
			{
				//删除操作
				//lv_list_remove(obj, select_index);
				sprintf(ac_str, "000000");
			}else
			{
				//添加操作
				sprintf(ac_str, "%02d%s%s", 
					(day_index+1),
					lv_btnmatrix_get_btn_text(lv_alarm_btnm, 1), 
					lv_btnmatrix_get_btn_text(lv_alarm_btnm, 2));
				
			}
			{
				lv_obj_t *label = lv_list_get_btn_label(select);
				lv_label_set_text(label,ac_str);
				
				g_config.hwatch_config.alarm_clock[select_index] = atoi(lv_label_get_text(label));
				printf("%06d\n", g_config.hwatch_config.alarm_clock[select_index]);
			}
			
			//重新设定焦点
			//if(prev != NULL) lv_list_focus_btn(obj, prev);
		}
		
	}else if(*key == LV_KEY_BACKSPACE)
	{
		//HWatchDevice_ConfigSave();	//退出设置页面时保存配置
		AlarmClock_FocusInit();
	}
}

static void event_ac_btnm_handler(lv_obj_t * obj, lv_event_t event)
{
	switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{			
				btnm_key_exec();
				
			}else if(*key == LV_KEY_BACKSPACE)
			{
				alarm_state_change();
				lv_group_focus_obj(m_tab);
			}
		}
			break;
	}
}

void AlarmClock_Init(lv_obj_t *tab, lv_group_t *group)
{
	u8 i;
	char clock[7];
	m_tab = tab;
	lv_coord_t tab_w = lv_obj_get_width(tab) - 6; 
	
	//创建按钮矩阵
	lv_alarm_btnm = lv_btnmatrix_create(tab, NULL);
	lv_obj_set_size(lv_alarm_btnm, tab_w, 46);
	lv_btnmatrix_set_map(lv_alarm_btnm, alarm_btn_map);
	lv_obj_align(lv_alarm_btnm, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	
	lv_group_add_obj( group, lv_alarm_btnm );
	lv_obj_set_event_cb(lv_alarm_btnm, event_ac_btnm_handler);
	
	//创建本地所保存的闹钟列表
	alarm_clock_list = lv_list_create(tab, NULL);
	lv_obj_set_width_fit(alarm_clock_list, tab_w);
	lv_obj_set_height_fit(alarm_clock_list, 72);
	lv_list_set_edge_flash(alarm_clock_list, true);
	lv_list_set_scroll_propagation(alarm_clock_list,true);
	lv_list_set_anim_time(alarm_clock_list, 100);
	lv_obj_align(alarm_clock_list, lv_alarm_btnm, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	
	lv_group_add_obj( group, alarm_clock_list );
	lv_obj_set_event_cb(alarm_clock_list, event_ac_list_handler);
	
	for(i=0;i<ALARM_CLOCK_CNT_MAX;i++)
	{
		u8 x = 0;
		sprintf(clock, "%06d", g_config.hwatch_config.alarm_clock[i]);
		lv_obj_t *item = lv_list_add_btn(alarm_clock_list, NULL, clock);
		//lv_btn_set_layout(item, LV_LAYOUT_OFF);
		
		lv_group_add_obj( group, item );
		//lv_obj_set_event_cb(item, event_ac_list_handler);

	}
	
	//信息提示标签
	lv_alarm_info_label = lv_label_create(tab, NULL);
	lv_label_set_text(lv_alarm_info_label,"D,H,M");
	lv_obj_add_style(lv_alarm_info_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_label_set_align(lv_alarm_info_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(lv_alarm_info_label, tab, LV_ALIGN_IN_BOTTOM_MID, 0, -8);

	//从配置设置闹钟状态
	alarm_state_change();
}

void AlarmClock_FocusInit()
{
	alarm_state_change();
	lv_group_focus_obj(lv_alarm_btnm);
}

static void btnm_key_exec()
{
	//const char * txt = lv_btnmatrix_get_active_btn_text(lv_alarm_btnm);
	//printf("Click key val: %s\n", txt);

	u16 cur_btn_index = lv_btnmatrix_get_focused_btn(lv_alarm_btnm);
	
	switch(cur_btn_index)
	{
		case 0: //设置星期
			day_index++;
			if(day_index>=7) day_index = 0;
			break;
		case 1:	//设置时
			hour_index++;
			if(hour_index >= 60) hour_index = 0; 
			break;
		case 2:	//设置分
			min_index++;
			if(min_index>=60)min_index = 0;
			break;
		case 3:	//添加闹钟
			alarm_set_type = 0;
			lv_group_focus_obj(alarm_clock_list);
			break;
		case 4:	//删除闹钟
			alarm_set_type = 1;
			lv_group_focus_obj(alarm_clock_list);
			return;
		case 5:	//开启|关闭闹钟
			g_config.hwatch_config.alarm_state = !g_config.hwatch_config.alarm_state;
			alarm_state_change();
			break;
	}
	
	alarm_state_change();
}

void alarm_state_change()
{
	//星期更改
	alarm_btn_map[0] = weeks[day_index];
	//时更改
	alarm_btn_map[1] = gui_number[hour_index];
	//分更改
	alarm_btn_map[2] = gui_number[min_index];
	
	if(g_config.hwatch_config.alarm_state)
	{
		btnm_map_change(6, "On");
	}else
	{
		btnm_map_change(6, "Off");
	}
}

static void btnm_map_change(u16 index, const char * txt)
{
	alarm_btn_map[index] = txt;
	lv_btnmatrix_set_map(lv_alarm_btnm, alarm_btn_map);
	lv_obj_invalidate(lv_alarm_btnm);
}
