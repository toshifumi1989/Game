#define _CRT_SECURE_NO_WARNINGS

#include<stdlib.h>
#include<stdio.h>
#include"glut.h"
#include <windows.h>
#include<assert.h>

static int frame = 0;

typedef struct { unsigned char b, g, r; }RGB;
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
RGB *pixels;

void read_bmp() {

	FILE *pFile = fopen("size.bmp", "rb");
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

	

}

void timer(int value) {
	frame++;
	glutPostRedisplay();	//Ä•`‰æ
	glutTimerFunc(
		1000 / 60,		//unsigned int millis,		‰½•bŒã‚É‹N“®‚·‚é‚©  1000‚Å1•b
		timer,		//void (GLUTCALLBACK *func)(int value)	‚È‚É‚ð‹N“®‚·‚é‚Ì‚©
		0);		//int value		ˆø”
}

void display(void) {
	//glClearColor(0, 0, 1, 1);
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

	gluLookAt(
		0,1,2,
		0,0,0,
		0, 1, 0);

	glTexImage2D(
		GL_TEXTURE_2D,		//GLenum target,
		0,					//GLint level,
		GL_RGB,				//GLint internalformat, 
		bi.biWidth, bi.biHeight,	//GLsizei width, GLsizei height, 
		0,					//GLint border, 
		GL_RGB,				//GLenum format, 
		GL_UNSIGNED_BYTE,	//GLenum type, 
		pixels);			//const GLvoid *pixels

	glTexParameteri(
		GL_TEXTURE_2D,//GLenum target, 
		GL_TEXTURE_MIN_FILTER,//GLenum pname, 
		GL_NEAREST);//GLint param

	glTexParameteri(
		GL_TEXTURE_2D,//GLenum target, 
		GL_TEXTURE_MAG_FILTER,//GLenum pname, 
		GL_NEAREST);//GLint param

	glEnable(GL_TEXTURE_2D);

	glScalef(.5, .5, .5);

	float v[] = {
		-1, 1, 1,	//0
		-1, -1, 1,	//1
		1, -1, 1,	//2
		1, 1, 1,	//3
		-1, 1, -1,	//4
		-1, 1, 1,	//5
		1, 1, 1,	//6
		1,1,-1,		//7
		-1,1,-1,		//8
		-1,-1,-1,	//9
		-1,-1,1,	//10
		-1,1,1,	//11
		1,1,1,		//12
		1,-1,1,	//13
		1,-1,-1,		//14
		1,1,-1,		//15
		-1,-1,1,	//16
		1,-1,1,	//17
		1,-1,-1,		//18
		-1,-1,-1,		//19
		1,1,-1,		//20
		1,-1,-1,		//21
		-1,-1,-1,	//22
		-1,1,-1		//23
		 };

	glEnableClientState(GL_VERTEX_ARRAY);//(GLenum array);

	glVertexPointer(
		3,				//GLint size,
		GL_FLOAT,		//GLenum type,
		0,				//GLsizei stride,
		v);				// const GLvoid *pointer

	float n[] = {
		0,0,1, 0,0,1, 0,0,1, 0,0,1,
		0,1,0, 0,1,0, 0,1,0, 0,1,0,
		-1,0,0, -1,0,0, -1,0,0, -1,0,0,
		1,0,0, 1,0,0, 1,0,0, 1,0,0,
		0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
		0,0,-1, 0,0,-1,0,0,-1,0,0,-1};

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT,0,n);

	unsigned char indices[] = { 
		0,1,2, 
		0,2,3,
		5,6,4,
		4,6,7,
		11,8,10,
		8,9,10,
		12,13,15,
		15,13,14,
		16,18,17,
		16,19,18,
		23,20,22,
		22,20,21};

	float t[] = {
		0,0,		//0
		0,1,		//1
		1 / 6.0f,1,	//2
		1 / 6.0f,0 ,//3
		1 / 6.0f,0,	//4
		1 / 6.0f,1,	//5
		2 / 6.0f,1,	//6
		2 / 6.0f,0,	//7
		2 / 6.0f,0,	//8
		2 / 6.0f,1,	//9
		3 / 6.0f,1,	//10
		3 / 6.0f,0,	//11
		3 / 6.0f,0,	//12
		3 / 6.0f,1,	//13
		4 / 6.0f,1,	//14
		4 / 6.0f,0,	//15
		4 / 6.0f,0,	//16
		4 / 6.0f,1,	//17
		5 / 6.0f,1,	//18
		5 / 6.0f,0,	//19
		5 / 6.0f,0,	//20
		5 / 6.0f,1,	//21
		1,1,		//22
		1,0 };		//23

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glRotatef(frame,1,1,0);	

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	// GLenum cap
	glEnable(GL_TEXTURE_2D);


	glDrawElements(
		GL_TRIANGLES,//GLenum mode,
		36,//GLsizei count, 
		GL_UNSIGNED_BYTE,//GLenum type, 
		indices);//const GLvoid *indices

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glFlush();
}

int main(int argc, char *argv[]) {
	read_bmp();
	glutInit(&argc, argv);
	glutCreateWindow("Tittle");
	glutDisplayFunc(display);//(void (GLUTCALLBACK *func)(void));
	glutTimerFunc(
		0,		//unsigned int millis,		‰½•bŒã‚É‹N“®‚·‚é‚©
		timer,		//void (GLUTCALLBACK *func)(int value)	‚È‚É‚ð‹N“®‚·‚é‚Ì‚©
		0);		//int value		ˆø”
	glutMainLoop();
}