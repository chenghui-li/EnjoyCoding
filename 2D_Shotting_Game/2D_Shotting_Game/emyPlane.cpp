#include "emyPlane.h"



emyPlane::emyPlane()
{
}


emyPlane::~emyPlane()
{
}

emyPlane::emyPlane(const emyPlane &r)
{
	emy_x = r.emy_x;
	emy_y = r.emy_y;
	emy_img = r.emy_img;
	emy_img_b = r.emy_img_b;
	bullet_img = r.bullet_img;
	bullet_img_b = r.bullet_img_b;
	emy_width = r.emy_width;
	emy_height = r.emy_height;
	D3DSURFACE_DESC desc;
	emy_blood = r.emy_blood;	//敌机血量
	emy_kill = r.emy_kill;	//敌机伤害
	b_speed = 1;	//子弹速度
	emy_speed = 0.5;	//敌机移动速度
	emy_alive = 2;	//存活状态，2：存活，1：刚死，需要画爆炸画面，0：无效弹出
	for (int i = 0; i < 20; i++)
	{
		emy_bullet[i] = r.emy_bullet[i];
	}

}

emy * emyPlane::Clone() const
{
	return new emyPlane(*this);
}
