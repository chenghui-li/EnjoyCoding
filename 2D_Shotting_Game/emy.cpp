#include "emy.h"
#include "MyDirectX.h"
#include <math.h>


emy::emy()
{
}

void emy::init_emy(string pemy,  string pbullet) {
	emy_img = LoadSurface(pemy);
	emy_img_b = LoadTexture(pemy);
	bullet_img = LoadSurface(pbullet);
	bullet_img_b = LoadTexture(pbullet);

}
float emy::get_y()const {
	return emy_y;
}
float emy::get_x()const {
	return emy_x;

}
float emy::get_x_max()
{
	emy_img->GetDesc(&desc);
	emy_width = desc.Width;
	return (desc.Width+emy_x);
}
float emy::get_y_max()
{
	emy_img->GetDesc(&desc);
	emy_width = desc.Height;
	return (emy_y+ desc.Height);
}
int emy::get_alive()
{
	return emy_alive;
}
void emy::change_alive(int status)
{
	emy_alive = status;
}
void emy::change_bullet_alive(int i,int status)
{
	emy_bullet[i].b_alive = status;
}
void emy::sub_blood()
{
	--emy_blood;
}
int emy::get_blood()
{
	return emy_blood;
}
void emy::move_bullet()
{

	for (int i = 0; i < 20; i++) {
		if (emy_bullet[i].b_alive == true) {
			double b_speedx = b_speed * cos(emy_bullet[i].rotation * 3.1415926 / 180);
			double b_speedy = b_speed * sin(emy_bullet[i].rotation * 3.1415926 / 180);
			emy_bullet[i].b_y += b_speedy;
			emy_bullet[i].b_x += b_speedx;
		}
		else {
			emy_bullet[i].b_x = (emy_x + emy_width / 2 - 5);
			emy_bullet[i].b_y = emy_y+emy_height;
			emy_bullet[i].rotation = (i * 10) % 180;
		}
	}
}
int emy::get_emy_width()
{
	emy_img->GetDesc(&desc);
	emy_width = desc.Width;
	return emy_width;
}
int emy::get_emy_height()
{
	emy_img->GetDesc(&desc);
	emy_height = desc.Height;
	return emy_height;
}
int emy::get_bullet_x(int i)
{
	return emy_bullet[i].b_x;
}
int emy::get_bullet_y(int i)
{
	return emy_bullet[i].b_y;
}
bool emy::bullet_alive(int i)
{
	return emy_bullet[i].b_alive == true? true:false;
}
void emy::get_bullet(int ms)
{
	if(ms %30 == 0)
	for (int  i = 0; i < 20; i++)
	{
		if (emy_bullet[i].b_alive == false) {
			emy_bullet[i].b_alive = true;
			break;
		}

	}
}
LPDIRECT3DTEXTURE9 emy::get_img_b() {
	return emy_img_b;
}
LPDIRECT3DTEXTURE9 emy::get_bullet_img_b()
{
	return bullet_img_b;
}
void emy::add_y() {
	emy_y += emy_speed;
}
emy::~emy()
{
}