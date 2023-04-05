/*#!/usr/local/bin/tcc -run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L.*/
// tcc "-run -Iinc -ILameUI/inc -IFontsEmbedded -lGL -lGLU -lglut -lm -L. LameUI/src/lame_ui.c" src/main.c -v

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <GL/glut.h>

#include "../../LameUI/lame_ui.h"	/* No need to set full paths like this. It's just to visualize */
#include "../../LameUI/fonts/montserrat_regular_48.h"
#include "../../LameUI/fonts/ubuntu_regular_32.h"
#include "../../LameUI/fonts/ubuntu_regular_20.h"
#include "res/grad.h"
#include "res/grad_color1_bg.h"

#define UNUSED(x) (void)(x)

// Set display resolution
// OpenGL will use it to create a window
#define HOR_RES		720
#define VERT_RES	640

#define DISP_BUFF_PX_CNT (HOR_RES * VERT_RES)

uint16_t disp_buffer[DISP_BUFF_PX_CNT];
uint8_t memblk[20000];

// following UI elements are made global so we can access them in the event handler
lui_touch_input_data_t g_input = 
{
	.is_pressed = 0,
	.x = -1,
	.y = -1
};
lui_obj_t* g_scene_one;
lui_obj_t* g_scene_two;
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
lui_obj_t* keyboard;
lui_obj_t* btngrid_numpad;

lui_obj_t* g_active_txtbox = NULL;

double g_points[50][2];
char g_btn_press_cnt[4] = {0};// For same reason as above
uint16_t g_btn_press_cnt_int = 0;


// Callback functions for LameUI ---------------------------------------------------
void lameui_input_read_cb (lui_touch_input_data_t *input);
void lameui_draw_disp_buff_cb (uint16_t* disp_buff, lui_area_t* area);

void count_and_reset_event_handler(lui_obj_t* obj);
void popupbtn_event_handler(lui_obj_t* obj);
void change_grph_event_handler(lui_obj_t* obj);
void enable_wifi_and_bt_event_handler(lui_obj_t* obj);
void slider_event_handler(lui_obj_t* obj);
void scn_change_event_handler(lui_obj_t* obj);
void textbox_callback(lui_obj_t* obj_txtbox);
void list1_cb(lui_obj_t* obj);
// End LameUI callbacks ------------------------------------------------------------

// opengl functions. Not specific to LameUI ----------------------------------------
void gl_init(int argc, char** argv);
// End opengl function -------------------------------------------------------------

// glut callback functions. These are not LameUI specific --------------------------
void glutDisplay();
void glutIdle();
void glut_mouse_move(int x, int y);
void glut_mouse_press_move(int x, int y);
void glut_mouse_press(int button, int state, int x, int y);
void glut_process_normal_keys(unsigned char key, int x, int y);
void glut_process_spcl_keys(int key, int x, int y);
// End glut callbacks ---------------------------------------------------------------

// user functions
void prepare_chart_data_1();
void prepare_chart_data_2(uint16_t val);
// end user funcions


