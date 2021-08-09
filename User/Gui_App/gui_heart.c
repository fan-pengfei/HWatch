#include "gui_heart.h"

static lv_obj_t *m_heart;

lv_obj_t *lv_title_heart_label;
lv_obj_t *lv_chart;
lv_obj_t *lv_heart_label;

lv_chart_series_t * ser1;
lv_task_t *beat_task;

PageRegHandler heart_handler;

static void chart_clear(void);
u16 ser_add_count = 0;

void PulseSensor_Callback(_PulseData *p)
{
	//printf("bpm: %d\n", p->bpm);
	lv_label_set_text_fmt(lv_heart_label,"%d", p->bpm);

}

void heart_task(lv_task_t *task)
{
	PulseSensor_Update();
	
	if(PulseSensor_IsRun())
	{
		lv_chart_set_next(lv_chart, ser1, HWatch_adc.adc_pulse/5);
		
		ser_add_count++;
		if(ser_add_count >= 200)
		{
			chart_clear();
			ser_add_count = 0;
		}else if(ser_add_count % 10 == 0){
			lv_chart_refresh(lv_chart);
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
				if(PulseSensor_IsRun() == FALSE)
				{
					PulseSensor_Start();
				}else{
					PulseSensor_Stop();
					lv_label_set_text(lv_heart_label,"0");
					chart_clear();
				}
			}else if(*key == LV_KEY_BACKSPACE){
				lv_task_del(beat_task);
				lv_label_set_text(lv_heart_label,"0");
				PulseSensor_Stop();
				chart_clear();
				heart_handler(p_apps, app_null);
			}
		}
				break;
    default:
        break;
    }
}

void GUI_Heart_Init(lv_obj_t *pheart, lv_group_t *group, PageRegHandler handler)
{
	m_heart = pheart;
	heart_handler = handler;
	
	lv_obj_set_size(m_heart, WIN_W, WIN_H);
	lv_obj_add_style(m_heart, LV_OBJ_PART_MAIN, &page_style);
	lv_obj_set_event_cb( m_heart, event_handler );
	lv_group_add_obj( group, m_heart );
	
	lv_title_heart_label = lv_label_create(m_heart, NULL);
	lv_label_set_text(lv_title_heart_label,APP_HEART);
	lv_obj_add_style(lv_title_heart_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(lv_title_heart_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
	
	//心率曲线图表
	lv_chart = lv_chart_create(m_heart, NULL);
	lv_obj_set_size(lv_chart, 160, 100);
//	lv_obj_set_style_local_size(lv_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); 
	lv_chart_set_range(lv_chart, 360, 480);
	lv_chart_set_point_count(lv_chart, 80);
	lv_chart_set_type(lv_chart, LV_CHART_TYPE_LINE);
	lv_chart_set_update_mode(lv_chart, LV_CHART_UPDATE_MODE_SHIFT);
//	lv_chart_set_update_mode(lv_chart, LV_CHART_UPDATE_MODE_CIRCULAR);
	lv_obj_align(lv_chart, lv_title_heart_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	ser1 = lv_chart_add_series(lv_chart, LV_COLOR_RED);	
	
	lv_obj_add_style(lv_chart, LV_CHART_PART_SERIES, &chart_style);
	lv_chart_refresh(lv_chart);
	
	lv_heart_label = lv_label_create(m_heart, NULL);
	lv_label_set_long_mode(lv_heart_label, LV_LABEL_LONG_SROLL_CIRC );
	lv_obj_set_size(lv_heart_label, 180, 36);
	lv_label_set_text(lv_heart_label,"0");
	lv_label_set_align(lv_heart_label, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(lv_heart_label, LV_LABEL_PART_MAIN, &font_style);
	lv_obj_align(lv_heart_label, lv_chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
	
	PulseSensorReg(PulseSensor_Callback);
}

void GUI_Heart_FocusInit()
{
	lv_group_focus_obj(m_heart);
	
	//创建任务
	beat_task = lv_task_create(heart_task, 20, LV_TASK_PRIO_MID, NULL);
}

static void chart_clear()
{
	lv_chart_clear_series(lv_chart, ser1);
	lv_chart_refresh(lv_chart);
	printf("chart clear\n");
}