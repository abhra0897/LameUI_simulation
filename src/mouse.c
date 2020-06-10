#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define   screenHeight  500

//initialization
void init( void )
{
  glClearColor( 1.0, 1.0, 1.0, 0.0 );	//get white background color
  glColor3f( 0.0f, 0.0f, 0.0f );	//set drawing color
  glPointSize( 4.0 );			//a dot is 4x4
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0, 500.0, 0.0, 500.0 );
} //init

void display()
{
	printf ("[DEBUG] void display()\n");
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glFlush();
}

void drawDot( int x, int y )
{
	printf ("[DEBUG] void drawDot( int x: %d, int y: %d )\n", x, y);
  glBegin( GL_POINTS );			
    glVertex2i( x, y );		//draw a point
  glEnd();
} //drawDot

void myMouse( int button, int state, int x, int y )
{
	printf ("[DEBUG] void myMouse( int button: %d, int state: %d, int x: %d, int y: %d )\n", button, state, x, y);
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    drawDot( x, screenHeight - y );
  glFlush();				//send all output to screen
}

void myMovedMouse(  int mouseX, int mouseY)
{
	printf ("[DEBUG] void myMovedMouse( int mouseX: %d, int mouseY: %d )\n", mouseX, mouseY);
  GLint x = mouseX;
  GLint y = screenHeight - mouseY;
  GLint brushsize = 6;
  glColor3f( 1.0, 0.0, 0.0 );
  glRecti ( x, y, x + brushsize, y + brushsize );
  glFlush();
} //myMovedMouse

void myKeyboard ( unsigned char key, int mouseX, int mouseY )
{
	printf ("[DEBUG] void myKeyboard( unsigned char key: %d, int mouseX: %d, int mouseY: %d )\n", key, mouseX, mouseY);
  GLint x = mouseX;
  GLint y = screenHeight - mouseY;
  switch( key )
  {
    case 'p':
 	drawDot ( x, y );
	break;
    case 'e':
	exit ( -1 );
    default :
	break;
  }  
}

//mouse_key_main.cpp: main loop of drawing program

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//initialization
void init(void);
void myMouse( int button, int state, int x, int y);
void myMovedMouse(  int mouseX, int mouseY);
void myKeyboard ( unsigned char key, int x, int y );
void display( void );

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, depth buffer.
 */
int main(int argc, char** argv)
{
  glutInit(&argc, argv);	//initialize toolkit
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB );	//set display mode
  glutInitWindowSize(500, 500);		//set window size on screen
  glutInitWindowPosition( 100, 150 ); 	//set window position on screen
  glutCreateWindow(argv[0]);		//open screen widow
  init();
  glutMouseFunc( myMouse );
  glutMotionFunc( myMovedMouse );
  glutKeyboardFunc( myKeyboard );
  glutDisplayFunc( display );
  glutMainLoop();			//go into perpetual loop
  return 0;
}
