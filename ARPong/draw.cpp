#include "ARPong.h"

const float BALL_Z = 10.0f;
const float BALL_SPEED = 80.0f; /* Actually speed/sqrt(2) */

float ball_x = 100.0f, ball_y = -50.0f;
float ball_vx = BALL_SPEED, ball_vy = BALL_SPEED;

GLuint field_list;
GLuint pad_list;

void draw_init(void)
{
	/* Display list for the field */
	field_list = glGenLists(1);

	glNewList(field_list, GL_COMPILE);
	glBegin(GL_QUADS);
	{
		size_t f;
		for(f = 0; f < field_geometry_quads; f++)
		{
			glNormal3f(field_geometry[f*15 + 0], field_geometry[f*15 + 1], field_geometry[f*15 + 2]);
			glVertex3f(field_geometry[f*15 + 3], field_geometry[f*15 + 4], field_geometry[f*15 + 5]);
			glVertex3f(field_geometry[f*15 + 6], field_geometry[f*15 + 7], field_geometry[f*15 + 8]);
			glVertex3f(field_geometry[f*15 + 9], field_geometry[f*15 + 10], field_geometry[f*15 + 11]);
			glVertex3f(field_geometry[f*15 + 12], field_geometry[f*15 + 13], field_geometry[f*15 + 14]);
		}
	}
	glEnd();
	glEndList();

	/* Display list for the pad */
	pad_list = glGenLists(1);

	glNewList(pad_list, GL_COMPILE);
	glBegin(GL_QUADS);
	{
		int f, v;
		for(f = 0; f < pad_geometry_quads; f++)
		{
			glNormal3f(pad_geometry[f*15 + 0], pad_geometry[f*15 + 1], pad_geometry[f*15 + 2]);
			glVertex3f(pad_geometry[f*15 + 3], pad_geometry[f*15 + 4], pad_geometry[f*15 + 5]);
			glVertex3f(pad_geometry[f*15 + 6], pad_geometry[f*15 + 7], pad_geometry[f*15 + 8]);
			glVertex3f(pad_geometry[f*15 + 9], pad_geometry[f*15 + 10], pad_geometry[f*15 + 11]);
			glVertex3f(pad_geometry[f*15 + 12], pad_geometry[f*15 + 13], pad_geometry[f*15 + 14]);
		}
	}
	glEnd();
	glEndList();
}

void reflectOnPad(ARMat *mat_field, double pad_trans[3][4])
{
	int i, j;
	/*
	* RAQ * BALL_LOCAL = FIELD * BALL_ABS
	* BALL_LOCAL = RAQ-1 * FIELD * BALL_ABS
	*/
	ARMat *mat_raq = arMatrixAlloc(4, 4);
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 4; j++)
			mat_raq->m[i*4 + j] = pad_trans[i][j];
	}
	mat_raq->m[3*4 + 0] = 0.0;
	mat_raq->m[3*4 + 1] = 0.0;
	mat_raq->m[3*4 + 2] = 0.0;
	mat_raq->m[3*4 + 3] = 1.0;
	arMatrixSelfInv(mat_raq);

	ARMat *transfo = arMatrixAlloc(4, 4);
	arMatrixMul(transfo, mat_raq, mat_field);

	float ball_lx = transfo->m[0*4 + 0] * ball_x + transfo->m[0*4 + 1] * ball_y + transfo->m[0*4 + 2] * BALL_Z + transfo->m[0*4 + 3];
	float ball_ly = transfo->m[1*4 + 0] * ball_x + transfo->m[1*4 + 1] * ball_y + transfo->m[1*4 + 2] * BALL_Z + transfo->m[1*4 + 3];
	float ball_lz = transfo->m[2*4 + 0] * ball_x + transfo->m[2*4 + 1] * ball_y + transfo->m[2*4 + 2] * BALL_Z + transfo->m[2*4 + 3];

	printf("Position: (%lf, %lf, %lf)\n", ball_lx, ball_ly, ball_lz);

	arMatrixFree(transfo);
	arMatrixFree(mat_raq);
}

void draw(bool field_visible, double field_trans[3][4], bool pad1_visible, double pad1_trans[3][4])
{
	double gl_para[16];
	GLfloat mat_ambient_blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_flash_blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_ambient_red[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_flash_red[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_flash_shiny[] = {50.0f};
	GLfloat light_position[] = {100.0f, -50.0f, 0.0f, 0.0f};
	GLfloat ambi[] = {0.1f, 0.1f, 0.1f, 0.1f};
	GLfloat lightZeroColor[] = {0.9f, 0.9f, 0.9f, 0.1f};

	argDrawMode3D();
	argDraw3dCamera(0, 0);
	glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

	if(field_visible)
	{
		/* Load the camera transformation matrix for the field */
		argConvGlpara(field_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_blue);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_blue);

		/* Draw the field */
		glCallList(field_list);

		/* Move the ball */
		float old_ball_x = ball_x;
		float old_ball_y = ball_y;
		float elapsed = timeElapsed();
		ball_x += ball_vx * elapsed;
		ball_y += ball_vy * elapsed;

		/* Draw the ball */
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_red);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_red);

		glPushMatrix();
		glTranslatef(ball_x, ball_y, BALL_Z);
		glutSolidSphere(15.0f, 16.0f, 16.0f);
		glPopMatrix();

		ARMat *mat_field = NULL;
		if(pad1_visible /* || pad2_visible*/)
		{
			int i, j;
			mat_field = arMatrixAlloc(4, 4);
			for(i = 0; i < 3; i++)
			{
				for(j = 0; j < 4; j++)
					mat_field->m[i*4 + j] = field_trans[i][j];
			}
			mat_field->m[3*4 + 0] = 0.0;
			mat_field->m[3*4 + 1] = 0.0;
			mat_field->m[3*4 + 2] = 0.0;
			mat_field->m[3*4 + 3] = 1.0;
		}

		if(pad1_visible)
		{
			/* Load the camera transformation matrix for pad1 */
			argConvGlpara(pad1_trans, gl_para);
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixd(gl_para);

			glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash_blue);
			glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_blue);

			glCallList(pad_list);

			reflectOnPad(mat_field, pad1_trans);
		}

		if(mat_field != NULL)
			arMatrixFree(mat_field);
	}

	glDisable(GL_LIGHT0);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
}
