#include "ARPong.h"
#include "glm.h"

const float BALL_Z = 10.0f;
const float BALL_SPEED = 60.0f; /* Actually speed/sqrt(2) */
const float BALL_RADIUS = 15.0f;

float ball_x = 100.0f, ball_y = -50.0f;
float ball_vx = -BALL_SPEED, ball_vy = BALL_SPEED;

GLMmodel *field;
GLMmodel *pad;
GLuint pad_texture;
GLuint field_texture;
GLuint overlay_texture;

int score = 0;
int lives = 3;

void draw_reset(void)
{
	ball_x = 100.0f; ball_y = -50.0f;
	ball_vx = -BALL_SPEED; ball_vy = BALL_SPEED;

	score = 0;
	lives = 3;
}

void draw_init(void)
{
	/* 3D model for the field */
	field = glmReadOBJ("Data/field.obj");
	if(field == NULL)
	{
		printf("Unable to load models\n");
		exit(1);
	}

	/* 3D model for the pad */
	pad = glmReadOBJ("Data/pad.obj");
	if(pad == NULL)
	{
		printf("Unable to load models\n");
		exit(1);
	}

	/* Texture for the pad */
	pad_texture = load_texture("Data/pad_texture.jpg");

	/* Texture for the playing field */
	field_texture = load_texture("Data/field_texture.jpg");

	/* Texture for the overlay */
	overlay_texture = load_texture("Data/numbers.jpg");
}

