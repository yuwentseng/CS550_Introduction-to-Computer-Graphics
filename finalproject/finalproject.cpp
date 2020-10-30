#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include "bmptotexture.cpp"

//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.

// title of these windows:

const char *WINDOWTITLE = { "Final Project -- YuWenTseng" };
const char *GLUITITLE   = { "User Interface Window" };

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b
#define MS_PER_CYCLE 100000

// initial window size:

const int INIT_WINDOW_SIZE = { 600 };

// size of the box:

const float BOXSIZE = { 2.f };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.025f };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};
// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		Projection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

int ChangeView;
int DisoButton;
int SnowButton;
int TripButton;
float ZMove = 0.;
float XMove = 0.;
float YMove = 0.;
int xDisLight1Dir = 0;
int yDisLight1Dir = 0;
int zDisLight1Dir = 0;
int xDisLight2Dir = 0;
int yDisLight2Dir = 0;
int zDisLight2Dir = 0;
int xDisLight3Dir = 0;
int yDisLight3Dir = 0;
int zDisLight3Dir = 0;
int xDisLight4Dir = 0;
int yDisLight4Dir = 0;
int zDisLight4Dir = 0;
int DisParaButton = 0;

GLuint SunList;
GLuint EarthList;
GLuint MoonList;
GLuint WoodList;
GLuint SkyList;
GLuint Suntexture;
GLuint Earthtexture;
GLuint Moontexture;
GLuint Simpsons;
GLuint Skytexture;
GLuint Wood;

int EarthOrigin;
int MoonOrigin;
float White[] = { 1., 1., 1., 1. };
bool Freeze = 0;
bool Light0On = 1;
bool Light1On = 1;
float angle = 0;
float Time;
int snownum;
int snowmove[50][30][2];
int snowfall[50][30];
float snowposition[50][30][3];
int snowgen = 0;
bool  Frozen;

struct point {
	float x, y, z;		
	float nx, ny, nz;	
	float s, t;		
};

int	   NumLngs, NumLats;
struct point *	Pts;
static GLint fogMode;

struct point * PtsPointer(int lat, int lng)
{
	if (lat < 0){
		lat += (NumLats - 1);
	}	
	else if (lat > NumLats - 1){
		lat -= (NumLats - 1);
	}	
	if (lng < 0){
		lng += (NumLngs - 1);
	}	
	else if (lng > NumLngs - 1){
		lng -= (NumLngs - 1);
	}	
	return &Pts[NumLngs*lat + lng];
}

// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void	Axes( float );
void	HsvRgb( float[3], float [3] );

void DrawPoint(struct point *p)
{
	glNormal3f(p->nx, p->ny, p->nz);
	glTexCoord2f(p->s, p->t);
	glVertex3f(p->x, p->y, p->z);
}

