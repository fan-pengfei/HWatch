/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */
 
#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif


/***************************
 * CURRENT VERSION OF LVGL
 ***************************/
#define LVGL_VERSION_MAJOR 7
#define LVGL_VERSION_MINOR 9
#define LVGL_VERSION_PATCH 0
#define LVGL_VERSION_INFO "dev"

/*********************
 *      INCLUDES
 *********************/

#include "master/src/lv_misc/lv_log.h"
#include "master/src/lv_misc/lv_task.h"
#include "master/src/lv_misc/lv_math.h"
#include "master/src/lv_misc/lv_async.h"

#include "master/src/lv_hal/lv_hal.h"

#include "master/src/lv_core/lv_obj.h"
#include "master/src/lv_core/lv_group.h"
#include "master/src/lv_core/lv_indev.h"

#include "master/src/lv_core/lv_refr.h"
#include "master/src/lv_core/lv_disp.h"

#include "master/src/lv_themes/lv_theme.h"

#include "master/src/lv_font/lv_font.h"
#include "master/src/lv_font/lv_font_loader.h"
#include "master/src/lv_font/lv_font_fmt_txt.h"
#include "master/src/lv_misc/lv_printf.h"

#include "master/src/lv_widgets/lv_btn.h"
#include "master/src/lv_widgets/lv_imgbtn.h"
#include "master/src/lv_widgets/lv_img.h"
#include "master/src/lv_widgets/lv_label.h"
#include "master/src/lv_widgets/lv_line.h"
#include "master/src/lv_widgets/lv_page.h"
#include "master/src/lv_widgets/lv_cont.h"
#include "master/src/lv_widgets/lv_list.h"
#include "master/src/lv_widgets/lv_chart.h"
#include "master/src/lv_widgets/lv_table.h"
#include "master/src/lv_widgets/lv_checkbox.h"
#include "master/src/lv_widgets/lv_cpicker.h"
#include "master/src/lv_widgets/lv_bar.h"
#include "master/src/lv_widgets/lv_slider.h"
#include "master/src/lv_widgets/lv_led.h"
#include "master/src/lv_widgets/lv_btnmatrix.h"
#include "master/src/lv_widgets/lv_keyboard.h"
#include "master/src/lv_widgets/lv_dropdown.h"
#include "master/src/lv_widgets/lv_roller.h"
#include "master/src/lv_widgets/lv_textarea.h"
#include "master/src/lv_widgets/lv_canvas.h"
#include "master/src/lv_widgets/lv_win.h"
#include "master/src/lv_widgets/lv_tabview.h"
#include "master/src/lv_widgets/lv_tileview.h"
#include "master/src/lv_widgets/lv_msgbox.h"
#include "master/src/lv_widgets/lv_objmask.h"
#include "master/src/lv_widgets/lv_gauge.h"
#include "master/src/lv_widgets/lv_linemeter.h"
#include "master/src/lv_widgets/lv_switch.h"
#include "master/src/lv_widgets/lv_arc.h"
#include "master/src/lv_widgets/lv_spinner.h"
#include "master/src/lv_widgets/lv_calendar.h"
#include "master/src/lv_widgets/lv_spinbox.h"

#include "master/src/lv_draw/lv_img_cache.h"

#include "master/src/lv_api_map.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 * */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))


#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/
