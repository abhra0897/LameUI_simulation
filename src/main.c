
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

tLuiTouchInputData g_input;	// Made input global so it can be changed by glut callback functions
tLuiScene *g_scn_one;		// Made scene global so glut callbacks can access it
tLuiLabel *g_lbl1;			// Made a label global so LameUI button callback can modify its text
tLuiLabel *g_lbl3;			// For same reason as above
char event_name[30] = {0}; 	// Made the text global so LameUI knows its address. Latter I'll try to make set_text functions scope independent
char btn_press_cnt[4] = {0};// For same reason as above
uint16_t btn_press_cnt_int = 0;

// Callback functions for LameUI ---------------------------------------------------
void my_button_event_handler(uint8_t event);
void my_switch_event_handler(uint8_t event, uint8_t value);
void my_input_read_opengl (tLuiTouchInputData *input);
void my_set_pixel_opengl (uint16_t x, uint16_t y, uint16_t color);
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
void my_input_read_opengl (tLuiTouchInputData *input)
{
	input->is_pressed = g_input.is_pressed;
	input->y = g_input.y;
	input->x = g_input.x;

	//printf("[DEBUG] void my_input_read_opengl(). x:%d\ty: %d\n", input->y, input->x);
}

// Event handler for button, called back by LameUI
void my_button_event_handler(uint8_t event)
{
	//printf("\nState Change occured. Event ID: %d", event);
	//memset(event_name, 0, strlen(event_name));
	switch (event)
	{
	case LUI_EVENT_NONE:
		strcpy(event_name, "EVENT NONE");
		break;
	case LUI_EVENT_SELECTED:
		strcpy(event_name, "EVENT SELECTED");
		break;
	case LUI_EVENT_SELECTION_LOST:
		strcpy(event_name, "EVENT SELECTION LOST");
		break;
	case LUI_EVENT_PRESSED:
		strcpy(event_name, "EVENT PRESSED");
		sprintf(btn_press_cnt, "%d", ++btn_press_cnt_int);
		lui_label_set_text(btn_press_cnt, g_lbl3);
		break;
	case LUI_EVENT_RELEASED:
		strcpy(event_name, "EVENT RELEASED");
		break;		
	case LUI_EVENT_ENTERED:
		strcpy(event_name, "EVENT ENTERED");
		break;
	default:
		break;
	}
	lui_label_set_text(event_name, g_lbl1);
}

// Event handler for switch. Called back by LameUI
void my_switch_event_handler(uint8_t event, uint8_t value)
{	
	// change background color
	if (event == LUI_EVENT_VALUE_CHANGED)
	{
		if (value == 1)
		{
			lui_scene_set_bg_color(0, g_scn_one);
		}
		else
		{
			lui_scene_set_bg_color(LUI_RGB(192, 183, 230), g_scn_one);
		}
	}
	
}


