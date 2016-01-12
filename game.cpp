#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define GRAVITY (0.05f)
#define VTX (256)

#include<stdio.h>
#include<math.h>
#include<Windows.h>
#include<vector>
#include<list>
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

	glm::vec3 lastPosition;

	bool targetOn = false;
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

	//RGBのRとBの入れ替え
	for (int i = 0; i < bi.biWidth * bi.biHeight; i++) {
		char sub = pixels[i].b;
		pixels[i].b = pixels[i].r;
		pixels[i].r = sub;
	}

	//画像の上下反転
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
			vtx.push_back((float)x * 2);
			vtx.push_back(y);
			vtx.push_back((float)z * 2);

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
	const int pressedKeys = buttonMask;					//押しているボタン
	const int releasedKeys = ~buttonMask;				//離れたボタン
	const int changedKeys = lastkeys ^ pressedKeys;		//切り替わったボタン
	const int downKeys = changedKeys & pressedKeys;		//今押したボタン
	const int upKeys = changedKeys & releasedKeys;		//今離れたボタン

														//プレイヤーの向き


	//ターゲットロックオン
	if (downKeys == GLUT_JOYSTICK_BUTTON_B) {
		if (player.targetOn == true) {
			player.targetOn = false;
			player.targetNum = 0;
		}
		else {
			player.targetOn = true;
			player.targetPos = enemy[player.targetNum].position;
		}

	}

	//ターゲット切り替え
	if (downKeys == 16) {
		if (player.targetOn == true) {
			if (player.targetNum < enemy.size() - 1) {
				player.targetNum++;
				player.targetPos = enemy[player.targetNum].position;

			}
			else if (player.targetNum == enemy.size() - 1) {
				player.targetNum = 0;
				player.targetPos = enemy[player.targetNum].position;
			}
		}
	}

	//ターゲット切り替え
	if (downKeys == 32) {
		if (player.targetOn == true) {
			if (player.targetNum > 0) {
				player.targetNum--;
				player.targetPos = enemy[player.targetNum].position;
			}
			else if (player.targetNum == 0) {
				player.targetNum = enemy.size() - 1;
				player.targetPos = enemy[player.targetNum].position;
			}
		}
	}


	if (player.targetOn == true) {

		camera.angle = atan2(player.position.x - enemy[player.targetNum].position.x, player.position.z - enemy[player.targetNum].position.z) * 180 / M_PI;

		
			if ((camera.angle - player.angle) > 1) {
				player.angle += (camera.angle - player.angle)/10.f;
			}
			
			

		
		
			if ((camera.angle - player.angle) < -1) {
				player.angle += (camera.angle - player.angle) / 10.f;
			}
			
	}


	//-----------------------------------------
	//動作
	//-----------------------------------------
	//ジャンプ
	if (downKeys == GLUT_JOYSTICK_BUTTON_A) {
		player.speed.y = 1.5f;
	}

	//プレイヤーの入力角度
	float angle = -atan2(x / 1000.f, -y / 1000.f) * 180 / M_PI + camera.angle;
	//横移動
	if (x < -200) {
		
		if ((angle - player.lastAngle) <-5 || (angle - player.lastAngle) > -170) {
			player.angle += (angle - player.lastAngle ) / 10.f;
			player.angle -= (camera.angle - player.angle) / 10.f;
		}
		

		player.speed.x += cos(camera.angle * M_PI / 180) * x / 20000;
		player.speed.z -= sin(camera.angle * M_PI / 180) * x / 20000;
	}

	if (200 < x) {
		if ((angle - player.lastAngle) > 5 || (angle - player.lastAngle) < 170) {
			player.angle += (angle - player.lastAngle) / 10.f;
			player.angle -= (camera.angle - player.angle) / 10.f;
		}
		


		player.speed.x += cos(camera.angle * M_PI / 180) * x / 20000;
		player.speed.z -= sin(camera.angle * M_PI / 180) * x / 20000;
	}

	//縦移動
	if (y < -200 ) {

		player.speed.x += sin(camera.angle * M_PI / 180) * y / 20000;
		player.speed.z += cos(camera.angle * M_PI / 180) * y / 20000;
	}
	else if (y > 200) {
		
		player.angle += (angle - player.lastAngle) / 10.f;

		player.speed.x += sin(camera.angle * M_PI / 180) * y / 20000;
		player.speed.z += cos(camera.angle * M_PI / 180) * y / 20000;
	}

	//弾の攻撃
	if (downKeys == GLUT_JOYSTICK_BUTTON_C) {
		if (player.targetOn == true) {
			Bullet bullet;
			bullet.OnFlag = true;
			bullet.position = player.position;

			//弾の方向と正規化

			glm::vec3 bulletToEnemy(enemy[player.targetNum].position.x - player.position.x, enemy[player.targetNum].position.y - player.position.y, enemy[player.targetNum].position.z - player.position.z);

			bullet.speed = glm::normalize(bulletToEnemy) * 1.2f;
			playerBullet.push_back(bullet);

			player.angle = camera.angle;

		}
	}

	printf("%f\n", camera.angle);

	lastkeys = buttonMask;					//このシーンで押していたボタンを保存するため
	player.lastAngle = player.angle;
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





	glColor3f(1, 1, 1);		//色の初期化
	glClearColor(0, 0.39f, 1, 0);



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	//テクスチャ設定----------------------------------------------------------

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

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//フィールド------------------------------------------

	const float *v = vtx.data();
	const float *n = normal.data();
	const unsigned int *ind = index.data();
	const float *t = tex.data();

	glVertexPointer(3, GL_FLOAT, 0, v);

	glNormalPointer(GL_FLOAT, 0, n);

	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glPushMatrix();
	{

		glTranslatef(-255, 0, -255);
		glDrawElements(
			GL_TRIANGLES,
			index.size(),
			GL_UNSIGNED_INT,
			ind);
	}
	glPopMatrix();



	//プレイヤー--------------------------------------------------------------------------------------


	//プレイヤーがフィールドからでないように調整
	if (player.position.x >= 254 || player.position.x <= -254) {
		player.speed.x = 0;
		player.position.x = player.lastPosition.x;
	}
	if (player.position.z >= 254 || player.position.z <= -254) {
		player.speed.z = 0;
		player.position.z = player.lastPosition.z;
	}
	if (player.position.y < 0) {
		player.position.y = 0;
	}


	//プレイヤー描画
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	{

		glTranslatef(player.position.x, player.position.y, player.position.z);
		glRotatef(player.angle, 0, 1, 0);

		player.playerDraw();
	}
	glPopMatrix();


	//プレイヤーバレット描画





	std::list<Bullet>::iterator playerBulletIter = playerBullet.begin();

	while (playerBulletIter != playerBullet.end()) {

		playerBulletIter->update();

		//敵とあたり判定
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

		//フィールドの外にでる
		if (playerBulletIter->position.x >= 254 || playerBulletIter->position.x <= -254 ||
			playerBulletIter->position.z >= 254 || playerBulletIter->position.z <= -254) {
			playerBulletIter->OnFlag = false;
		}

		//消滅判定
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







	//エネミ-------------------------------------

	for (int i = 0; i < enemy.size(); i++) {

		if (enemy[i].HP <= 0) {

			player.targetOn = false;

			for (int j = 0; j < 2; j++) {
				Character gm;
				gm.HP = 10;
				gm.position.x = rand() % 400 - 200;
				gm.position.y = rand() % 20;
				gm.position.z = rand() % 400 - 200;

				enemy.push_back(gm);
			}

		}

	}



	//消滅判定

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


	//エネミー描画

	for (int i = 0; i < enemy.size(); i++) {

		glPushMatrix();
		{
			glTranslatef(enemy[i].position.x, enemy[i].position.y, enemy[i].position.z);
			enemy[i].enemyDraw();
		}
		glPopMatrix();
	}


	//カメラ設定------------------------------------------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		90,		//GLdouble fovy		表示するものの角度
		1000.f / 700.f,		//GLdouble aspect	表示する幅に対する高さ
		1,	//GLdouble zNear,		表示の一番近いところ
		500);	//GLdouble zFar		表示の一番遠いところ

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (player.targetOn == true) {

		camera.eyes.x = player.position.x + 4 * sin(camera.angle * M_PI / 180) * 1.0f;
		camera.eyes.y = player.position.y + 2;
		camera.eyes.z = player.position.z + 4 * cos(camera.angle * M_PI / 180) * 1.0f;


		camera.targetPos.x = (player.position.x + player.targetPos.x) / 2;
		camera.targetPos.y = (player.position.y + player.targetPos.y) / 2;
		camera.targetPos.z = (player.position.z + player.targetPos.z) / 2;
	}
	else {

		camera.eyes.x = player.position.x + 4 * sin(camera.angle * M_PI / 180) * 1.0f;
		camera.eyes.y = player.position.y + 2;
		camera.eyes.z = player.position.z + 4 * cos(camera.angle * M_PI / 180) * 1.0f;
		camera.targetPos.x = player.position.x;
		camera.targetPos.y = player.position.y + 2;
		camera.targetPos.z = player.position.z;
	}

	gluLookAt(
		camera.eyes.x, camera.eyes.y, camera.eyes.z,
		camera.targetPos.x, camera.targetPos.y, camera.targetPos.z,
		0, 1, 0);



	player.lastPosition = player.position;
	glFlush();
}



int main(int argc, char *argv[]) {
	glGenTextures(2, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	read_texture("texture.bmp");
	field();

	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//read_texture("target.bmp");


	//敵の出現
	while (4 > enemy.size()) {
		Character gm;
		gm.position.x = rand() % 400 - 200;
		gm.position.y = rand() % 20;
		gm.position.z = rand() % 400 - 200;

		enemy.push_back(gm);
	}

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