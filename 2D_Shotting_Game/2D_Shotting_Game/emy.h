#pragma once
#include "MyDirectX.h"
class emy
{
public:
	emy();
	virtual ~emy();
	emy(float x, float y, int blood, int kill,int _alive) :emy_x(x), emy_y(y), emy_blood(blood), emy_kill(kill),emy_alive(_alive) {};
	void init_emy(string pemy,string pbullet);	//获得敌机图片
	void add_y();	//改变敌机的纵坐标，即向下移动效果
	virtual emy* Clone() const = 0;		//克隆函数，用于调用拷贝构造函数
	float get_y()const;		//获得敌机的y坐标
	float get_x()const;		//获得敌机的x坐标
	float get_x_max();		//获得敌机的x右边界，即x坐标加图片宽度
	float get_y_max();		//获得敌机的y下边界，即y坐标加图片高度
	int get_alive();		//获得敌机的存活状态
	void change_alive(int);	//改变敌机存活状态
	void change_bullet_alive(int,int);		//改变子弹的状态
	void sub_blood();		//敌机血量减少
	int get_blood();		//获得敌机血量
	void move_bullet();		//移动子弹函数
	int get_emy_width();	//获得敌机图片宽度
	int get_emy_height();	//获得敌机图片高度
	int get_bullet_x(int);	//获得敌机子弹的x坐标
	int get_bullet_y(int);	//获得敌机子弹的y坐标
	bool bullet_alive(int);	//获得敌机子弹状态
	void get_bullet(int ms);	//每隔一定时间产生子弹
	LPDIRECT3DTEXTURE9 get_img_b();	//获得敌机图片
	LPDIRECT3DTEXTURE9 get_bullet_img_b();//获得敌机子弹图片
protected:	
	float emy_x;		//敌机x坐标
	float emy_y;		//敌机y坐标
	int emy_width;		//敌机的宽度
	int emy_height;		//敌机的高度
	D3DSURFACE_DESC desc;
	int emy_blood;	//敌机血量
	int emy_kill;	//敌机伤害
	int b_speed = 1;	//子弹速度
	float emy_speed = 0.4;	//敌机移动速度
	int emy_alive=2;	//存活状态，2：存活，1：刚死，需要画爆炸画面，0：无效弹出
	LPDIRECT3DSURFACE9 emy_img = NULL;
	LPDIRECT3DTEXTURE9 emy_img_b = NULL;
	LPDIRECT3DSURFACE9 bullet_img = NULL;
	LPDIRECT3DTEXTURE9 bullet_img_b = NULL;
	struct Emy_bullet
	{
		double b_x;	//子弹位置
		double b_y;
		int rotation; //子弹发射旋转角度
		
		bool b_alive;	//子弹是否存在
		
	}emy_bullet[20];

};

