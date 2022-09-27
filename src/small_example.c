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
#define HOR_RES		240
#define VERT_RES	320


// following UI elements are made global so we can access them in the event handler
lui_touch_input_data_t g_input = 
{
	.is_pressed = 0,
	.x = -1,
	.y = -1
};
double g_points[50][2];
char name[50];
char addr[50];

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
lui_obj_t* g_slider1;
lui_obj_t* slider2;
lui_obj_t* list1;
lui_obj_t* chkbox_hockey;
lui_obj_t* chkbox_cricket;
lui_obj_t* chkbox_football;

lui_obj_t* g_keyboard;
uint32_t g_disp_buffer_counter = 0;
uint32_t g_sidp_buffer_max_size = HOR_RES * VERT_RES;


// Callback functions for LameUI ---------------------------------------------------
void textbox_callback(lui_obj_t* obj_txtbox);
void count_and_reset_event_handler(lui_obj_t* obj);
void popupbtn_event_handler(lui_obj_t* obj);
void change_grph_event_handler(lui_obj_t* obj);
void enable_wifi_and_bt_event_handler(lui_obj_t* obj);
void slider_event_handler_cb(lui_obj_t* obj);
void scn_change_event_handler(lui_obj_t* obj);
void my_input_read_opengl (lui_touch_input_data_t *input);
void my_set_pixel_area_opengl (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
// End LameUI callbacks ------------------------------------------------------------

void prepare_chart_data_2(uint16_t sin_val)
{
	for (int i = 0; i < 50; i++)
	{
		g_points[i][0] = i;		//x value of i'th element
		//g_points[i][1] = sin((double)i+10)/((double)i+10);	//y value of i'th element
		g_points[i][1] = sin((double)i+sin_val)/((double)i+sin_val);	//y value of i'th element
	}
}

// Event handler for slider. Changes a label's text based on slider value
void slider_text_update()
{

  int16_t val = lui_slider_get_value(g_slider1);

  static char buf_slider1[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
  snprintf(buf_slider1, 5, "%d", val);
  lui_label_set_text(g_lbl_slider1_val, buf_slider1);
}


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
		char* txt = lui_list_get_item_text(obj, lui_list_get_selected_item_index(obj));
		if (txt != NULL)
			fprintf(stderr, "Selected: %s\n", txt);
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

  //----------------------------------------------------------
	//Prepare some data for a graph
	prepare_chart_data_2(10);
	//----------------------------------------------------------

  // -------------------------------------------
	// create popup group
	g_popup_panel = lui_panel_create();
	lui_object_add_to_parent(g_popup_panel, g_scene_one);
	g_popup_label = lui_label_create();
	lui_obj_t* popup_btn = lui_button_create();
	lui_object_add_to_parent(popup_btn, g_popup_panel);
	lui_object_add_to_parent(g_popup_label, g_popup_panel);
	
	lui_label_set_text(g_popup_label, "This is a popup");
	
	lui_object_set_position(popup_btn, 60, 40);
	lui_object_set_position(g_popup_label, 5, 10);
	lui_object_set_position(g_popup_panel, 20, 80);

	//lui_object_set_area(g_popup_label, 158, 18);
	lui_object_set_area(popup_btn, 80, 25);
	lui_object_set_area(g_popup_panel, 200, 80);

	lui_object_set_visibility(g_popup_panel, 0);
	lui_object_set_layer(g_popup_panel, LUI_LAYER_POPUP);


	lui_button_set_label_text(popup_btn, "OK");
	//lui_button_set_label_color(popup_btn, ILI_COLOR_WHITE);
	//lui_button_set_extra_colors(popup_btn, lui_rgb(112, 0, 77), lui_rgb(171, 0, 117));
	lui_object_set_callback(popup_btn, popupbtn_event_handler);
	
	//----------------------------------------------------------

  //----------------------------------------------------------
	//create a line chart with above data
	g_grph = lui_linechart_create();
	//lui_linechart_set_line_color(g_grph, 0xFFFF);
	//lui_linechart_set_grid_color(g_grph, lui_rgb(120, 120, 120));
	lui_object_add_to_parent(g_grph, g_scene_one);
	lui_linechart_set_data_source(g_grph, (double* )&g_points, 50);
	//lui_linechart_set_data_range(g_grph, -4, 4);
	lui_object_set_position(g_grph, 10, 1);
	lui_object_set_area(g_grph, 220, 60);

	//----------------------------------------------------------


	// group all the switches and their labels
	lui_obj_t* panel_group_switches =  lui_panel_create();
	lui_object_add_to_parent(panel_group_switches, g_scene_one);
  lui_object_set_border_visibility(panel_group_switches, 1);
	lui_object_set_position(panel_group_switches, 10, 62);
	lui_object_set_area(panel_group_switches, 220, 68);
  //lui_object_set_enable_input(panel_group_switches, 0);

	lui_obj_t* lbl_enable_wifi = lui_label_create();
	lui_object_add_to_parent(lbl_enable_wifi, panel_group_switches);
	lui_label_set_text(lbl_enable_wifi, LUI_ICON_WIFI " WiFi");
	lui_object_set_position(lbl_enable_wifi, 5, 2);

	g_swtch_enable_wifi = lui_switch_create();
	lui_object_add_to_parent(g_swtch_enable_wifi, panel_group_switches);
	lui_object_set_position(g_swtch_enable_wifi, 150, 2);
  // lui_object_set_layer(g_swtch_enable_wifi, 10);
	lui_object_set_callback(g_swtch_enable_wifi, enable_wifi_and_bt_event_handler);

	lui_obj_t* lbl_enable_bluetooth = lui_label_create();
	lui_object_add_to_parent(lbl_enable_bluetooth, panel_group_switches);
	lui_label_set_text(lbl_enable_bluetooth, LUI_ICON_BLUETOOTH " Bluetooth");
	lui_object_set_position(lbl_enable_bluetooth, 5, 24);

	g_swtch_enable_bluetooth = lui_switch_create();
  	lui_switch_set_on(g_swtch_enable_bluetooth);
	lui_object_add_to_parent(g_swtch_enable_bluetooth, panel_group_switches);
	lui_object_set_position(g_swtch_enable_bluetooth, 150, 24);
	//lui_object_set_callback(g_swtch_enable_bluetooth, enable_wifi_and_bt_event_handler);


	lui_obj_t* lbl_notification = lui_label_create();
	lui_object_add_to_parent(lbl_notification, panel_group_switches);
	lui_label_set_text(lbl_notification, LUI_ICON_LOCATION " Location");
	lui_object_set_position(lbl_notification, 5, 46);

	lui_obj_t* swtch_notification = lui_switch_create();
	lui_object_add_to_parent(swtch_notification, panel_group_switches);
	lui_object_set_position(swtch_notification, 150, 46);

  g_slider1 = lui_slider_create();
	lui_object_add_to_parent(g_slider1, g_scene_one);
	lui_object_set_position(g_slider1, 10, 133);
	lui_object_set_area(g_slider1, 160, 20);
	lui_slider_set_range(g_slider1, -100, 100);
	lui_slider_set_value(g_slider1, 50);
	lui_object_set_border_visibility(g_slider1, 1);
  lui_object_set_callback(g_slider1, slider_event_handler_cb);

  g_lbl_slider1_val = lui_label_create();
	lui_object_add_to_parent(g_lbl_slider1_val, g_scene_one);
	lui_label_set_text(g_lbl_slider1_val, "50");
	lui_object_set_position(g_lbl_slider1_val, 180, 133);
	lui_object_set_area(g_lbl_slider1_val, 80, 20);


	// add a small list in scene 
	list1 = lui_list_create();
	lui_list_set_dropdown_mode(list1, 1);
	lui_list_set_max_items_count(list1, 20);
	//lui_list_set_font(list1, &font_microsoft_16);
	lui_object_set_position(list1, 10, 155);
	lui_object_set_area(list1, 110, 160);
	lui_object_set_border_visibility(list1, 1);
	lui_list_add_item(list1, "--Select--");
	lui_list_add_item(list1, "Algerian");
	lui_list_add_item(list1, " Amharic ");
	lui_list_add_item(list1, "Assamese");
	lui_list_add_item(list1, "Bavarian");
	lui_list_add_item(list1, "Bengali");
	lui_list_add_item(list1, "Czech");
	lui_list_add_item(list1, "Deccan");
	lui_list_add_item(list1, "Dutch");
	lui_list_add_item(list1, "Egyptian");
	lui_list_add_item(list1, "English");
	lui_list_add_item(list1, "French");
	lui_list_add_item(list1, "German");
	lui_list_add_item(list1, "Greek");
	lui_list_add_item(list1, "Hindi");
	lui_list_add_item(list1, "Hungarian");
	lui_list_add_item(list1, "Italian");
	lui_list_add_item(list1, "Japanese");
	lui_list_prepare(list1);
	lui_object_set_callback(list1, list1_cb);
	lui_object_add_to_parent(list1, g_scene_one);


  
    // add a label as heading for checkboxes
    lui_obj_t* lbl_sport_question = lui_label_create();
    lui_object_add_to_parent(lbl_sport_question, g_scene_one);
    lui_label_set_text(lbl_sport_question, "your fav?");
    lui_label_set_text_color(lbl_sport_question, lui_rgb(200, 50, 240));
    lui_object_set_position(lbl_sport_question, 122, 155);

    chkbox_football = lui_checkbox_create();
    lui_checkbox_set_checked(chkbox_football);
    lui_object_set_position(chkbox_football, 125, 180);
    lui_object_add_to_parent(chkbox_football, g_scene_one);

    lui_obj_t* lbl_football = lui_label_create();
    lui_object_add_to_parent(lbl_football, g_scene_one);
    lui_label_set_text(lbl_football, "Football");
    lui_object_set_position(lbl_football, 150, 180);

    chkbox_cricket = lui_checkbox_create();
    lui_object_set_position(chkbox_cricket, 125, 202);
    lui_object_add_to_parent(chkbox_cricket, g_scene_one);

    lui_obj_t* lbl_cricket = lui_label_create();
    lui_object_add_to_parent(lbl_cricket, g_scene_one);
    lui_label_set_text(lbl_cricket, "Cricket");
    lui_object_set_position(lbl_cricket, 150, 202);

    chkbox_hockey = lui_checkbox_create();
    lui_object_set_position(chkbox_hockey, 125, 224);
    lui_object_add_to_parent(chkbox_hockey, g_scene_one);

    lui_obj_t* lbl_hockey = lui_label_create();
    lui_object_add_to_parent(lbl_hockey, g_scene_one);
    lui_label_set_text(lbl_hockey, "Hockey");
    lui_object_set_position(lbl_hockey, 150, 224);
  
	

	// add a button to go to next scene (g_scene_two)
	g_btn_nxt_scn = lui_button_create();
	lui_object_add_to_parent(g_btn_nxt_scn, g_scene_one);
	lui_object_set_position(g_btn_nxt_scn, 124, 260);
	lui_object_set_area(g_btn_nxt_scn, 110, 40);
	lui_button_set_label_text(g_btn_nxt_scn, LUI_ICON_HOME);
  lui_button_set_label_font(g_btn_nxt_scn, &FONT_montserrat_regular_32);
	//lui_object_set_callback(g_btn_nxt_scn, scn_change_event_handler);



  // --------------------------------------------------------------
  g_keyboard = lui_keyboard_create();
	//lui_object_set_visibility(g_keyboard, 1);
	lui_object_add_to_parent(g_keyboard, g_scene_two);
  

  lui_obj_t* lbl_txtbox_name = lui_label_create();
  lui_label_set_text(lbl_txtbox_name, (char*)"Your Name:");
  lui_object_set_position(lbl_txtbox_name, 10, 10);
  lui_object_add_to_parent(lbl_txtbox_name, g_scene_two);

  lui_obj_t* txtbox_name = lui_textbox_create();
  lui_object_set_callback(txtbox_name, textbox_callback);
  lui_textbox_set_text_buffer(txtbox_name, name, 40);
  lui_textbox_insert_string(txtbox_name, (char*)"Avra Mitra", 10);
  lui_object_add_to_parent(txtbox_name, g_scene_two);
  lui_object_set_position(txtbox_name, 10, 35);
  lui_object_set_area(txtbox_name, 200, 40);



  lui_obj_t* lbl_txtbox_addr = lui_label_create();
  lui_label_set_text(lbl_txtbox_addr, (char*)"Street Address:");
  lui_object_set_position(lbl_txtbox_addr, 10, 80);
  lui_object_add_to_parent(lbl_txtbox_addr, g_scene_two);

  lui_obj_t* txtbox_address = lui_textbox_create();
  lui_object_set_callback(txtbox_address, textbox_callback);
  lui_textbox_set_text_buffer(txtbox_address, addr, 40);
  lui_textbox_insert_string(txtbox_address, (char*)"Kalyani,\nWB, 741235", 20);
  lui_object_add_to_parent(txtbox_address, g_scene_two);
  lui_object_set_position(txtbox_address, 10, 110);
  lui_object_set_area(txtbox_address, 200, 80);

//   lui_scene_set_active(g_scene_two);
	
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

// Event handler for slider. Changes a label's text based on slider value
void slider_event_handler_cb(lui_obj_t* obj)
{
	if (lui_object_get_event(obj) == LUI_EVENT_VALUE_CHANGED)
	{
		int16_t val = lui_slider_get_value(g_slider1);

    static char buf_slider1[5]; /* max 3 bytes for number plus 1 sign plus 1 null terminating byte */
    snprintf(buf_slider1, 5, "%d", val);
    lui_label_set_text(g_lbl_slider1_val, buf_slider1);
		prepare_chart_data_2(val);
    lui_linechart_set_data_source(g_grph, (double* )&g_points, 50);
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


// Event handler for popup buttons
void popupbtn_event_handler(lui_obj_t* obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	fprintf(stderr, "Invisible!\n");
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

void textbox_callback(lui_obj_t* obj_txtbox)
{
	int8_t event = lui_object_get_event(obj_txtbox);
	if (event == LUI_EVENT_ENTERED)
	{
		lui_keyboard_set_target_txtbox(g_keyboard, obj_txtbox);
	}
	else if (event == LUI_EVENT_EXITED)
	{
		lui_keyboard_set_target_txtbox(g_keyboard, NULL);
	}
}