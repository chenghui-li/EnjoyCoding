#include "player.h"
#include "MyDirectX.h"
extern const int SCREENH;
extern const int SCREENW;
player *player::_instance = NULL;
player* player::Instence() {
	if (_instance == NULL)
		_instance = new player(SCREENW / 2, SCREENW / 2, 4);
	return _instance;
}
void player::init_player(string pplayer,string pbullet) {
	p_img = LoadSurface(pplayer);
	p_img_b = LoadTexture(pplayer);
	p_bullet_img = LoadSurface(pbullet);
	p_bullet_img_b = LoadTexture(pbullet);
}

void player::set_left()
{
	if (p_x <= 0)
		p_x = 0;
}

void player::set_right(int max)
{
	if (p_x >= max)
		p_x = max;
}

void player::set_up()
{
	if (p_y <= 0)
		p_y = 0;
}

void player::set_down(int max)
{
	if (p_y >= max)
		p_y = max;
}



int player::get_p_img_width()
{
	p_img->GetDesc(&desc);
	p_img_w = desc.Width;
	return desc.Width;
}

int player::get_p_img_height()
{
	p_img->GetDesc(&desc);
	p_img_h = desc.Height;
	return desc.Height;
}

int player::get_bullet_x(int i)
{
	return pbullet[i].b_x;
}

int player::get_bullet_y(int i)
{
	return pbullet[i].b_y;
}

LPDIRECT3DTEXTURE9 player::get_img()
{
	return p_img_b;
}

LPDIRECT3DSURFACE9 player::get_img_surf()
{
	return p_img;
}

LPDIRECT3DTEXTURE9 player::get_bullet_img()
{
	return p_bullet_img_b;
}

void player::add_x()
{
	p_x += speed;
}

void player::add_y()
{
	p_y += speed;
}

void player::sub_x()
{
	p_x -= speed;
}

void player::sub_y()
{
	p_y -= speed;
}

void player::judge_bullet()
{
	int i;
	for (i = 0; i < 20; i++)
	{
		if (pbullet[i].b_alive == false) {
			pbullet[i].b_alive = true;
			break;
		}
			

	}
}

void player::move_bullet()
{
	for (int i = 0; i < 20; i++) {
		if (pbullet[i].b_alive == true)
			pbullet[i].b_y -= pbullet[i].b_speed;
		else {
			pbullet[i].b_x = (p_x + p_img_w / 2-5);
			pbullet[i].b_y = p_y;
		}
	}
}

void player::bullet_over()
{
	for (int i = 0; i < 20; i++) {
		if (pbullet[i].b_y < 0 || alive <2)		//或者发生碰撞
			pbullet[i].b_alive = false;
	}
}

void player::Collision(emy *aEmy)
{
	if(alive>1)
	for (int i = 0; i < 20; i++) {		//敌机被击中
		if(p_y>aEmy->get_y())
		if(pbullet[i].b_x >= aEmy->get_x() && pbullet[i].b_x<=aEmy->get_x_max())
			if (pbullet[i].b_y <=( aEmy->get_y()+20)) {
				aEmy->sub_blood();
				pbullet[i].b_alive = false;
				//if(pbullet[i].b_y == (aEmy->get_y() + 10))
					
				//break;
			}
			
	}
	for (int i = 0; i < 20; i++) {
		if (alive>1) {		//玩家被击中
			if (aEmy->get_bullet_x(i) >= p_x && aEmy->get_bullet_x(i) <= p_x_max)
				if (aEmy->get_bullet_y(i) == (p_y + 10)) {
					--blood;
					if (blood <= 0)
						alive = 1;
					aEmy->change_bullet_alive(i, false);
				}
					
		}
	}

}

bool player::bullet_alive(int i)
{
	if (pbullet[i].b_alive == true)
		return true;
	else
		return false;
	//return pbullet[i].b_alive == true ? true : false;
}

int player::player_alive()
{
	return alive;
}

void player::change_alive(int)
{
	alive = 0;
	
}

int player::get_player_x()
{
	return p_x;
}

int player::get_player_y()
{
	return p_y;
}

void player::get_player_x_max()
{
	p_x_max = (p_x + p_img_w);
	//return p_x_max;
}

player::~player()
{
}