// Set an individual pixel at x, y position with rgp565 color
// This method is not fast if called continuously to fill an area..
// To fill an area with a specific color, use the set_pixel_area() func.
void my_set_pixel_opengl (uint16_t p_x, uint16_t p_y, uint16_t p_color)
{
	GLint x = (GLint)p_x;
	GLint y = (GLint)p_y;

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

	glBegin(GL_POINTS);

	// glVertex2i just draws a point on specified co-ordinate
	glVertex2i(x, y);
	glEnd();
	// Flush the buffer and display its content
	glFlush();
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

		}
	}
	// Flush the buffer and display its content
	glFlush();
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
	tLuiDispDrv *my_display_driver = lui_dispdrv_create();
	lui_dispdrv_register(my_display_driver);
	lui_dispdrv_set_resolution(240, 320, my_display_driver);
	lui_dispdrv_set_draw_pixel_cb(my_set_pixel_opengl, my_display_driver);
	lui_dispdrv_set_draw_pixels_area_cb(my_set_pixel_area_opengl, my_display_driver);
	//my_display_driver = lui_dispdrv_destroy(my_display_driver);

	tLuiTouchInputDev *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_read_opengl, my_input_device);	

	//----------------------------------------------------------
	//create and add scenes
	g_scn_one = lui_scene_create();
	lui_scene_set_bg_color(0, g_scn_one);
	lui_scene_set_font(&font_microsoft_16, g_scn_one);
	//g_scn_one = lui_scene_destroy(g_scn_one);	// For destroying, assigning the return value is mandatory

	
	//----------------------------------------------------------
	//create a label
	g_lbl1 = lui_label_create();
	lui_label_add_to_scene(g_lbl1, g_scn_one);
	lui_label_set_text("--", g_lbl1);
	lui_label_set_position(52, 0, g_lbl1);
	lui_label_set_area(190, 20, g_lbl1);
	lui_label_set_colors(ILI_COLOR_GREEN, LUI_RGB(60, 60, 60), g_lbl1);
	//lui_label_set_font(&font_microsoft_16, g_lbl1);
	//g_lbl1 = lui_label_destroy(g_lbl1);

	tLuiLabel *lbl2 = lui_label_create();
	lui_label_add_to_scene(lbl2, g_scn_one);
	lui_label_set_text("Event:", lbl2);
	lui_label_set_position(0, 0, lbl2);
	lui_label_set_area(50, 20, lbl2);
	lui_label_set_colors(ILI_COLOR_RED, LUI_RGB(60, 60, 60), lbl2);

	g_lbl3 = lui_label_create();
	lui_label_add_to_scene(g_lbl3, g_scn_one);
	lui_label_set_text("0", g_lbl3);
	lui_label_set_position(132, 25, g_lbl3);
	lui_label_set_area(50, 20, g_lbl3);
	lui_label_set_colors(ILI_COLOR_GREEN, LUI_RGB(60, 60, 60), g_lbl3);

	tLuiLabel *lbl4 = lui_label_create();
	lui_label_add_to_scene(lbl4, g_scn_one);
	lui_label_set_text("Click Count:", lbl4);
	lui_label_set_position(0, 25, lbl4);
	lui_label_set_area(130, 20, lbl4);
	lui_label_set_colors(ILI_COLOR_RED, LUI_RGB(60, 60, 60), lbl4);


	//----------------------------------------------------------
	//Prepare some data for a graph
	double points[30][2];
	for (int i = 0; i < 30; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = sin(i);	//y value of i'th element
	}

	//----------------------------------------------------------
	//create a line chart with above data
	tLuiLineChart *grph = lui_linechart_create();
	lui_linechart_add_to_scene(grph, g_scn_one);
	lui_linechart_set_data_source((double *)&points, 30, grph);
	lui_linechart_set_data_auto_scale(0, grph);
	lui_linechart_set_data_range(-2, 2, grph);
	lui_linechart_set_position(60, 50, grph);
	lui_linechart_set_area(110, 200, grph);
	lui_linechart_set_grid(ILI_COLOR_BLACK, 4, 3, grph);
	lui_linechart_set_colors(ILI_COLOR_RED, ILI_COLOR_WHITE, grph);
	lui_linechart_set_border(ILI_COLOR_BLACK, grph);


	//----------------------------------------------------------
	//create a button
	tLuiButton *btn = lui_button_create();
	lui_button_add_to_scene(btn, g_scn_one);
	lui_button_set_position(80, 280, btn);
	lui_button_set_area(80, 30, btn);
	lui_button_set_label_text("Button", btn);
	lui_button_set_label_font(&font_microsoft_16, btn);
	lui_button_set_label_color(ILI_COLOR_MAROON, btn);
	lui_button_set_colors(ILI_COLOR_CYAN, ILI_COLOR_GREEN, ILI_COLOR_YELLOW, btn);
	lui_button_set_event_cb(my_button_event_handler, btn);

	tLuiLabel *lbl_light_mode = lui_label_create();
	lui_label_add_to_scene(lbl_light_mode, g_scn_one);
	lui_label_set_text("Light mode", lbl_light_mode);
	lui_label_set_position(5, 255, lbl_light_mode);
	lui_label_set_area(90, 20, lbl_light_mode);
	lui_label_set_colors(ILI_COLOR_RED, LUI_RGB(60, 60, 60), lbl_light_mode);

	tLuiSwitch *swtch = lui_switch_create();
	lui_switch_add_to_scene(swtch, g_scn_one);
	lui_switch_set_position(100, 255, swtch);
	lui_switch_set_colors(0xFFFF, LUI_RGB(60, 60, 60), ILI_COLOR_YELLOW, swtch);
	//swtch = lui_switch_destroy(swtch); // For destroying, assigning the return value is mandatory
	lui_switch_set_value(1, swtch);
	lui_switch_set_event_cb(my_switch_event_handler, swtch);

	tLuiLabel *lbl_dark_mode = lui_label_create();
	lui_label_add_to_scene(lbl_dark_mode, g_scn_one);
	lui_label_set_text("Dark mode", lbl_dark_mode);
	lui_label_set_position(145, 255, lbl_dark_mode);
	lui_label_set_area(90, 20, lbl_dark_mode);
	lui_label_set_colors(ILI_COLOR_RED, LUI_RGB(60, 60, 60), lbl_dark_mode);


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
	lui_scene_render(g_scn_one);
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