void MjbSphere(float radius, int slices, int stacks)
{
	struct point top, bottom;		
	struct point *p;

	NumLngs = slices;
	NumLats = stacks;

	if (NumLngs < 3){
		NumLngs = 3;
	}

	if (NumLats < 3){
		NumLats = 3;
	}

	Pts = new struct point[NumLngs * NumLats];

	// fill the Pts structure:
	int ilat = 0;
	while(ilat < NumLats){
		float lat = -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1);
		float xz = cos(lat);
		float y = sin(lat);
		int ilng = 0;
		while(ilng < NumLngs){
			float lng = -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1);
			float x = xz * cos(lng);
			float z = -xz * sin(lng);
			p = PtsPointer(ilat, ilng);
			p->x = radius * x;
			p->y = radius * y;
			p->z = radius * z;
			p->nx = x;
			p->ny = y;
			p->nz = z;
			p->s = (lng + M_PI) / (2.*M_PI);
			p->t = (lat + M_PI / 2.) / M_PI;
			ilng++;
		}
		ilat ++;
	}
	top.x = 0.;		top.y = radius;		top.z = 0.;
	top.nx = 0.;	top.ny = 1.;		top.nz = 0.;
	top.s = 0.;		top.t = 1.;

	bottom.x = 0.;		bottom.y = -radius;	bottom.z = 0.;
	bottom.nx = 0.;	bottom.ny = -1.;		bottom.nz = 0.;
	bottom.s = 0.;		bottom.t = 0.;

	glBegin(GL_QUADS);
	int ilng = 0;
	while(ilng < NumLngs - 1){
		p = PtsPointer(NumLats - 1, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng);
		DrawPoint(p);

		p = PtsPointer(NumLats - 2, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(NumLats - 1, ilng + 1);
		DrawPoint(p);
		ilng++;
	}
	glEnd();

	glBegin(GL_QUADS);
	ilng = 0;
	while(ilng < NumLngs - 1){
		p = PtsPointer(0, ilng);
		DrawPoint(p);

		p = PtsPointer(0, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng + 1);
		DrawPoint(p);

		p = PtsPointer(1, ilng);
		DrawPoint(p);
		ilng++;
	}
	glEnd();

	glBegin(GL_QUADS);
	ilat = 2;
	while(ilat < NumLats - 1){
		int ilng = 0;
		while(ilng < NumLngs - 1){
			p = PtsPointer(ilat - 1, ilng);
			DrawPoint(p);

			p = PtsPointer(ilat - 1, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng + 1);
			DrawPoint(p);

			p = PtsPointer(ilat, ilng);
			DrawPoint(p);
			ilng++;
		}
		ilat++;
	}
	glEnd();

	delete[] Pts;
	Pts = NULL;
}

void CircleInsideBall(float r)
{
	glColor3f(1., 1., 1.);
	glBegin(GL_LINE_STRIP);
	int i = 1;
	while(i <= 100){
		glVertex3f(r*cos((2 * M_PI*i) / 100), 0., r*sin((2 * M_PI*i) / 100));
		i++;
	}
	glEnd();
}

void DrawStage(float r1, float r2, float h)
{
	float angle = 2* M_PI / 500;
	float s0 = 1. / 50;
	point point1, point2, point3, point4;
	float nx, ny, nz;
	glBegin(GL_QUADS);
	int i = 0;
	while(i<500){
		point1.x = r1*cos((float)i*angle);
		point1.y = h / 2;
		point1.z = r1*sin((float)i*angle);
		point1.s = (float)i * s0;
		point1.t = 1;
		point2.x = r1*cos((float)(i+1)*angle);
		point2.y = h / 2;
		point2.z = r1*sin((float)(i+1)*angle);
		point2.s = ((float)i + 1.) * s0;
		point2.t = 1;
		point3.x = r2*cos((float)(i+1)*angle);
		point3.y = - h / 2;
		point3.z = r2*sin((float)(i+1)*angle);
		point3.s = (float)(i+1) * s0;
		point3.t = 0;
		point4.x = r2*cos((float)i*angle);
		point4.y = - h / 2;
		point4.z = r2*sin((float)i*angle);
		point4.s = (float)i * s0;
		point4.t = 0;
		nx = ((point2.y - point1.y)*(point3.z - point1.z) - (point2.z - point1.z)*(point3.y - point1.y));
		ny = ((point2.z - point1.z)*(point3.x - point1.x) - (point2.x - point1.x)*(point3.z - point1.z));
		nz = ((point2.x - point1.x)*(point3.y - point1.y) - (point2.y - point1.y)*(point3.x - point1.x));

		glNormal3f(nx, ny, nz);
		glTexCoord2f(point1.s, point1.t);
		glVertex3f(point1.x, point1.y, point1.z);

		glTexCoord2f(point2.s, point2.t);
		glVertex3f(point2.x, point2.y, point2.z);

		glTexCoord2f(point3.s, point3.t);
		glVertex3f(point3.x, point3.y, point3.z);

		glTexCoord2f(point4.s, point4.t);
		glVertex3f(point4.x, point4.y, point4.z);
		i++;
	}
	glEnd();

}

int random_gen(int range_bottom, int range_top)
{
	int number;
	int temp;
	temp = range_top - range_bottom;
	number = rand() % temp + range_bottom;
	return number;
}

