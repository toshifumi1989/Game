#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define VTX (256)

#include<stdio.h>
#include<math.h>
#include<Windows.h>
#include<vector>
#include<list>
#include<time.h>
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

GLuint textures[2];

class Object {
	//using vec = glm::vec3;
public:
	Object() {}
	~Object() {}

	glm::vec3 position;
	glm::vec3 speed = glm::vec3(0, 0, 0);

	float angle = 0;
	float lastAngle = 0;

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

	int HP = 10;
	int MaxHP = 10;
	unsigned char action = 0;
	glm::vec3 lastPosition;

	char targetNum = 0;
	glm::vec3 targetPos;

	void update() {

		speed.x *= 0.95;
		speed.z *= 0.95;
		speed.y -= 0.05;
		position += speed;

		if (position.y < 0) {
			position.y = 0;
		}
	}

	void draw() {};

	void playerDraw() {
		glColor3f(0, 0, 1);

		glBegin(GL_TRIANGLES);
		{
			glVertex3f(0, 1, 2);
			glVertex3f(0.5f, 1, 0);
			glVertex3f(-0.5f, 1, 0);
		}
		glEnd();
		glTranslatef(0, 1, 2);
		glutSolidCube(0.5f);
	}

	void enemyDraw() {
		glColor3f(1, 0, 0);
		glutSolidSphere(0.5f, 30, 30);
		glTranslatef(0, 0, 1);
		glutSolidCube(1);
	}

};


class Bullet :public Object {
public:
	Bullet() {}
	~Bullet() {}
	Bullet(glm::vec3 _pos) {
		position = _pos;
	}

	bool OnFlag = false;

	void update() {
		position += speed;
	}

	void draw() {
		glutSolidSphere(0.2f, 30, 30);
	}
};

Character player;

std::vector<Character> enemy;

std::list<Bullet> playerBullet;
std::list<Bullet> enemyBullet;

class Camera {
public:
	Camera() {}
	~Camera() {}

	float angle = 0;

	glm::vec3 eyes;
	glm::vec3 eyesSpeed;
	glm::vec3 targetPos;
};

Camera camera;

class Effect {
public:
	Effect() {}
	~Effect() {}

	float alpha;

	glm::vec3 position;
	glm::vec3 scale;

};


void read_texture(const char* _texture) {

	FILE *pFile = fopen(_texture, "rb");
	assert(pFile != NULL);

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, pFile);

	int size = bi.biWidth *bi.biHeight * sizeof(RGB);
	pixels = (RGB*)malloc(size);
	fread(pixels, size, 1, pFile);

	fclose(pFile);

	//RGB��R��B�̓���ւ�
	for (int i = 0; i < bi.biWidth * bi.biHeight; i++) {
		char sub = pixels[i].b;
		pixels[i].b = pixels[i].r;
		pixels[i].r = sub;
	}

	//�摜�̏㉺���]
	for (int p = 0; p < bi.biHeight / 2; p++) {
		for (int q = 0; q < bi.biWidth; q++) {
			RGB sub = pixels[bi.biWidth * p + q];
			pixels[bi.biWidth * p + q] = pixels[bi.biWidth *(bi.biHeight - 1 - p) + q];
			pixels[bi.biWidth *(bi.biHeight - 1 - p) + q] = sub;
		}
	}


	//glTexImage2D(
	//	GL_TEXTURE_2D,		//GLenum target,
	//	0,					//GLint level,
	//	GL_RGB,				//GLint internalformat, 
	//	bi.biWidth, bi.biHeight,	//GLsizei width, GLsizei height, 
	//	0,					//GLint border, 
	//	GL_RGB,				//GLenum format, 
	//	GL_UNSIGNED_BYTE,	//GLenum type, 
	//	pixels);			//const GLvoid *pixels

}



