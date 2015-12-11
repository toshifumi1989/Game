#define _CRT_SECURE_NO_WARNINGS

#define posSpeed ( - 0.5 + rand() % 10 / 10)
#define sclSpeed ( 0.01f + rand() % 10 / 5000.f)

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


class Vec3 {
public:
	float x, y, z;

	Vec3(){}
	Vec3(float _x, float _y, float _z) { x = _x, y = _y, z = _z; }
	~Vec3(){}

	Vec3 &operator += (const Vec3 &_speed) {
		this->x += _speed.x;
		this->y += _speed.y;
		this->z += _speed.z;
		return *this;
	}
};

class Smoke {
public:
	Smoke(){}
	~Smoke(){}

	float alpha;

	//updata��rand���g���Ă����Ƃ�
	float pos_y;
	float scl_x;
	float scl_y;
	float scl_z;

	Vec3 position;
	Vec3 scale;
	Vec3 pos_speed;
	Vec3 scl_speed;

	/*void updata() {
		alpha -= rand() % 10 / 500.f;
		scl_x += rand() % 10 / 300.f;
		scl_y += rand() % 10 / 300.f;
		scl_z += rand() % 10 / 300.f;
		pos_y += 0.001f + rand() % 10 / 500.f;
	}*/

	void updata() {
		alpha -= 0.01f;
		position += pos_speed;
		scale += scl_speed;
	}
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

}

void timer(int value) {
	glutPostRedisplay();	//�ĕ`��
	glutTimerFunc(
		1000 / 60,		//unsigned int millis,		���b��ɋN�����邩  1000��1�b
		timer,		//void (GLUTCALLBACK *func)(int value)	�Ȃɂ��N������̂�
		0);		//int value		����
}

void display(void) {

	//�w�i�F
	glClearColor(0, 0, .5, 1);

	//�F�Ɛ[�x�̃N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//�J�����ݒ�
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		�\��������̂̊p�x
		1,		//GLdouble aspect	�\�����镝�ɑ΂��鍂��
		0.1,	//GLdouble zNear,	�\���̈�ԋ߂��Ƃ���
		50);	//GLdouble zFar		�\���̈�ԉ����Ƃ���

	glMatrixMode(GL_MODELVIEW);//GLenum mode
	glLoadIdentity();

	static float r = 0;
	r += 3.14 / 500;
	gluLookAt(
		sin(r) * 3, 3, cos(r) * 3,//GLdouble eyex,GLdouble eyey,GLdouble eyez,
		0, 0, 0,//GLdouble centerx,GLdouble centery,GLdouble centerz,
		0, 1, 0);//GLdouble upx,GLdouble upy,GLdouble upz


	//�e�N�X�`���ݒ�
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

	//�J�����O�E�[�x�ݒ�
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	//���̕��ʐ}
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



	//�A���t�@�ݒ�
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




	frame++;

	//���̕`��Ԋu���L���邽��
	if (frame == 10) {
		//���̏����l�ݒ�
		Smoke smk;

		/*smk.pos_y = 0;
		smk.scl_x = 0;
		smk.scl_y = 0;
		smk.scl_z = 0;*/

		smk.alpha = 1;

		smk.position = Vec3(0, 0, 0);
		smk.scale = Vec3(0, 0, 0);

		smk.pos_speed.x = posSpeed;
		smk.pos_speed.y = posSpeed;
		smk.pos_speed.z = posSpeed;

		smk.scl_speed.x = sclSpeed;
		smk.scl_speed.y = sclSpeed;
		smk.scl_speed.z = sclSpeed;


		smoke.push_back(smk);
		frame = 0;
	}

	/*std::list<Smoke> smoke_list;

	std::list<Smoke>::iterator it = smoke_list.begin();

	while (it != smoke_list.end()) {

		int n = (*it).n;

		it++;
	}
*/

	for (int i = 0; i < smoke.size(); i++) {
		glPushMatrix();
		{
			//���̕`�����ɐ��ʂɌ�����i�r���{�[�h
			glm::mat4 view;
			glGetFloatv(
				GL_MODELVIEW_MATRIX,
				(float*)&view
				);

			glm::mat4 m = inverse(view);
			m[3][0] = m[3][1] = m[3][2] = 0;
			glMultMatrixf((float*)&m);//const GL_FSloat *m

			smoke[i].updata();

		
			/*glColor4f(1, 1, 1, smoke[i].alpha);
			glScalef(smoke[i].scl_x, smoke[i].scl_y, smoke[i].scl_z);
			glTranslatef(0, smoke[i].pos_y, 0);*/

			glColor4f(1, 1, 1, smoke[i].alpha);
			glScalef(smoke[i].scale.x,smoke[i].scale.y, smoke[i].scale.z);
			glTranslatef(smoke[i].scale.x, smoke[i].scale.y, smoke[i].scale.z);

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
		0,		//unsigned int millis,		���b��ɋN�����邩
		timer,		//void (GLUTCALLBACK *func)(int value)	�Ȃɂ��N������̂�
		0);		//int value		����
	glutMainLoop();
}