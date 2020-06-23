//****************************************************
// Starter code for assignment #1.  It is provided to 
// help get you started, but you are not obligated to
// use this starter code.
//****************************************************

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

//include header file for glfw library so that we can use OpenGL
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include "linmath.h"
#ifdef _WIN32
static long long lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265 // Should be used from mathlib

using namespace std;

//****************************************************
// Global Variables
// Generally speaking, global variables should be 
// avoided, but for this small assignment we'll make
// and exception.
//****************************************************

GLfloat Translation[3] = {0.0f, 0.0f, 0.0f};
bool Auto_strech = false;
int  Width_global = 400;
int  Height_global = 400;

int  SizeX_saved_global;
int  SizeY_saved_global;

int  PosX_saved_global;
int  PosY_saved_global;

const GLFWvidmode * VideoMode_global = NULL;

inline float sqr(float x) { return x*x; }

//****************************************************
// Simple init function
//****************************************************

void initializeRendering()
{
    glfwInit();
}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x+0.5, y+0.5);  
    // The 0.5 is to target pixel centers
    // Note that some OpenGL implementations have created gaps in the past.
}


//****************************************************
// Keyboard inputs
//****************************************************

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
                    
        // Hint on making up/down left/right work: the variable Translation [0] and [1].
                
        case GLFW_KEY_ESCAPE: 
        case GLFW_KEY_Q:      
            glfwSetWindowShouldClose(window, GLFW_TRUE); 
            break;
        case GLFW_KEY_LEFT :
            break;
        case GLFW_KEY_RIGHT:
            break;
        case GLFW_KEY_UP   :
            break;
        case GLFW_KEY_DOWN :
            break;
        case GLFW_KEY_F:
            if (action) {
                Auto_strech = !Auto_strech;                 
                if (Auto_strech){
                    glfwGetWindowPos(window, &PosX_saved_global, &PosY_saved_global);
                    glfwGetWindowSize(window, &SizeX_saved_global, &SizeY_saved_global);
                    glfwSetWindowSize(window, VideoMode_global->width, VideoMode_global->height);
                    glfwSetWindowPos(window, 0, 0);
                }else{
                    glfwSetWindowSize(window, SizeX_saved_global, SizeY_saved_global);
                    glfwSetWindowPos(window, PosX_saved_global, PosY_saved_global);
                }
            }
            break;
        case GLFW_KEY_SPACE: 
            break;  
        default: 
            break;
    }
    
}


void normalize(vec3 v)
{
	float norm = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
}

//****************************************************
// Draw a filled circle.
//****************************************************
vec3 light = {0.9,0.4,0.4}; //intense of light(basic)
double Kd = 0.8;  //diffuse coefficient
double Ks = 0.5;  //specular coefficient
void drawCircle(float centerX, float centerY, float radius) {

    // Start drawing a list of points
    glBegin(GL_POINTS);

    // We could eliminate wasted work by only looping over the pixels
    // inside the sphere's radius.  But the example is more clear this
    // way.  In general drawing an object by looping over the whole
    // screen is wasteful.

    int minI = max(0,(int)floor(centerX-radius));
    int maxI = min(Width_global-1,(int)ceil(centerX+radius));

    int minJ = max(0,(int)floor(centerY-radius));
    int maxJ = min(Height_global-1,(int)ceil(centerY+radius));

    for (float i = 0,j=0; i < Width_global,j<Height_global; i++,j++) {
		for (float j = 0; j < Height_global; j++)
		{

			// Location of the center of pixel relative to center of sphere
			float x = (i + 0.5 - centerX);
			float y = (j + 0.5 - centerY);

			float dist = sqrt(sqr(x) + sqr(y));

			if (dist <= radius) {
				// This is the front-facing Z coordinate
				float z = sqrt(radius*radius - dist * dist);
				vec3 ray = { 500 - x,500 - y, 500 - z };  //point light to pixel
				vec3 eye = { -x,-y,500 - z };  //eye to pixel
				vec3 reflection; //reflection light
				vec3 colora = { 0.05,0.05,0.05 };   //ambient color
				vec3 colord; //diffuse color
				vec3 colors;    //specular color
				vec3 color;//phong color
				vec3 norm = { x,y,z };

				normalize(ray);
				normalize(eye);
				normalize(norm);

				vec3_reflect(reflection, ray, norm);
				normalize(reflection);
				reflection[0]=-reflection[0];
				reflection[1]=-reflection[1];
				reflection[2]=-reflection[2];

                float pd = Kd * max(0.0f,vec3_mul_inner(ray, norm));
				float ps = Ks * pow(max(0.0f,vec3_mul_inner(reflection, eye)), 256);
				vec3_scale(colord, light, pd);
				vec3_scale(colors, light, ps);

				vec3_add(color, colord, colors);
				vec3_add(color, color, colora);
				GLfloat r = color[0];
				GLfloat g = color[1];
				GLfloat b = color[2];

				if (max(b, max(r, g)) > 0)
				{
					printf("...");
				}

				setPixel(i, j, r, g, b);

				// This is amusing, but it assumes negative color values are treated reasonably.
				// setPixel(i,j, x/radius, y/radius, z/radius );
			}
			else {
				continue;
			}
		}
        }
    glEnd();
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************

void display( GLFWwindow* window )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );      //clear background screen to black
    
    glClear(GL_COLOR_BUFFER_BIT);                // clear the color buffer (sets everything to black)
    
    glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    glLoadIdentity();                            // make sure transformation is "zero'd"
    
    //----------------------- code to draw objects --------------------------
    glPushMatrix();
    glTranslatef (Translation[0], Translation[1], Translation[2]);
    drawCircle(
        Width_global / 2.0 , 
        Height_global / 2.0 , 
        min(Width_global, Height_global) * 0.8 / 2.0);  // What do you think this is doing?
    glPopMatrix();

    glfwSwapBuffers(window);
    
}

//****************************************************
// function that is called when window is resized
//***************************************************

void size_callback(GLFWwindow* window, int width, int height)
{
    // The width and height arguments are not used
    // because they are not the size of the window 
    // in pixels.

    // Get the pixel coordinate of the window
    // it returns the size, in pixels, of the 
    // framebuffer of the specified window
    glfwGetFramebufferSize(window, &Width_global, &Height_global);
    
    glViewport(0, 0, Width_global, Height_global);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Width_global, 0, Height_global, 1, -1);
    
    display(window);
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************


int main(int argc, char *argv[]) {

    //This initializes glfw
    initializeRendering();
    
    GLFWwindow* window = glfwCreateWindow( Width_global, Height_global, "Computer Graphics", NULL, NULL );
    if ( !window )
    {
        cerr << "Error on window creating" << endl;
        glfwTerminate();
        return -1;
    }
    
    VideoMode_global = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if ( !VideoMode_global )
    {
        cerr << "Error on getting monitor" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent( window );

    size_callback(window, 0, 0);

    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
                
    while( !glfwWindowShouldClose( window ) ) // main loop to draw object again and again
    {   
        display( window );

        glfwPollEvents();        
    }

    return 0;
}








