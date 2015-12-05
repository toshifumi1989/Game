#define _CRT_SECURE_NO_WARNINGS


#include<stdlib.h>
#include<stdio.h>
#include <windows.h>
#include<assert.h>
#include<vector>
#include <time.h>
#include"glm\glm.hpp"
#include"glut.h"
static float frame = 0;

typedef struct { unsigned char b, g, r; }RGB;
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
RGB *pixels;

std::vector<unsigned char>texture;

//class Vec3 {
//public:
//	float x, y, z;
//	Vec3();
//	Vec3(float _x, float _y, float _z) { x = _x, y = _y, z = _z; }
//};

class Smoke {
public:
	float alpha;
	float pos_y;
	float scl_x;
	float scl_y;
	float scl_z;
	/*Vec3 scl;
	Vec3 speed;*/

	void update() {
		alpha -= rand() % 10 /500.f;
		scl_x += rand() % 10 / 300.f;
		scl_y += rand() % 10 / 300.f;
		scl_z += rand() % 10 / 300.f;
		pos_y += 0.001f + rand() % 10 / 500.f;
	}
	/*void updata() {
		alpha -= 0.1f;
		scl_x += 0.1f + rand() % 10 / 300.f;
		scl_y += 0.2f + rand() % 10 / 300.f;
		scl_z += 0.1f + rand() % 10 / 300.f;
		pos_y += 0.01f + rand() % 10 / 500.f;
	}*/
};

std::vector<Smoke>smoke;

void read_bmp() {

	FILE *pFile = fopen("smokevol1.bmp", "rb");
	assert(pFile != NULL);

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, pFile);

	int size = bi.biWidth *bi.biHeight * sizeof(RGB);
	pixels = (RGB*)malloc(size);
	fread(pixels, size, 1, pFile);

	fclose(pFile);		//FILE * _File
	for (int i = 0; i < bi.biWidth * bi.biHeight; i++) {
		char sub = pixels[i].b;
		pixels[i].b = pixels[i].r;
		pixels[i].r = sub;
	}

	for (int p = 0; p < bi.biHeight / 2; p++) {
		for (int q = 0; q < bi.biWidth; q++) {
			RGB sub = pixels[bi.biWidth * p + q];
			pixels[bi.biWidth * p + q] = pixels[bi.biWidth *(bi.biHeight - 1 - p) + q];
			pixels[bi.biWidth *(bi.biHeight - 1 - p) + q] = sub;
		}
	}

	for (int i = 0; i < bi.biHeight*bi.biWidth; i++) {
		float n = (pixels[i].r + pixels[i].g + pixels[i].b) / 3;
		texture.push_back(n);
	}
	printf("%d\n", texture[128 * 50 + 50]);
}

void timer(int value) {
	glutPostRedisplay();	//Ä•`‰æ
	glutTimerFunc(
		1000 / 60,		//unsigned int millis,		‰½•bŒã‚É‹N“®‚·‚é‚©  1000‚Å1•b
		timer,		//void (GLUTCALLBACK *func)(int value)	‚È‚É‚ð‹N“®‚·‚é‚Ì‚©
		0);		//int value		ˆø”
}

