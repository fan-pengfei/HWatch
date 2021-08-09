#include "gui_setting_time.h"

static const char * time_btn_map[] = {	
	"0", "0", "0", "\n",
	"0", "1", "1","SUN.","",
};

TimeBtnsIndex time_btn_indexs;

//使用按钮组实现
lv_obj_t *m_tab_time;
//按钮组
//时、分、秒
//年、月、日、星期
///////////////////////
//年：0-99
//月：1-12
//日：1-31
//时：0-23
//分：0-59
//秒：0-59
//星期：1-7 //1为星期日
//////////////////////
lv_obj_t *lv_time_btnm;
lv_obj_t *lv_time_info_label;

static void btnm_key_exec(void);
static void btnm_map_change(u16 index, const char * txt);

static void event_ac_btnm_handler(lv_obj_t * obj, lv_event_t event)
{
	switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{			
				//btnm_key_exec();
				
			}else if(*key == LV_KEY_BACKSPACE)
			{
				lv_group_focus_obj(m_tab_time);
			}else if(*key == LV_KEY_UP)
			{
				btnm_key_exec();
			}else if(*key == LV_KEY_DOWN)
			{
				btnm_key_exec();
			}
		}
			break;
	}
}

void TimeSet_Init(lv_obj_t *tab, lv_group_t *group)
{
	m_tab_time = tab;
	lv_coord_t tab_w = lv_obj_get_width(tab) - 6; 
	
	//创建按钮矩阵
	lv_time_btnm = lv_btnmatrix_create(tab, NULL);
	lv_obj_set_size(lv_time_btnm, tab_w, 80);
	lv_btnmatrix_set_map(lv_time_btnm, time_btn_map);
	lv_obj_align(lv_time_btnm, tab, LV_ALIGN_IN_TOP_MID, 0, 10);
	
	lv_group_add_obj( group, lv_time_btnm );
	lv_obj_set_event_cb(lv_time_btnm, event_ac_btnm_handler);
	
	//信息提示标签
	lv_time_info_label = lv_label_create(tab, NULL);
	lv_label_set_text(lv_time_info_label,"Hour,Min,Sec\nYear,Month,Date,Day");
	lv_obj_add_style(lv_time_info_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_label_set_align(lv_time_info_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(lv_time_info_label, tab, LV_ALIGN_IN_BOTTOM_MID, 0, -8);
	
	//初始化相关变量
	time_btn_indexs.month_index = 1;
	time_btn_indexs.date_index = 1;
}

void TimeSet_FocusInit()
{
	lv_group_focus_obj(lv_time_btnm);
}

static void btnm_key_exec()
{
	//const char * txt = lv_btnmatrix_get_active_btn_text(lv_time_btnm);
	//printf("Click key val: %s\n", txt);

	u16 cur_btn_index = lv_btnmatrix_get_focused_btn(lv_time_btnm);
	
	switch(cur_btn_index)
	{
		case 0:	//设置时
			time_btn_indexs.hour_index++;
			if(time_btn_indexs.hour_index >= 24) time_btn_indexs.hour_index = 0;
			time_btn_map[0] = gui_number[time_btn_indexs.hour_index];
			break;
		case 1:	//设置分
			time_btn_indexs.min_index++;
			if(time_btn_indexs.min_index >= 60) time_btn_indexs.min_index = 0;
			time_btn_map[1] = gui_number[time_btn_indexs.min_index];
			break;
		case 2:	//设置秒
			time_btn_indexs.sec_index++;
			if(time_btn_indexs.sec_index >= 60) time_btn_indexs.sec_index = 0;
			time_btn_map[2] = gui_number[time_btn_indexs.sec_index];
			break;
		case 3:	//设置年
			time_btn_indexs.year_index++;
			if(time_btn_indexs.year_index >= 100) time_btn_indexs.year_index = 0;
			time_btn_map[4] = gui_number[time_btn_indexs.year_index];
			break;
		case 4:	//设置月
			time_btn_indexs.month_index++;
			if(time_btn_indexs.month_index >= 13) time_btn_indexs.month_index = 1;
			time_btn_map[5] = gui_number[time_btn_indexs.month_index];
			break;
		case 5:	//设置日
			time_btn_indexs.date_index++;
			if(time_btn_indexs.date_index >= 32) time_btn_indexs.date_index = 1;
			time_btn_map[6] = gui_number[time_btn_indexs.date_index];
			break;
		case 6:	//设置星期
			time_btn_indexs.day_index++;
			if(time_btn_indexs.day_index >= 7) time_btn_indexs.day_index = 0;
			time_btn_map[7] = weeks[time_btn_indexs.day_index];
			break;
	}
	
	//重新设置map数组
	lv_btnmatrix_set_map(lv_time_btnm, time_btn_map);
	lv_obj_invalidate(lv_time_btnm);
	
	lv_btnmatrix_set_focused_btn(lv_time_btnm, cur_btn_index);
}

