#define _USE_MATH_DEFINES

#include<stdio.h>
#include<math.h>
#include<vector>
#include"glut.h"
#include"glm\glm.hpp"

unsigned char keys[256] = {};

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
	}

	void draw(){}
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

void keyboard(unsigned char key, int x, int y) {
	keys[key] = 1;
}
void keyboardUp(unsigned char key, int x, int y) {
	keys[key] = 0;
}
void keyboardOperate() {
	if (keys['w'] == true) {
		player.speed.x -= sin(player.angle * M_PI / 180) / 200;
		player.speed.z -= cos(player.angle * M_PI / 180) / 200;
	}
	if (keys['s'] == true) {
		player.speed.x += sin(player.angle * M_PI / 180) / 200;
		player.speed.z += cos(player.angle * M_PI / 180) / 200;
	}

	if (keys[' '] == true) {
		player.speed.y = 1;
	}
}

void joystick(unsigned int buttonMask, int x, int y, int z) {
	printf("buttonMask:%u,x:%d,y:%d,z:%d\n", buttonMask, x, y, z);
	//pressedKeys = buttonMask;				//押しているボタン
	//releasedKeys = ~buttonMask;				//離れたボタン
	//changedKeys = lastkeys ^ pressedKeys;	//切り替わったボタン
	//downKeys = changedKeys & pressedKeys;	//今押したボタン
	//upKeys = changedKeys & releasedKeys;	//今離れたボタン



	//lastkeys = buttonMask;					//このシーンで押していたボタンを保存するため
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

	float gravity = 0.098;

	player.speed.y -= gravity;

	if (player.position.y < 0) {
		player.position.y = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//カメラ設定-----------------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		表示するものの角度
		1000.f/700,		//GLdouble aspect	表示する幅に対する高さ
		1,	//GLdouble zNear,		表示の一番近いところ
		500);	//GLdouble zFar		表示の一番遠いところ

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		player.position.x + 4 * sin(player.angle  *  M_PI / 180)*1.0f, player.position.y + 4, player.position.z + 4 * cos(player.angle * M_PI / 180)*1.0f,	
		player.position.x, player.position.y, player.position.z,
		0, 1, 0);

	//フィールド------------------------------------------
	glColor3f(1, 1, 1);
	glPushMatrix();
	{
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
	glPopMatrix();

	//プレイヤー---------------------------------
	glTranslatef(player.position.x, player.position.y, player.position.z);
	glutSolidCube(1);


	glFlush();
}

int main(int argc, char *argv[]) {
	glutInit(&argc,argv);
	glutInitWindowSize(1000, 700);
	glutCreateWindow("game");
	glutDisplayFunc(display);
	glEnable(GL_DEPTH_TEST);
	glutTimerFunc(0,timer,0);
	glutJoystickFunc(joystick,0);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutIgnoreKeyRepeat(GL_TRUE);
	glutMainLoop();
}