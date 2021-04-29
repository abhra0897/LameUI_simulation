/*#!/usr/local/bin/tcc -run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L.*/
// tcc "-run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L. LameUI/src/lame_ui.c" src/main.c -v

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "GL/glut.h"
#include <math.h>
#include <inttypes.h>
#include "lame_ui.h"
#include "font_fixedsys_mono_16.h"
#include "font_microsoft_16.h"
#include "font_ubuntu_48.h"
#include "font_ubuntu_16.h"
#include <unistd.h>

// Set display resolution
// OpenGL will use it to create a window
#define HOR_RES		480
#define VERT_RES	640


// following UI elements are made global so we can access them in the event handler
lui_touch_input_data_t g_input;
lui_obj_t *g_scene_one;
lui_obj_t *g_scene_two;
lui_obj_t *g_lbl_cntr_value;
lui_obj_t *g_popup_panel;
lui_obj_t *g_popup_label;
lui_obj_t *g_grph;
lui_obj_t *g_btn_count;
lui_obj_t *g_btn_reset;
lui_obj_t *g_swtch_enable_bluetooth;
lui_obj_t *g_swtch_enable_wifi;
lui_obj_t *g_btn_nxt_scn;
lui_obj_t *g_btn_prev_scn;
lui_obj_t *g_lbl_slider1_val;
lui_obj_t *g_lbl_slider2_val;
lui_obj_t *slider1;
lui_obj_t *slider2;


double g_points[50][2];
char g_btn_press_cnt[4] = {0};// For same reason as above
uint16_t g_btn_press_cnt_int = 0;
uint32_t g_disp_buffer_counter = 0;
uint32_t g_sidp_buffer_max_size = 100 * HOR_RES; //HOR_RES * VERT_RES;

