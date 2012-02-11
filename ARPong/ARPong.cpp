#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

/* Camera configuration; on Windows, we need to flip the image vertically */
#ifdef _WIN32
char *VCONF = "Data\\WDM_camera_flipV.xml";
#else
char *VCONF = "";
#endif

const int THRESHOLD = 100;
const char *CPARAM_NAME = "Data/camera_para.dat";
const char *PATT_NAME = "Data/patt.hiro";
const double PATT_WIDTH = 80.0;

/** The pattern we are using */
int patt_id;

int total_frame_count = 0;

static void init(void);
static void cleanup(void);
static void keyEvent(unsigned char key, int x, int y);
static void mainLoop(void);
static void draw(double patt_trans[3][4]);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	init();

    arVideoCapStart();
    argMainLoop(NULL, keyEvent, mainLoop);
	return 0;
}

/**
 * Function called on keyboard events.
 */
static void keyEvent(unsigned char key, int x, int y)
{
    if(key == 0x1B)
    {
        /* Quit if the ESC key is pressed */
        printf("*** %lf (frame/sec)\n", (double)total_frame_count/arUtilTimer());
        cleanup();
        exit(0);
    }
}

/**
 * Initialization function.
 */
static void init(void)
{
    ARParam wparam, cparam;
    int xsize, ysize;

    /* Open the video using a configuration file */
    if(arVideoOpen(VCONF) < 0)
        exit(1);
    /* Find the size of the window */
    if(arVideoInqSize(&xsize, &ysize) < 0)
        exit(1);
    printf("Image size: %dx%d\n", xsize, ysize);

    /* Load the camera parameters from the file and set options */
    if(arParamLoad(CPARAM_NAME, 1, &wparam) < 0)
    {
        printf("Unable to load camera parameter file\n");
        exit(1);
    }
    arParamChangeSize(&wparam, xsize, ysize, &cparam);
    arInitCparam(&cparam);
    printf("*** Camera Parameter ***\n");
    arParamDisp(&cparam);

    if( (patt_id=arLoadPatt(PATT_NAME)) < 0)
    {
        printf("Unable to load the pattern file\n");
        exit(1);
    }

    /* open the graphics window */
    argInit(&cparam, 1.0, 0, 0, 0, 0);
}

/**
 * Main loop.
 *
 * Function called at regular intervals to do stuff and render something.
 */
static void mainLoop(void)
{
    ARUint8 *dataPtr;
    ARMarkerInfo *marker_info;
    int marker_num;
    int j, k;

    /* Grab a video frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL)
    {
        /* If we can't get one, just wait and try again... */
        arUtilSleep(2);
        return;
    }

    /* FPS measurement */
    if(total_frame_count == 0)
        arUtilTimerReset();
    total_frame_count++;

    argDrawMode2D();
    argDispImage(dataPtr, 0, 0);

    /* Detect the markers in the video frame */
    if(arDetectMarker(dataPtr, THRESHOLD, &marker_info, &marker_num) < 0)
    {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    /* Check for object visibility */
    k = -1;
    for(j = 0; j < marker_num; j++)
    {
        if(patt_id == marker_info[j].id)
        {
            if(k == -1)
                k = j;
            else if(marker_info[k].cf < marker_info[j].cf)
                k = j;
        }
    }
    if(k == -1)
    {
        argSwapBuffers();
        return;
    }

    /* Get the transformation between the marker and the real camera */
    double patt_center[2] = {0.0, 0.0};
    double patt_trans[3][4];
    arGetTransMat(&marker_info[k], patt_center, PATT_WIDTH, patt_trans);

    draw(patt_trans);

    argSwapBuffers();
}

/**
 * Cleanup function called when the program exits.
 */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static void draw(double patt_trans[3][4])
{
    double gl_para[16];
    GLfloat mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat mat_flash_shiny[] = {50.0};
    GLfloat light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

    argDrawMode3D();
    argDraw3dCamera(0, 0);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /* Load the camera transformation matrix */
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(gl_para);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(0.0, 0.0, 25.0);
    glutSolidCube(50.0);
    glDisable(GL_LIGHTING);

    glDisable(GL_DEPTH_TEST);
}
