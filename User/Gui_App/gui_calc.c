#include "gui_calc.h"

static lv_obj_t *m_calc;

lv_obj_t *lv_title_calc_label;
lv_obj_t *lv_enter_label;
lv_obj_t *lv_result_label;
lv_obj_t *lv_btnm;

PageRegHandler calc_handler;

/**

KEY

0 	1 	2 	3
4 	5 	6 	7
8 	9 	10 	11
12  13 	14 	15
16  17 	18 	19

*/

static const char * calc_btn_map[] = {	
	"(", ")", "CE", "/", "\n",
	"7", "8", "9", "*", "\n",
	"4", "5", "6", "-", "\n",
	"1", "2", "3", "+", "\n",
	".", "0", "ANS", "=", ""
};

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{
				//const char * txt = lv_btnmatrix_get_active_btn_text(obj);
				//printf("Click key val: %s\n", txt);
				
				Calc_Run(lv_btnmatrix_get_active_btn(lv_btnm));
				
			}else if(*key == LV_KEY_BACKSPACE){
				Calc_Exit();
				lv_label_set_text(lv_enter_label,"");
				lv_label_set_text_fmt(lv_result_label, "0");
				calc_handler(p_apps, app_null);
			}
		}
				break;
    default:
        break;
    }
}

void calc_callback(char *enter_str, double result)
{
	char out[100];
	printf("%s, %1f\n", enter_str, result);
	sprintf(out, "%1f", result);
	lv_label_set_text(lv_enter_label,enter_str);
	lv_label_set_text_fmt(lv_result_label, out);
}

void GUI_Calc_Init(lv_obj_t *pcalc, lv_group_t *group, PageRegHandler handler)
{
	m_calc = pcalc;
	calc_handler = handler;
	
	lv_obj_set_size(m_calc, WIN_W, WIN_H);
	lv_obj_add_style(m_calc, LV_OBJ_PART_MAIN, &page_style);
	lv_obj_set_event_cb( m_calc, event_handler );
	
	lv_title_calc_label = lv_label_create(m_calc, NULL);
	lv_label_set_text(lv_title_calc_label,APP_CALC);
	lv_obj_add_style(lv_title_calc_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(lv_title_calc_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
	
	//输入显示标签
	lv_enter_label = lv_label_create(m_calc, NULL);
	lv_label_set_long_mode(lv_enter_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_size(lv_enter_label, 168, 16);
	lv_label_set_text(lv_enter_label,"");
	lv_label_set_align(lv_enter_label, LV_LABEL_ALIGN_RIGHT);
	lv_obj_add_style(lv_enter_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_enter_label, lv_title_calc_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	
	//结果显示标签
	lv_result_label = lv_label_create(m_calc, NULL);
	lv_label_set_long_mode(lv_result_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_size(lv_result_label, 168, 16);
	lv_label_set_text(lv_result_label,"0");
	lv_label_set_align(lv_result_label, LV_LABEL_ALIGN_RIGHT);
	lv_obj_add_style(lv_result_label, LV_LABEL_PART_MAIN, &font_style1);
	lv_obj_align(lv_result_label, lv_enter_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

	//按钮矩阵
	lv_btnm = lv_btnmatrix_create(m_calc, NULL);
	lv_obj_set_size(lv_btnm, 180, 140);
	lv_btnmatrix_set_map(lv_btnm, calc_btn_map);
	lv_obj_align(lv_btnm, lv_result_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
	
	lv_group_add_obj( group, lv_btnm );
	lv_obj_set_event_cb(lv_btnm, event_handler);
	
	//计算器相关初始化
	Init_Calc_Book(calc_callback);
}

void GUI_Calc_FocusInit()
{
	lv_group_focus_obj(lv_btnm);
}