void field() {

	float y = 0;

	for (int z = 0; z < VTX; z++) {
		for (int x = 0; x < VTX; x++) {
			//texture
			tex.push_back((float)x / (VTX - 1));
			tex.push_back((float)z / (VTX - 1));

			y = (pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].r + pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].g + pixels[VTX * z * (256 / VTX) + x * (256 / VTX)].b) / 3.f / 100 - 1;

			//vertex
			vtx.push_back((float)x);
			vtx.push_back(y);
			vtx.push_back((float)z);

			//normal
			normal.push_back(0);
			normal.push_back(1);
			normal.push_back(0);
		}
	}

	for (int z = 0; z < VTX - 1; z++) {//�s�̐�
		for (int x = 0; x < VTX - 1; x++) {//��̐�
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
		player.speed.x -= sin(camera.angle * M_PI / 180) / 100;
		player.speed.z -= cos(camera.angle * M_PI / 180) / 100;
	}
	if (keys['s'] == 1) {
		player.speed.x += sin(camera.angle * M_PI / 180) / 100;
		player.speed.z += cos(camera.angle * M_PI / 180) / 100;
	}
	if (keys['a'] == 1) {
		player.speed.x -= cos(camera.angle * M_PI / 180) / 100;
		player.speed.z -= sin(camera.angle * M_PI / 180) / 100;
	}
	if (keys['d'] == 1) {
		player.speed.x += cos(camera.angle * M_PI / 180) / 100;
		player.speed.z += sin(camera.angle * M_PI / 180) / 100;
	}


	if (keys[' '] == 1 && keys['w'] == 1) {
		player.speed.x -= sin(camera.angle * M_PI / 180) / 10;
		player.speed.z -= cos(camera.angle * M_PI / 180) / 10;
	}
	else if (keys[' '] == 1 && keys['s'] == 1) {
		player.speed.x += sin(camera.angle * M_PI / 180) / 10;
		player.speed.z += cos(camera.angle * M_PI / 180) / 10;
	}
	else if (keys[' '] == 1) {
		player.speed.y = 0.5f;
	}
}





void joystick(unsigned int buttonMask, int x, int y, int z) {
	printf("buttonMask:%u,x:%d,y:%d,z:%d\n", buttonMask, x, y, z);
	const int pressedKeys = buttonMask;					//�����Ă���{�^��
	const int releasedKeys = ~buttonMask;				//���ꂽ�{�^��
	const int changedKeys = lastkeys ^ pressedKeys;		//�؂�ւ�����{�^��
	const int downKeys = changedKeys & pressedKeys;		//���������{�^��
	const int upKeys = changedKeys & releasedKeys;		//�����ꂽ�{�^��




	//�^�[�Q�b�g�؂�ւ�
	if (downKeys == 16) {

		if (player.targetNum < enemy.size() - 1) {
			player.targetNum++;
			player.targetPos = enemy[player.targetNum].position;

		}
		else if (player.targetNum == enemy.size() - 1) {
			player.targetNum = 0;
			player.targetPos = enemy[player.targetNum].position;
		}

	}


	//�^�[�Q�b�g�؂�ւ�
	if (downKeys == 32) {

		if (player.targetNum > 0) {
			player.targetNum--;
			player.targetPos = enemy[player.targetNum].position;
		}
		else if (player.targetNum == 0) {
			player.targetNum = enemy.size() - 1;
			player.targetPos = enemy[player.targetNum].position;
		}

	}



	//�e�̍U��
	if (downKeys == GLUT_JOYSTICK_BUTTON_C) {

		Bullet bullet;
		bullet.OnFlag = true;
		bullet.position = player.position;

		//�e�̕����Ɛ��K��

		glm::vec3 bulletToEnemy(enemy[player.targetNum].position.x - player.position.x, enemy[player.targetNum].position.y - player.position.y, enemy[player.targetNum].position.z - player.position.z);

		bullet.speed = glm::normalize(bulletToEnemy) * 1.2f;
		playerBullet.push_back(bullet);

		//player.angle = camera.angle;

	}



	//-----------------------------------------
	//����
	//-----------------------------------------


	//�W�����v
	if (downKeys == GLUT_JOYSTICK_BUTTON_A) {
		player.speed.y = 1.5f;
	}

	//�v���C���[�̓��͊p�x
	float angle;
	float playerRad = player.angle * (M_PI / 180.0f);

	glm::vec3 playerDirec = glm::vec3(-sin(playerRad), 0, -cos(playerRad));
	glm::vec3 targetDirec;

	glm::vec2 stickDirec(x, y);

	if (glm::dot(stickDirec, stickDirec) > 200 * 200) {

		angle = -atan2(x / 1000.f, -y / 1000.f) * 180 / M_PI + camera.angle;
		float targetRad = angle * (M_PI / 180.0f);
		targetDirec = glm::vec3(-sin(targetRad), 0, -cos(targetRad));
		player.speed.x += cos(camera.angle * M_PI / 180) * x / 20000 + sin(camera.angle * M_PI / 180) * y / 20000;
		player.speed.z += cos(camera.angle * M_PI / 180) * y / 20000 - sin(camera.angle * M_PI / 180) * x / 20000;

	}
	else {
		float cameraRad = camera.angle * (M_PI / 180.0f);
		targetDirec = glm::vec3(-sin(cameraRad), 0, -cos(cameraRad));
		angle = camera.angle;
	}

	float resultY = glm::cross(playerDirec, targetDirec).y;
	float resultDot = glm::dot(playerDirec, targetDirec);
	float torque = (-resultDot + 1) * 0.5f + 0.3f;

	if (resultY*resultY < (10 * M_PI / 180) * (10 * M_PI / 180)) {
		if (0 < resultDot) {
			player.angle = angle;
		}
	}
	else if (resultY > 0) {
		player.angle += 30.0f *torque;
	}
	else {
		player.angle -= 30.0f*torque;
	}


	lastkeys = buttonMask;					//���̃V�[���ŉ����Ă����{�^����ۑ����邽��

}

