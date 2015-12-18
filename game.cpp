#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define GRAVITY (0.02)
#define VTX (256)

#include<stdio.h>
#include<math.h>
#include<Windows.h>
#include<vector>
#include"glut.h"
#include"glm\glm.hpp"

unsigned char keys[256] = {};
unsigned char lastkeys = 0;

std::vector<float>vtx;
std::vector<float>normal;
std::vector<unsigned int>index;
std::vector<float>tex;

typedef struct { unsigned char b, g, r; }RGB;
BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
RGB *pixels;

class Object {
	//using vec = glm::vec3;
public:
	Object() {}
	~Object() {}

	glm::vec3 position;
	glm::vec3 speed;
	float angle;
	virtual void update() = 0;
	virtual void draw() = 0;

};

class Character :public Object {
public:
	Character() {}
	~Character() {}
	Character(glm::vec3 _pos) {
		position = _pos;
	}

	void update() {

		speed *= 0.95;
		position += speed;

		speed.y -= GRAVITY;

		if (position.y < 0) {
			position.y = 0;
		}
	}

	void draw() {};

	void playerDraw() {
		glColor3f(0, 0, 1);
		glutSolidCube(1);
	}

	void enemyDraw() {
		glColor3f(1, 0, 0);
		glutSolidSphere(0.5f, 30, 30);
	}

};


class Bullet :public Object {
public:

	bool bulletOnFlag = false;

	Bullet() {}
	~Bullet() {}
	Bullet(glm::vec3 _pos) {
		position = _pos;
	}

	void update() {

	}

	void draw() {
		if (bulletOnFlag == 1) {
			glTranslatef(position.x, position.y, position.z);
			glutSolidSphere(0.5f, 30, 30);
		}
	}
};

Character player;
std::vector<Character> enemy;

void read_texture() {

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

void field() {

	float y = 0;

	for (int z = 0; z < VTX; z++) {
		for (int x = 0; x < VTX; x++) {
			//texture
			tex.push_back((float)x / (VTX - 1));
			tex.push_back((float)z / (VTX - 1));

			y = (pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].r + pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].g + pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].b) / 3.f / 100 -1;

			//vertex
			vtx.push_back(/*-1.f +*/ (float)x /*/ (VTX / 2)*/);
			vtx.push_back(y);
			vtx.push_back(/*-1.f +*/ (float)z /*/ (VTX / 2)*/);

			//normal
			normal.push_back(0);
			normal.push_back(1);
			normal.push_back(0);
		}
	}

	for (int z = 0; z < VTX - 1; z++) {//行の数
		for (int x = 0; x < VTX - 1; x++) {//列の数
			index.push_back(VTX * z + x);
			index.push_back(VTX * z + x + VTX);
			index.push_back(VTX * z + x + 1);
			index.push_back(VTX * z + x + 1);
			index.push_back(VTX * z + x + VTX);
			index.push_back(VTX * z + x + VTX + 1);
		}
	}

}



void keyboard(unsigned char key, int x, int y) {
	keys[key] = 1;
}
void keyboardUp(unsigned char key, int x, int y) {
	keys[key] = 0;
}
void keyboardOperate() {

	if (keys['w'] == 1) {
		player.speed.x -= sin(player.angle * M_PI / 180) / 100;
		player.speed.z -= cos(player.angle * M_PI / 180) / 100;
	}
	if (keys['s'] == 1) {
		player.speed.x += sin(player.angle * M_PI / 180) / 100;
		player.speed.z += cos(player.angle * M_PI / 180) / 100;
	}
	if (keys['a'] == 1) {
		player.speed.x -= cos(player.angle * M_PI / 180) / 100;
		player.speed.z -= sin(player.angle * M_PI / 180) / 100;
	}
	if (keys['d'] == 1) {
		player.speed.x += cos(player.angle * M_PI / 180) / 100;
		player.speed.z += sin(player.angle * M_PI / 180) / 100;
	}


	if (keys[' '] == 1 && keys['w'] == 1) {
		player.speed.x -= sin(player.angle * M_PI / 180) / 10;
		player.speed.z -= cos(player.angle * M_PI / 180) / 10;
	}
	else if (keys[' '] == 1 && keys['s'] == 1) {
		player.speed.x += sin(player.angle * M_PI / 180) / 10;
		player.speed.z += cos(player.angle * M_PI / 180) / 10;
	}
	else if (keys[' '] == 1) {
		player.speed.y = 0.5f;
	}
}

