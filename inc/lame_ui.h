/*
 * lame_ui.h
 *
 *  Created on: 02-Apr-2020
 *	Last updated: 18-Apr-2021
 *      Author: rik
 */

#ifndef INC_LAME_UI_H_
#define INC_LAME_UI_H_

#include <stdlib.h>
#include <stdio.h>
#include "bitmap_typedefs.h"

#ifndef NULL
	#define NULL							((void *)0)
#endif

#define	LUI_MAX_OBJECTS						130
#define LUI_MAX_SCENES						3


#define	_LUI_R_POS_RGB   					11	// Red last bit position for RGB display
#define	_LUI_G_POS_RGB   					5 	// Green last bit position for RGB display
#define	_LUI_B_POS_RGB   					0	// Blue last bit position for RGB display

#define	_LUI_RGB(R,G,B) 					(((uint16_t)(R >> 3) << _LUI_R_POS_RGB) | \
											((uint16_t)(G >> 2) << _LUI_G_POS_RGB) | \
											((uint16_t)(B >> 3) << _LUI_B_POS_RGB))


#define LUI_STYLE_BUTTON_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_BUTTON_PRESSED_COLOR		_LUI_RGB(0, 211, 222)
#define	LUI_STYLE_BUTTON_SELECTION_COLOR	_LUI_RGB(0, 170, 179)
#define LUI_STYLE_BUTTON_BG_COLOR			_LUI_RGB(0, 146, 153)
#define LUI_STYLE_BUTTON_BORDER_COLOR		_LUI_RGB(57, 62, 70)
#define LUI_STYLE_BUTTON_BORDER_VISIBLE		0
#define LUI_STYLE_BUTTON_WIDTH				40
#define LUI_STYLE_BUTTON_HEIGHT				20

#define LUI_STYLE_LABEL_TEXT_COLOR			_LUI_RGB(238, 238, 238)
#define LUI_STYLE_LABEL_BG_COLOR			_LUI_RGB(34, 40, 49)
#define LUI_STYLE_LABEL_BORDER_COLOR		_LUI_RGB(196, 196, 196)
#define LUI_STYLE_LABEL_BORDER_VISIBLE		0
#define LUI_STYLE_LABEL_WIDTH				40
#define LUI_STYLE_LABEL_HEIGHT				20

#define	LUI_STYLE_SWITCH_SELECTION_COLOR	0x1234
#define	LUI_STYLE_SWITCH_KNOB_COLOR			0xFFFF
#define LUI_STYLE_SWITCH_BG_COLOR			0x1111
#define LUI_STYLE_SWITCH_BORDER_COLOR		0xFFFF
#define LUI_STYLE_SWITCH_BORDER_VISIBLE		1
#define LUI_STYLE_SWITCH_WIDTH				40
#define LUI_STYLE_SWITCH_HEIGHT				20

#define LUI_STYLE_LINECHART_LINE_COLOR		0xFFFF
#define LUI_STYLE_LINECHART_GRID_COLOR		0xFFFF
#define LUI_STYLE_LINECHART_GRID_VISIBLE	1
#define LUI_STYLE_LINECHART_BG_COLOR		0x0000
#define LUI_STYLE_LINECHART_BORDER_COLOR	0xFFFF
#define LUI_STYLE_LINECHART_BORDER_VISIBLE	0
#define LUI_STYLE_LINECHART_WIDTH			40
#define LUI_STYLE_LINECHART_HEIGHT			20

