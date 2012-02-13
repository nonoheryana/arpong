#include "ARPong.h"

void draw(bool field_visible, double field_trans[3][4], bool pad1_visible, double pad1_trans[3][4])
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
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if(pad1_visible)
	{
		/* Load the camera transformation matrix for pad1 */
		argConvGlpara(pad1_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
		glutSolidCube(50.0);
	}

	if(field_visible)
	{
		/* Load the camera transformation matrix for the field */
		argConvGlpara(field_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
		glScalef(3.0f, 3.0f, 0.3f);
		glutSolidCube(50.0);
	}
	
	glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST);
}