// Callback functions for LameUI ---------------------------------------------------
void count_and_reset_event_handler(lui_obj_t *obj);
void popupbtn_event_handler(lui_obj_t *obj);
void change_grph_event_handler(lui_obj_t *obj);
void enable_wifi_and_bt_event_handler(lui_obj_t *obj);
void slider_event_handler(lui_obj_t *obj);
void scn_change_event_handler(lui_obj_t *obj);
void my_input_read_opengl (lui_touch_input_data_t *input);
void my_set_pixel_area_opengl (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
// End LameUI callbacks ------------------------------------------------------------

// opengl functions. Not specific to LameUI ----------------------------------------
void gl_init();
// End opengl function -------------------------------------------------------------

// glut callback functions. These are not LameUI specific --------------------------
void myDisplay();
void myIdle();
void myMouseMove(int x, int y);
void myMousePressMove(int x, int y);
void myMousePress(int button, int state, int x, int y);
// End glut callbacks ---------------------------------------------------------------

// user functions
void prepare_chart_data_1();
void prepare_chart_data_2(uint16_t val);
// end user funcions


// Initialize OpenGL
void gl_init()
{
	// making background color black as first
	// 3 arguments all are 0.0
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// breadth of picture boundary is 1 pixel
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// setting window dimension in X- and Y- direction
	//gluOrtho2D(-25, HOR_RES+25, VERT_RES+25, -25);
	gluOrtho2D(-1, HOR_RES - 1, VERT_RES - 1, -1);

	// Clear everything
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
}


// Read input from an input device
// It should take device position (x, y) and a button's status
void my_input_read_opengl (lui_touch_input_data_t *input)
{
	input->is_pressed = g_input.is_pressed;
	input->y = g_input.y;
	input->x = g_input.x;

	//printf("[DEBUG] void my_input_read_opengl(). x:%d\ty: %d\n", input->y, input->x);
}

// Event handler for button, called back by LameUI
void count_and_reset_event_handler(lui_obj_t *obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	if (event ==  LUI_EVENT_RELEASED)
	{
		if (obj == g_btn_count)
		{
			sprintf(g_btn_press_cnt, "%d", ++g_btn_press_cnt_int);
			lui_label_set_text(g_btn_press_cnt, g_lbl_cntr_value);		
		}
		else if (obj == g_btn_reset)
		{
			g_btn_press_cnt_int = 0;
			sprintf(g_btn_press_cnt, "%d", g_btn_press_cnt_int);
			lui_label_set_text(g_btn_press_cnt, g_lbl_cntr_value);
		}

		prepare_chart_data_2(g_btn_press_cnt_int + 5);
		lui_linechart_set_data_source((double *)&g_points, 50, g_grph);
	}
}

// Event handler for popup buttons
void popupbtn_event_handler(lui_obj_t *obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	switch (event)
	{
	case LUI_EVENT_RELEASED:
		lui_scene_unset_popup(g_scene_one);
		break;	
	default:
		break;
	}
}


// Event handler for switch change graph. Called back by LameUI
void enable_wifi_and_bt_event_handler(lui_obj_t *obj)
{	
	// change background color
	uint8_t event = lui_object_get_event(obj);
	if (event == LUI_EVENT_VALUE_CHANGED)
	{
		uint8_t val = lui_switch_get_value(obj);
		if (obj == g_swtch_enable_wifi)
		{
			if (val == 1)
			{
				lui_label_set_text("Alert:\nWiFi is enabled", g_popup_label);
				lui_scene_set_popup(g_popup_panel, g_scene_one);
			}
			else
			{
				lui_label_set_text("Alert:\nWiFi is disabled", g_popup_label);
				lui_scene_set_popup(g_popup_panel, g_scene_one);
			}
		}
		else if (obj = g_swtch_enable_bluetooth)
		{
			if (val == 1)
			{
				lui_label_set_text("Alert:\nBluetooth is enabled", g_popup_label);
				lui_scene_set_popup(g_popup_panel, g_scene_one);
			}
			else
			{
				lui_label_set_text("Alert:\nBluetooth is disabled", g_popup_label);
				lui_scene_set_popup(g_popup_panel, g_scene_one);
			}
		}
	}
	
}

// Event handler for slider. Changes a label's text based on slider value
void slider_event_handler(lui_obj_t *obj)
{
	if (lui_object_get_event(obj) == LUI_EVENT_VALUE_CHANGED)
	{
		int16_t val = lui_slider_get_value(obj);
		if (obj == slider1)
		{
			static char buf_slider1[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
			snprintf(buf_slider1, 5, "%d", val);
			lui_label_set_text(buf_slider1, g_lbl_slider1_val);
		}
		else if (obj == slider2)
		{
			static char buf_slider2[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
			snprintf(buf_slider2, 5, "%d", val);
			lui_label_set_text(buf_slider2, g_lbl_slider2_val);
		}
		
	}
}

// Event handler for next_scene button and prev_scene button press
void scn_change_event_handler(lui_obj_t *obj)
{
	uint8_t event = lui_object_get_event(obj);
	
	if (event == LUI_EVENT_RELEASED)
	{
		if (obj == g_btn_nxt_scn)
		{
			lui_scene_set_active(g_scene_two); 
		}
		else if (obj == g_btn_prev_scn)
		{
			lui_scene_set_active(g_scene_one); 
		}

	}
}

// Flush the current buffer when this callback is called by LameUI.
// Using this, no need to flush every time set_pixel_cb is called. Rather buffer it and flush all together
void my_render_complete_handler()
{
	glFlush();
	g_disp_buffer_counter = 0; //reset the counter
}


// Draw an area of pixels in one go.
// This is faster because the opengl buffer is filled using a loop,
// and then the preapred buffer is fflushed in one go
void my_set_pixel_area_opengl (uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height, uint16_t p_color)
{
	uint16_t temp_x;
	uint16_t temp_y;

	// Seperating RGB565 color
	uint8_t uint_red_5 = (p_color >> 8) & 0xF8;
	uint8_t uint_green_6 = (p_color >> 3) & 0xFC;
	uint8_t uint_blue_5 = (p_color << 3);

	// Normalizing value within range 0.0 to 1.0
	GLfloat glf_red = (GLfloat)uint_red_5 / 255.0;
	GLfloat glf_green = (GLfloat)uint_green_6 / 255.0;
	GLfloat glf_blue = (GLfloat)uint_blue_5 / 255.0;

	// Set the color
	glColor3f(glf_red, glf_green, glf_blue);

	// Prepare the display buffer
	// After the loop ends, the prepared buffer is flushed
	for (temp_y = p_y; temp_y <= p_y + p_height - 1; temp_y++)
	{
		for (temp_x = p_x; temp_x <= p_x + p_width - 1; temp_x++)
		{
			GLint x = (GLint)temp_x;
			GLint y = (GLint)temp_y;

			glBegin(GL_POINTS);

			// glVertex2i just draws a point on specified co-ordinate
			glVertex2i(x, y);
			glEnd();

			// increase the buffer counter
			g_disp_buffer_counter++;

			// If size reached max buffer size, flush it now
			if (g_disp_buffer_counter >= g_sidp_buffer_max_size)
			{
				glFlush();
				g_disp_buffer_counter = 0; //reset the counter
			}
		}
	}

	// If size reached max buffer size, flush it now
	if (g_disp_buffer_counter >= g_sidp_buffer_max_size)
	{
		glFlush();
		g_disp_buffer_counter = 0; //reset the counter
	}
	
}


int main (int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	// giving window size in X- and Y- direction
	// pading in both sides
	// glutInitWindowSize(HOR_RES + (PADDING * 2), VERT_RES + (PADDING * 2));
	glutInitWindowSize(HOR_RES, VERT_RES);
	glutInitWindowPosition(0, 0);

	// Giving name to window
	glutCreateWindow("LameUI Simulator");

	// Initialize opengl
	gl_init();	

	/*###################################################################################
	 #		Starts LameUI Based Code. The Below Part Is Hardware/Platform Agnostic		#
	 ###################################################################################*/

	// [IMPORTANT:] do it at the begining. Mandatory!
	lui_init();

	//----------------------------------------------------------
	//creating display driver variable for lame_ui
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(HOR_RES, VERT_RES, my_display_driver);
	lui_dispdrv_set_draw_pixels_area_cb(my_set_pixel_area_opengl, my_display_driver);
	lui_dispdrv_set_render_complete_cb(my_render_complete_handler, my_display_driver);
	//my_display_driver = lui_dispdrv_destroy(my_display_driver);

	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_read_opengl, my_input_device);	

	//----------------------------------------------------------
	//create and add scenes
	g_scene_one = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_one);
	lui_scene_set_font(&font_microsoft_16, g_scene_one);
	//g_scene_one = lui_scene_destroy(g_scene_one);	// For destroying, assigning the return value is mandatory

	//create and add scenes
	g_scene_two = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_two);
	lui_scene_set_font(&font_microsoft_16, g_scene_two);

	//set the active scene. This scene will be rendered iby the lui_update()
	lui_scene_set_active(g_scene_one);

	//----------------------------------------------------------
	//create label

	lui_obj_t *lbl_heading = lui_label_create();
	lui_object_add_to_parent(lbl_heading, g_scene_one);
	lui_label_set_text("This is a demo of LameUI :)", lbl_heading);
	lui_object_set_position(0, 1, lbl_heading);
	lui_object_set_area(HOR_RES, 20, lbl_heading);
	lui_object_set_bg_color(LUI_STYLE_BUTTON_BG_COLOR, lbl_heading);
	//lui_label_set_text_color(lui_rgb(0, 0, 0), lbl_heading);
	lui_object_set_border_visibility(1, lbl_heading);

	
	//----------------------------------------------------------
	//Prepare some data for a graph
	prepare_chart_data_2(10);
	
	//----------------------------------------------------------
	//create a line chart with above data
	g_grph = lui_linechart_create();
	lui_object_add_to_parent(g_grph, g_scene_one);
	lui_linechart_set_data_source((double *)&g_points, 50, g_grph);
	//lui_linechart_set_data_range(-4, 4, g_grph);
	lui_object_set_position(20, 50, g_grph);
	lui_object_set_area(440, 200, g_grph);


	//----------------------------------------------------------

	lui_obj_t *lbl_cntr_txt = lui_label_create();
	lui_object_add_to_parent(lbl_cntr_txt, g_scene_one);
	lui_label_set_text("Counter:", lbl_cntr_txt);
	lui_object_set_position(10, 285, lbl_cntr_txt);
	lui_object_set_area(70, 20, lbl_cntr_txt);


	g_lbl_cntr_value = lui_label_create();
	lui_object_add_to_parent(g_lbl_cntr_value, g_scene_one);
	lui_label_set_font(&font_ubuntu_48, g_lbl_cntr_value);
	lui_label_set_text("0", g_lbl_cntr_value);
	lui_object_set_x_pos(85, g_lbl_cntr_value);
	lui_object_set_y_pos(270, g_lbl_cntr_value);
	lui_object_set_area(110, 54, g_lbl_cntr_value);
	lui_object_set_border_visibility(1, g_lbl_cntr_value);


	//create two buttons. One for count up, another for reset
	g_btn_count = lui_button_create();
	lui_object_add_to_parent(g_btn_count, g_scene_one);
	lui_object_set_position(230, 280, g_btn_count);
	lui_object_set_area(100, 40, g_btn_count);
	lui_button_set_label_text("Count", g_btn_count);
	lui_object_set_callback(count_and_reset_event_handler, g_btn_count);

	g_btn_reset = lui_button_create();
	lui_object_add_to_parent(g_btn_reset, g_scene_one);
	lui_object_set_position(350, 280, g_btn_reset);
	lui_object_set_area(100, 40, g_btn_reset);
	lui_button_set_label_text("Reset", g_btn_reset);
	lui_object_set_callback(count_and_reset_event_handler, g_btn_reset);


	// group all the switches and their labels
	lui_obj_t *panel_group_switches =  lui_panel_create();
	lui_object_set_position(5, 340, panel_group_switches);
	lui_object_set_area(240, 120, panel_group_switches);
	lui_object_add_to_parent(panel_group_switches, g_scene_one);

	lui_obj_t *lbl_enable_wifi = lui_label_create();
	lui_object_add_to_parent(lbl_enable_wifi, panel_group_switches);
	lui_label_set_text("Enable WiFi", lbl_enable_wifi);
	lui_object_set_position(5, 5, lbl_enable_wifi);
	lui_object_set_area(140, 20, lbl_enable_wifi);

	g_swtch_enable_wifi = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_wifi, panel_group_switches);
	lui_object_set_position(190, 5, g_swtch_enable_wifi);
	lui_object_set_callback(enable_wifi_and_bt_event_handler, g_swtch_enable_wifi);

	lui_obj_t *lbl_enable_bluetooth = lui_label_create();
	lui_object_add_to_parent(lbl_enable_bluetooth, panel_group_switches);
	lui_label_set_text("Enable Bluetooth", lbl_enable_bluetooth);
	lui_object_set_position(5, 35, lbl_enable_bluetooth);
	lui_object_set_area(140, 20, lbl_enable_bluetooth);

	g_swtch_enable_bluetooth = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_bluetooth, panel_group_switches);
	lui_object_set_position(190, 35, g_swtch_enable_bluetooth);
	lui_object_set_callback(enable_wifi_and_bt_event_handler, g_swtch_enable_bluetooth);


	lui_obj_t *lbl_priv_chat = lui_label_create();
	lui_object_add_to_parent(lbl_priv_chat, panel_group_switches);
	lui_label_set_text("Private chat", lbl_priv_chat);
	lui_object_set_position(5, 65, lbl_priv_chat);
	lui_object_set_area(140, 20, lbl_priv_chat);

	lui_obj_t *swtch_priv_chat = lui_switch_create();
	lui_object_add_to_parent(swtch_priv_chat, panel_group_switches);
	lui_object_set_position(190, 65, swtch_priv_chat);

	lui_obj_t *lbl_notification = lui_label_create();
	lui_object_add_to_parent(lbl_notification, panel_group_switches);
	lui_label_set_text("Notifications", lbl_notification);
	lui_object_set_position(5, 95, lbl_notification);
	lui_object_set_area(140, 20, lbl_notification);

	lui_obj_t *swtch_notification = lui_switch_create();
	lui_object_add_to_parent(swtch_notification, panel_group_switches);
	lui_object_set_position(190, 95, swtch_notification);


	// add a long text label
	lui_obj_t *lbl_long_text = lui_label_create();
	lui_object_add_to_parent(lbl_long_text, g_scene_one);
	lui_label_set_text("The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog ate my head ", lbl_long_text);
	lui_object_set_position(5, 470, lbl_long_text);
	lui_object_set_area(240, 120, lbl_long_text);
	lui_object_set_border_visibility(1, lbl_long_text);



	// -------------------------------------------
	// create popup group
	g_popup_panel = lui_panel_create();
	g_popup_label = lui_label_create();
	lui_obj_t *popup_btn = lui_button_create();
	lui_object_add_to_parent(g_popup_label, g_popup_panel);
	lui_object_add_to_parent(popup_btn, g_popup_panel);
	lui_label_set_text("This is a popup", g_popup_label);
	
	lui_object_set_position(110, 80, popup_btn);
	lui_object_set_position(5, 12, g_popup_label);
	lui_object_set_position(80, 240, g_popup_panel);

	lui_object_set_area(158, 18, g_popup_label);
	lui_object_set_area(80, 28, popup_btn);
	lui_object_set_area(300, 140, g_popup_panel);


	
	lui_button_set_label_text("OK", popup_btn);
	lui_button_set_label_font(&font_microsoft_16, popup_btn);
	//lui_button_set_label_color(ILI_COLOR_WHITE, popup_btn);
	//lui_button_set_extra_colors(lui_rgb(112, 0, 77), lui_rgb(171, 0, 117), popup_btn);
	lui_object_set_callback(popupbtn_event_handler, popup_btn);


	// add a small list in scene 
	lui_obj_t *list1 = lui_list_create();
	lui_object_set_position(250, 340, list1);
	lui_object_set_area(215, 250, list1);
	lui_object_set_border_visibility(1, list1);
	lui_obj_t *item1 = lui_list_add_item("Shut Down", list1);
	lui_obj_t *item2 = lui_list_add_item("Restart", list1);
	lui_obj_t *item3 = lui_list_add_item("Suspend", list1);
	lui_obj_t *item4 = lui_list_add_item("Log Out", list1);
	lui_obj_t *item5 = lui_list_add_item("Switch User", list1);
	lui_obj_t *item6 = lui_list_add_item("Hibernate", list1);
	lui_obj_t *item7 = lui_list_add_item("Lock screen", list1);
	lui_list_prepare(list1);
	lui_object_add_to_parent(list1, g_scene_one);
	

	// add a button to go to next scene (g_scene_two)
	g_btn_nxt_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_nxt_scn, g_scene_one);
	lui_object_set_position(140, 595, g_btn_nxt_scn);
	lui_object_set_area(200, 40, g_btn_nxt_scn);
	lui_button_set_label_text("Next Scene ->", g_btn_nxt_scn);
	lui_object_set_callback(scn_change_event_handler, g_btn_nxt_scn);

	// add a button to go to prev scene (g_scene_one)
	g_btn_prev_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_prev_scn, g_scene_two);
	lui_object_set_position(140, 595, g_btn_prev_scn);
	lui_object_set_area(200, 40, g_btn_prev_scn);
	lui_button_set_label_text("<- Prev Scene", g_btn_prev_scn);
	lui_object_set_callback(scn_change_event_handler, g_btn_prev_scn);

	// a text label for g_scene_two
	lui_obj_t *lbl_list_descr = lui_label_create();
	lui_object_add_to_parent(lbl_list_descr, g_scene_two);
	lui_label_set_text("This is a long list. For long lists that require multiple pages, navigation buttons are automatically added..", lbl_list_descr);
	lui_object_set_position(90, 10, lbl_list_descr);
	lui_object_set_area(300, 70, lbl_list_descr);

	// add a big list in scene two
	lui_obj_t *list2 = lui_list_create();
	lui_object_set_position(90, 90, list2);
	lui_object_set_area(300, 280, list2);
	lui_object_set_border_visibility(1, list2);
	lui_obj_t *item_list2_1 = lui_list_add_item("Hello", list2);
	lui_obj_t *item_list2_2 = lui_list_add_item("Hi there", list2);
	lui_obj_t *item_list2_3 = lui_list_add_item("I am good", list2);
	lui_obj_t *item_list2_4 = lui_list_add_item("This is", list2);
	lui_obj_t *item_list2_5 = lui_list_add_item("a test", list2);
	lui_obj_t *item_list2_6 = lui_list_add_item("and it's", list2);
	lui_obj_t *item_list2_7 = lui_list_add_item("working!!", list2);
	lui_obj_t *item_list2_8 = lui_list_add_item("I'm happy", list2);
	lui_obj_t *item_list2_9 = lui_list_add_item("yaaaawn", list2);
	lui_obj_t *item_list2_10 = lui_list_add_item("Gonna sleep", list2);
	lui_obj_t *item_list2_11 = lui_list_add_item("Worked hard", list2);
	lui_obj_t *item_list2_12 = lui_list_add_item("Now bye", list2);
	lui_obj_t *item_list2_13 = lui_list_add_item("Hey you..!!", list2);
	lui_obj_t *item_list2_14 = lui_list_add_item("Adding more items", list2);
	lui_obj_t *item_list2_15 = lui_list_add_item("Ha hahahah", list2);
	lui_obj_t *item_list2_16 = lui_list_add_item("It's simple", list2);
	lui_obj_t *item_list2_17 = lui_list_add_item("and stupid", list2);
	lui_obj_t *item_list2_18 = lui_list_add_item("but it works!", list2);
	lui_obj_t *item_list2_19 = lui_list_add_item("bla bla bla", list2);
	lui_obj_t *item_list2_20 = lui_list_add_item("More bla bla bla", list2);
	lui_obj_t *item_list2_21 = lui_list_add_item("Enddddddddddd", list2);
	lui_list_prepare(list2);
	lui_object_add_to_parent(list2, g_scene_two);

	// add a slider
	slider1 = lui_slider_create();
	lui_object_add_to_parent(slider1, g_scene_two);
	lui_object_set_position(90, 390, slider1);
	lui_object_set_area(270, 20, slider1);
	lui_slider_set_range(-100, 100, slider1);
	lui_slider_set_value(50, slider1);
	lui_object_set_callback(slider_event_handler, slider1);

	// a label to show its value
	g_lbl_slider1_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider1_val, g_scene_two);
	lui_label_set_text("50", g_lbl_slider1_val);
	lui_object_set_position(370, 390, g_lbl_slider1_val);
	lui_object_set_area(100, 20, g_lbl_slider1_val);

	// add second slider
	slider2 = lui_slider_create();
	lui_object_add_to_parent(slider2, g_scene_two);
	lui_object_set_position(90, 420, slider2);
	lui_object_set_area(270, 20, slider2);
	lui_slider_set_range(-100, 100, slider2);
	lui_slider_set_value(-50, slider2);
	lui_object_set_callback(slider_event_handler, slider2);

	// another label to show second slider's value
	g_lbl_slider2_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider2_val, g_scene_two);
	lui_label_set_text("-50", g_lbl_slider2_val);
	lui_object_set_position(370, 420, g_lbl_slider2_val);
	lui_object_set_area(100, 20, g_lbl_slider2_val);


	// add a label as heading for checkboxes
	lui_obj_t *lbl_sport_question = lui_label_create();
	lui_object_add_to_parent(lbl_sport_question, g_scene_two);
	lui_label_set_text("What sports do you like?", lbl_sport_question);
	lui_object_set_position(90, 470, lbl_sport_question);
	lui_object_set_area(300, 20, lbl_sport_question);

	lui_obj_t *chkbox_football = lui_checkbox_create();
	lui_object_set_position(90, 495, chkbox_football);
	lui_object_add_to_parent(chkbox_football, g_scene_two);

	lui_obj_t *lbl_football = lui_label_create();
	lui_object_add_to_parent(lbl_football, g_scene_two);
	lui_label_set_text("Football", lbl_football);
	lui_object_set_position(130, 495, lbl_football);
	lui_object_set_area(300, 20, lbl_football);

	lui_obj_t *chkbox_cricket = lui_checkbox_create();
	lui_object_set_position(90, 520, chkbox_cricket);
	lui_object_add_to_parent(chkbox_cricket, g_scene_two);

	lui_obj_t *lbl_cricket = lui_label_create();
	lui_object_add_to_parent(lbl_cricket, g_scene_two);
	lui_label_set_text("Cricket", lbl_cricket);
	lui_object_set_position(130, 520, lbl_cricket);
	lui_object_set_area(300, 20, lbl_cricket);

	lui_obj_t *chkbox_hockey = lui_checkbox_create();
	lui_object_set_position(90, 545, chkbox_hockey);
	lui_object_add_to_parent(chkbox_hockey, g_scene_two);

	lui_obj_t *lbl_hockey = lui_label_create();
	lui_object_add_to_parent(lbl_hockey, g_scene_two);
	lui_label_set_text("Hockey", lbl_hockey);
	lui_object_set_position(130, 545, lbl_hockey);
	lui_object_set_area(300, 20, lbl_hockey);

	

	

	
	/*-----------------------------------------------------------------------------------
	 -		Glut related functions for drawing and input handling						-
	 -----------------------------------------------------------------------------------*/
	glutMouseFunc(myMousePress);		// to handle mouse press while not being moved
	glutMotionFunc(myMousePressMove);	// to handle mouse movement while being clicked
	glutPassiveMotionFunc(myMouseMove);	// to handle mouse movement while NOT being pressed
	glutIdleFunc(myIdle);
	glutDisplayFunc(myDisplay);
	glutMainLoop();

	return 0;
}

