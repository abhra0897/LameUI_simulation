/*#!/usr/local/bin/tcc -run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L.*/
// tcc "-run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L. LameUI/src/lame_ui.c" src/main.c -v

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <inttypes.h>
#include "../LameUI/lame_ui.h"
#include <unistd.h>
#include "fonts/montserrat_regular_32.h"
#include "fonts/ubuntu_regular_17.h"

// Set display resolution
// OpenGL will use it to create a window
#define HOR_RES		720
#define VERT_RES	640


// following UI elements are made global so we can access them in the event handler
lui_touch_input_data_t g_input = 
{
	.is_pressed = 0,
	.x = -1,
	.y = -1
};
lui_obj_t* g_scene_one;
lui_obj_t* g_scene_two;
lui_obj_t* g_scene_three;
lui_obj_t* g_lbl_cntr_value;
lui_obj_t* g_popup_panel;
lui_obj_t* g_popup_label;
lui_obj_t* g_grph;
lui_obj_t* g_btn_count;
lui_obj_t* g_btn_reset;
lui_obj_t* g_swtch_enable_bluetooth;
lui_obj_t* g_swtch_enable_wifi;
lui_obj_t* g_btn_nxt_scn;
lui_obj_t* g_btn_prev_scn;
lui_obj_t* g_lbl_slider1_val;
lui_obj_t* g_lbl_slider2_val;
lui_obj_t* slider1;
lui_obj_t* slider2;

lui_obj_t* btngrid;


double g_points[50][2];
char g_btn_press_cnt[4] = {0};// For same reason as above
uint16_t g_btn_press_cnt_int = 0;
uint32_t g_disp_buffer_counter = 0;
uint32_t g_sidp_buffer_max_size = HOR_RES * VERT_RES;


// Callback functions for LameUI ---------------------------------------------------
void count_and_reset_event_handler(lui_obj_t* obj);
void popupbtn_event_handler(lui_obj_t* obj);
void change_grph_event_handler(lui_obj_t* obj);
void enable_wifi_and_bt_event_handler(lui_obj_t* obj);
void slider_event_handler(lui_obj_t* obj);
void scn_change_event_handler(lui_obj_t* obj);
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
void count_and_reset_event_handler(lui_obj_t* obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	if (event ==  LUI_EVENT_PRESSED)
	{
		if (obj == g_btn_count)
		{
			sprintf(g_btn_press_cnt, "%d", ++g_btn_press_cnt_int);
			lui_label_set_text(g_lbl_cntr_value, g_btn_press_cnt);		
		}
		else /* if (obj == g_btn_reset) */
		{
			g_btn_press_cnt_int = 0;
			sprintf(g_btn_press_cnt, "%d", g_btn_press_cnt_int);
			lui_label_set_text(g_lbl_cntr_value, g_btn_press_cnt);
		}

		prepare_chart_data_2(g_btn_press_cnt_int + 5);
		lui_linechart_set_data_source(g_grph, (double* )&g_points, 50);
	}
}

void textbox_callback(lui_obj_t* obj_txtbox)
{
	uint8_t event = lui_object_get_event(obj_txtbox);
	if (event == LUI_EVENT_ENTERED)
	{
		lui_keyboard_set_target_txtbox(btngrid, obj_txtbox);
	}
	else if (event == LUI_EVENT_EXITED)
	{
		lui_keyboard_set_target_txtbox(btngrid, NULL);
	}
}

// Event handler for popup buttons
void popupbtn_event_handler(lui_obj_t* obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	switch (event)
	{
	case LUI_EVENT_PRESSED:
		lui_object_set_visibility(g_popup_panel, 0);
		fprintf(stderr, "Invisible!\n");
		break;	
	default:
		break;
	}
}


