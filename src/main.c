#include <stdlib.h>
#if defined(__APPLE__) && !defined(USE_FREEGLUT)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <math.h>
#include "scene.h"
#include "math_utils.h"
#include "lighting.h"
#include "textures.h"

static void createMenu(void);

/* GLUT application state and input callbacks. */

enum { BUTTON_PRESSED = 1, BUTTON_NOT_PRESSED = 0, INSIDE = 1, OUTSIDE = 0 };

static int sceneDisplayList;
static int leftButtonPressed = BUTTON_NOT_PRESSED;
static int panButtonPressed = BUTTON_NOT_PRESSED;
static int middleButtonPressed = BUTTON_NOT_PRESSED;
static Point3D startPoint, endPoint;
static float dx, dy, dz = -4.0f;
static float w, h;
static float Z;
static float R2;
static float matrix[16];
static float currentMatrix[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};
static float matrixProduct[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

static int backgroundIndex = 0;
typedef enum {BG_MOON,BG_MARS,BG_VENUS,COUNTRY_USA,COUNTRY_SPAIN,COUNTRY_USSR} MenuOption;

static void onMouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        leftButtonPressed = state == GLUT_DOWN;
        startPoint = endPoint = makePoint3D(x - w / 2, h / 2 - y, 0.0f);
        if (state == GLUT_UP) copyMatrix(currentMatrix, matrixProduct);
    } else if (button == GLUT_MIDDLE_BUTTON) {
        panButtonPressed = state == GLUT_DOWN &&
                           (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
        middleButtonPressed = state == GLUT_DOWN && !panButtonPressed;
        startPoint = makePoint3D(x, y, 0.0f);
    }
}

