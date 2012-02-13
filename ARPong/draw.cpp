#include "ARPong.h"

void draw(bool field_visible, double field_trans[3][4], bool pad1_visible, double pad1_trans[3][4])
{
    double gl_para[16];
    GLfloat mat_ambient_blue[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat mat_flash_blue[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat mat_ambient_red[]     = {1.0, 0.0, 0.0, 1.0};
    GLfloat mat_flash_red[]       = {1.0, 0.0, 0.0, 1.0};
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

	if(field_visible)
	{
		/* Load the camera transformation matrix for the field */
		argConvGlpara(field_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);
		
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_blue);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_blue);

		glBegin(GL_QUADS);

		/* 1 */
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-25.0f,   25.0f, 0.0f);
		glVertex3f(-25.0f, -100.0f, 0.0f);
		glVertex3f(-25.0f, -100.0f, 50.0f);
		glVertex3f(-25.0f,   25.0f, 50.0f);
		
		/* 2 */
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f( -25.0f, 25.0f,  0.0f);
		glVertex3f( 220.0f, 25.0f,  0.0f);
		glVertex3f( 220.0f, 25.0f, 50.0f);
		glVertex3f( -25.0f, 25.0f, 50.0f);

		/* 3 */
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(220.0f, -100.0f, 0.0f);
		glVertex3f(220.0f,   25.0f, 0.0f);
		glVertex3f(220.0f,   25.0f, 50.0f);
		glVertex3f(220.0f, -100.0f, 50.0f);

		/* 4 */
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-25.0f, -100.0f, 0.0f);
		glVertex3f(-15.0f, -100.0f, 0.0f);
		glVertex3f(-15.0f, -100.0f, 50.0f);
		glVertex3f(-25.0f, -100.0f, 50.0f);

		/* 5 */
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(220.0f, -100.0f, 0.0f);
		glVertex3f(210.0f, -100.0f, 0.0f);
		glVertex3f(210.0f, -100.0f, 50.0f);
		glVertex3f(220.0f, -100.0f, 50.0f);

		/* 6 */
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(210.0f,   15.0f, 0.0f);
		glVertex3f(210.0f, -100.0f, 0.0f);
		glVertex3f(210.0f, -100.0f, 50.0f);
		glVertex3f(210.0f,   15.0f, 50.0f);

		/* 7 */
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-15.0f,   15.0f, 0.0f);
		glVertex3f(-15.0f, -100.0f, 0.0f);
		glVertex3f(-15.0f, -100.0f, 50.0f);
		glVertex3f(-15.0f,   15.0f, 50.0f);

		/* 8 */
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-15.0f, 15.0f, 0.0f);
		glVertex3f(210.0f, 15.0f, 0.0f);
		glVertex3f(210.0f, 15.0f, 50.0f);
		glVertex3f(-15.0f, 15.0f, 50.0f);

		/* 9 */
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-25.0f, -100.0f, 50.0f);
		glVertex3f(-15.0f, -100.0f, 50.0f);
		glVertex3f(-15.0f,   25.0f, 50.0f);
		glVertex3f(-25.0f,   25.0f, 50.0f);
		
		/* 10 */
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-15.0f, 15.0f, 50.0f);
		glVertex3f(210.0f, 15.0f, 50.0f);
		glVertex3f(210.0f,  25.0f, 50.0f);
		glVertex3f(-15.0f,  25.0f, 50.0f);

		/* 11 */
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(210.0f, -100.0f, 50.0f);
		glVertex3f(220.0f, -100.0f, 50.0f);
		glVertex3f(220.0f,   25.0f, 50.0f);
		glVertex3f(210.0f,   25.0f, 50.0f);

		glEnd();

		if(pad1_visible)
		{
			/* Load the camera transformation matrix for pad1 */
			argConvGlpara(pad1_trans, gl_para);
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixd(gl_para);

			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_blue);
			glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_blue);

			glTranslatef(0.0, 0.0, 25.0);
			glutSolidCube(50.0);
		}

		glDisable(GL_LIGHT0);
	}
	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
}
