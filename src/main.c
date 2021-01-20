
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <inttypes.h>
#include "lame_ui.h"
#include "font_microsoft_16.h"
#include "font_ubuntu_48.h"
#include <time.h>

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
#define HOR_RES 240
#define VERT_RES 320

int g = 90;
tLuiTouchInputData g_input;
tLuiScene g_scn_one;

void my_button_event_handler(uint8_t event);
void my_input_read_opengl (tLuiTouchInputData *input);
void gl_init();
void my_set_pixel_opengl (uint16_t x, uint16_t y, uint16_t color);
void my_set_pixel_area_opengl (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void myDisplay();



//tLuiLabel lbl1;
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
    gluOrtho2D(-25, HOR_RES+25, VERT_RES+25, -25);

    // Clear everything
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
}



// Read input from an input device
// It should take device position (x, y) and a button's status
void my_input_read_opengl (tLuiTouchInputData *input)
{
	//glutPassiveMotionFunc(myMouse);
	input->is_pressed = g_input.is_pressed;
	input->y = g_input.y;
	input->x = g_input.x;
}


void my_button_event_handler(uint8_t event)
{
	char event_name[30] = {0};
	//printf("\nState Change occured. Event ID: %d", event);
	switch (event)
	{
	case LUI_EVENT_NONE:
		strcat(event_name, "EVENT NONE");
		break;
	case LUI_EVENT_SELECTED:
		strcat(event_name, "EVENT SELECTED");
		break;
	case LUI_EVENT_SELECTION_LOST:
		strcat(event_name, "EVENT SELECTION LOST");
		break;
	case LUI_EVENT_PRESSED:
		strcat(event_name, "EVENT PRESSED");
		break;
	case LUI_EVENT_RELEASED:
		strcat(event_name, "EVENT RELEASED");
		break;		
	case LUI_EVENT_ENTERED:
		strcat(event_name, "EVENT ENTERED");
		break;
	default:
		break;
	}
	//printf(" Event Name: %s", event_name);
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
	GLfloat glf_red = (GLfloat)uint_red_5 / 32.0;
	GLfloat glf_green = (GLfloat)uint_green_6 / 64.0;
	GLfloat glf_blue = (GLfloat)uint_blue_5 / 32.0;

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
	GLfloat glf_red = (GLfloat)uint_red_5 / 32.0;
	GLfloat glf_green = (GLfloat)uint_green_6 / 64.0;
	GLfloat glf_blue = (GLfloat)uint_blue_5 / 32.0;

	// Set the color
	glColor3f(glf_red, glf_green, glf_blue);

	// Prepare the display buffer
	// After the loop ends, the prepared buffer is flushed
	for (temp_y = p_y; temp_y < p_y + p_height; temp_y++)
	{
		for (temp_x = p_x; temp_x < p_x + p_width; temp_x++)
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
    // 50 is the padding size
    glutInitWindowSize(HOR_RES + 50, VERT_RES + 50);
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
	tLuiScene *g_scn_one = lui_scene_create();
	lui_scene_set_bg_color(ILI_COLOR_BLUE, g_scn_one);
	lui_scene_set_font(&font_microsoft_16, g_scn_one);
	//g_scn_one = lui_scene_destroy(g_scn_one);	// For destroying, assigning the return value is mandatory

	
	//----------------------------------------------------------
	//create a label
	tLuiLabel *lbl1 = lui_label_create();
	lui_label_add_to_scene(lbl1, g_scn_one);
	lui_label_set_text("This is a label hehe", lbl1);
	lui_label_set_position(0, 0, lbl1);
	lui_label_set_area(100, 50, lbl1);
	lui_label_set_colors(ILI_COLOR_GREEN, ILI_COLOR_BLACK, lbl1);
	lui_label_set_font(&font_microsoft_16, lbl1);
	//lbl1 = lui_label_destroy(lbl1);
	lui_label_remove_from_scene(lbl1);
	lui_label_add_to_scene(lbl1, g_scn_one);

	//----------------------------------------------------------
	//Prepare some data for a graph
	double points[10][2];
	for (int i = 0; i < 10; i++)
	{
		points[i][0] = i;		//x value of i'th element
		points[i][1] = i*i*i;	//y value of i'th element
	}

	//----------------------------------------------------------
	//create a line chart with above data
	// tLuiLineChart grph = lui_linechart_create();
	// lui_linechart_add_to_scene(&grph, &g_scn_one);
	// lui_linechart_set_data_source((double *)&points, 10, &grph);
	// lui_linechart_set_position(60, 50, &grph);
	// lui_linechart_set_area(110, 200, &grph);
	// lui_linechart_set_grid(ILI_COLOR_BLACK, 4, 3, &grph);
	// lui_linechart_set_colors(ILI_COLOR_RED, ILI_COLOR_BLUE, &grph);



	//----------------------------------------------------------
	//create a button
	// tLuiButton btn = lui_button_create();
	// //lui_button_add_to_scene(&btn, &g_scn_one);
	// lui_button_set_position(80, 280, &btn);
	// lui_button_set_area(80, 30, &btn);
	// lui_button_set_label_text("Button", &btn);
	// lui_button_set_label_font(&font_microsoft_16, &btn);
	// lui_button_set_label_color(ILI_COLOR_MAROON, &btn);
	// lui_button_set_colors(ILI_COLOR_CYAN, ILI_COLOR_GREEN, ILI_COLOR_YELLOW, &btn);
	// lui_button_set_event_cb(my_button_event_handler, &btn);

	tLuiSwitch *swtch = lui_switch_create();
	lui_switch_add_to_scene(swtch, g_scn_one);
	lui_switch_set_position(50, 50, swtch);
	// lui_switch_remove_from_scene(swtch);
	// lui_switch_add_to_scene(swtch, &g_scn_one);
	swtch = lui_switch_destroy(swtch); // For destroying, assigning the return value is mandatory
	lui_switch_add_to_scene(swtch, g_scn_one);

	clock_t start; 
	start = clock();
	//Finally, render the scene
	g_input.x = 80;
	g_input.y = 282;
	g_input.is_pressed = 1;
	uint16_t orig_x = g_input.x;
	// while(1)
	// {
	// 	if (clock() - start > (clock_t)500000)
	// 	{
	// 		start = clock();
			
	 		lui_scene_render(g_scn_one);

	// 		g_input.is_pressed = (start >> 18) & 1;
	// 		g_input.x = (start >> 19) & 1 ? orig_x : 10; // if random == 1, position is on button else position is 10
	// 	}
		
	// }

	// glutPassiveMotionFunc(myMouse);
	// glutDisplayFunc(myDisplay);
	 glutMainLoop();

    return 0;
}


void myDisplay()
{
	lui_scene_render(&g_scn_one);
	// Flush drawing commands
    glFlush();
}