float cude_cal(float num)
{
	float res = pow(num, 3);
	return res;
}

// utility to create an array from 3 separate values:
float *
Array3(float a, float b, float c)
{
	static float array[4];
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from 4 separate values:
float * Array4(float r, float g, float b, float a)
{
	static float array[4];
	array[0] = r;
	array[1] = g;
	array[2] = b;
	array[3] = a;
	return array;
}

// utility to create an array from a multiplier and an array:
float *
MulArray3(float factor, float array0[3])
{
	static float array[4];
	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}

void
SetMaterial(float r, float g, float b, float shininess)
{
	glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(.4f, White));
	glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., White));
	glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
	glMaterialf(GL_BACK, GL_SHININESS, 2.f);
	glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
	glMaterialfv(GL_FRONT, GL_AMBIENT, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Array3(r, g, b));
	glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(.8f, White));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void
SetPointLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

void
SetSpotLight(int ilight, float x, float y, float z, float xdir, float ydir, float zdir, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array3(x, y, z));
	glLightfv(ilight, GL_SPOT_DIRECTION, Array3(xdir, ydir, zdir));
	glLightf(ilight, GL_SPOT_EXPONENT, 1.);
	glLightf(ilight, GL_SPOT_CUTOFF, 30.);
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

void
SetParallelLight(int ilight, float x, float y, float z, float r, float g, float b)
{
	glLightfv(ilight, GL_POSITION, Array4(x, y, z, 0.));
	glLightfv(ilight, GL_AMBIENT, Array3(0., 0., 0.));
	glLightfv(ilight, GL_DIFFUSE, Array3(r, g, b));
	glLightfv(ilight, GL_SPECULAR, Array3(r, g, b));
	glLightf(ilight, GL_CONSTANT_ATTENUATION, 1.);
	glLightf(ilight, GL_LINEAR_ATTENUATION, 0.);
	glLightf(ilight, GL_QUADRATIC_ATTENUATION, 0.);
	glEnable(ilight);
}

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display structures that will not change:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// this is here to make the compiler happy:

	return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:
	int ms = glutGet(GLUT_ELAPSED_TIME);
	int msp = ms;
	ms %= MS_PER_CYCLE;
	Time = (float)ms / (float)(MS_PER_CYCLE - 1);

	if ((ms%16)%4 == 0)
	{
		xDisLight1Dir = random_gen(-30, 30);
		yDisLight1Dir = random_gen(0, 30);
		zDisLight1Dir = random_gen(-30, 30);
		xDisLight2Dir = random_gen(-30, 30);
		yDisLight2Dir = random_gen(0, 30);
		zDisLight2Dir = random_gen(-30, 30);
		xDisLight3Dir = random_gen(-30, 30);
		yDisLight3Dir = random_gen(0, 30);
		zDisLight3Dir = random_gen(-30, 30);
		xDisLight4Dir = random_gen(-30, 30);
		yDisLight4Dir = random_gen(0, 30);
		zDisLight4Dir = random_gen(-30, 30);
	}

	if (ms%20 <= 10)
	{
		DisParaButton = 1;
	}
	else
	{
		DisParaButton = 0;
	}

	if (ms%20 == 0)
	{
		snownum = random_gen(15, 30);
		int i = 0;
		while(i <= snownum){
			snowmove[snowgen][i][0] = random_gen(-2, 2);
			snowmove[snowgen][i][1] = random_gen(-2, 2);
			snowposition[snowgen][i][0] = random_gen(-50, 50);
			snowposition[snowgen][i][1] = random_gen(30, 40);
			snowposition[snowgen][i][2] = random_gen(-50, 50);
			snowfall[snowgen][i] = random_gen(-2.5, -0.5);
			i++;
		}
		snowgen++;
		if (snowgen >= 50){
			snowgen = 0;
		}
			
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}

	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );

	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	// specify shading to be flat:

	//glShadeModel( GL_FLAT );
	glShadeModel(GL_SMOOTH);

	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );

	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( Projection == ORTHO ){
		glOrtho( -3., 3.,     -3., 3.,     0.1, 2000. );
	}
	else
		gluPerspective( 90., 1.,	0.1, 2000. );

	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	if (TripButton == 0)
	{
		if (ChangeView == 0)
		{
			gluLookAt(0., 0., 100., 0., 0., 0., 0., 1., 0.);
		}
		else if (ChangeView == 1)
		{
			gluLookAt(0., 0., 41.5, 0., 0., 0., 0., 1., 0.);
		}
	}
	else if (TripButton == 1)
	{
		gluLookAt((6.25 + +(sin(2 * M_PI*Time) + 1) / 2 * 28.)*cos(2 * M_PI*Time), 4., (6.25 + (sin(2 * M_PI*Time) + 1) / 2 * 28.)*sin(2 * M_PI*Time), (6.25 + (sin(2 * M_PI*Time) + 1) / 2 * 28.) * cos(2 * M_PI*Time + M_PI / 3), 2., (6.25 + (sin(2 * M_PI*Time) + 1) / 2 * 28.) * sin(2 * M_PI*Time + M_PI / 3), sin(2 * M_PI*Time)*0.3, 1., 0.);
		ChangeView = 1;
	}
	
	SetParallelLight(GL_LIGHT1, 1., 1., 0.75, 1., 1., 1.);

	if (ChangeView == 1)
	{
		glTranslatef((GLfloat)XMove, (GLfloat)YMove, (GLfloat)ZMove);
	}

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	if( DepthCueOn != 0 )
	{
		glEnable( GL_FOG ); 
		{
			fogMode = GL_EXP;
			GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};
			glFogi( GL_FOG_MODE, fogMode );
			glFogfv( GL_FOG_COLOR, fogColor );
			glFogf( GL_FOG_DENSITY, 0.35 );
			glHint (GL_FOG_HINT, GL_DONT_CARE);
			glFogf( GL_FOG_START, 1.0 );
			glFogf( GL_FOG_END, 5.0 );
		}
		glClearColor(0.5, 0.5, 0.5, 1.0);
	}
	else
	{
		glDisable( GL_FOG );
	}

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}

	glEnable( GL_NORMALIZE );
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(.2, White));
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

	SetPointLight(GL_LIGHT0, 0., 0., 0., 1., 1., 1.);

	SetSpotLight(GL_LIGHT2, 10., -sqrt(42.5*42.5 - 27.*27.) - 7.5, 10., xDisLight1Dir, yDisLight1Dir, zDisLight1Dir, 1., 0., 0.);
	SetSpotLight(GL_LIGHT3, 10., -sqrt(42.5*42.5 - 27.*27.) - 7.5, -10., xDisLight2Dir, yDisLight2Dir, zDisLight2Dir, 0., 1., 0.);
	SetSpotLight(GL_LIGHT4, -10., -sqrt(42.5*42.5 - 27.*27.) - 7.5, -10., xDisLight3Dir, yDisLight3Dir, zDisLight3Dir, 0., 0., 1.);
	SetSpotLight(GL_LIGHT5, -10., -sqrt(42.5*42.5 - 27.*27.) - 7.5, 10., xDisLight4Dir, yDisLight4Dir, zDisLight4Dir, 1., 1., 0.);
	SetParallelLight(GL_LIGHT6, 0., 1., 1., 1., 1., 1.);

	glEnable(GL_LIGHTING);
	switch(ChangeView){
		case 0:
			glDisable(GL_LIGHT0);
			glEnable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHT3);
			glDisable(GL_LIGHT4);
			glDisable(GL_LIGHT5);
			glDisable(GL_LIGHT6);

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
			glMaterialfv(GL_FRONT, GL_AMBIENT, Array4(1., 1., 1., 1.));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Array4(1., 1., 1., 1.));
			glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.8f, White));
			glMaterialf(GL_FRONT, GL_SHININESS, 32);

			glutSolidSphere(42.5, 100, 100);
			glPopMatrix();
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);

			glPushMatrix();
			glTranslatef(0., -sqrt(42.5*42.5 - 27.*27.) - 7.5, 0.);
			glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
			glMaterialfv(GL_FRONT, GL_AMBIENT, Array4(1., 1., 1., 0.25));
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Array4(1., 1., 1., 0.25));
			glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.8f, White));
			glMaterialf(GL_FRONT, GL_SHININESS, 10);
			glCallList(WoodList);
			glPopMatrix();

			glDisable(GL_LIGHT1);
			break;
		case 1:
			glEnable(GL_LIGHTING);
			glDisable(GL_LIGHT1);
			glEnable(GL_LIGHT0);
			glCallList(SkyList);
			break;
	}
	
	glPushMatrix();
	glRotatef(45., 0., 1., 0.);
	glRotatef(30., 0., 0., 1.);
	switch(DisoButton){
		case 0:
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT2);
			glDisable(GL_LIGHT3);
			glDisable(GL_LIGHT4);
			glDisable(GL_LIGHT5);
			glDisable(GL_LIGHT6);
			break;
		case 1:
			glDisable(GL_LIGHT0);
			glEnable(GL_LIGHT2);
			glEnable(GL_LIGHT3);
			glEnable(GL_LIGHT4);
			glEnable(GL_LIGHT5);
			switch(DisParaButton){
				case 1:
					glEnable(GL_LIGHT6);
					break;
				case 0:
					glDisable(GL_LIGHT6);
					break;
			}
			break;
	}

	//sun
	glPushMatrix();
	switch(DisoButton){
		case 0:
			glDisable(GL_LIGHTING);
			glRotatef(360 * Time, 0., 1., 0.);
			glCallList(SunList);
			glEnable(GL_LIGHTING);
			break;
		case 1:
			glRotatef(360 * Time, 0., 1., 0.);
			glCallList(SunList);
			break;
	}
	glPopMatrix();
	
	//earth + moon
	glDisable(GL_LIGHTING);
	CircleInsideBall(11.5);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	glRotatef(360 * Time * sqrt(cude_cal(38.)) / sqrt(cude_cal(11.5)), 0., 1., 0.);
	glRotated(EarthOrigin, 0, 1, 0);
	glTranslatef(11.5, 0., 0.);
	glRotatef(360 * EarthOrigin/10 * Time, 0., 1., 0.);
	SetMaterial(1.0, 1.0, 1.0, 2.);
	glCallList( EarthList );
	glDisable(GL_LIGHTING);
	CircleInsideBall(1.5625);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	glRotatef(360 * Time * sqrt(cude_cal(38.)) / sqrt(cude_cal(1.5625)), 0., 1., 0.);
	glRotated(MoonOrigin, 0, 1, 0);
	glTranslatef(-1.5625, 0., 0.);
	glRotatef(360 * MoonOrigin/5 * Time, 0., 1., 0.);
	SetMaterial(1.0, 1.0, 1.0, 2.);
	glCallList(MoonList);
	glPopMatrix();
	glPopMatrix();
			
	glPopMatrix();

	glDisable(GL_LIGHTING);
	int j = 0;
	int q = 0;
	if (SnowButton == 1)
	{
		glColor3f(1., 1., 1.);
			while(j < snownum){
				while(q < snowgen){
					snowposition[q][j][0] += Time * snowmove[q][j][0];
					snowposition[q][j][1] += Time * snowfall[q][j];
					snowposition[q][j][2] += Time * snowmove[q][j][1];
					if (sqrt(snowposition[q][j][0] * snowposition[q][j][0] + snowposition[q][j][1] * snowposition[q][j][1] + snowposition[q][j][2] * snowposition[q][j][2]) <= 38)
					{
						glPushMatrix();
						glTranslatef(snowposition[q][j][0], snowposition[q][j][1], snowposition[q][j][2]);
						glutSolidSphere(0.5, 50, 50);
						glPopMatrix();
					}
					q++;
				}
				j++;
			}
	}

	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0., 100.,     0., 100. );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1., 1., 1. );
	DoRasterString( 5., 5., 0., "Sun-Earth-Moon in a Crystal Ball" );

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}

