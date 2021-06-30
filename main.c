/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/
// NAME : ROHAN ALI
// STUDENT NUMBER: 500777991

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels
static int rw1 = 0, rw2 = 0, lw1 = 0, lw2 = 0, tw1 = 0, tw2 = 0; //initializing the propellers values, to later be able to spin them with the Key 's' or 'S'
static int r = 0, l = 0, t = 0; //initializing the z axis for the drones body
static int ry = 0, ly = 0, ty = 0;//initializing the y axis for the drones body
static int rx = 0, lx = 0, tx = 0;//initializing the x axis for the drones body

static int currentButton;
static unsigned char currentKey;

// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 1");

    // Initialize GL
    initOpenGL(vWidth, vHeight);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);


    // Start event loop, never returns
    glutMainLoop();

    return 0;

}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);   // This light is currently off

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Drone
	//We want to draw a drone with total of three propellers, which all start with a white colour and one commanded to do something
	//it turns green. And we also want the drone to stay connected with all the propellers at alltimes, by using gltransalatef and
	//glrotate functions.


	//For one of the requirements, we have to build a Building in the back which is static and does not move at all 
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 0.0, 0.0);
	glScalef(-3.0, 5.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();

	//first right blade
	//by using the gltransalatef function here, this is the starting position for the right propellers, we connect the blades relative to the body by 
	//modifying the x and y values relative to the body. We use GLFloat and values such as rx, lx, r so later we can give keep the blades connected with the body
	//whilst moving forward, back, up and down.
	//The glrotatef sets the orginal angle of the blades, and once the correct keys are pressed, they start to spin 
	//We surrounded the the drawing of the robot with glPushMatrix()/glPopMatrix()
	//glPushMatrix will *copy* the current CTM (which is just I, then it goes through gltransalef, then Rotate, and finally Scaling it to its desired size. 
	//CTM = I * T1 * R1 * S1 
	//Since most of the code is repetitve and similar cause all this whole part is doing is creating the blades for the propellers and setting it up
	//for later use by pressing various keys, the CTM for them are I *T1*R1*S1
	glPushMatrix();
	glTranslatef((GLfloat)rx + 2.0, (GLfloat)ry + 4.5, (GLfloat)r);
	glRotatef((GLfloat)rw1+45, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();

	//second right blade
	glPushMatrix();
	glTranslatef((GLfloat)rx + 2.0, (GLfloat)ry + 4.5, (GLfloat)r);
	glRotatef((GLfloat)rw2 + 45, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();

	//first left blade
	glPushMatrix();
	glTranslatef((GLfloat)lx - 2.0, (GLfloat)ly + 4.5, (GLfloat)l + 2.5);
	glRotatef((GLfloat)lw1, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();

	//second left blade
	glPushMatrix();
	glTranslatef((GLfloat)lx - 2.0, (GLfloat)ly + 4.5, (GLfloat)l + 2.5);
	glRotatef((GLfloat)lw2, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();

	//third propeller 
	glPushMatrix();
	glTranslatef((GLfloat)tx,(GLfloat)ty + 5.0,(GLfloat)t);
	glRotatef(0.0, 0.0, 0.0, 0.0);
	glScalef(1.0, 2.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	//third properller blade 1
	glPushMatrix();
	glTranslatef((GLfloat)tx, (GLfloat)ty + 6.0, (GLfloat)t);
	glRotatef((GLfloat)tw1, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();
	//third propeller blade 2
	glPushMatrix();
	glTranslatef((GLfloat)tx, (GLfloat)ty + 6.0, (GLfloat)t);
	glRotatef((GLfloat)tw2, 0.0, 1.0, 0.0);
	glScalef(4.0, 1.0, 1.0);
	glutSolidCube(0.5);
	glPopMatrix();


    // Set drone material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

    // Apply transformations to move drone
    // ...

    // Apply transformations to construct drone, modify this!
    glPushMatrix();
    glTranslatef((GLfloat)rx, (GLfloat)ry + 4.0, (GLfloat)r);
    glRotatef(45.0, 0.0, 1.0, 0.0);
    glScalef(6.0, 1.0, 1.0);
    glutSolidCube(1.0);
    glPopMatrix();

	glPushMatrix();
	glTranslatef((GLfloat)lx, (GLfloat)ly + 4.0, (GLfloat)l);
	glRotatef(120.0, 0.0, 1.0, 0.0);
	glScalef(6.0, 1.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

    // Draw ground mesh
    DrawMeshQM(&groundMesh, meshSize);

    glutSwapBuffers();   // Double buffering, swap buffers
}

 
 

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    // Set up viewport, projection, then change to modelview matrix mode - 
    // display function will then set up camera and do modeling transforms.
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
    gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
/*All of this is the action that happens when a certain key (non-arrows) are pressed
for example when pressing S the propellers start to spin*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 's':
		rw1 = (rw1 + 10) % 360;
		rw2 = (rw2 + 50) % 360; 
		lw1 = (lw1 + 10) % 360;
		lw2 = (lw2 + 50) % 360;
		tw1 = (tw1 + 10) % 360;
		tw2 = (tw2 + 50) % 360;
		glutPostRedisplay();
		break;

	case 'S':
		rw1 = (rw1 + 10) % 360;
		rw2 = (rw2 + 50) % 360;
		lw1 = (lw1 + 10) % 360;
		lw2 = (lw2 + 50) % 360;
		tw1 = (tw1 + 10) % 360;
		tw2 = (tw2 + 50) % 360;
		glutPostRedisplay();
		break;
	case 'f':
		r = (r - 2);
		l = (l - 2);
		t = (t - 2);
		glutPostRedisplay();
		break;
	case 'b':
		r = (r + 2);
		l = (l + 2);
		t = (t + 2);
		glutPostRedisplay();
		break;
	 

	default:
		break;
	}
}

    //glutPostRedisplay();   // Trigger a window redisplay

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("'s' is to start propellers \n 'f' and 'b' are used for forward and back\n right and left arrow keys are to go side to side, turning the drone.");
	}
	// Doing transformations with arrow keys

	else if (key == GLUT_KEY_RIGHT) {
		rx = (rx + 2);
		lx = (lx + 2);
		tx = (tx + 2);
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_LEFT) {
		rx = (rx - 2);
		lx = (lx - 2);
		tx = (tx - 2);
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_UP) {
		ry = (ry + 2);
		ly = (ly + 2);
		ty = (ty + 2);
		glutPostRedisplay();
	}
	else if (key == GLUT_KEY_DOWN) {
		ry = (ry - 2);
		ly = (ly - 2);
		ty = (ty - 2);
		glutPostRedisplay();
	}

     
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
    currentButton = button;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
            ;

        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
        {
            ;
        }
        break;
    default:
        break;
    }

    glutPostRedisplay();   
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
    if (currentButton == GLUT_LEFT_BUTTON)
    {
        ;
    }

    glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}

 
 