#define LUI_STYLE_LIST_NAV_BG_COLOR			_LUI_RGB(0, 146, 153)
#define LUI_STYLE_LIST_NAV_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define LUI_STYLE_LIST_NAV_PRESSED_COLOR	_LUI_RGB(0, 211, 222)
#define LUI_STYLE_LIST_NAV_SELECTION_COLOR	_LUI_RGB(0, 170, 179)
#define LUI_STYLE_LIST_ITEM_BG_COLOR		_LUI_RGB(57, 62, 70)
#define LUI_STYLE_LIST_ITEM_SELECTION_COLOR	_LUI_RGB(84, 91, 102)
#define LUI_STYLE_LIST_ITEM_PRESSED_COLOR	_LUI_RGB(109, 118, 133)
#define LUI_STYLE_LIST_ITEM_LABEL_COLOR		_LUI_RGB(238, 238, 238)
#define	LUI_STYLE_LIST_ITEM_BORDER_COLOR	_LUI_RGB(75, 81, 92)
#define LUI_STYLE_LIST_ITEM_BORDER_VISIBLE	1
#define LUI_STYLE_LIST_ITEM_MIN_HEIGHT		30
#define LUI_STYLE_LIST_BORDER_COLOR			_LUI_RGB(88, 95, 107)
#define LUI_STYLE_LIST_BORDER_VISIBLE		0
#define LUI_STYLE_LIST_WIDTH				40
#define LUI_STYLE_LIST_HEIGHT				60

#define LUI_STYLE_PANEL_BG_COLOR			0x0000
#define LUI_STYLE_PANEL_BORDER_COLOR		0xFFFF
#define LUI_STYLE_PANEL_BORDER_VISIBLE		0
#define LUI_STYLE_PANEL_WIDTH				100
#define LUI_STYLE_PANEL_HEIGHT				100

#define LUI_STYLE_SCENE_BG_COLOR			_LUI_RGB(34, 40, 49)



#define	LUI_STATE_IDLE						0
#define	LUI_STATE_SELECTED					1
#define	LUI_STATE_PRESSED					2
#define	LUI_STATE_ENTERED					3


#define	LUI_EVENT_NONE						0
#define	LUI_EVENT_SELECTED					1
#define	LUI_EVENT_SELECTION_LOST			2
#define	LUI_EVENT_PRESSED					3
#define	LUI_EVENT_RELEASED					4
#define	LUI_EVENT_ENTERED					5
#define	LUI_EVENT_EXITED					6
#define	LUI_EVENT_VALUE_CHANGED				7


#define	LUI_OBJ_NONE						0
#define	LUI_OBJ_LABEL						1
#define	LUI_OBJ_BUTTON						2
#define	LUI_OBJ_SWITCH						3
#define	LUI_OBJ_LINECHART					4
#define	LUI_OBJ_PANEL						5
#define	LUI_OBJ_SCENE						6
#define	LUI_OBJ_GENERIC						7
#define LUI_OBJ_LIST						8

struct _lui_common_style_s
{
	uint16_t bg_color;
	uint16_t border_color;
	uint8_t border_visible;
	uint16_t width;
	uint16_t height;
};

struct _lui_label_style_s
{
	uint16_t text_color;
};
struct _lui_button_style_s
{
	uint16_t label_color;
	uint16_t pressed_color;
	uint16_t selection_color;
};
struct _lui_switch_style_s
{
	uint16_t knob_color;
	uint16_t selection_color;
};
struct _lui_linechart_style_s
{
	uint16_t line_color;
	uint16_t grid_color;
	uint8_t grid_visible;
};
struct _lui_list_style_s
{
	struct _lui_button_style_s item_btn;
	struct _lui_common_style_s item_btn_common;
};


typedef struct _lui_obj_s
{
	uint16_t x;
	uint16_t y;
	struct _lui_common_style_s common_style;
	uint8_t state;
	uint8_t event;
	int32_t value;
	void (*obj_event_cb)(struct _lui_obj_s *obj);
	//private use only
	uint8_t needs_refresh;
	uint8_t visible;
	int8_t index;
	//int8_t parent_index;
	
	uint8_t obj_type;

	struct _lui_obj_s *parent;
	struct _lui_obj_s **children;
	uint8_t children_count;

	void *obj_main_data;
}lui_obj_t;