void draw(bool field_visible, double field_trans[3][4], bool pad1_visible, double pad1_trans[3][4])
{
	double gl_para[16];
	GLfloat mat_ambient_red[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_flash_red[] = {1.0f, 0.0f, 0.0f, 1.0f};
	GLfloat mat_flash_shiny[] = {50.0f};
	GLfloat light_position[] = {-100.0f, -50.0f, 0.0f, 0.0f};
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

		/* Draw the field */
		glPushMatrix();
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, field_texture);
		glmDraw(field, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		/* Move the ball */
		float old_ball_x = ball_x;
		float old_ball_y = ball_y;
		float elapsed = timeElapsed();
		ball_x += ball_vx * elapsed;
		ball_y += ball_vy * elapsed;

		/* Draw the ball */
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_flash_red);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_flash_shiny);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient_red);

		glPushMatrix();
		glTranslatef(ball_x, ball_y, BALL_Z);
		glutSolidSphere(BALL_RADIUS, 16.0f, 16.0f);
		glPopMatrix();

		ARMat *mat_inv_field = NULL;
		if(pad1_visible /* || pad2_visible*/)
		{
			int i, j;
			mat_inv_field = arMatrixAlloc(4, 4);
			for(i = 0; i < 3; i++)
			{
				for(j = 0; j < 4; j++)
					mat_inv_field->m[i*4 + j] = field_trans[i][j];
			}
			mat_inv_field->m[3*4 + 0] = 0.0;
			mat_inv_field->m[3*4 + 1] = 0.0;
			mat_inv_field->m[3*4 + 2] = 0.0;
			mat_inv_field->m[3*4 + 3] = 1.0;
			arMatrixSelfInv(mat_inv_field);
		}

		if(pad1_visible)
		{
			/* Calculate the pad coordinates relative to the field */
			float pad_x, pad_y, pad_z;
			{
				int i, j;
				/*
				* FIELD * M = PAD
				* M = FIELD-1 * PAD
				*/
				ARMat *mat_pad = arMatrixAlloc(4, 4);
				for(i = 0; i < 3; i++)
				{
					for(j = 0; j < 4; j++)
						mat_pad->m[i*4 + j] = pad1_trans[i][j];
				}
				mat_pad->m[3*4 + 0] = 0.0;
				mat_pad->m[3*4 + 1] = 0.0;
				mat_pad->m[3*4 + 2] = 0.0;
				mat_pad->m[3*4 + 3] = 1.0;
				ARMat *transfo = arMatrixAllocMul(mat_inv_field, mat_pad);
				pad_x = transfo->m[0*4 + 3];
				pad_y = transfo->m[1*4 + 3];
				pad_z = transfo->m[2*4 + 3];
				arMatrixFree(transfo);
			}

			glPushMatrix();
			glTranslatef(pad_x, pad_y, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, pad_texture);
			glmDraw(pad, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
			glDisable(GL_TEXTURE_2D);
			glPopMatrix();

			/* Collision detection */
			if(ball_x >= pad_x - 45.0f - BALL_RADIUS && ball_x <= pad_x - 45.0f + BALL_RADIUS
			 && ball_y >= pad_y - 45.0f - BALL_RADIUS && ball_y <= pad_y + 45.0f + BALL_RADIUS
			 && ball_vx > 0.0f)
			{
				ball_vx *= -1.0f;
				if(ball_vx < 0.0f) ball_vx -= BALL_SPEED*0.5f;
				else ball_vx += BALL_SPEED*0.5f;
				if(ball_vy < 0.0f) ball_vy -= BALL_SPEED*0.4f;
				else ball_vy += BALL_SPEED*0.4f;
				score += 10;
			}
		}

		/* Collision with the left bound (no second pad) */
		if(lives > 0 && ball_x <= -40.0f + BALL_RADIUS && ball_vx < 0.0f)
			ball_vx *= -1.0f;

		/* Collision with the top/bottom bounds */
		if(lives > 0 && ball_x <= 210.0f + BALL_RADIUS && ball_x >= -40.0f - BALL_RADIUS)
		{
			if(ball_y <= -130.0f + BALL_RADIUS && ball_vy < 0.0f)
				ball_vy *= -1.0f;
			else if(ball_y >= 30.0f - BALL_RADIUS && ball_vy > 0.0f)
				ball_vy *= -1.0f;
		}

		/* Ball exiting the field */
		if(lives > 0 && ball_x >= 220.0f)
		{
			lives--;

			if(lives == 0)
			{
				ball_x = -10E5f;
				ball_y = 10E5f;
			}
			else
			{
				ball_x = 100.0f;
				while(ball_y > 0.0f)
					ball_y -= 60.0f;
				while(ball_y < -70.0f)
					ball_y += 50.0f;
				ball_vx = -BALL_SPEED; ball_vy = BALL_SPEED;
			}
		}

		if(mat_inv_field != NULL)
			arMatrixFree(mat_inv_field);
	}

	glDisable(GL_LIGHT0);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	/* Overlay */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 4.0, 3.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Score */
	{
		int i = 0;
		int score_ = score;

		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex3f(3.95, 0.05f, 0);
			glVertex3f(2.95f, 0.05f, 0);
			glVertex3f(2.95f, 0.35f, 0);
			glVertex3f(3.95f, 0.35f, 0);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, overlay_texture);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		do
		{
			int digit = score_ % 10;
			int u = digit % 4;
			int v = digit / 4;

			glBegin(GL_QUADS);
				glTexCoord2f(u*0.25f, v*0.25f);
				glVertex2f(3.7f - 0.2f*i, 0.1f);

				glTexCoord2f(u*0.25f + 0.25f, v*0.25f);
				glVertex2f(3.9f - 0.2f*i, 0.1f);

				glTexCoord2f(u*0.25f + 0.25f, v*0.25f + 0.25f);
				glVertex2f(3.9f - 0.2f*i, 0.3f);

				glTexCoord2f(u*0.25f, v*0.25f + 0.25f);
				glVertex2f(3.7f - 0.2f*i, 0.3f);
			glEnd();
			
			score_ /= 10;
			i++;
		}
		while(score_ > 0);
		glDisable(GL_TEXTURE_2D);
	}

	/* Lives */
	{
		int i;
		
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex3f(0.05f, 0.05f, 0);
			glVertex3f(0.75f, 0.05f, 0);
			glVertex3f(0.75f, 0.35f, 0);
			glVertex3f(0.05f, 0.35f, 0);
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, overlay_texture);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		for(i = 0; i < lives; i++)
		{
			glBegin(GL_QUADS);
				glTexCoord2f(0.5f, 0.5f);
				glVertex3f(0.1f + 0.2f*i, 0.1f, 0);

				glTexCoord2f(0.75f, 0.5f);
				glVertex3f(0.3f + 0.2f*i, 0.1f, 0);

				glTexCoord2f(0.75f, 0.75f);
				glVertex3f(0.3f + 0.2f*i, 0.3f, 0);

				glTexCoord2f(0.5f, 0.75f);
				glVertex3f(0.1f + 0.2f*i, 0.3f, 0);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
	}
}