void joystick(unsigned int buttonMask, int x, int y, int z) {
	printf("buttonMask:%u,x:%d,y:%d,z:%d\n", buttonMask, x, y, z);
	const int pressedKeys = buttonMask;				//押しているボタン
	const int releasedKeys = ~buttonMask;			//離れたボタン
	const int changedKeys = lastkeys ^ pressedKeys;	//切り替わったボタン
	const int downKeys = changedKeys & pressedKeys;	//今押したボタン
	const int upKeys = changedKeys & releasedKeys;		//今離れたボタン

	/*if (pressedKeys == 16) {
		angle_speed += M_PI / 180 * 10.0f / 3;
	}
	if (pressedKeys == 32) {
		angle_speed -= M_PI / 180 * 10.0f / 3;
	}*/

	if (x < -200 || 200 < x) {
		player.speed.x += cos(player.angle * M_PI / 180) * x;
		player.speed.z -= sin(player.angle * M_PI / 180) * x;
		player.angle = -atan2(x / 1000.f, -y / 1000.f) * 180 / M_PI;//+ angle;
	}

	if (y < -200 || 200 < y) {
		player.speed.x += sin(player.angle * M_PI / 180) * y;
		player.speed.z += cos(player.angle * M_PI / 180) * y;
		player.angle = -atan2(x / 1000.f, -y / 1000.f) * 180 / M_PI; //+ angle;
	}



	lastkeys = buttonMask;					//このシーンで押していたボタンを保存するため
}

void timer(int value) {
	glutPostRedisplay();	//再描画
	glutTimerFunc(
		1000 / 60,			//unsigned int millis,		何秒後に起動するか  1000で1秒
		timer,				//void (GLUTCALLBACK *func)(int value)	なにを起動するのか
		0);					//int value		引数
}

void display(void) {

	glutForceJoystickFunc();
	keyboardOperate();
	player.update();
	glColor3f(1, 1, 1);

	while (4 > enemy.size()) {
		Character gm;
		gm.position.x = rand() % 40 - 20;
		gm.position.y = rand() % 5;
		gm.position.z = rand() % 40 - 20;

		enemy.push_back(gm);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//カメラ設定-----------------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		表示するものの角度
		1000.f / 700.f,		//GLdouble aspect	表示する幅に対する高さ
		1,	//GLdouble zNear,		表示の一番近いところ
		500);	//GLdouble zFar		表示の一番遠いところ

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		player.position.x + 4 * sin(player.angle  *  M_PI / 180)*1.0f, player.position.y + 4, player.position.z + 4 * cos(player.angle * M_PI / 180)*1.0f,
		player.position.x, player.position.y, player.position.z,
		0, 1, 0);

	//テクスチャ設定

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

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);

	//フィールド------------------------------------------

	float *v = vtx.data();
	float *n = normal.data();
	unsigned int *ind = index.data();
	float *t = tex.data();

	glVertexPointer(3, GL_FLOAT, 0, v);

	glNormalPointer(GL_FLOAT, 0, n);

	glTexCoordPointer(2, GL_FLOAT, 0, t);
	
	glPushMatrix();
	{
		glTranslatef(-128, 0, -128);
		glDrawElements(
			GL_TRIANGLES,
			index.size(),
			GL_UNSIGNED_INT,
			ind);
	}
	glPopMatrix();
	

	/*glPushMatrix();
	{
		glColor3f(1, 1, 1);
		for (int x = -30; x <= 30; x++) {
			glBegin(GL_LINES);
			{
				glVertex3f(x, 0, -30);
				glVertex3f(x, 0, 30);
			}
			glEnd();
			glBegin(GL_LINES);
			{
				glVertex3f(-30, 0, x);
				glVertex3f(30, 0, x);
			}
			glEnd();
		}
	}
	glPopMatrix();*/


	//プレイヤー---------------------------------
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	{
		glTranslatef(player.position.x, player.position.y, player.position.z);
		player.playerDraw();
	}
	glPopMatrix();

	//エネミ-------------------------------------
	for (int i = 0; i < enemy.size(); i++) {
		glPushMatrix();
		{
			glTranslatef(enemy[i].position.x, enemy[i].position.y, enemy[i].position.z);
			enemy[i].enemyDraw();
		}
		glPopMatrix();
	}


	
	glFlush();
}

int main(int argc, char *argv[]) {
	read_texture();
	field();
	glutInit(&argc, argv);
	glutInitWindowSize(1000, 700);
	glutCreateWindow("game");
	glutDisplayFunc(display);
	glEnable(GL_DEPTH_TEST);
	glutTimerFunc(0, timer, 0);
	glutJoystickFunc(joystick, 0);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutIgnoreKeyRepeat(GL_TRUE);
	glutMainLoop();
}