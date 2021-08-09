#include "gui_setting_task.h"

static const char * task_btnm_map[] = {
	"Screen lock","\n",
	"Start","Reset",""
};

lv_obj_t *setting_task_tab;
lv_group_t *gg;

lv_obj_t *reset_time_dlist;
lv_obj_t *lv_task_btnm;
lv_obj_t *lv_stopwatch_label;

//秒表列表
lv_obj_t *stop_watch_list;
lv_obj_t *stop_watch_listbtn[STOP_WATCH_MAX];

//秒表数值
StopWatchVars stop_watch_vars;
//秒表更新任务
lv_task_t *stop_watch_task;

static void stop_watch_add(void);
static void stop_watch_clear(void);
static void stop_watch_set(u8 min, u8 sec, u8 ms);
static void btnm_click_exec(void);

static void watch_update_task(lv_task_t *task)
{
	//10ms更新一次
	if(++stop_watch_vars.watch_ms >= 100)
	{
		stop_watch_vars.watch_ms = 0;
		//更新秒数
		if(++stop_watch_vars.watch_sec >= 60)
		{
			stop_watch_vars.watch_sec = 0;
			//更新分数
			if(++stop_watch_vars.watch_min >= 60)
				stop_watch_vars.watch_min = 0;
		}
	}
	
	//printf("%03d\n", stop_watch_vars.watch_ms);
	//更新标签显示
	stop_watch_set(stop_watch_vars.watch_min, stop_watch_vars.watch_sec, stop_watch_vars.watch_ms);
}

static void event_task_handler(lv_obj_t * obj, lv_event_t event)
{
	
	switch (event) {
		case LV_EVENT_VALUE_CHANGED:
		{
				if(obj == reset_time_dlist)
				{
					char buf[6];
					lv_dropdown_get_selected_str(reset_time_dlist, buf, 3);
					g_config.hwatch_config.home_screen_lock_t = atoi(buf);
					printf("Screen lock time: %d\n", g_config.hwatch_config.home_screen_lock_t);
				}
		}
			break;
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{			
				if(obj == lv_task_btnm)
				{
					btnm_click_exec();
				}else if(obj == stop_watch_list)
				{
					//记录秒表数据
					stop_watch_add();
				}
			}else if(*key == LV_KEY_BACKSPACE)
			{
				if(obj == lv_task_btnm)
				{
					lv_group_focus_obj(setting_task_tab);
				}else
				{
					lv_group_focus_obj(lv_task_btnm);
					
					//删除秒表更新任务
					if(obj == stop_watch_list)
					{
						lv_task_del(stop_watch_task);
					}
				}
			}
		}
			break;
	}
}