void display(void) {

	glClearColor(0, 0, .5, 1);
	glMatrixMode(GL_PROJECTION);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		•\Ž¦‚·‚é‚à‚Ì‚ÌŠp“x
		1,		//GLdouble aspect	•\Ž¦‚·‚é•‚É‘Î‚·‚é‚‚³
		0.1,	//GLdouble zNear,	•\Ž¦‚Ìˆê”Ô‹ß‚¢‚Æ‚±‚ë
		50);	//GLdouble zFar		•\Ž¦‚Ìˆê”Ô‰“‚¢‚Æ‚±‚ë

	glMatrixMode(GL_MODELVIEW);//GLenum mode
	glLoadIdentity();

	static float r = 0;
	r += 3.14 / 500;
	gluLookAt(
		sin(r) * 3, 3, cos(r) * 3,//GLdouble eyex,GLdouble eyey,GLdouble eyez,
		0, 0, 0,//GLdouble centerx,GLdouble centery,GLdouble centerz,
		0, 1, 0);//GLdouble upx,GLdouble upy,GLdouble upz

	glTexImage2D(
		GL_TEXTURE_2D,		//GLenum target,
		0,					//GLint level,
		GL_ALPHA,				//GLint internalformat, 
		bi.biWidth, bi.biHeight,	//GLsizei width, GLsizei height, 
		0,					//GLint border, 
		GL_ALPHA,				//GLenum format, 
		GL_UNSIGNED_BYTE,	//GLenum type, 
		texture.data());			//const GLvoid *pixels

	glTexParameteri(
		GL_TEXTURE_2D,//GLenum target, 
		GL_TEXTURE_MIN_FILTER,//GLenum pname, 
		GL_NEAREST);//GLint param

	glTexParameteri(
		GL_TEXTURE_2D,//GLenum target, 
		GL_TEXTURE_MAG_FILTER,//GLenum pname, 
		GL_NEAREST);//GLint param

	glEnable(GL_TEXTURE_2D);

	float v[] = {
		-1,2,
		-1,0,
		1,2,
		1,0
	};

	glEnableClientState(GL_VERTEX_ARRAY);//(GLenum array);

	glVertexPointer(
		2,				//GLint size,
		GL_FLOAT,		//GLenum type,
		0,				//GLsizei stride,
		v);				// const GLvoid *pointer


	float t[] = {
		0,0,
		0,1,
		1,0,
		1,1
	};

	glPixelStorei(
		GL_UNPACK_ALIGNMENT,// GLenum pname
		1);                 // GLint param
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, t);

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,           // GLenum sfactor
		GL_ONE_MINUS_SRC_ALPHA);// GLenum dfactor

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	for (int x = -5; x <= 5; x++) {
		glBegin(GL_LINES);
		{
			glVertex3f(x, 0, -5);
			glVertex3f(x, 0, 5);
		}
		glEnd();

		glBegin(GL_LINES);
		{
			glVertex3f(-5, 0, x);
			glVertex3f(5, 0, x);
		}
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);

	Smoke smk;
	smk.pos_y = 0;
	smk.scl_x = 0;
	smk.scl_y = 0;
	smk.scl_z = 0;
	smk.alpha = 1;
	/*smk.speed.x = rand() % 10 / 100;
	smk.speed.y = rand() % 10 / 100;
	smk.speed.z = rand() % 10 / 100;*/
	frame++;

	if(frame == 10){
		smoke.push_back(smk);
		frame = 0;
	}
	
	for (int i = 0; i < smoke.size(); i++) {
		glPushMatrix();
		{	
		glm::mat4 view;
		glGetFloatv(
			GL_MODELVIEW_MATRIX,
			(float*)&view
			);

		glm::mat4 m = inverse(view);
		m[3][0] = m[3][1] = m[3][2] = 0;
		glMultMatrixf((float*)&m);//const GL_FSloat *m
		
		smoke[i].update();

		glColor4f(1, 1, 1, smoke[i].alpha);
		glScalef(smoke[i].scl_x, smoke[i].scl_y, smoke[i].scl_z);
		glTranslatef(0, smoke[i].pos_y, 0);

		glDrawArrays(
			GL_TRIANGLE_STRIP,
			0,
			4);

		}
		glPopMatrix();
	}

	glFlush();
}

int main(int argc, char *argv[]) {
	read_bmp();
	srand(time(NULL));
	glutInit(&argc, argv);
	glutCreateWindow("Tittle");
	glutDisplayFunc(display);//(void (GLUTCALLBACK *func)(void));
	glutTimerFunc(
		0,		//unsigned int millis,		‰½•bŒã‚É‹N“®‚·‚é‚©
		timer,		//void (GLUTCALLBACK *func)(int value)	‚È‚É‚ð‹N“®‚·‚é‚Ì‚©
		0);		//int value		ˆø”
	glutMainLoop();
}