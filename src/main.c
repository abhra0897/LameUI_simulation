
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <inttypes.h>
#include "lame_ui.h"
#include "font_microsoft_16.h"
#include "font_ubuntu_48.h"

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

void my_input_read_opengl (tLuiInputDev *input);
void gl_init();
void my_set_pixel_opengl (uint16_t x, uint16_t y, uint16_t color);
void my_set_pixel_area_opengl (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

tLuiLabel lbl1;;
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
}


// Read input from an input device
// It should take device position (x, y) and a button's status
void my_input_read_opengl (tLuiInputDev *input)
{
	/* NOT IMPLEMENTED (SAMPLE BELOW) */

	//input->is_pressed = read_indput_btn_status();
	//input->position.y = read_input_x_pos();
	//input->position.x = read_input_y_pos();
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
	tLuiDispDrv my_display_driver = lui_dispdrv_create();

	//pass which functions will be called back by lame_ui for drawing pixel/pixels
	my_display_driver.draw_pixel_cb = my_set_pixel_opengl;
	my_display_driver.draw_pixels_area_cb = my_set_pixel_area_opengl;
	my_display_driver.read_input_cb = my_input_read_opengl;

	//pass the resolution of your display
	my_display_driver.display_hor_res = 240;
	my_display_driver.display_vert_res = 320;

	//register the display driver with lame_ui
	lui_dispdrv_register(&my_display_driver);

	//----------------------------------------------------------
	//create and add scenes
	tLuiScene scn_one = lui_scene_create();
	lui_scene_add(&scn_one);


	//----------------------------------------------------------
	//Set a bg color and global font for scn_one (scn_one)
	scn_one.font = &font_microsoft_16;
	scn_one.background.color = 0x0006;


	//----------------------------------------------------------
	//create a label
	//tLuiLabel lbl1 = lui_label_create();
	lbl1 = lui_label_create();
	lbl1.text = "This is a LABEL\nBelow is a LINE CHART";
	lbl1.x = 0;
	lbl1.y = 0;
	lbl1.width = 50;
	lbl1.height = 50;
	lbl1.color = ILI_COLOR_GREEN;

	//Add this label to a scene (scene_scnd)
	lui_label_add_to_scene(&lbl1, &scn_one);


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
	tLuiLineChart grph = lui_linechart_create();
	grph.data.source = (double *)points;
	grph.x = 60;
	grph.y = 50;
	grph.data.points = 10;
	grph.width = 110;
	grph.height = 200;
	grph.grid.vert_count = 3;
	grph.grid.hor_count = 4;
	grph.grid.color = 0;
	grph.bg_color = scn_one.background.color;
	grph.line.color = ILI_COLOR_RED;

	// Add this line chart to a scene
	lui_linechart_add_to_scene(&grph, &scn_one);

	//----------------------------------------------------------
	//create a button
	tLuiButton btn = lui_button_create();
	btn.x = 80;
	btn.y = 280;
	btn.width = 80;
	btn.height = 30;
	btn.label.text = "Button";
	btn.label.font = &font_microsoft_16;
	btn.label.color = ILI_COLOR_MAROON;
	btn.color = ILI_COLOR_CYAN;

	// Add the button to a scene
	lui_button_add_to_scene(&btn, &scn_one);

	// Finally, render the scene
	lui_scene_render(&scn_one);
	lui_scene_render(&scn_one);
	//lui_scene_render(&scn_one);

    while(1)
    {
    	//lui_scene_render(&scn_one);
    }

    return 0;
}
