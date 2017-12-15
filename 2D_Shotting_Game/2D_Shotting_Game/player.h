#pragma once
#include "MyDirectX.h"
#include "emy.h"
class player
{
public:
	//
	static player * Instence();
	player(int x, int y, int _blood) :p_x(x), p_y(y), blood(_blood) {}
	~player();
	void init_player(string pplayer,string pbullet);
	void set_left();		//设置左边界
	void set_right(int);	//设置右边界
	void set_up();			//设置上边界
	void set_down(int);		//设置下边界
	void add_x();			//飞机左右上下移动
	void add_y();			//y坐标增加
	void sub_x();			//x坐标减少
	void sub_y();			//y坐标减少
	void judge_bullet();	//生成子弹
	void move_bullet();		//子弹移动
	void bullet_over();		//子弹出界或击中
	void Collision(emy *aEmy);
	bool bullet_alive(int );	//判断子弹是否存在
	int player_alive();
	void change_alive(int);
	int get_player_x();		//得到飞机x，y
	int get_player_y();
	void get_player_x_max();	//获得飞机最右边的x坐标
	
	int get_p_img_width();	//得到飞机图片长宽
	int get_p_img_height();
	int get_bullet_x(int);	//得到子弹x，y
	int get_bullet_y(int);
	LPDIRECT3DTEXTURE9 get_img();
	LPDIRECT3DSURFACE9 get_img_surf();
	LPDIRECT3DTEXTURE9 get_bullet_img();
protected:
	player() {};
private:
	
	static player* _instance;	//单件接口
	int p_x;			//飞机x坐标
	int p_y;			//飞机y坐标
	int p_x_max;		//飞机左右边的x坐标
	int alive = 2;		//存活状态
	int blood;			//血量
	int speed = 4;		//移动速度
	int p_img_w;		//飞机图片宽度
	int p_img_h;		//飞机图片高度
	D3DSURFACE_DESC desc;
	LPDIRECT3DSURFACE9 p_img = NULL;
	LPDIRECT3DTEXTURE9 p_img_b = NULL;
	LPDIRECT3DSURFACE9 p_bullet_img = NULL;
	LPDIRECT3DTEXTURE9 p_bullet_img_b = NULL;
	struct bullet {
		int b_x;	//子弹x坐标
		int b_y;	//子弹y坐标
		bool b_alive = false;	//子弹状态
		int b_speed = 4;		//子弹移动速度

	}pbullet[20];
};

