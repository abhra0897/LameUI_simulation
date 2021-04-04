
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <inttypes.h>
#include "lame_ui.h"
#include "font_microsoft_16.h"
#include "font_ubuntu_48.h"
#include <unistd.h>

// Color definitions
#define ILI_COLOR_BLACK       0x0000  ///<   0,   0,   0
#define ILI_COLOR_NAVY        0x000F  ///<   0,   0, 123
#define ILI_COLOR_DARKGREEN   0x03E0  ///<   0, 125,   0
#define ILI_COLOR_DARKCYAN    0x03EF  ///<   0, 125, 123
#define ILI_COLOR_MAROON      0x7800  ///< 123,   0,   0
#define ILI_COLOR_PURPLE      0x780F  ///< 123,   0, 123
#define ILI_COLOR_OLIVE       0x7BE0  ///< 123, 125,   0
#define ILI_COLOR_LIGHTGREY   0xC618  ///< 198, 195, 198
#define ILI_COLOR_DARKGREY    0x7BEF  ///< 123, 125, 123
#define ILI_COLOR_BLUE        0x001F  ///<   0,   0, 255
#define ILI_COLOR_GREEN       0x07E0  ///<   0, 255,   0
#define ILI_COLOR_CYAN        0x07FF  ///<   0, 255, 255
#define ILI_COLOR_RED         0xF800  ///< 255,   0,   0
#define ILI_COLOR_MAGENTA     0xF81F  ///< 255,   0, 255
#define ILI_COLOR_YELLOW      0xFFE0  ///< 255, 255,   0
#define ILI_COLOR_WHITE       0xFFFF  ///< 255, 255, 255
#define ILI_COLOR_ORANGE      0xFD20  ///< 255, 165,   0
#define ILI_COLOR_GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define ILI_COLOR_PINK        0xFC18  ///< 255, 130, 198

// Set display resolution
// OpenGL will use it to create a window
#define HOR_RES		240
#define VERT_RES	320
#define PADDING		25

lui_touch_input_data_t g_input;	// Made input global so it can be changed by glut callback functions
lui_obj_t *g_scn_one;		// Made scene global so glut callbacks can access it
lui_obj_t *g_lbl3;			// For same reason as above
lui_obj_t *g_popup_panel;
lui_obj_t *popup_label;
lui_obj_t *grph;
lui_obj_t *btn;
lui_obj_t *btn_reset;

double points[50][2];
char btn_press_cnt[4] = {0};// For same reason as above
uint16_t btn_press_cnt_int = 0;
uint32_t g_disp_buffer_counter = 0;
uint32_t g_sidp_buffer_max_size = HOR_RES * VERT_RES;

// Callback functions for LameUI ---------------------------------------------------
void my_obj_event_handler(lui_obj_t *obj);
void my_popupbtn_event_handler(lui_obj_t *obj);
void my_swtch_change_grph_event_handler(lui_obj_t *obj);
void my_swtch_enable_wifi_event_handler(lui_obj_t *obj);
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
void prepare_chart_data_2();
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
	gluOrtho2D(0, HOR_RES, VERT_RES, 0);

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
void my_obj_event_handler(lui_obj_t *obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	switch (event)
	{
	case LUI_EVENT_NONE:
		break;
	case LUI_EVENT_SELECTED:
		break;
	case LUI_EVENT_SELECTION_LOST:
		break;
	case LUI_EVENT_PRESSED:
		if (obj == btn)
		{
			sprintf(btn_press_cnt, "%d", ++btn_press_cnt_int);
			lui_label_set_text(btn_press_cnt, g_lbl3);
		}
		else if (obj == btn_reset)
		{
			btn_press_cnt_int = 0;
			sprintf(btn_press_cnt, "%d", btn_press_cnt_int);
			lui_label_set_text(btn_press_cnt, g_lbl3);
		}
		break;
	case LUI_EVENT_RELEASED:
		break;		
	case LUI_EVENT_ENTERED:
		break;
	default:
		break;
	}
}


void my_popupbtn_event_handler(lui_obj_t *obj)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	uint8_t event = lui_object_get_event(obj);
	switch (event)
	{
	case LUI_EVENT_PRESSED:
		lui_scene_unset_popup(g_scn_one);
		break;
	case LUI_EVENT_RELEASED:
		break;		
	default:
		break;
	}
}

// Event handler for switch change graph. Called back by LameUI
void my_swtch_change_grph_event_handler(lui_obj_t *obj)
{	
	// change background color
	uint8_t event = lui_object_get_event(obj);
	if (event == LUI_EVENT_VALUE_CHANGED)
	{
		uint8_t val = lui_switch_get_value(obj);	
		if (val == 1)
		{
			prepare_chart_data_1();
			lui_linechart_set_data_source((double *)&points, 50, grph);
		}
		else
		{
			prepare_chart_data_2();
			lui_linechart_set_data_source((double *)&points, 50, grph);
		}
	}
}

