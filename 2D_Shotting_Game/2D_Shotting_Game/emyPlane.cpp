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
	emy_blood = r.emy_blood;	//�л�Ѫ��
	emy_kill = r.emy_kill;	//�л��˺�
	b_speed = 1;	//�ӵ��ٶ�
	emy_speed = 0.5;	//�л��ƶ��ٶ�
	emy_alive = 2;	//���״̬��2����1����������Ҫ����ը���棬0����Ч����
	for (int i = 0; i < 20; i++)
	{
		emy_bullet[i] = r.emy_bullet[i];
	}

}

emy * emyPlane::Clone() const
{
	return new emyPlane(*this);
}