void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void
DoProjectMenu( int id )
{
	Projection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

void DoViewMenu(int id)
{
	ChangeView = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void DoDiscoMenu(int id)
{
	DisoButton = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void DoSnowMenu(int id)
{
	SnowButton = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void DoTripMenu(int id)
{
	TripButton = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}

// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}

// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );
	return (float)ms / 1000.f;
}

// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int viewmenu = glutCreateMenu( DoViewMenu );
	glutAddMenuEntry( "Outside",  0 );
	glutAddMenuEntry( "Inside",  1 );

	int discomenu = glutCreateMenu( DoDiscoMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",  1 );

	int snowmenu = glutCreateMenu( DoSnowMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",  1 );

	int tripmenu = glutCreateMenu( DoTripMenu );
	glutAddMenuEntry("Off", 0);
	glutAddMenuEntry("On", 1);

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddSubMenu(   "View",          viewmenu);
	glutAddSubMenu(   "DISCO",        discomenu);
	glutAddSubMenu(   "SNOW",         snowmenu);
	glutAddSubMenu(   "SPACE TRIP",   tripmenu);
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}

// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	unsigned char *Texture;
	int Width, Height;
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( Animate );

	//Sun Texture
	Texture = BmpToTexture("sunmap.bmp", &Width, &Height);
	if (Texture == NULL || Width != 1024 || Height != 512){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Suntexture);
	glBindTexture(GL_TEXTURE_2D, Suntexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
	
	//Earth Texture
	Texture = BmpToTexture("earthmap.bmp", &Width, &Height);
	if (Texture == NULL || Width != 1024 || Height != 512){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Earthtexture);
	glBindTexture(GL_TEXTURE_2D, Earthtexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	//Moon Texture
	Texture = BmpToTexture("moonmap.bmp", &Width, &Height);
	if (Texture == NULL || Width != 1024 || Height != 512){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Moontexture);
	glBindTexture(GL_TEXTURE_2D, Moontexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
	
	EarthOrigin = random_gen(0, 360);
	MoonOrigin = random_gen(0, 360);

	//Simpsons Texture
	Texture = BmpToTexture("Simpsons.bmp", &Width, &Height);
	if (Texture == NULL || Width != 1024 || Height != 512){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Simpsons);
	glBindTexture(GL_TEXTURE_2D, Simpsons);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	//Sky Texture
	Texture = BmpToTexture("skymap.bmp", &Width, &Height);
	if (Texture == NULL || Width != 1024 || Height != 1024){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Skytexture);
	glBindTexture(GL_TEXTURE_2D, Skytexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	//Wood Texture
	Texture = BmpToTexture("wood.bmp", &Width, &Height);
	if (Texture == NULL || Width != 512 || Height != 512){
		printf("Wrong Reading BMP File!");
		exit(1);
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Wood);
	glBindTexture(GL_TEXTURE_2D, Wood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );
	// Sun:
	SunList = glGenLists( 1 );
	glNewList( SunList, GL_COMPILE );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Suntexture);
	glColor3f(1., 1., 1.);
	MjbSphere(5., 100, 100);
	glDisable(GL_TEXTURE_2D);
	glEndList( );
	//Earth
	EarthList = glGenLists(1);
	glNewList(EarthList, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Earthtexture);
	glColor3f(1., 1., 1.);
	MjbSphere(1., 100, 100);
	glDisable(GL_TEXTURE_2D);
	glEndList();
	//Moon
	MoonList = glGenLists(1);
	glNewList(MoonList, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Moontexture);
	glColor3f(1., 1., 1.);
	MjbSphere(0.375, 100, 100);
	glDisable(GL_TEXTURE_2D);
	glEndList();
	//wood stage
	WoodList = glGenLists(1);
	glNewList(WoodList, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Simpsons);
	glColor3f(1., 1., 1.);
	DrawStage(27., 40., 15.);

	glBegin(GL_POLYGON);
	glNormal3f(0., 1., 0.);
	int i = 0;
	while(i <= 100){
		glTexCoord2f(1 - sin(0.01 * M_PI * i), 0.5 + 0.5 * sin(0.02 * M_PI * i));
		glVertex3f(40. * cos(0.02 * M_PI * i), -7.5, 40. * sin(0.02 * M_PI * i));
		i++;
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, Wood);
	glBegin(GL_POLYGON);
	glNormal3f(0., 1., 0.);
	i = 0;
	while(i <= 100){
		glTexCoord2f(0.5 + 0.5 * cos(0.02 * M_PI * i), 0.5 + 0.5 * sin(0.02 * M_PI * i));
		glVertex3f(27. * cos(0.02 * M_PI * i), 7.5, 27. * sin(0.02 * M_PI * i));
		i++;
	}
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEndList();
	//sky box
	SkyList = glGenLists(1);
	glNewList(SkyList, GL_COMPILE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Skytexture);
	glColor3f(1., 1., 1.);

	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(100., 100., -100);
	glTexCoord2f(1., 0.);
	glVertex3f(100., -100., -100);
	glTexCoord2f(1., 1.);
	glVertex3f(-100., -100., -100);
	glTexCoord2f(0., 1.);
	glVertex3f(-100., 100., -100);
	glEnd();
	
	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(100., 100., 100);
	glTexCoord2f(1., 0.);
	glVertex3f(100., -100., 100);
	glTexCoord2f(1., 1.);
	glVertex3f(-100., -100., 100);
	glTexCoord2f(0., 1.);
	glVertex3f(-100., 100., 100);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(100., -100., 100);
	glTexCoord2f(1., 0.);
	glVertex3f(100., -100., -100);
	glTexCoord2f(1., 1.);
	glVertex3f(-100., -100., -100);
	glTexCoord2f(0., 1.);
	glVertex3f(-100., -100., 100);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(100., 100., 100);
	glTexCoord2f(1., 0.);
	glVertex3f(100., 100., -100);
	glTexCoord2f(1., 1.);
	glVertex3f(-100., 100., -100);
	glTexCoord2f(0., 1.);
	glVertex3f(-100., 100., 100);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(100., 100., 100);
	glTexCoord2f(1., 0.);
	glVertex3f(100., 100., -100);
	glTexCoord2f(1., 1.);
	glVertex3f(100., -100., -100);
	glTexCoord2f(0., 1.);
	glVertex3f(100., -100., 100);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex3f(-100., 100., 100);
	glTexCoord2f(1., 0.);
	glVertex3f(-100., 100., -100);
	glTexCoord2f(1., 1.);
	glVertex3f(-100., -100., -100);
	glTexCoord2f(0., 1.);
	glVertex3f(-100., -100., 100);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEndList();
	// create the axes:
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}

// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o': case 'O':
			Projection = ORTHO;
			break;

		case 'p': case 'P':
			Projection = PERSP;
			break;

		case 'q': case 'Q': case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		case 'w': case 'W':
			ZMove += 0.2;
			if (ZMove >= 75.)
			{
				ZMove = 75.;
			}
			break;

		case 's': case 'S':
			ZMove -= 0.2;
			if (ZMove <= -75.)
			{
				ZMove = -75.;
			}
			break;

		case 'a': case 'A':
			XMove += 0.2;
			if (XMove >= 75.)
			{
				XMove = 75.;
			}
			break;

		case 'd': case 'D':
			XMove -= 0.2;
			if (XMove <= -75.)
			{
				XMove = -75.;
			}
			break;

		case 'f': case 'F':
            Frozen = ! Frozen;
            if( Frozen )
                glutIdleFunc( NULL );
            else
                glutIdleFunc( Animate );
            break;

		case 'x': case 'X':
			YMove += 0.2;
			if (YMove >= 75.)
			{
				YMove = 75.;
			}
			break;

		case 'z': case 'Z':
			YMove -= 0.2;
			if (YMove <= -75.)
			{
				YMove = -75.;
			}
			break;

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		
			break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		
			break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}

// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	Projection = PERSP;
	Xrot = Yrot = 0.;
	XMove = YMove = ZMove = 0.;
	ChangeView = 0;
	DisoButton = 0;
	SnowButton = 0;
	TripButton = 0;
	Frozen = false;
}

// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