static int isInsideWindow(int x, int y) {
  int inside = INSIDE;

  if(x < 0 || y < 0 || x > w || y > h) inside = OUTSIDE;

  return (inside);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void onMouseMotion(int x, int y) {
  if(leftButtonPressed == BUTTON_PRESSED && isInsideWindow(x,y)) {
    endPoint = makePoint3D(x-(w/2), (h/2)-y, 0.0f);
    if(pointsDiffer(startPoint, endPoint) == DIFFERENT) {
      computeTrackballMatrix(endPoint, startPoint, matrix, R2);
      multiplyMatrices(matrix, currentMatrix, matrixProduct);
      glutPostRedisplay();
    }
  }
  else if(panButtonPressed == BUTTON_PRESSED && isInsideWindow(x, y)) {
    endPoint = makePoint3D(x, y, 0.0f);
    dx += (float)(endPoint.x - startPoint.x)/200.0f;
    dy += (float)(startPoint.y - endPoint.y)/200.0f;
    startPoint = endPoint;
    glutPostRedisplay();
  }
  else if(middleButtonPressed == BUTTON_PRESSED && isInsideWindow(x,y)) {
    endPoint = makePoint3D(x, y, 0.0f);
    dz -= (float)(startPoint.y - endPoint.y)/200.0f;
    startPoint = endPoint;
    glutPostRedisplay();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void onKeyboard (unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
	switch (key)
	{
		case 27 :
			exit (0);
			break;
	}
}

/*--------------------------------------------------------------------------
   Clears the window and sets draw color and line width
  --------------------------------------------------------------------------*/
static void display(void)
{
     
    float colors[3][3] = {
          {0.0f,0.0f,0.0f},  // 0 - Black
          {0.70f,0.43f,0.21f}, // 1 - Red (Martian sky)
          {0.93f,0.77f,0.51f}}; // 2 - Yellow (Venus atmosphere)
     
	glClearColor (colors[backgroundIndex][0],colors[backgroundIndex][1],colors[backgroundIndex][2], 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity ();

	glPushMatrix();
	glTranslatef(dx, dy, dz);
	glMultMatrixf(matrixProduct);

	glColor3f(0.0f, 0.0f, 0.0f);
	glCallList(sceneDisplayList);
	glPopMatrix();
	glutSwapBuffers ();
}


/*--------------------------------------------------------------------------
   Binds keyboard event handlers
  --------------------------------------------------------------------------*/
static void initKeyboard (void) {
  glutKeyboardFunc (onKeyboard);
}


/*--------------------------------------------------------------------------
   Sets up the view parameters...
  --------------------------------------------------------------------------*/
static void initView (void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, (GLdouble)w/h, 0.1, 10.0);
	//glOrtho(-2.5f, 2.5f, -2.5f, 2.5f, -4.0f, 4.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace (GL_CCW);
	glCullFace (GL_BACK);
	glEnable (GL_NORMALIZE);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	
}


/*--------------------------------------------------------------------------
   Handles window resize
  --------------------------------------------------------------------------*/
static void reshape (int width, int height) {

	w = width > 0 ? width : 1;
	h = height > 0 ? height : 1;
	glViewport(0, 0, (int)w, (int)h);
	if(w > h) Z = w;
	else Z = h;
	R2 = (w*w+h*h+Z*Z)/4;
	initView ();
}


/*--------------------------------------------------------------------------
   Creates the window
  --------------------------------------------------------------------------*/
static void initWindow (int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (700, 700);
  glutInitWindowPosition (200, 30);
  glutCreateWindow (argv[0]);
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
}

static void initMouse(void) {
  glutMouseFunc(onMouseButton); // Mouse button press events
  glutMotionFunc(onMouseMotion);
}

static void initDisplayLists(void) {
  sceneDisplayList = glGenLists(1);
  if (sceneDisplayList == 0) {
    fprintf(stderr, "Could not allocate the scene display list.\n");
    exit(EXIT_FAILURE);
  }
  glNewList(sceneDisplayList, GL_COMPILE);
  glColor3f(0.0f, 0.0f, 0.0f);
  buildScene();
  glEndList();
}


/*--------------------------------------------------------------------------
   Runs the application
  --------------------------------------------------------------------------*/
static void runApplication (int argc, char **argv)
{
  initWindow (argc, argv);
  initKeyboard ();
  initMouse ();
  reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  if (!initTextures()) exit(EXIT_FAILURE);
  initDisplayLists();
  initLights();
  createMenu();
  glutMainLoop();
}

static void onMenu(int option){
     switch(option){
           case BG_MOON:
                if (loadTexture("assets/textures/moon.tga", TEXTURE_GROUND)) backgroundIndex = 0;
                break;
		   
           case BG_MARS:
                if (loadTexture("assets/textures/mars.tga", TEXTURE_GROUND)) backgroundIndex = 1;
                break;
		   
           case BG_VENUS:
                if (loadTexture("assets/textures/venus.tga", TEXTURE_GROUND)) backgroundIndex = 2;
                break;
		   
		   case COUNTRY_USA:
			    loadTexture("assets/textures/usa.tga", TEXTURE_FLAG);
		   		break;
		   
		   case COUNTRY_SPAIN:
		   	    loadTexture("assets/textures/spain.tga", TEXTURE_FLAG);
		   		break;
		   		
           case COUNTRY_USSR:
			    loadTexture("assets/textures/ussr.tga", TEXTURE_FLAG);
		   		break;
     }
     glutPostRedisplay();
}

/* Options menu */
static void createMenu(void){
     int backgroundMenu,countryMenu;
     
     backgroundMenu = glutCreateMenu(onMenu);
     glutAddMenuEntry("Moon",BG_MOON);
     glutAddMenuEntry("Mars",BG_MARS);
     glutAddMenuEntry("Venus",BG_VENUS);
     
	 countryMenu = glutCreateMenu(onMenu);
     glutAddMenuEntry("USA",COUNTRY_USA);
     glutAddMenuEntry("SPAIN",COUNTRY_SPAIN);
     glutAddMenuEntry("USSR",COUNTRY_USSR);
	
     glutCreateMenu(onMenu);
	 glutAddSubMenu("Choose Country",countryMenu);
	 glutAddSubMenu("Choose Location",backgroundMenu);
	 
     glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main (int argc, char** argv)
{
	runApplication(argc, argv);
	return EXIT_SUCCESS;
}
