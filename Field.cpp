#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<vector>
#include"glut.h"

#define VTX (256)

float angle = 0;

std::vector<float>vtx;
std::vector<float>normal;
std::vector<unsigned int>index;
std::vector<float>tex;

float y = 0;

typedef struct { unsigned char b, g, r; }RGB;
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
RGB *pixels;

void read() {
	FILE *pFile = fopen("texture.bmp", "rb");
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

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 2, 2,
		0, 0, 0,
		0, 1, 0);

	/*float v[] = {
	0,0,0,
	5,0,0,
	10,0,0,
	0,0,5,
	5,0,5,
	10,0,5,
	0,0,10,
	5,0,10,
	10,0,10,
	};*/

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

	//float n[] = {
	//	0,1,0,	//0
	//	0,1,0,	//1
	//	0,1,0,	//2
	//	0,1,0,	//3
	//	0,1,0,	//4
	//	0,1,0,	//5
	//	0,1,0,	//6
	//	0,1,0,	//7
	//	0,1,0	//8
	//};

	float *v = vtx.data();
	float *n = normal.data();
	unsigned int *ind = index.data();
	float *t = tex.data();

	/*float t[] = {
		0,0,
		1/2.f,0,
		1,0,
		0,1/2.f,
		1/2.f,1/2.f,
		1,1/2.f,
		1,1,
		1,1/2.f,
		1,1,
	};*/

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, v);
	glNormalPointer(GL_FLOAT, 0, n);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	//glPolygonMode(
	//	GL_FRONT_AND_BACK,  // GLenum face
	//	GL_LINE);

	glRotatef(angle, 0, 1, 0);

	glDrawElements(
		GL_TRIANGLES,
		index.size(),
		GL_UNSIGNED_INT,
		ind);

	glFlush();
}

void timer(int value) {
	angle += 1;
	glutPostRedisplay();	//Ä•`‰æ
	glutTimerFunc(
		1000 / 60,		//unsigned int millis,		‰½•bŒã‚É‹N“®‚·‚é‚©  1000‚Å1•b
		timer,		//void (GLUTCALLBACK *func)(int value)	‚È‚É‚ð‹N“®‚·‚é‚Ì‚©
		0);		//int value		ˆø”
}

int main(int argc, char *argv[]) {
	read();

	for (int z = 0; z < VTX; z++) {
		for (int x = 0; x < VTX; x++) {
			//texture
			tex.push_back((float)x / (VTX - 1));
			tex.push_back((float)z / (VTX - 1));

			y = (pixels[VTX*z*(256 / VTX) + x*(256/VTX)].r + pixels[VTX*z*(256 / VTX) + x*(256 / VTX)].g + pixels[VTX*z*(256 / VTX) + x*(256 / VTX)].b) / 3.f/256;
			
			//vertex
			vtx.push_back(-1.f + (float)x / (VTX / 2));
			vtx.push_back(y);
			vtx.push_back(-1.f + (float)z / (VTX / 2));

			//normal
			normal.push_back(0);
			normal.push_back(1);
			normal.push_back(0);
		}
	}
	

	/*for (int z = 0; z < VTX; z++) {
		for (int x = 0; x < VTX; x++) {
			

		}
	}

	for (int z = 0; z < VTX; z++) {
		for (int x = 0; x < VTX; x++) {

		}
	}
*/
	for (int z = 0; z < VTX - 1; z++) {//s‚Ì”
		for (int x = 0; x < VTX - 1; x++) {//—ñ‚Ì”
			index.push_back(VTX * z + x);
			index.push_back(VTX * z + x + VTX);
			index.push_back(VTX * z + x + 1);
			index.push_back(VTX * z + x + 1);
			index.push_back(VTX * z + x + VTX);
			index.push_back(VTX * z + x + VTX + 1);
		}
	}

	glutInit(&argc, argv);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Test");
	glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}