void SettingTask_Init(lv_obj_t *tab, lv_group_t *group)
{
	setting_task_tab = tab;
	gg = group;
	lv_coord_t tab_w = lv_obj_get_width(tab) - 5; 
	
	//创建切换控制按钮矩阵
	lv_task_btnm = lv_btnmatrix_create(tab, NULL);
	lv_obj_set_size(lv_task_btnm, tab_w, 46);
	lv_btnmatrix_set_map(lv_task_btnm, task_btnm_map);
	lv_obj_align(lv_task_btnm, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	
	lv_group_add_obj( group, lv_task_btnm );
	lv_obj_set_event_cb(lv_task_btnm, event_task_handler);
	
	//息屏时间下拉列表
	reset_time_dlist = lv_dropdown_create(tab, NULL);
	lv_dropdown_set_max_height(reset_time_dlist, 60);
	lv_dropdown_set_options(reset_time_dlist, SCREEN_RESET_TIME_OPTIONS);
	lv_obj_align(reset_time_dlist, lv_task_btnm, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);
	
	lv_group_add_obj( group, reset_time_dlist );
	lv_obj_set_event_cb(reset_time_dlist, event_task_handler);
	
	lv_dropdown_set_selected(reset_time_dlist, (g_config.hwatch_config.home_screen_lock_t/10)-1);
	
	//创建秒表显示标签
	lv_stopwatch_label = lv_label_create(tab, NULL);
	lv_label_set_text(lv_stopwatch_label,"00:00:00");
	lv_obj_add_style(lv_stopwatch_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_label_set_align(lv_stopwatch_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(lv_stopwatch_label, reset_time_dlist, LV_ALIGN_OUT_RIGHT_MID, 26, 0);
	
	//创建秒表列表
	stop_watch_list = lv_list_create(tab, NULL);
	lv_obj_set_width_fit(stop_watch_list, tab_w);
	lv_obj_set_height_fit(stop_watch_list, 68);
	lv_list_set_edge_flash(stop_watch_list, true);
	lv_list_set_scroll_propagation(stop_watch_list,true);
	lv_list_set_anim_time(stop_watch_list, 100);
	lv_obj_align(stop_watch_list, tab, LV_ALIGN_IN_BOTTOM_MID, 0, -6);
	
	lv_group_add_obj( group, stop_watch_list );
	lv_obj_set_event_cb(stop_watch_list, event_task_handler);

}

void SettingTask_FocusInit(void)
{
	lv_group_focus_obj(lv_task_btnm);
}

static void btnm_click_exec()
{
	u16 cur_btn_index = lv_btnmatrix_get_focused_btn(lv_task_btnm);
	
	switch(cur_btn_index)
	{
		case 0:	//设置息屏时间
			lv_dropdown_set_selected(reset_time_dlist, (g_config.hwatch_config.home_screen_lock_t/10)-1);
			lv_group_focus_obj(reset_time_dlist);
			break;
		case 1:	//秒表功能
			//重置秒表
			stop_watch_set(0, 0, 0);
			//创建秒表更新任务，10ms更新
			stop_watch_task = lv_task_create(watch_update_task, 10, LV_TASK_PRIO_HIGHEST, NULL);
			lv_group_focus_obj(stop_watch_list);
			break;
		case 2:	//重置秒表
			stop_watch_clear();
			break;
	}
}

static void stop_watch_set(u8 min, u8 sec, u8 ms)
{	
	stop_watch_vars.watch_min = min;
	stop_watch_vars.watch_sec = sec;
	stop_watch_vars.watch_ms = ms;
	
	lv_label_set_text_fmt(lv_stopwatch_label, "%02d:%02d:%02d", 
												min,
												sec,
												ms);
}

static void stop_watch_add()
{
	
	if(stop_watch_vars.watch_num > STOP_WATCH_MAX)
	{
		printf("Stop watch max\n");
		//达到上限
		return;
	}
	
	//lv_obj_t *cur_select = lv_list_get_btn_selected(stop_watch_list);
	
	char watch_data[10];
	sprintf(watch_data, "%02d -- %02d:%02d:%02d", 
											stop_watch_vars.watch_num,
											stop_watch_vars.watch_min,
											stop_watch_vars.watch_sec,
											stop_watch_vars.watch_ms);
	stop_watch_listbtn[stop_watch_vars.watch_num] = lv_list_add_btn(stop_watch_list, NULL, watch_data);
	
	//焦点切换
	if(stop_watch_vars.watch_num > 0) lv_group_remove_obj(stop_watch_listbtn[stop_watch_vars.watch_num-1]);
	lv_group_add_obj( gg, stop_watch_listbtn[stop_watch_vars.watch_num] );
	lv_group_focus_obj(stop_watch_listbtn[stop_watch_vars.watch_num]);
	
	stop_watch_vars.watch_num++;
}

static void stop_watch_clear()
{
	u16 i;
	stop_watch_vars.watch_num = 0;
	stop_watch_set(0, 0, 0);

	u16 size =  lv_list_get_size(stop_watch_list); 
	for(i=0;i<size;i++)
	{
		//printf("remove focus %d\n", i);
		lv_group_remove_obj(stop_watch_listbtn[i]);
	}
	
	//清除列表
	lv_list_clean(stop_watch_list);
}