// Event handler for switch change graph. Called back by LameUI
void my_swtch_enable_wifi_event_handler(lui_obj_t *obj)
{	
	// change background color
	uint8_t event = lui_object_get_event(obj);
	if (event == LUI_EVENT_VALUE_CHANGED)
	{
		uint8_t val = lui_switch_get_value(obj);
		
		if (val == 1)
		{
			lui_label_set_text("Alert:\nWiFi is enabled", popup_label);
			lui_scene_set_popup(g_popup_panel, g_scn_one);
		}
		else
		{
			lui_label_set_text("Alert:\nWiFi is disabled", popup_label);
			lui_scene_set_popup(g_popup_panel, g_scn_one);
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
		}
	}
	// Flush the buffer and display its content
	//glFlush();

	
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


	//----------------------------------------------------------
	//creating display driver variable for lame_ui
	lui_dispdrv_t *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(240, 320, my_display_driver);
	lui_dispdrv_set_draw_pixels_area_cb(my_set_pixel_area_opengl, my_display_driver);
	lui_dispdrv_set_render_complete_cb(my_render_complete_handler, my_display_driver);
	//my_display_driver = lui_dispdrv_destroy(my_display_driver);

	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_read_opengl, my_input_device);	

	//----------------------------------------------------------
	//create and add scenes
	g_scn_one = lui_scene_create();
	lui_object_set_bg_color(0, g_scn_one);
	lui_scene_set_font(&font_microsoft_16, g_scn_one);
	//g_scn_one = lui_scene_destroy(g_scn_one);	// For destroying, assigning the return value is mandatory

	
	//----------------------------------------------------------
	//create label

	lui_obj_t *lbl_heading = lui_label_create();
	lui_object_add_to_parent(lbl_heading, g_scn_one);
	lui_label_set_text("This is a demo of LameUI :)", lbl_heading);
	lui_object_set_position(1, 1, lbl_heading);
	lui_object_set_area(250, 20, lbl_heading);
	lui_label_set_colors(ILI_COLOR_WHITE, LUI_RGB(219, 0, 150), lbl_heading);
	lui_object_set_border_visibility(1, lbl_heading);

	lui_obj_t *lbl4 = lui_label_create();
	lui_object_add_to_parent(lbl4, g_scn_one);
	lui_label_set_text("Counter:", lbl4);
	lui_object_set_position(10, 175, lbl4);
	lui_object_set_area(70, 20, lbl4);
	lui_label_set_colors(ILI_COLOR_PINK, LUI_RGB(0, 0, 0), lbl4);

	g_lbl3 = lui_label_create();
	lui_object_add_to_parent(g_lbl3, g_scn_one);
	lui_label_set_font(&font_ubuntu_48, g_lbl3);
	lui_label_set_text("0", g_lbl3);
	lui_object_set_position(85, 160, g_lbl3);
	lui_object_set_area(110, 50, g_lbl3);
	lui_label_set_colors(ILI_COLOR_GREEN, LUI_RGB(60, 60, 60), g_lbl3);
	lui_object_set_border_visibility(1, g_lbl3);
	lui_object_set_border_color(LUI_RGB(100, 100, 100), g_lbl3);

	


	//----------------------------------------------------------
	//Prepare some data for a graph
	prepare_chart_data_2();
	
	//----------------------------------------------------------
	//create a line chart with above data
	grph = lui_linechart_create();
	lui_object_add_to_parent(grph, g_scn_one);
	lui_linechart_set_data_source((double *)&points, 50, grph);
	lui_linechart_set_data_auto_scale(1, grph);
	//lui_linechart_set_data_range(-4, 4, grph);
	lui_object_set_position(20, 50, grph);
	lui_object_set_area(200, 100, grph);
	lui_linechart_set_grid(LUI_RGB(60, 60, 60), 4, 3, grph);
	lui_linechart_set_colors(ILI_COLOR_PINK, ILI_COLOR_BLACK, grph);
	lui_object_set_border_visibility(1, grph);
	lui_object_set_border_color(ILI_COLOR_PINK, grph);


	//----------------------------------------------------------
	

	lui_obj_t *lbl_enable_wifi = lui_label_create();
	lui_object_add_to_parent(lbl_enable_wifi, g_scn_one);
	lui_label_set_text("Enable WiFi", lbl_enable_wifi);
	lui_object_set_position(5, 225, lbl_enable_wifi);
	lui_object_set_area(120, 20, lbl_enable_wifi);
	lui_label_set_colors(ILI_COLOR_PINK, LUI_RGB(0, 0, 0), lbl_enable_wifi);

	lui_obj_t *swtch_enable_wifi = lui_switch_create();
	lui_object_add_to_parent(swtch_enable_wifi, g_scn_one);
	lui_object_set_position(140, 225, swtch_enable_wifi);
	lui_switch_set_colors(0xFFFF, LUI_RGB(60, 60, 60), LUI_RGB(171, 0, 117), swtch_enable_wifi);
	lui_object_set_border_visibility(1, swtch_enable_wifi);
	lui_switch_set_value(0, swtch_enable_wifi);
	lui_object_set_callback(my_swtch_enable_wifi_event_handler, swtch_enable_wifi);

	lui_obj_t *lbl_change_grph = lui_label_create();
	lui_object_add_to_parent(lbl_change_grph, g_scn_one);
	lui_label_set_text("Change graph", lbl_change_grph);
	lui_object_set_position(5, 250, lbl_change_grph);
	lui_object_set_area(120, 20, lbl_change_grph);
	lui_label_set_colors(ILI_COLOR_PINK, LUI_RGB(0, 0, 0), lbl_change_grph);

	lui_obj_t *swtch_change_grph = lui_switch_create();
	lui_object_add_to_parent(swtch_change_grph, g_scn_one);
	lui_object_set_position(140, 250, swtch_change_grph);
	lui_switch_set_colors(0xFFFF, LUI_RGB(60, 60, 60), LUI_RGB(171, 0, 117), swtch_change_grph);
	lui_object_set_border_visibility(1, swtch_change_grph);
	lui_switch_set_value(0, swtch_change_grph);
	lui_object_set_callback(my_swtch_change_grph_event_handler, swtch_change_grph);

	
	//create a button
	btn = lui_button_create();
	lui_object_add_to_parent(btn, g_scn_one);
	lui_object_set_position(35, 280, btn);
	lui_object_set_area(80, 30, btn);
	lui_button_set_label_text("Count", btn);
	lui_button_set_label_font(&font_microsoft_16, btn);
	lui_button_set_label_color(ILI_COLOR_WHITE, btn);
	lui_button_set_colors(LUI_RGB(219, 0, 150), LUI_RGB(112, 0, 77), LUI_RGB(171, 0, 117), btn);
	lui_object_set_callback(my_obj_event_handler, btn);

	btn_reset = lui_button_create();
	lui_object_add_to_parent(btn_reset, g_scn_one);
	lui_object_set_position(125, 280, btn_reset);
	lui_object_set_area(80, 30, btn_reset);
	lui_button_set_label_text("Reset", btn_reset);
	lui_button_set_label_font(&font_microsoft_16, btn_reset);
	lui_button_set_label_color(ILI_COLOR_WHITE, btn_reset);
	lui_button_set_colors(ILI_COLOR_BLACK, LUI_RGB(40, 40, 40), LUI_RGB(80, 80, 80), btn_reset);
	lui_object_set_border_visibility(1, btn_reset);
	lui_object_set_border_color(ILI_COLOR_PINK, btn_reset);
	lui_object_set_callback(my_obj_event_handler, btn_reset);


	// -------------------------------------------
	// create popup group
	g_popup_panel = lui_panel_create();
	popup_label = lui_label_create();
	lui_obj_t *popup_btn = lui_button_create();
	lui_object_add_to_parent(popup_label, g_popup_panel);
	lui_object_add_to_parent(popup_btn, g_popup_panel);
	lui_label_set_text("This is a popup", popup_label);
	lui_object_set_bg_color(LUI_RGB(50, 50, 50), popup_label);

	lui_object_set_position(40, 60, popup_btn);
	lui_object_set_position(5, 2, popup_label);
	lui_object_set_position(40, 130, g_popup_panel);

	lui_object_set_area(140, 18, popup_label);
	lui_object_set_area(80, 28, popup_btn);
	lui_object_set_area(160, 100, g_popup_panel);

	lui_object_set_border_visibility(1, g_popup_panel);
	lui_object_set_bg_color(LUI_RGB(50, 50, 50), g_popup_panel);
	
	lui_button_set_label_text("OK", popup_btn);
	lui_button_set_label_font(&font_microsoft_16, popup_btn);
	lui_button_set_label_color(ILI_COLOR_WHITE, popup_btn);
	lui_button_set_colors(LUI_RGB(219, 0, 150), LUI_RGB(112, 0, 77), LUI_RGB(171, 0, 117), popup_btn);
	lui_object_set_callback(my_popupbtn_event_handler, popup_btn);

	//set the active scene. This scene will be rendered iby the lui_update()
	lui_scene_set_active(g_scn_one);

	/*###################################################################################
	 #		Glut related functions for drawing and input handling						#
	 ###################################################################################*/
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

// prepare a set of data to be plotted
void prepare_chart_data_1()
{
	for (int i = 0; i < 20; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = i*i;	//y value of i'th element
	}
	for (int i = 20; i < 50; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = (i / sin(i*i));	//y value of i'th element
	}
}

void prepare_chart_data_2()
{
	for (int i = 0; i < 20; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = i*i;	//y value of i'th element
	}
	for (int i = 20; i < 50; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = (i / sin(i));	//y value of i'th element
	}
}