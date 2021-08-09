#include "gui_games.h"

static const char* list_item_name[] ={
	"Snake",
	"Tank",
	"Tetris",
	"2048",
	"Hit&Plane",
	"Block",
};

static lv_obj_t *m_games;

lv_obj_t *lv_title_games_label;
lv_obj_t * games_list;

static lv_obj_t * list_btns[GAME_COUNT];

PageRegHandler games_handler;

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    switch (event) {
		case LV_EVENT_KEY:
		{
			const u32 *key = lv_event_get_data();
			if(*key == LV_KEY_ENTER)
			{
				lv_obj_t *select = lv_list_get_btn_selected(obj);
				
				if(select != NULL)
				{
					u32 select_index = lv_list_get_btn_index(obj, select);
					printf("Clicked: %s\n", lv_list_get_btn_text(select));
					
					games_handler(p_games, select_index);
				}
				
			}else if(*key == LV_KEY_BACKSPACE){
				games_handler(p_apps, app_null);
				
			}
		}
				break;
    default:
        break;
    }
}

void GUI_Games_Init(lv_obj_t *pgames, lv_group_t *group, PageRegHandler handler)
{
	u8 i;
	m_games = pgames;
	games_handler = handler;
	
	lv_obj_set_size(m_games, WIN_W, WIN_H);
	lv_obj_add_style(m_games, LV_OBJ_PART_MAIN, &page_style);
	lv_obj_set_event_cb( m_games, event_handler );
	lv_group_add_obj( group, m_games );
	
	lv_title_games_label = lv_label_create(m_games, NULL);
	lv_label_set_text(lv_title_games_label,APP_GAMES);
	lv_obj_add_style(lv_title_games_label, LV_LABEL_PART_MAIN, &font_style2);
	lv_obj_align(lv_title_games_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

	games_list = lv_list_create(m_games, NULL);
	lv_obj_set_width_fit(games_list, 120);
	lv_obj_set_height_fit(games_list, 160);
	lv_list_set_edge_flash(games_list, true);
	lv_list_set_scroll_propagation(games_list,true);
	lv_list_set_anim_time(games_list, 100);
	lv_obj_align(games_list, lv_title_games_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	lv_group_add_obj( group, games_list );
	lv_obj_set_event_cb( games_list, event_handler );
	
	for(i=0;i<GAME_COUNT;i++)
	{
		list_btns[i] = lv_list_add_btn(games_list, NULL, list_item_name[i]);
	
	}
}

void GUI_Games_FocusInit()
{
	lv_group_focus_obj(games_list);
}