typedef struct _lui_linechart_s
{
	struct
	{
		uint8_t hor_count;
		uint8_t vert_count;
		//uint16_t color;
		//uint8_t is_grid;
	}grid;

	struct _lui_linechart_style_s style;

	struct
	{
		double *source;
		uint16_t points;
		uint8_t auto_scale;
		double y_max_value;
		double y_min_value;
	}data;

	//uint16_t color;
	tFont *font;
}lui_chart_t;


typedef struct _lui_label_s
{
	char *text;
	tFont *font;
	//uint16_t color;
	struct _lui_label_style_s style;
}lui_label_t;


typedef struct _lui_button_s
{
	struct
	{
		char *text;
		//uint16_t color;
		tFont *font;
	}label;

	//uint16_t pressed_color;
	//uint16_t selection_color;

	int8_t dpad_row_pos;
	int8_t dpad_col_pos;

	struct _lui_button_style_s style;
}lui_button_t;


typedef struct _lui_switch_s
{
	//uint16_t selection_color;
	//uint16_t knob_color;

	int8_t dpad_row_pos;
	int8_t dpad_col_pos;

	struct _lui_switch_style_s style;
}lui_switch_t;

typedef struct _lui_list_s
{
	uint8_t page_count;
	uint8_t current_page_index;
	int8_t selected_button_index; // not using currently
	uint8_t buttons_per_page;
	uint8_t button_item_min_height;
	tFont *font;

	//struct _lui_list_style_s style;
}lui_list_t;


typedef struct _lui_panel_s
{
	tImage *bg_image;
}lui_panel_t;

typedef struct _lui_scene_s
{
	tImage *bg_image;
	lui_obj_t *obj_popup;
	tFont *font;
	struct
	{
		int8_t max_row_pos;
		int8_t max_col_pos;
		int8_t current_row_pos;
		int8_t current_col_pos;
	}dpad; // dpad controls are scene specific, so storing it in the scene variable

}lui_scene_t;


typedef struct _lui_touch_input_data_s
{
	uint8_t is_pressed;
	int16_t x;
	int16_t y;
}lui_touch_input_data_t;


typedef struct _lui_dpad_input_data_s
{
	uint8_t is_left_pressed;
	uint8_t is_right_pressed;
	uint8_t is_down_pressed;
	uint8_t is_up_pressed;
	uint8_t is_enter_pressed;
	uint8_t is_cancel_pressed;
}lui_dpad_input_data_t;


typedef struct _lui_disp_drv_s
{
	void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	void (*render_complete_cb)();
	uint16_t display_hor_res;
	uint16_t display_vert_res;
}lui_dispdrv_t;

typedef struct _lui_touch_input_dev_s
{
	void (*read_touch_input_cb)(lui_touch_input_data_t *input);
	//lui_touch_input_data_t touch_data;
}lui_touch_input_dev_t;

typedef struct _lui_dpad_input_dev_s
{
	void (*read_dpad_input_cb)(lui_dpad_input_data_t *input);
	//lui_dpad_input_data_t dpad_data;
	
}lui_dpad_input_dev_t;


typedef struct _lui_main_s
{
	lui_obj_t *scenes[LUI_MAX_SCENES];
	lui_obj_t *active_scene;
	lui_obj_t *active_obj;
	lui_dispdrv_t *disp_drv;
	lui_touch_input_dev_t *touch_input_dev;
	lui_dpad_input_dev_t *dpad_input_dev;
	// lui_touch_input_data_t last_touch_data;
	uint8_t total_scenes;
	uint8_t total_created_objects;	//increases as new objs are created. It never decreases
}lui_main_t;



void lui_init();
void lui_update();