void timer(int value) {
	glutPostRedisplay();	//�ĕ`��
	glutTimerFunc(
		1000 / 60,			//unsigned int millis,		���b��ɋN�����邩  1000��1�b
		timer,				//void (GLUTCALLBACK *func)(int value)	�Ȃɂ��N������̂�
		0);					//int value		����
}

void display(void) {

	srand(time(NULL));//�����_���̂��߂ɏ�����

	//�J�����̊p�x
	camera.angle = atan2(player.position.x - enemy[player.targetNum].position.x, player.position.z - enemy[player.targetNum].position.z) * 180 / M_PI;

	//���Ŕ���(�G�l�~�[
	for (std::vector<Character>::iterator enemyIter = enemy.begin();
	enemyIter != enemy.end();) {
		Character &enemy_it = *enemyIter;
		if (enemyIter->HP <= 0) {
			enemyIter = enemy.erase(enemyIter);

		}
		else {
			enemyIter++;
		}

	}


	glutForceJoystickFunc();	//�W���C�X�e�B�b�N
	keyboardOperate();			//�L�[�{�[�h
	player.update();			//�v���C���[�̍X�V
	player.targetPos = enemy[player.targetNum].position;
	//�G�̍X�V
	for (int i = 0; i < enemy.size(); i++) {

		enemy[i].position += enemy[i].speed;
		enemy[i].speed *= 0.95f;

	}


	glColor3f(1, 1, 1);		//�F�̏�����
	glClearColor(0, 0.39f, 1, 0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	//�e�N�X�`���ݒ�----------------------------------------------------------

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

	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_CULL_FACE);		//�J�����O
	glEnable(GL_DEPTH_TEST);	//�[�x�e�X�g



	//�t�B�[���h-------------------------------------------------------

	const float *v = vtx.data();
	const float *n = normal.data();
	const unsigned int *ind = index.data();
	const float *t = tex.data();

	glVertexPointer(3, GL_FLOAT, 0, v);

	glNormalPointer(GL_FLOAT, 0, n);

	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glPushMatrix();
	{

		glTranslatef(-127, 0, -127);
		glDrawElements(
			GL_TRIANGLES,
			index.size(),
			GL_UNSIGNED_INT,
			ind);
	}
	glPopMatrix();


	//�v���C���[--------------------------------------------------------------------------------------

	//�v���C���[���t�B�[���h����łȂ��悤�ɒ���
	if (player.position.x >= 127 || player.position.x <= -127) {
		player.speed.x = 0;
		player.position.x = player.lastPosition.x;
	}
	if (player.position.z >= 127 || player.position.z <= -127) {
		player.speed.z = 0;
		player.position.z = player.lastPosition.z;
	}
	if (player.position.y < 0) {
		player.position.y = 0;
	}

	printf("%f", player.angle);

	//�v���C���[�`��
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	{

		glTranslatef(player.position.x, player.position.y, player.position.z);
		glRotatef(player.angle, 0, 1, 0);

		player.playerDraw();

	}
	glPopMatrix();

	glPushMatrix();
	{
		glOrtho(0, 1000, 700, 0, -1, 1);// ���ˉe�ϊ��ݒ�

		glColor3f(0, player.HP / player.MaxHP, 0);
		glBegin(GL_QUADS);
		{
			glVertex2f(0, 0);
			glVertex2f(player.HP / 10.f, 0);
			glVertex2f(player.HP / 10.f, 0.5f);
			glVertex2f(0, 0.5f);
		}
		glEnd();
	}
	glPopMatrix();

	player.lastPosition = player.position;

	//�v���C���[�o���b�g�`��

	std::list<Bullet>::iterator playerBulletIter = playerBullet.begin();

	while (playerBulletIter != playerBullet.end()) {

		playerBulletIter->update();

		//�G�Ƃ����蔻��
		for (int i = 0; i < enemy.size(); i++) {

			const float bulletToEnemy =
				(playerBulletIter->position.x - enemy[i].position.x) * (playerBulletIter->position.x - enemy[i].position.x)
				+ (playerBulletIter->position.y - enemy[i].position.y) * (playerBulletIter->position.y - enemy[i].position.y)
				+ (playerBulletIter->position.z - enemy[i].position.z) * (playerBulletIter->position.z - enemy[i].position.z);

			if (bulletToEnemy <= 1) {
				enemy[i].HP -= 2;
				playerBulletIter->OnFlag = false;
			}

		}

		//�t�B�[���h�̊O�ɂł�
		if (playerBulletIter->position.x >= 127 || playerBulletIter->position.x <= -127 ||
			playerBulletIter->position.z >= 127 || playerBulletIter->position.z <= -127) {
			playerBulletIter->OnFlag = false;
		}

		//���Ŕ���
		if (playerBulletIter->OnFlag == false) {
			playerBulletIter = playerBullet.erase(playerBulletIter);
		}
		else {
			glPushMatrix();
			{
				glTranslatef(playerBulletIter->position.x, playerBulletIter->position.y, playerBulletIter->position.z);
				playerBulletIter->draw();
				++playerBulletIter;
			}
			glPopMatrix();
		}


	}


	//�G�l�~------------------------------------------------------------------------------------------------------------

	//�G�̏o������
	for (int i = 0; i < enemy.size(); i++) {

		if (enemy[i].HP <= 0) {

			for (int j = 0; j < 2; j++) {
				Character gm;
				gm.action = rand() % 60;
				gm.HP = 10;
				gm.position.x = rand() % 400 - 200;
				gm.position.y = rand() % 20;
				gm.position.z = rand() % 400 - 200;

				enemy.push_back(gm);
			}

		}

	}

	//�G�̓���

	for (int i = 0; i < enemy.size(); i++) {
		enemy[i].action++;//�s���̊Ԋu���L���邽�߂ɃJ�E���g


		//�P�b���Ƃɓ��삪�ς��
		if (enemy[i].action == 120) {
			enemy[i].action = 0;
			enemy[i].speed = glm::vec3(0, 0, 0);

			int random = rand() % 5;

			if (random == 0 || random == 1) {
				if (random == 0) {
					enemy[i].speed.x = 1;
				}
				else {
					enemy[i].speed.x = -1;
				}

			}
			else if (random == 2 || random == 3) {
				if (random == 2) {
					enemy[i].speed.z = 1;
				}
				else {
					enemy[i].speed.z = -1;
				}

			}
			else {
				//�o���b�g(�G�l�~�[

				Bullet bullet;
				bullet.OnFlag = true;
				bullet.position = enemy[i].position;

				glm::vec3 bulletToPlayer(player.position.x - enemy[i].position.x, player.position.y - enemy[i].position.y, player.position.z - enemy[i].position.z);
				bullet.speed = glm::normalize(bulletToPlayer);

				enemyBullet.push_back(bullet);
			}

		}

		if (enemy[i].position.x > 127 || enemy[i].position.x < -127 ||
			enemy[i].position.z > 127 || enemy[i].position.x < -127) {
			enemy[i].position = enemy[i].lastPosition;
		}

		enemy[i].lastPosition = enemy[i].position;
	}


	//�G�l�~�[�`��

	for (int i = 0; i < enemy.size(); i++) {

		glPushMatrix();
		{
			enemy[i].angle = atan2(enemy[player.targetNum].position.x - player.position.x, enemy[player.targetNum].position.z - player.position.z) * 180 / M_PI;

			glTranslatef(enemy[i].position.x, enemy[i].position.y, enemy[i].position.z);
			glRotatef(enemy[i].angle, 0, 1, 0);
			enemy[i].enemyDraw();

			glm::mat4 view;
			glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&view);

			glm::mat4 m = glm::inverse(view);
			m[3][0] = m[3][1] = m[3][2] = 0;

			glMultMatrixf((float*)&m);

			glTranslatef(-1, 2, 0);

			glColor3f(1, 1, 0);
			glBegin(GL_QUAD_STRIP);
			{

				glVertex3f(enemy[i].HP / 5.f, 0.25f, 0);
				glVertex3f(0, 0.25f, 0);
				glVertex3f(enemy[i].HP / 5.f, -0.25f, 0);
				glVertex3f(0, -0.25f, 0);
			}
			glEnd();

		}
		glPopMatrix();
	}


	//�G�l�~�[�o���b�g�`��Ɠ����蔻��

	std::list<Bullet>::iterator enemyBulletIter = enemyBullet.begin();

	while (enemyBulletIter != enemyBullet.end()) {
		enemyBulletIter->update();

		const float bulletToPlayer =
			(enemyBulletIter->position.x - player.position.x) * (enemyBulletIter->position.x - player.position.x)
			+ (enemyBulletIter->position.y - player.position.y) * (enemyBulletIter->position.y - player.position.y)
			+ (enemyBulletIter->position.z - player.position.z) * (enemyBulletIter->position.z - player.position.z);

		if (bulletToPlayer <= 1) {
			player.HP -= 1;
			enemyBulletIter->OnFlag = false;
		}

		if (enemyBulletIter->position.x >= 127 || enemyBulletIter->position.x <= -127 ||
			enemyBulletIter->position.z >= 127 || enemyBulletIter->position.z <= -127) {
			enemyBulletIter->OnFlag = false;
		}


		if (enemyBulletIter->OnFlag == false) {
			enemyBulletIter = enemyBullet.erase(enemyBulletIter);
		}
		else {
			glPushMatrix();
			{
				glColor3f(1, 1, 1);
				glTranslatef(enemyBulletIter->position.x, enemyBulletIter->position.y, enemyBulletIter->position.z);
				enemyBulletIter->draw();
				++enemyBulletIter;
			}
			glPopMatrix();
		}
	}



	//�J�����ݒ�------------------------------------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		�\��������̂̊p�x
		1000.f / 700.f,		//GLdouble aspect	�\�����镝�ɑ΂��鍂��
		1,	//GLdouble zNear,		�\���̈�ԋ߂��Ƃ���
		500);	//GLdouble zFar		�\���̈�ԉ����Ƃ���

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	camera.eyes.x = player.position.x + 4 * sin(camera.angle * M_PI / 180) * 1.0f;
	camera.eyes.y = player.position.y + 2;
	camera.eyes.z = player.position.z + 4 * cos(camera.angle * M_PI / 180) * 1.0f;


	camera.targetPos.x = (player.position.x + player.targetPos.x) / 2;
	camera.targetPos.y = (player.position.y + player.targetPos.y) / 2;
	camera.targetPos.z = (player.position.z + player.targetPos.z) / 2;


	gluLookAt(
		camera.eyes.x, camera.eyes.y, camera.eyes.z,
		camera.targetPos.x, camera.targetPos.y, camera.targetPos.z,
		0, 1, 0);



	glFlush();
}



int main(int argc, char *argv[]) {
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	read_texture("texture.bmp");
	field();

	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//read_texture("target.bmp");


	//�G�̏o��
	while (10 > enemy.size()) {
		Character gm;
		gm.action = rand() % 60;
		gm.position.x = rand() % 200 - 100;
		gm.position.y = rand() % 20;
		gm.position.z = rand() % 200 - 100;

		enemy.push_back(gm);

	}

	player.targetNum = 0;
	player.targetPos = enemy[0].position;
	player.angle = atan2(player.position.x - enemy[player.targetNum].position.x, player.position.z - enemy[player.targetNum].position.z) * 180 / M_PI;

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