// Event handler for switch change graph. Called back by LameUI
void enable_wifi_and_bt_event_handler(lui_obj_t* obj)
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
				lui_label_set_text(g_popup_label, "Alert:\nWiFi is enabled");
				lui_object_set_visibility(g_popup_panel, 1);
				fprintf(stderr, "Visible!\n");
			}
			else
			{
				lui_label_set_text(g_popup_label, "Alert:\nWiFi is disabled");
				lui_object_set_visibility(g_popup_panel, 1);
				fprintf(stderr, "Visible!\n");
			}
		}
		else if (obj == g_swtch_enable_bluetooth)
		{
			if (val == 1)
			{
				lui_label_set_text(g_popup_label, "Alert:\nBluetooth is enabled");
				lui_object_set_visibility(g_popup_panel, 1);
				fprintf(stderr, "Visible!\n");
			}
			else
			{
				lui_label_set_text(g_popup_label, "Alert:\nBluetooth is disabled");
				lui_object_set_visibility(g_popup_panel, 1);
				fprintf(stderr, "Visible!\n");
			}
		}
	}
	
}

// Event handler for slider. Changes a label's text based on slider value
void slider_event_handler(lui_obj_t* obj)
{
	if (lui_object_get_event(obj) == LUI_EVENT_VALUE_CHANGED)
	{
		int16_t val = lui_slider_get_value(obj);
		if (obj == slider1)
		{
			static char buf_slider1[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
			snprintf(buf_slider1, 5, "%d", val);
			lui_label_set_text(g_lbl_slider1_val, buf_slider1);
		}
		else if (obj == slider2)
		{
			static char buf_slider2[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
			snprintf(buf_slider2, 5, "%d", val);
			lui_label_set_text(g_lbl_slider2_val, buf_slider2);
		}
		
	}
}

// Event handler for next_scene button and prev_scene button press
void scn_change_event_handler(lui_obj_t* obj)
{
	uint8_t event = lui_object_get_event(obj);
	
	if (event == LUI_EVENT_PRESSED)
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
	uint8_t memblk[20000];
	lui_init(memblk, sizeof(memblk));

	//----------------------------------------------------------
	//creating display driver variable for lame_ui
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(my_display_driver, HOR_RES, VERT_RES);
	lui_dispdrv_set_draw_pixels_area_cb(my_display_driver, my_set_pixel_area_opengl);
	lui_dispdrv_set_render_complete_cb(my_display_driver, my_render_complete_handler);
	//my_display_driver = lui_dispdrv_destroy(my_display_driver);

	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_device, my_input_read_opengl);	

	//----------------------------------------------------------
	//create and add scenes
	g_scene_one = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_one);
	//lui_scene_set_font(g_scene_one, &font_microsoft_16);
	//g_scene_one = lui_scene_destroy(g_scene_one);	// For destroying, assigning the return value is mandatory

	//create and add scenes
	g_scene_two = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_two);
	//lui_scene_set_font(g_scene_two, &font_microsoft_16);


	g_scene_three = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_two);
	//lui_scene_set_font(g_scene_two, &font_microsoft_16);

	//set the active scene. This scene will be rendered iby the lui_update()
	lui_scene_set_active(g_scene_one);

	//----------------------------------------------------------
	//create label

	lui_obj_t* lbl_heading = lui_label_create();
	lui_object_add_to_parent(lbl_heading, g_scene_one);
	lui_label_set_text(lbl_heading, "This is a demo of LameUI\nIt's work in progress :)");
	lui_object_set_position(lbl_heading, 0, 1);
	//lui_object_set_area(lbl_heading, HOR_RES, 20);
	lui_object_set_bg_color(lbl_heading, LUI_STYLE_BUTTON_BG_COLOR);
	lui_label_set_text_color(lbl_heading, lui_rgb(238, 238, 238));
	lui_object_set_border_visibility(lbl_heading, 1);

	
	//----------------------------------------------------------
	//Prepare some data for a graph
	prepare_chart_data_2(10);
	
	//----------------------------------------------------------
	//create a line chart with above data
	g_grph = lui_linechart_create();
	lui_object_add_to_parent(g_grph, g_scene_one);
	lui_linechart_set_data_source(g_grph, (double* )&g_points, 50);
	//lui_linechart_set_data_range(g_grph, -4, 4);
	lui_object_set_position(g_grph, 20, 50);
	lui_object_set_area(g_grph, 440, 200);


	//----------------------------------------------------------

	lui_obj_t* lbl_cntr_txt = lui_label_create();
	lui_object_add_to_parent(lbl_cntr_txt, g_scene_one);
	lui_label_set_text(lbl_cntr_txt, "Counter:");
	lui_object_set_position(lbl_cntr_txt, 10, 285);
	lui_object_set_area(lbl_cntr_txt, 70, 20);


	g_lbl_cntr_value = lui_label_create();
	lui_object_add_to_parent(g_lbl_cntr_value, g_scene_one);
	//lui_label_set_font(g_lbl_cntr_value, &font_ubuntu_48);
	lui_label_set_text(g_lbl_cntr_value, "0");
	lui_object_set_x_pos(g_lbl_cntr_value, 85);
	lui_object_set_y_pos(g_lbl_cntr_value, 270);
	lui_object_set_area(g_lbl_cntr_value, 110, 54);
	lui_object_set_border_visibility(g_lbl_cntr_value, 1);


	//create two buttons. One for count up, another for reset
	g_btn_count = lui_button_create();
	lui_object_add_to_parent(g_btn_count, g_scene_one);
	lui_object_set_position(g_btn_count, 230, 280);
	lui_object_set_area(g_btn_count, 100, 40);
	lui_button_set_label_text(g_btn_count, "Count");
	lui_object_set_callback(g_btn_count, count_and_reset_event_handler);

	g_btn_reset = lui_button_create();
	lui_object_add_to_parent(g_btn_reset, g_scene_one);
	lui_object_set_position(g_btn_reset, 350, 280);
	lui_object_set_area(g_btn_reset, 100, 40);
	lui_button_set_label_text(g_btn_reset, "Reset");
	lui_object_set_callback(g_btn_reset, count_and_reset_event_handler);


	// group all the switches and their labels
	lui_obj_t* panel_group_switches =  lui_panel_create();
	lui_object_set_position(panel_group_switches, 5, 340);
	lui_object_set_layer(panel_group_switches, 10);
	// lui_object_set_position(panel_group_switches, 35, 340);
	lui_object_set_area(panel_group_switches, 240, 120);
	lui_object_add_to_parent(panel_group_switches, g_scene_one);

	lui_obj_t* lbl_enable_wifi = lui_label_create();
	lui_object_add_to_parent(lbl_enable_wifi, panel_group_switches);
	lui_label_set_text(lbl_enable_wifi, "Enable WiFi");
	lui_object_set_position(lbl_enable_wifi, 5, 5);
	lui_object_set_area(lbl_enable_wifi, 140, 20);

	g_swtch_enable_wifi = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_wifi, panel_group_switches);
	lui_object_set_position(g_swtch_enable_wifi, 190, 5);
	lui_object_set_callback(g_swtch_enable_wifi, enable_wifi_and_bt_event_handler);

	lui_obj_t* lbl_enable_bluetooth = lui_label_create();
	lui_object_add_to_parent(lbl_enable_bluetooth, panel_group_switches);
	lui_label_set_text(lbl_enable_bluetooth, "Enable Bluetooth");
	lui_object_set_position(lbl_enable_bluetooth, 5, 35);
	lui_object_set_area(lbl_enable_bluetooth, 140, 20);

	g_swtch_enable_bluetooth = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_bluetooth, panel_group_switches);
	lui_object_set_position(g_swtch_enable_bluetooth, 190, 35);
	lui_object_set_callback(g_swtch_enable_bluetooth, enable_wifi_and_bt_event_handler);

	lui_obj_t* lbl_priv_chat = lui_label_create();
	lui_object_add_to_parent(lbl_priv_chat, panel_group_switches);
	lui_label_set_text(lbl_priv_chat, "Private chat");
	lui_object_set_position(lbl_priv_chat, 5, 65);
	lui_object_set_area(lbl_priv_chat, 140, 20);

	lui_obj_t* swtch_priv_chat = lui_switch_create();
	lui_object_add_to_parent(swtch_priv_chat, panel_group_switches);
	lui_object_set_position(swtch_priv_chat, 190, 65);

	lui_obj_t* lbl_notification = lui_label_create();
	lui_object_add_to_parent(lbl_notification, panel_group_switches);
	lui_label_set_text(lbl_notification, "Notifications");
	lui_object_set_position(lbl_notification, 5, 95);
	lui_object_set_area(lbl_notification, 140, 20);

	lui_obj_t* swtch_notification = lui_switch_create();
	lui_object_add_to_parent(swtch_notification, panel_group_switches);
	lui_object_set_position(swtch_notification, 190, 95);


	// add a long text label
	lui_obj_t* lbl_long_text = lui_label_create();
	lui_object_add_to_parent(lbl_long_text, g_scene_one);
	lui_label_set_text(lbl_long_text, "The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog The quick brown fox jumps over the lazy dog ate my head ");
	lui_object_set_position(lbl_long_text, 5, 470);
	lui_object_set_area(lbl_long_text, 240, 120);
	lui_object_set_border_visibility(lbl_long_text, 1);

	// -------------------------------------------
	// create popup group
	g_popup_panel = lui_panel_create();
	lui_object_add_to_parent(g_popup_panel, g_scene_one);
	g_popup_label = lui_label_create();
	lui_obj_t* popup_btn = lui_button_create();
	lui_object_add_to_parent(popup_btn, g_popup_panel);
	lui_object_add_to_parent(g_popup_label, g_popup_panel);
	
	lui_label_set_text(g_popup_label, "This is a popup");
	
	lui_object_set_position(popup_btn, 110, 80);
	lui_object_set_position(g_popup_label, 5, 12);
	lui_object_set_position(g_popup_panel, 80, 240);

	//lui_object_set_area(g_popup_label, 158, 18);
	lui_object_set_area(popup_btn, 80, 28);
	lui_object_set_area(g_popup_panel, 300, 140);

	//lui_object_set_visibility(g_popup_panel, 0);
	lui_object_set_layer(g_popup_panel, LUI_LAYER_POPUP);


	lui_button_set_label_text(popup_btn, "OK");
	//lui_button_set_label_font(popup_btn, &font_microsoft_16);
	//lui_button_set_label_color(popup_btn, ILI_COLOR_WHITE);
	//lui_button_set_extra_colors(popup_btn, lui_rgb(112, 0, 77), lui_rgb(171, 0, 117));
	lui_object_set_callback(popup_btn, popupbtn_event_handler);


	// add a small list in scene 
	lui_obj_t* list1 = lui_list_create();
	//lui_list_set_font(list1, &font_microsoft_16);
	lui_object_set_position(list1, 250, 340);
	lui_object_set_area(list1, 215, 250);
	lui_object_set_border_visibility(list1, 1);
	lui_list_set_text_align(list1, ALIGN_CENTER);
	lui_obj_t* item1 = lui_list_add_item(list1, "Shut Down");
	lui_object_set_callback(item1, count_and_reset_event_handler);
	lui_obj_t* item2 = lui_list_add_item(list1, LUI_ICON_RELAOD " Restart " LUI_ICON_POWER);
	lui_obj_t* item3 = lui_list_add_item(list1, "Suspend");
	lui_obj_t* item4 = lui_list_add_item(list1, "Log Out");
	lui_obj_t* item5 = lui_list_add_item(list1, "Switch User");
	lui_obj_t* item6 = lui_list_add_item(list1, "Hibernate");
	lui_obj_t* item7 = lui_list_add_item(list1, "Lock screen");
	lui_list_prepare(list1);
	lui_object_add_to_parent(list1, g_scene_one);
	

	// add a button to go to next scene (g_scene_two)
	g_btn_nxt_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_nxt_scn, g_scene_one);
	lui_object_set_position(g_btn_nxt_scn, 140, 595);
	lui_object_set_area(g_btn_nxt_scn, 200, 40);
	lui_button_set_label_text(g_btn_nxt_scn, "Next Scene ->");
	lui_object_set_callback(g_btn_nxt_scn, scn_change_event_handler);

	// add a button to go to prev scene (g_scene_one)
	g_btn_prev_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_prev_scn, g_scene_two);
	lui_object_set_position(g_btn_prev_scn, 140, 595);
	lui_object_set_area(g_btn_prev_scn, 200, 40);
	lui_button_set_label_text(g_btn_prev_scn, "<- Prev Scene");
	lui_object_set_callback(g_btn_prev_scn, scn_change_event_handler);

	// a text label for g_scene_two
	lui_obj_t* lbl_list_descr = lui_label_create();
	lui_object_add_to_parent(lbl_list_descr, g_scene_two);
	lui_label_set_text(lbl_list_descr, "This is a long list. For long lists that require multiple pages, navigation buttons are automatically added..");
	lui_object_set_position(lbl_list_descr, 90, 10);
	lui_object_set_area(lbl_list_descr, 300, 70);

	// add a big list in scene two
	lui_obj_t* list2 = lui_list_create();
	//lui_list_set_font(list2, &font_fixedsys_mono_16);
	lui_object_set_position(list2, 90, 90);
	lui_object_set_area(list2, 300, 280);
	lui_object_set_border_visibility(list2, 1);
	lui_obj_t* item_list2_1 = lui_list_add_item(list2, "Hello");
	lui_obj_t* item_list2_2 = lui_list_add_item(list2, "Hi there");
	lui_obj_t* item_list2_3 = lui_list_add_item(list2, "I am good");
	lui_obj_t* item_list2_4 = lui_list_add_item(list2, "This is");
	lui_obj_t* item_list2_5 = lui_list_add_item(list2, "a test");
	lui_obj_t* item_list2_6 = lui_list_add_item(list2, "and it's");
	lui_obj_t* item_list2_7 = lui_list_add_item(list2, "working!!");
	lui_obj_t* item_list2_8 = lui_list_add_item(list2, "I'm happy");
	lui_obj_t* item_list2_9 = lui_list_add_item(list2, "yaaaawn");
	lui_obj_t* item_list2_10 = lui_list_add_item(list2, "Gonna sleep");
	lui_obj_t* item_list2_11 = lui_list_add_item(list2, "Worked hard");
	lui_obj_t* item_list2_12 = lui_list_add_item(list2, "Now bye");
	lui_obj_t* item_list2_13 = lui_list_add_item(list2, "Hey you..!!");
	lui_obj_t* item_list2_14 = lui_list_add_item(list2, "Adding more items");
	lui_obj_t* item_list2_15 = lui_list_add_item(list2, "Ha hahahah");
	lui_obj_t* item_list2_16 = lui_list_add_item(list2, "It's simple");
	lui_obj_t* item_list2_17 = lui_list_add_item(list2, "and stupid");
	lui_obj_t* item_list2_18 = lui_list_add_item(list2, "but it works!");
	lui_obj_t* item_list2_19 = lui_list_add_item(list2, "bla bla bla");
	lui_obj_t* item_list2_20 = lui_list_add_item(list2, "More bla bla bla");
	lui_obj_t* item_list2_21 = lui_list_add_item(list2, "Enddddddddddd");
	lui_list_prepare(list2);
	lui_object_add_to_parent(list2, g_scene_two);
	lui_object_set_enable_input(item_list2_3, 0);

	// add a slider
	slider1 = lui_slider_create();
	lui_object_add_to_parent(slider1, g_scene_two);
	lui_object_set_position(slider1, 90, 390);
	lui_object_set_area(slider1, 270, 20);
	lui_slider_set_range(slider1, -100, 100);
	lui_slider_set_value(slider1, 50);
	lui_object_set_border_visibility(slider1, 1);
	lui_object_set_callback(slider1, slider_event_handler);

	// a label to show its value
	g_lbl_slider1_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider1_val, g_scene_two);
	lui_label_set_text(g_lbl_slider1_val, "50");
	lui_object_set_position(g_lbl_slider1_val, 370, 390);
	lui_object_set_area(g_lbl_slider1_val, 100, 20);

	// add second slider
	slider2 = lui_slider_create();
	lui_object_add_to_parent(slider2, g_scene_two);
	lui_object_set_position(slider2, 90, 420);
	lui_object_set_area(slider2, 270, 20);
	lui_slider_set_range(slider2, -100, 100);
	lui_slider_set_value(slider2, -50);
	lui_object_set_callback(slider2, slider_event_handler);

	// another label to show second slider's value
	g_lbl_slider2_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider2_val, g_scene_two);
	lui_label_set_text(g_lbl_slider2_val, "-50");
	lui_object_set_position(g_lbl_slider2_val, 370, 420);
	lui_object_set_area(g_lbl_slider2_val, 100, 20);


	// add a label as heading for checkboxes
	lui_obj_t* lbl_sport_question = lui_label_create();
	lui_object_add_to_parent(lbl_sport_question, g_scene_two);
	lui_label_set_text(lbl_sport_question, "What sports do you like?");
	lui_object_set_position(lbl_sport_question, 90, 470);
	lui_object_set_area(lbl_sport_question, 300, 20);

	lui_obj_t* chkbox_football = lui_checkbox_create();
	lui_object_set_position(chkbox_football, 90, 495);
	lui_object_add_to_parent(chkbox_football, g_scene_two);

	lui_obj_t* lbl_football = lui_label_create();
	lui_object_add_to_parent(lbl_football, g_scene_two);
	lui_label_set_text(lbl_football, "Football");
	lui_object_set_position(lbl_football, 130, 495);
	lui_object_set_area(lbl_football, 300, 20);

	lui_obj_t* chkbox_cricket = lui_checkbox_create();
	lui_object_set_position(chkbox_cricket, 90, 520);
	lui_object_add_to_parent(chkbox_cricket, g_scene_two);

	lui_obj_t* lbl_cricket = lui_label_create();
	lui_object_add_to_parent(lbl_cricket, g_scene_two);
	lui_label_set_text(lbl_cricket, "Cricket");
	lui_object_set_position(lbl_cricket, 130, 520);
	lui_object_set_area(lbl_cricket, 300, 20);

	lui_obj_t* chkbox_hockey = lui_checkbox_create();
	lui_object_set_position(chkbox_hockey, 90, 545);
	lui_object_add_to_parent(chkbox_hockey, g_scene_two);

	lui_obj_t* lbl_hockey = lui_label_create();
	lui_object_add_to_parent(lbl_hockey, g_scene_two);
	lui_label_set_text(lbl_hockey, "Hockey");
	lui_object_set_position(lbl_hockey, 130, 545);
	lui_object_set_area(lbl_hockey, 300, 20);


	/* Add a btngrid in scene three */
	btngrid = lui_keyboard_create();
	//lui_object_set_visibility(btngrid, 0);
	lui_object_add_to_parent(btngrid, g_scene_one);
	lui_keyboard_set_font(btngrid, &FONT_montserrat_regular_32);
	// lui_object_set_area(btngrid, HOR_RES, 300);
	// lui_btngrid_set_textmap(btngrid, btnm_map);
	// lui_btngrid_set_propertymap(btngrid, default_kb_ctrl_lc_map);
	// lui_btngrid_set_font(btngrid, &font_ubuntu_16);
	// lui_btngrid_set_btn_checkable(btngrid, 0, 1);
	// lui_btngrid_set_btn_checked(btngrid, 0, 1);
	// lui_btngrid_set_btn_hidden(btngrid, 1, 1);
	// lui_object_set_callback(btngrid, test);
	// lui_object_set_visibility(btngrid, 1);
	// lui_btngrid_set_btn_margin(btngrid, 3, 6);
	// lui_btngrid_set_extra_colors(btngrid, lui_rgb(255, 0, 0), 0xffff, lui_rgb(10, 150, 0));
	
	//lui_scene_set_active(g_scene_three);

	
		lui_obj_t* lbl_txtbox_name = lui_label_create();
		lui_label_set_text(lbl_txtbox_name, "Your Name:");
		lui_object_set_position(lbl_txtbox_name, 480, 25);
		lui_object_add_to_parent(lbl_txtbox_name, g_scene_one);

		lui_obj_t* txtbox_name = lui_textbox_create();
		lui_object_set_border_visibility(txtbox_name, 1);
		lui_object_set_callback(txtbox_name, textbox_callback);
		char name[50];
		lui_textbox_set_text_buffer(txtbox_name, name, 40);
		lui_object_add_to_parent(txtbox_name, g_scene_one);
		lui_object_set_position(txtbox_name, 480, 50);
		lui_object_set_area(txtbox_name, 720-490, 40);
	

	
		lui_obj_t* lbl_txtbox_addr = lui_label_create();
		lui_label_set_text(lbl_txtbox_addr, "Street Address:");
		lui_object_set_position(lbl_txtbox_addr, 480, 100);
		lui_object_add_to_parent(lbl_txtbox_addr, g_scene_one);

		lui_obj_t* txtbox_address = lui_textbox_create();
		lui_object_set_border_visibility(txtbox_address, 1);
		lui_object_set_callback(txtbox_address, textbox_callback);
		char addr[50];
		lui_textbox_set_text_buffer(txtbox_address, addr, 40);
		lui_object_add_to_parent(txtbox_address, g_scene_one);
		lui_object_set_position(txtbox_address, 480, 125);
		lui_object_set_area(txtbox_address, 720-490, 80);
	

	
		lui_obj_t* lbl_txtbox_age = lui_label_create();
		lui_label_set_text(lbl_txtbox_age, "Age:");
		lui_object_set_position(lbl_txtbox_age, 480, 210);
		lui_object_add_to_parent(lbl_txtbox_age, g_scene_one);

		lui_obj_t* txtbox_age = lui_textbox_create();
		lui_object_set_border_visibility(txtbox_age, 1);
		lui_object_set_callback(txtbox_age, textbox_callback);
		char age[3];
		lui_textbox_set_text_buffer(txtbox_age, age, 40);
		lui_object_add_to_parent(txtbox_age, g_scene_one);
		lui_object_set_position(txtbox_age, 480, 235);
		lui_object_set_area(txtbox_age, 720-540, 40);
	

	
		lui_obj_t* lbl_txtbox_father = lui_label_create();
		lui_label_set_text(lbl_txtbox_father, "Father's Name:");
		lui_object_set_position(lbl_txtbox_father, 480, 280);
		lui_object_add_to_parent(lbl_txtbox_father, g_scene_one);

		lui_obj_t* txtbox_father = lui_textbox_create();
		lui_object_set_border_visibility(txtbox_father, 1);
		lui_object_set_callback(txtbox_father, textbox_callback);
		char father[50];
		lui_textbox_set_text_buffer(txtbox_father, father, 40);
		lui_object_add_to_parent(txtbox_father, g_scene_one);
		lui_object_set_position(txtbox_father, 480, 305);
		lui_object_set_area(txtbox_father, 720-490, 40);
	

	
		lui_obj_t* lbl_txtbox_mother = lui_label_create();
		lui_label_set_text(lbl_txtbox_mother, "Mother's Name:");
		lui_object_set_position(lbl_txtbox_mother, 480, 350);
		lui_object_add_to_parent(lbl_txtbox_mother, g_scene_one);

		lui_obj_t* txtbox_mother = lui_textbox_create();
		lui_object_set_border_visibility(txtbox_mother, 1);
		lui_object_set_callback(txtbox_mother, textbox_callback);
		char mother[50];
		lui_textbox_set_text_buffer(txtbox_mother, mother, 40);
		lui_object_add_to_parent(txtbox_mother, g_scene_one);
		lui_object_set_position(txtbox_mother, 480, 375);
		lui_object_set_area(txtbox_mother, 720-490, 40);
	
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

	/* Commented out to simulate touch input */
	g_input.is_pressed = 0;
	g_input.x = -1;
	g_input.y = -1;
}

// This function is called back by glut when mouse is moved while being pressed
// We're setting the global input variable's value here
void myMousePressMove(int x, int y)
{
	g_input.x = x;
	g_input.y = y;
}

// This function is called back by glut when mouse is pressed
// This is to simulates touch input
void myMousePress(int button, int state, int x, int y)
{
	g_input.is_pressed = 0;
	g_input.x = -1;
	g_input.y = -1;
	if (button == GLUT_LEFT)
	{
		if (state == GLUT_DOWN)
		{
			g_input.is_pressed = 1;
			g_input.x = x;
			g_input.y = y;
		}
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