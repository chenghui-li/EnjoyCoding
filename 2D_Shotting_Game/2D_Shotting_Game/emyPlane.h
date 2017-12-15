#pragma once
#include "emy.h"
class emyPlane :
	public emy
{
public:
	emyPlane();
	emyPlane(float x, float y, int blood, int kill, int _alive) {
		emy_x = x;
		emy_y = y;
		emy_blood = blood;
		emy_kill = kill;
		emy_alive = _alive;

	}	//构造函数的实现
	~emyPlane();
	emyPlane(const emyPlane& );		//拷贝构造函数
	virtual emy* Clone()const ;		//实现父类的克隆接口
};