// This function is called back by glut when drawing is needed
// Here we call our render function
void myDisplay()
{
	// the main update function to process input and display output
	lui_update();
	// Flush drawing commands
	glFlush();
}

// This function is called back by glut when mouse is moved passively
// We're setting the global input variable's value here
void myMouseMove(int x, int y)
{
	//printf ("[DEBUG] void myMouse( int x: %d, int y: %d )\n", x, y);			//send all output to screen
	g_input.x = x;
	g_input.y = y;
}

// This function is called back by glut when mouse is moved while being pressed
// We're setting the global input variable's value here
void myMousePressMove(int x, int y)
{
	g_input.x = x;
	g_input.y = y;
}

// This function is called back by glut when mouse is pressed
void myMousePress(int button, int state, int x, int y)
{
	g_input.x = x;
	g_input.y = y;

	if (button == GLUT_LEFT)
	{
		if (state == GLUT_DOWN)
			g_input.is_pressed = 1;
		else
			g_input.is_pressed = 0;
	}
}

// This function is called back by glut during idle time (when not drawing)
// Here we wait a bit and force a redisplay function
void myIdle()
{
	sleep(.015); //wait 15ms to save CPU
	glutPostRedisplay();
}

void prepare_chart_data_2(uint16_t sin_val)
{
	for (int i = 0; i < 50; i++)
	{
		g_points[i][0] = i;		//x value of i'th element
		//g_points[i][1] = sin((double)i+10)/((double)i+10);	//y value of i'th element
		g_points[i][1] = sin((double)i+sin_val)/((double)i+sin_val);	//y value of i'th element
	}
}