int main (int argc, char** argv)
{
	// Initialize opengl
	gl_init(argc, argv);	

	/*###################################################################################
	 #		Starts LameUI Based Code. The Below Part Is Hardware/Platform Agnostic		#
	 ###################################################################################*/

	// [IMPORTANT:] do it at the begining. Mandatory!
	lui_init(memblk, sizeof(memblk));

	//----------------------------------------------------------
	//creating display driver variable for lame_ui
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(my_display_driver, HOR_RES, VERT_RES);
	lui_dispdrv_set_draw_disp_buff_cb(my_display_driver, lameui_draw_disp_buff_cb);
    lui_dispdrv_set_disp_buff(my_display_driver, disp_buffer, DISP_BUFF_PX_CNT);
    lui_dispdrv_set_draw_disp_buff_cb(my_display_driver, lameui_draw_disp_buff_cb);

	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_device, lameui_input_read_cb);	

	//----------------------------------------------------------
	//create and add scenes
	g_scene_one = lui_scene_create();
	lui_scene_set_bitmap_image(g_scene_one, &BITMAP_grad_color1_bg);
	//lui_object_set_bg_color(0, g_scene_one);
	//lui_scene_set_font(g_scene_one, &font_microsoft_16);
	//g_scene_one = lui_scene_destroy(g_scene_one);	// For destroying, assigning the return value is mandatory

	//create and add scenes
	g_scene_two = lui_scene_create();
	//lui_object_set_bg_color(0, g_scene_two);
	//lui_scene_set_font(g_scene_two, &font_microsoft_16);


	//lui_object_set_bg_color(0, g_scene_two);
	//lui_scene_set_font(g_scene_two, &font_microsoft_16);

	//set the active scene. This scene will be rendered iby the lui_update()
	lui_scene_set_active(g_scene_one);

	//----------------------------------------------------------
	//create label

	lui_obj_t* lbl_heading = lui_label_create();
	lui_object_set_area(lbl_heading, HOR_RES, lui_gfx_get_font_height(&FONT_ubuntu_regular_32));
	lui_label_set_font(lbl_heading, &FONT_ubuntu_regular_32);
	lui_object_add_to_parent(lbl_heading, g_scene_one);
	lui_label_set_text(lbl_heading, "LameUI Demo Application");
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
	lui_object_set_position(g_grph, 20, 60);
	lui_object_set_area(g_grph, 440, 200);


	//----------------------------------------------------------

	lui_obj_t* lbl_cntr_txt = lui_label_create();
	lui_object_add_to_parent(lbl_cntr_txt, g_scene_one);
	lui_label_set_text(lbl_cntr_txt, "Counter:");
	lui_object_set_position(lbl_cntr_txt, 10, 285);
	lui_object_set_area(lbl_cntr_txt, 70, 20);


	g_lbl_cntr_value = lui_label_create();
	lui_object_add_to_parent(g_lbl_cntr_value, g_scene_one);
	lui_label_set_font(g_lbl_cntr_value, &FONT_montserrat_regular_48);
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
	lui_button_set_label_text(g_btn_count, LUI_ICON_ADD " Count");
	lui_object_set_callback(g_btn_count, count_and_reset_event_handler);

	g_btn_reset = lui_button_create();
	lui_object_add_to_parent(g_btn_reset, g_scene_one);
	lui_object_set_position(g_btn_reset, 350, 280);
	lui_object_set_area(g_btn_reset, 100, 40);
	lui_button_set_label_text(g_btn_reset, LUI_ICON_RELOAD " Reset");
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
	lui_label_set_text(lbl_enable_wifi, "  " LUI_ICON_WIFI "   WiFi");
	lui_object_set_position(lbl_enable_wifi, 5, 5);
	lui_object_set_area(lbl_enable_wifi, 140, 20);

	g_swtch_enable_wifi = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_wifi, panel_group_switches);
	lui_object_set_position(g_swtch_enable_wifi, 190, 5);
	lui_object_set_callback(g_swtch_enable_wifi, enable_wifi_and_bt_event_handler);

	lui_obj_t* lbl_enable_bluetooth = lui_label_create();
	lui_object_add_to_parent(lbl_enable_bluetooth, panel_group_switches);
	lui_label_set_text(lbl_enable_bluetooth, "  " LUI_ICON_BLUETOOTH "   Bluetooth");
	lui_object_set_position(lbl_enable_bluetooth, 5, 35);
	lui_object_set_area(lbl_enable_bluetooth, 140, 20);

	g_swtch_enable_bluetooth = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_bluetooth, panel_group_switches);
	lui_object_set_position(g_swtch_enable_bluetooth, 190, 35);
	lui_object_set_callback(g_swtch_enable_bluetooth, enable_wifi_and_bt_event_handler);

	lui_obj_t* lbl_location = lui_label_create();
	lui_object_add_to_parent(lbl_location, panel_group_switches);
	lui_label_set_text(lbl_location, "  " LUI_ICON_LOCATION "   Location");
	lui_object_set_position(lbl_location, 5, 65);
	lui_object_set_area(lbl_location, 140, 20);

	lui_obj_t* swtch_location = lui_switch_create();
	lui_object_add_to_parent(swtch_location, panel_group_switches);
	lui_object_set_position(swtch_location, 190, 65);

	lui_obj_t* lbl_notification = lui_label_create();
	lui_object_add_to_parent(lbl_notification, panel_group_switches);
	lui_label_set_text(lbl_notification, "  " LUI_ICON_VOLUME_MEDIUM "   Sound");
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
	lui_label_set_bg_transparent(lbl_long_text, 1);

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
	lui_list_set_max_items_count(list1, 20);
	lui_list_set_dropdown_mode(list1, 1);
	lui_object_set_position(list1, 250, 340);
	lui_object_set_area(list1, 215, 250);
	lui_object_set_border_visibility(list1, 0);
	lui_list_add_item(list1, "-- Select Language --");
	lui_list_add_item(list1, "Algerian");
	lui_list_add_item(list1, " Amharic ");
	lui_list_add_item(list1, "Assamese");
	lui_list_add_item(list1, "Bavarian");
	lui_list_add_item(list1, "Bengali");
	lui_list_add_item(list1, "Czech");
	lui_list_add_item(list1, "Deccan");
	lui_list_add_item(list1, "Dutch");
	lui_list_prepare(list1);
	lui_object_set_callback(list1, list1_cb);
	lui_object_add_to_parent(list1, g_scene_one);
	

	// add a button to go to next scene (g_scene_two)
	g_btn_nxt_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_nxt_scn, g_scene_one);
	lui_object_set_position(g_btn_nxt_scn, 140, 595);
	lui_object_set_area(g_btn_nxt_scn, 200, 40);
	lui_button_set_label_text(g_btn_nxt_scn, "Next Scene " LUI_ICON_ARROW_FORWARD);
	lui_object_set_callback(g_btn_nxt_scn, scn_change_event_handler);

	// add a button to go to prev scene (g_scene_one)
	g_btn_prev_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_prev_scn, g_scene_two);
	lui_object_set_position(g_btn_prev_scn, 140, 595);
	lui_object_set_area(g_btn_prev_scn, 200, 40);
	lui_button_set_label_text(g_btn_prev_scn, "<- Prev Scene");
	lui_object_set_callback(g_btn_prev_scn, scn_change_event_handler);


	// add a slider
	slider1 = lui_slider_create();
	lui_object_add_to_parent(slider1, g_scene_one);
	lui_object_set_position(slider1, 250, 390);
	lui_object_set_area(slider1, 180, 20);
	lui_slider_set_range(slider1, -100, 100);
	lui_slider_set_value(slider1, 50);
	lui_object_set_border_visibility(slider1, 1);
	lui_object_set_callback(slider1, slider_event_handler);

	// a label to show its value
	g_lbl_slider1_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider1_val, g_scene_one);
	lui_label_set_text(g_lbl_slider1_val, "50");
	lui_object_set_position(g_lbl_slider1_val, 440, 390);
	lui_object_set_area(g_lbl_slider1_val, 40, 20);
	lui_label_set_bg_transparent(g_lbl_slider1_val, 1);

	// add progress bar
	slider2 = lui_slider_create();
	lui_object_add_to_parent(slider2, g_scene_one);
	lui_slider_set_progress_bar(slider2, 1);
	lui_slider_set_show_value(slider2, 1);
	lui_slider_set_text(slider2, " % progress");
	lui_object_set_position(slider2, 250, 420);
	lui_object_set_area(slider2, 180, 20);
	lui_slider_set_range(slider2, 0, 100);
	lui_slider_set_value(slider2, 48);


	// add a label as heading for checkboxes
	lui_obj_t* lbl_sport_question = lui_label_create();
	lui_object_add_to_parent(lbl_sport_question, g_scene_one);
	lui_label_set_text(lbl_sport_question, "What sports do you like?");
	lui_object_set_position(lbl_sport_question, 260, 460);

	lui_obj_t* chkbox_football = lui_checkbox_create();
	lui_checkbox_set_label_text(chkbox_football, "  Football");
	lui_object_set_position(chkbox_football, 260, 495);
	lui_object_add_to_parent(chkbox_football, g_scene_one);

	lui_obj_t* chkbox_cricket = lui_checkbox_create();
	lui_checkbox_set_label_text(chkbox_cricket, "  Cricket");
	lui_object_set_position(chkbox_cricket, 260, 520);
	lui_object_add_to_parent(chkbox_cricket, g_scene_one);

	lui_obj_t* chkbox_hockey = lui_checkbox_create();
	lui_checkbox_set_label_text(chkbox_hockey, "  Hockey");
	lui_object_set_position(chkbox_hockey, 260, 545);
	lui_object_add_to_parent(chkbox_hockey, g_scene_one);


	/* Add a btngrid in scene three */
	keyboard = lui_keyboard_create();
	//lui_object_set_visibility(btngrid, 0);
	lui_object_add_to_parent(keyboard, g_scene_one);
	lui_keyboard_set_font(keyboard, &FONT_ubuntu_regular_32);

	const char* numpad_txt_map[] =
	{
		"7", "8", "9", "\n",
		"4", "5", "6", "\n",
		"1", "2", "3", "\n",
		"0", "00", ".", "\n",
		"+", "-", "=", "\0"
	};
	// Property map. Notice how the "=" button has twice the width of "+" and "-".
	const uint8_t numpad_property_map[] =
	{
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 2
	};

	btngrid_numpad = lui_btngrid_create();
	lui_btngrid_set_font(btngrid_numpad, &FONT_ubuntu_regular_20);
	lui_object_set_area(btngrid_numpad, 230, 200);
	lui_object_set_position(btngrid_numpad, 480, 420);
	lui_btngrid_set_textmap(btngrid_numpad, numpad_txt_map);
	lui_btngrid_set_propertymap(btngrid_numpad, numpad_property_map);
	lui_btngrid_set_btn_margin(btngrid_numpad, 5, 5);
	lui_object_add_to_parent(btngrid_numpad, g_scene_one);
	// lui_btngrid_set_extra_colors(btngrid_numpad, lui_rgb(255, 0, 0), 0xffff, lui_rgb(10, 150, 0));
	

	
	lui_obj_t* lbl_txtbox_name = lui_label_create();
	lui_label_set_text(lbl_txtbox_name, "Your Name:");
	lui_object_set_position(lbl_txtbox_name, 480, 60);
	lui_object_add_to_parent(lbl_txtbox_name, g_scene_one);

	lui_obj_t* txtbox_name = lui_textbox_create();
	lui_object_set_border_visibility(txtbox_name, 1);
	lui_object_set_callback(txtbox_name, textbox_callback);
	char name[50];
	lui_textbox_set_text_buffer(txtbox_name, name, 40);
	lui_textbox_insert_string(txtbox_name, "John Doe", 8);
	lui_object_add_to_parent(txtbox_name, g_scene_one);
	lui_object_set_position(txtbox_name, 480, 85);
	lui_object_set_area(txtbox_name, 720-490, 40);



	lui_obj_t* lbl_txtbox_addr = lui_label_create();
	lui_label_set_text(lbl_txtbox_addr, "Street Address:");
	lui_object_set_position(lbl_txtbox_addr, 480, 135);
	lui_object_add_to_parent(lbl_txtbox_addr, g_scene_one);

	lui_obj_t* txtbox_address = lui_textbox_create();
	lui_object_set_border_visibility(txtbox_address, 1);
	lui_object_set_callback(txtbox_address, textbox_callback);
	char addr[50];
	lui_textbox_set_text_buffer(txtbox_address, addr, 40);
	lui_textbox_insert_string(txtbox_address, "22/7 Camac Street", 17);
	lui_object_add_to_parent(txtbox_address, g_scene_one);
	lui_object_set_position(txtbox_address, 480, 160);
	lui_object_set_area(txtbox_address, 720-490, 80);



	lui_obj_t* lbl_txtbox_age = lui_label_create();
	lui_label_set_text(lbl_txtbox_age, "Age:");
	lui_object_set_position(lbl_txtbox_age, 480, 245);
	lui_object_add_to_parent(lbl_txtbox_age, g_scene_one);

	lui_obj_t* txtbox_age = lui_textbox_create();
	lui_object_set_border_visibility(txtbox_age, 1);
	lui_object_set_callback(txtbox_age, textbox_callback);
	char age[3];
	lui_textbox_set_text_buffer(txtbox_age, age, 40);
	lui_textbox_insert_string(txtbox_age, "25", 2);
	lui_object_add_to_parent(txtbox_age, g_scene_one);
	lui_object_set_position(txtbox_age, 480, 270);
	lui_object_set_area(txtbox_age, 720-540, 40);


	lui_obj_t* txtbox_numpad = lui_textbox_create();
	lui_object_set_border_visibility(txtbox_numpad, 1);
	lui_object_set_callback(txtbox_numpad, textbox_callback);
	char numpad_number[50];
	lui_textbox_set_text_buffer(txtbox_numpad, numpad_number, 40);
	lui_object_add_to_parent(txtbox_numpad, g_scene_one);
	lui_object_set_position(txtbox_numpad, 480, 375);
	lui_object_set_area(txtbox_numpad, 720-490, 40);

	/* Add another slider to scene 2 */
	lui_obj_t* slider_vert = lui_slider_create();
	lui_object_set_area(slider_vert, 30, 100);
	lui_object_add_to_parent(slider_vert, g_scene_two);
	lui_object_set_position(slider_vert, 50, 200);
	// lui_slider_set_value(slider_vert, 33);

	/*-----------------------------------------------------------------------------------
	 -		Glut related functions for drawing and input handling						-
	 -----------------------------------------------------------------------------------*/
	glutMouseFunc(glut_mouse_press);		// to handle mouse press while not being moved
	glutMotionFunc(glut_mouse_press_move);	// to handle mouse movement while being clicked
	glutPassiveMotionFunc(glut_mouse_move);	// to handle mouse movement while NOT being pressed
	glutKeyboardFunc(glut_process_normal_keys);
	glutSpecialFunc(glut_process_spcl_keys);
	glutIdleFunc(glutIdle);
	glutDisplayFunc(glutDisplay);
	glutMainLoop();

	return 0;
}

