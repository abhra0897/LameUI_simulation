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

lui_obj_t* scene_1;
lui_obj_t* label_1;
lui_obj_t* button_1;

uint32_t g_disp_buffer_counter = 0;
uint32_t g_sidp_buffer_max_size = HOR_RES * VERT_RES;

void my_input_read_cb (lui_touch_input_data_t *input);
void my_draw_pixels_cb (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
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
	gluOrtho2D(-1, HOR_RES - 1, VERT_RES - 1, -1);

	// Clear everything
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
}


// Read input from an input device
// It should take device position (x, y) and a button's status
void my_input_read_cb (lui_touch_input_data_t *input)
{
	input->is_pressed = g_input.is_pressed;
	input->y = g_input.y;
	input->x = g_input.x;

	//printf("[DEBUG] void my_input_read_cb(). x:%d\ty: %d\n", input->y, input->x);
}

// Flush the current buffer when this callback is called by LameUI.
// Using this, no need to flush every time set_pixel_cb is called. Rather buffer it and flush all together
void my_render_cmplt_cb()
{
	glFlush();
	g_disp_buffer_counter = 0; //reset the counter
}


// Draw an area of pixels in one go.
// This is faster because the opengl buffer is filled using a loop,
// and then the preapred buffer is fflushed in one go
void my_draw_pixels_cb (uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height, uint16_t p_color)
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
	lui_dispdrv_set_draw_pixels_area_cb(my_display_driver, my_draw_pixels_cb);
	lui_dispdrv_set_render_complete_cb(my_display_driver, my_render_cmplt_cb);
	//my_display_driver = lui_dispdrv_destroy(my_display_driver);

	lui_touch_input_dev_t *my_input_device = lui_touch_inputdev_create();
	lui_touch_inputdev_register(my_input_device);
	lui_touch_inputdev_set_read_input_cb(my_input_device, my_input_read_cb);	

	//----------------------------------------------------------
	//create and add scenes
	scene_1 = lui_scene_create();
	lui_scene_set_active(scene_1);

    // add a button to go to next scene (g_scene_two)
	label_1 = lui_label_create();
	lui_object_add_to_parent(label_1, scene_1);
	lui_object_set_position(label_1, 5, 50);
	lui_label_set_text(label_1, "This is Label 1. Below is Button 1");

	// add a button to go to next scene (g_scene_two)
	button_1 = lui_button_create();
	lui_object_add_to_parent(button_1, scene_1);
	lui_object_set_area(button_1, 110, 40);
	lui_object_set_position(button_1, 65, 100);
	lui_button_set_label_text(button_1, "Button 1");
	//lui_object_set_callback(g_btn_nxt_scn, scn_change_event_handler);

  
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