lui_obj_t* _lui_object_create();
void lui_object_add_to_parent(lui_obj_t *obj, lui_obj_t *parent_obj);
void lui_object_remove_from_parent(lui_obj_t *obj);
void lui_object_set_area(uint16_t width, uint16_t height, lui_obj_t *obj);
void lui_object_set_width(uint16_t width, lui_obj_t *obj);
void lui_object_set_height(uint16_t height, lui_obj_t *obj);
void lui_object_set_position(uint16_t x, uint16_t y, lui_obj_t *obj);
void lui_object_set_x_pos(uint16_t x, lui_obj_t *obj);
void lui_object_set_y_pos(uint16_t y, lui_obj_t *obj);
void lui_object_set_border_color(uint16_t border_color, lui_obj_t *obj);
void lui_object_set_border_visibility(uint8_t is_visible, lui_obj_t *obj);
void lui_object_set_bg_color(uint16_t color, lui_obj_t *obj);
void lui_object_set_callback(void (*obj_event_cb)(lui_obj_t *), lui_obj_t *obj);
int8_t lui_object_get_state(lui_obj_t *obj);
int8_t lui_object_get_event(lui_obj_t *obj);
void lui_object_set_visibility(uint8_t visible, lui_obj_t *obj);


lui_obj_t* lui_label_create();
void lui_label_draw (lui_obj_t *lbl);
void lui_label_set_font(const tFont *font, lui_obj_t *obj);
void lui_label_set_text(const char *text, lui_obj_t *obj);
void lui_label_set_text_color(uint16_t text_color, lui_obj_t *obj);


lui_obj_t* lui_linechart_create();
void lui_linechart_draw (lui_obj_t *obj);
void lui_linechart_set_grid(uint16_t color, uint16_t hor_lines, uint16_t vert_lines, lui_obj_t *obj);
void lui_linechart_set_grid_visible(uint8_t state, lui_obj_t *obj);
void lui_linechart_set_line_color(uint16_t line_color, lui_obj_t *obj);
void lui_linechart_set_data_auto_scale(uint8_t auto_scale, lui_obj_t *obj);
void lui_linechart_set_data_range(double y_min, double y_max, lui_obj_t *obj);
void lui_linechart_set_data_source(double *source, uint16_t points, lui_obj_t *obj);


lui_obj_t* lui_button_create();
void lui_button_draw(lui_obj_t *btn);
void lui_button_set_label_text(const char *text, lui_obj_t *btn);
void lui_button_set_label_color(uint16_t color, lui_obj_t *btn);
void lui_button_set_label_font(const tFont *font, lui_obj_t *btn);
void lui_button_set_extra_colors(uint16_t pressed_color, uint16_t selection_color, lui_obj_t *btn);


lui_obj_t* lui_switch_create();
void lui_switch_draw(lui_obj_t *swtch);
void lui_switch_set_extra_colors(uint16_t knob_color, uint16_t selection_color, lui_obj_t *obj);
int8_t lui_switch_get_value(lui_obj_t *obj);
void lui_switch_set_value(uint8_t value, lui_obj_t *obj);
void lui_switch_set_on(lui_obj_t *obj);
void lui_switch_set_off(lui_obj_t *obj);


lui_obj_t* lui_list_create();
void lui_list_draw(lui_obj_t *obj);
lui_obj_t* lui_list_add_item(const char *text, lui_obj_t *obj);
void lui_list_prepare(lui_obj_t *obj);
void lui_list_set_item_min_height(uint8_t height, lui_obj_t *obj);
void lui_list_set_font(const tFont *font, lui_obj_t *obj);
void lui_list_set_nav_btn_label_color(uint16_t color, lui_obj_t *obj);
void lui_list_set_nav_btn_bg_color(uint16_t color, lui_obj_t *obj);
void lui_list_set_nav_btn_extra_colors(uint16_t pressed_color, uint16_t selection_color, lui_obj_t *obj);
void _lui_list_add_nav_buttons(lui_obj_t *obj);
void _lui_list_nav_btn_cb(lui_obj_t *obj);
void _lui_list_add_button_obj(lui_obj_t *obj_btn, lui_obj_t *obj_list);