// Initialize OpenGL
void gl_init(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	/* giving window size in X- and Y- direction
	pading in both sides
	glutInitWindowSize(HOR_RES + (PADDING * 2), VERT_RES + (PADDING * 2)); */
	glutInitWindowSize(HOR_RES, VERT_RES);
	glutInitWindowPosition(0, 0);

	/* Giving name to window */
	glutCreateWindow("LameUI Simulator");
    
	// making background color black as first
	// 3 arguments all are 0.0
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// breadth of picture boundary is 1 pixel
	glPointSize(1.0);

     // set up orthographic projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// setting window dimension in X- and Y- direction
	//gluOrtho2D(-25, HOR_RES+25, VERT_RES+25, -25);
	gluOrtho2D(0, HOR_RES, VERT_RES, 0);

    glFlush();
}


// Read input from an input device
// It should take device position (x, y) and a button's status
void lameui_input_read_cb (lui_touch_input_data_t *input)
{
	input->is_pressed = g_input.is_pressed;
	input->y = g_input.y;
	input->x = g_input.x;

	//printf("[DEBUG] void lameui_input_read_cb(). x:%d\ty: %d\n", input->y, input->x);
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
		// lui_keyboard_set_target_txtbox(keyboard, obj_txtbox);
		/* Exit from previously active textbox (if any) */
		lui_textbox_exit_edit_mode(g_active_txtbox);
		g_active_txtbox = obj_txtbox;	// for physical keyboard
	}
	else if (event == LUI_EVENT_EXITED)
	{
		lui_keyboard_set_target_txtbox(keyboard, NULL);
		g_active_txtbox = NULL;		// for physical keyboard
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

void list1_cb(lui_obj_t* obj)
{
	uint8_t event = lui_object_get_event(obj);
	if (event == LUI_EVENT_PRESSED)
	{
		const char* txt = lui_list_get_item_text(obj, lui_list_get_selected_item_index(obj));
		if (txt != NULL)
			fprintf(stderr, "Selected: %s\n", txt);
	}
}


// Draw an area of pixels in one go.
// This is faster because the opengl buffer is filled using a loop,
// and then the preapred buffer is flushed in one go
void lameui_draw_disp_buff_cb (uint16_t* disp_buff, lui_area_t* area)
{
    uint16_t temp_x;
    uint16_t temp_y;

    glBegin(GL_POINTS);

    // Prepare the display buffer
    // After the loop ends, the prepared buffer is flushed
    for (temp_y = area->y; temp_y <= area->y + area->h - 1; temp_y++)
    {
        for (temp_x = area->x; temp_x <= area->x + area->w - 1; temp_x++)
        {
            uint8_t r_lsb = (*disp_buff >> 11) & 0x1F;
            uint8_t g_lsb = (*disp_buff >> 5) & 0x3F;
            uint8_t b_lsb = (*disp_buff >> 0) & 0x1F;

            // Set the color
            glColor3ub(r_lsb << 3, g_lsb << 2, b_lsb << 3);
            /**
             * glVertex2i just draws a point on specified co-ordinate.
             * Actual drawing is done after calling glFlush()
             */
            glVertex2i(temp_x, temp_y);

            ++disp_buff;
        }
    }
    glEnd();

    /* No need to flush here. We're flusdhing using a timer already (every 16 ms) */
    // glFlush();
    // // glutSwapBuffers();   /* Can be used instead of glFlush() */
}

// This function is called back by glut when drawing is needed
// Here we call our render function
void glutDisplay()
{
	// the main update function to process input and display output
	lui_update();

	
	// Flush drawing commands
	glFlush();
}

// This function is called back by glut when mouse is moved passively
// We're setting the global input variable's value here
void glut_mouse_move(int x, int y)
{
	UNUSED(x);
	UNUSED(y);
	//printf ("[DEBUG] void myMouse( int x: %d, int y: %d )\n", x, y);			//send all output to screen

	/* Commented out to simulate touch input */
	g_input.is_pressed = 0;
	g_input.x = -1;
	g_input.y = -1;
}

// This function is called back by glut when mouse is moved while being pressed
// We're setting the global input variable's value here
void glut_mouse_press_move(int x, int y)
{
	g_input.x = x;
	g_input.y = y;
}

// This function is called back by glut when mouse is pressed
// This is to simulates touch input
void glut_mouse_press(int button, int state, int x, int y)
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

void glut_process_normal_keys(unsigned char key, int x, int y)
{
	fprintf(stderr, "		KEY: %d\n", key);
	if (!g_active_txtbox)
		return;

	uint16_t caret_index = lui_textbox_get_caret_index(g_active_txtbox);
	if (key >= 32 && key <= 126)
	{
		lui_textbox_insert_char(g_active_txtbox, (char)key);
		// lui_textbox_set_caret_index(g_active_txtbox, ++caret_index);
	}
	/* Return (Enter) key */
	else if (key == 13)
	{
		lui_textbox_insert_char(g_active_txtbox, '\n');
		// lui_textbox_set_caret_index(g_active_txtbox, ++caret_index);
	}
	/* Backspace key */
	else if (key == 8)
	{
		if (caret_index > 0)
		{
			lui_textbox_set_caret_index(g_active_txtbox, --caret_index);
			lui_textbox_delete_char(g_active_txtbox);
		}
	}
	/* Escape key */
	else if (key == 27)
	{
		lui_textbox_exit_edit_mode(g_active_txtbox);
	}
}

void glut_process_spcl_keys(int key, int x, int y)
{
	// fprintf(stderr, "		KEY: %d\n", key);
	if (!g_active_txtbox)
		return;

	uint16_t caret_index = lui_textbox_get_caret_index(g_active_txtbox);

	if (key == GLUT_KEY_LEFT)
	{
		if (caret_index > 0)
			lui_textbox_set_caret_index(g_active_txtbox, --caret_index);
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		lui_textbox_set_caret_index(g_active_txtbox, ++caret_index);
	}
}

// This function is called back by glut during idle time (when not drawing)
// Here we wait a bit and force a redisplay function
void glutIdle()
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