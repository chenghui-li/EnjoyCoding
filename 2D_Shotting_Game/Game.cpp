#include "DirectSound.h"
#include "MyDirectX.h"
#include "player.h"
#include <sstream>
#include "emy.h"
#include "emyPlane.h"
#include<list>
using namespace std;
const string APPTITLE = "Shooting Game";
const int SCREENW = 1024;
const int SCREENH = 768;
long playerWidth;
long playerHeight;
LPDIRECT3DTEXTURE9 boom[10];
LPDIRECT3DTEXTURE9 explore = NULL;
LPDIRECT3DTEXTURE9 background = NULL;
int background_pos = 2000;
//float boom_x;
//float boom_y;
clock_t cstart;
clock_t ccend;
int frame = 0;
int starttime = 0;
list<class emy *> mylist;
player *aPlayer = player::Instence();
void get_emy() {
	
	for (int i = 1; i < 18; i++) {
		float R = (rand()% SCREENW )*1.0/ (1+SCREENW);
		emy *aEmy = new emyPlane(SCREENW*R , -300, 7 *i+5, i +5,2);		//越靠后的敌机，伤害越大，血量越大
		string s1 = "amy/Amy";		//拼接字符串
		string s2 = ".png";
		string result = "";
		ostringstream oss;
		oss << s1 << i << s2;
		result += oss.str();
		aEmy->init_emy(result,"player/B2.png");
		for (int i = 0; i < 2; i++) {
			emy *bEmy = aEmy->Clone();
			mylist.push_back(bEmy);
		}
	}
}
void control() {
	int x = aPlayer->get_player_y();
	if (aPlayer->player_alive() == 2) {
		if (Key_Down(DIK_LEFT))	aPlayer->sub_x();
		if (Key_Down(DIK_RIGHT)) aPlayer->add_x();
		if (Key_Down(DIK_DOWN)) {
			aPlayer->add_y();
			if (x >= (int)400)
				background_pos += 4.0f;
		}
		if (Key_Down(DIK_UP)) {
			aPlayer->sub_y();
			if (x <= (int)300)
				background_pos -= 4.0f;
		}
	}

}
void border() {
	aPlayer->set_left();
	aPlayer->set_right(SCREENW - playerWidth);
	aPlayer->set_up();
	aPlayer->set_down(SCREENH - playerHeight);
}
void bullet_init(int ms) {
	if (aPlayer->player_alive() == 2) {
		if (ms % 5 == 0) {
			aPlayer->judge_bullet();
		}
	}
}
void init_boom() {
	string s1 = "boom/";		//拼接字符串
	string s2 = ".png";
	for (int i = 1; i < 10; i++) {
		string result = "";
		ostringstream oss;
		oss << s1 << i  << s2;
		result += oss.str();
		boom[i] = LoadTexture(result);
	}
}
bool Game_Init(HWND window)
{
	Direct3D_Init(window, SCREENW, SCREENH, false);
	DirectInput_Init(window);
	DirectSound_Init(window);
	
	aPlayer->init_player("player/p1.png", "1.png");
	init_boom();
	explore = LoadTexture("boom.png");
	background = LoadTexture("back1.png");
	/*player_surf = LoadSurface("player/p1.png"); //加载图片
	bullet_surf = LoadSurface("1.png");
	bullet1 = LoadTexture("1.png");
	player1 = LoadTexture("player/p1.png");
	player_size = GetBitmapSize("player/p1.png");*/

	/*player_surf->GetDesc(&desc);*/
	playerWidth = aPlayer->get_p_img_width();
	playerHeight = aPlayer->get_p_img_height();

	cstart = clock();
	get_emy();		//所有敌机初始化
	/*if (!player_surf)
	{
		MessageBox(window, "Error Loading player!", "Error", MB_OK);
		return false;
	}*/
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	/*for (int i = 0; i < 200; i++)
	{
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].alive = false;
	}*/
	return true;
}
void Game_Run(HWND window) {
	ccend = clock();
	if (!d3ddev) return;
	DirectInput_Update();
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));

	if (aPlayer->player_alive() == 2) {
		control();		//键盘控制
		border();		//只在窗口内移动
		aPlayer->move_bullet();		//如果子弹存在，则移动，不存在，定义发射位置
		bullet_init(ccend - cstart);//间隔一定时间，将不存在的子弹创建
		aPlayer->get_player_x_max();
	}

	if (!mylist.empty()) {
		aPlayer->Collision(mylist.front());
		if (mylist.front()->get_blood() <= 0) {
			mylist.front()->change_alive(1);
		}
		if (mylist.front()->get_y() > SCREENH) {
			//mylist.front().emy_alive = false;
			mylist.pop_front();
		}
		else {
			mylist.front()->get_emy_height();
			mylist.front()->get_emy_width();
			mylist.front()->move_bullet();
			mylist.front()->get_bullet(ccend - cstart);
			mylist.front()->add_y();
		}

	}
	aPlayer->bullet_over();		//如果飞出边界，子弹定义为不存在

		

	//绘制
	if (d3ddev->BeginScene())
	{
		D3DXVECTOR3 pos(aPlayer->get_player_x(), aPlayer->get_player_y(), 0);
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

		D3DXVECTOR3 pos2(0, 0, 0);		//加背景
		RECT rect;
		rect.left = 0;
		rect.right = 1024;
		rect.top = background_pos;
		rect.bottom = background_pos + 768;
		spriteobj->Draw(background, &rect, NULL, &pos2, D3DCOLOR_XRGB(255, 255, 255));
		if(aPlayer->player_alive() == 2)	//画玩家
			spriteobj->Draw(aPlayer->get_img(), NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
		else if (aPlayer->player_alive() == 1) {	//刚死alive = 1，画爆炸效果
			/*for (int i = 1; i < 10; i++) {
				spriteobj->Draw(boom[i], NULL, NULL, &pos, D3DCOLOR_XRGB(255, 255, 255));
				
			}*/
			Sprite_Animate(frame, 0, 8, 1, starttime, 30);
			Sprite_Draw_Frame(explore, aPlayer->get_player_x(), aPlayer->get_player_y(), frame, 150, 150, 3);
			aPlayer->change_alive(0);
		}
		for (int i = 0; i < 20; i++)
		{
			D3DXVECTOR3 pos1 (aPlayer->get_bullet_x(i), aPlayer->get_bullet_y(i), 0);
			if (aPlayer->bullet_alive(i) == true)
				spriteobj->Draw(aPlayer->get_bullet_img(), NULL, NULL, &pos1, D3DCOLOR_XRGB(255, 255, 255));
		}

		if (!mylist.empty()) {
			emy *aEmy = mylist.front();
			D3DXVECTOR3 pos1(aEmy->get_x(), aEmy->get_y(), 0);
			for (int i = 0; i < 20; i++) {
				D3DXVECTOR3 pos1(aEmy->get_bullet_x(i), aEmy->get_bullet_y(i), 0);
				if(aEmy->bullet_alive(i))
					spriteobj->Draw(aEmy->get_bullet_img_b(), NULL, NULL, &pos1, D3DCOLOR_XRGB(255, 255, 255));
			}
			if(mylist.front()->get_alive() == 2)
				spriteobj->Draw(aEmy->get_img_b(), NULL, NULL, &pos1, D3DCOLOR_XRGB(255, 255, 255));
			if (mylist.front()->get_alive() == 1) {
				/*for (int i = 1; i < 10; i++) {
					spriteobj->Draw(boom[i], NULL, NULL, &pos1, D3DCOLOR_XRGB(255, 255, 255));

				}*/
				int id = DSound_Load_WAV(window, (LPWSTR)"wind.wav");
				sound_fx[id].dsbuffer->Play(0, 0, NULL);
				Sprite_Animate(frame, 0, 8, 1, starttime, 30);
				Sprite_Draw_Frame(explore,mylist.front()->get_x(),mylist.front()->get_y(), frame, 150, 150, 3);

				mylist.pop_front();
			}
		}
		spriteobj->End();
		d3ddev->EndScene();	
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
	if (Key_Down(DIK_BACKSPACE))
		gameover = true;
}

void Game_End()
{
	if (aPlayer->get_img()) aPlayer->get_img()->Release();
	if (aPlayer->get_img_surf()) aPlayer->get_img_surf()->Release();
	Direct3D_Shutdown();
	DirectInput_Shutdown();
	DirectSound_Shutdown();
}