lui_obj_t* lui_panel_create();
void lui_panel_draw(lui_obj_t *obj);
void lui_panel_set_bg_image(const tImage *image, lui_obj_t *obj_scene);


lui_obj_t* lui_scene_create();
void lui_scene_draw(lui_obj_t *obj);
void lui_scene_set_active(lui_obj_t *obj_scene);
lui_obj_t* lui_scene_get_active();
void lui_scene_set_bg_image(const tImage *image, lui_obj_t *obj_scene);
void lui_scene_set_font(const tFont *font, lui_obj_t *obj_scene);
void lui_scene_set_popup(lui_obj_t *obj, lui_obj_t *obj_scene);
void lui_scene_unset_popup(lui_obj_t *obj_scene);


lui_obj_t* _lui_process_touch_input_of_act_scene();
void _lui_process_dpad_input(lui_scene_t *scene);
void _lui_set_obj_props_on_input(lui_touch_input_data_t input, lui_obj_t *obj);
uint8_t _lui_check_if_active_obj(lui_touch_input_data_t input, lui_obj_t *obj);


lui_dispdrv_t* lui_dispdrv_create();
void lui_dispdrv_register(lui_dispdrv_t *dispdrv);
void* lui_dispdrv_destroy(lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_resolution(uint16_t hor_res, uint16_t vert_res, lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_draw_pixels_area_cb(void (*draw_pixels_area_cb)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color), lui_dispdrv_t *dispdrv);
void lui_dispdrv_set_render_complete_cb(void (*render_complete_cb)(), lui_dispdrv_t *dispdrv);
uint8_t _lui_disp_drv_check();


lui_touch_input_dev_t* lui_touch_inputdev_create();
void lui_touch_inputdev_register (lui_touch_input_dev_t *touch_inputdev);
void lui_touch_inputdev_set_read_input_cb(void (*read_touch_input_cb)(lui_touch_input_data_t *touch_inputdata), lui_touch_input_dev_t *touch_inputdev);
lui_dpad_input_dev_t* lui_dpad_inputdev_create();
void lui_dpad_inputdev_register (lui_dpad_input_dev_t *dpad_inputdev);
void lui_dpad_inputdev_set_read_input_cb(void (*read_dpad_input_cb)(lui_dpad_input_data_t *dpad_inputdata), lui_dpad_input_dev_t *dpad_inputdev);


//-------------------------------------------------------------------------------
//-------------------------------- HELPER FUNCTIONS------------------------------
//-------------------------------------------------------------------------------
void _lui_object_set_need_refresh(lui_obj_t *obj);
void _lui_object_render_parent_with_children(lui_obj_t *obj);
void _lui_object_render(lui_obj_t *obj);
lui_obj_t* _lui_scan_all_obj_for_input(lui_touch_input_data_t input_data, lui_obj_t *obj_root, lui_obj_t *obj_excluded);
lui_obj_t* _lui_scan_individual_object_for_input(lui_touch_input_data_t input_data, lui_obj_t *obj, lui_obj_t *obj_excluded);
tFont* _lui_get_font_from_active_scene();
uint8_t _lui_get_event_against_state(uint8_t new_state, uint8_t old_state);
void _lui_draw_char(uint16_t x, uint16_t y, uint16_t fore_color, const tImage *glyph);
void _get_string_dimension(const char *str, const tFont *font_obj, uint8_t *str_dim);
void _lui_plot_line_low(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_plot_line_high(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
void _lui_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t line_width, uint16_t color);
double _lui_map_range(double old_val, double old_max, double old_min, double new_max, double new_min);
void _lui_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t line_width, uint16_t color);
void _lui_draw_rect_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
uint16_t lui_rgb(uint16_t red, uint16_t green, uint16_t blue);
#endif /* INC_LAME_UI_H_ */
