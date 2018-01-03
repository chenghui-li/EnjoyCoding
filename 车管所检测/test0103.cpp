// ConsoleApplication2.cpp : 定义控制台应用程序的入口点。
//


#include<iostream>
#include<stdio.h>
#include<queue>
#include<algorithm>
#include<list>
#include<cstring>
using namespace std;
#define MAXN 100
int s[8][3];
int Min = 0x3f3f3f3f;
int Max = 0x3f3f3f3f;
struct Car {
	int a[4] = { 0 };	//123
	int tim = 0;
	int i = 0; //012
	int j = 0;
	int now = 0;	//当前位置
	int next = 0;	//下一位置
	int flag = 0;
	int id;
};
struct buff1 {		//第一线 
	int sta[5];		//123
	int tim[4];		//123 
	bool empty = true;
}buff1;
struct buff2 {		//第二线 
	int sta[5];		//每一个服务位是否为空 
	int tim[4];		//每一个服务位的时间 
	bool empty = true;	//整体是否空 
}buff2;
int everyTime[] = { 0,2,6,4 };
int flag = 0;    //输出答案 
int ansMin[MAXN];
int ansMax[MAXN];
int carNum;		//车的总数 
int num[MAXN];		//每次输入的key 
int anstime = 0;
queue<Car> q;		//等待的车队列
list<Car> l1;		//第一个线的车队列
list<Car> l2;		//第二线的车队列
					//每种标号的映射 
void init() {
	s[1][0] = 1;
	s[1][1] = 2;
	s[1][2] = 3;
	s[2][0] = 1;
	s[3][0] = 1;
	s[3][1] = 2;
	s[4][0] = 2;
	s[5][0] = 2;
	s[5][1] = 3;
	s[6][0] = 3;
	s[7][0] = 1;
	s[7][1] = 3;
}
void doit() {
	Car car;
	car.j = -1;
	anstime = 0;
	while (!q.empty() || !l1.empty() || !l2.empty()) {    //当车队列，一线，二线都不为空时 
		if (car.j == -1 && !q.empty()) {
			car = q.front();

		}

		if (buff1.sta[1] == 0 && buff1.sta[car.a[car.i]] == 0 && !q.empty())	//一线入口处空且待服务号位置空 
		{
			//车进入一线 
			if (flag)
				printf("%d car come in buff1 at time %d\n", car.id, anstime);
			if (car.a[car.i] == 1 || buff1.sta[car.a[car.i]] == 0 && buff1.sta[car.a[car.i] - 1] == 1 || buff1.sta[car.a[car.i]] == 1 && buff1.sta[car.a[car.i] - 1] == 1) {
				car.now = 1;
				car.next = car.now + 1;
				buff1.sta[1] = 1;
			}
			else {
				if (buff1.sta[car.a[car.i]] == 0 && buff1.sta[car.a[car.i] - 1] == 0) {
					car.now = car.a[car.i];
					car.next = car.now + 1;
					buff1.sta[car.a[car.i]] = 1;
				}
				else if (buff1.sta[car.a[car.i]] == 1 && buff1.sta[car.a[car.i] - 1] == 0) {
					car.now = car.a[car.i] - 1;
					car.next = car.now + 1;
					buff1.sta[car.a[car.i] - 1] = 1;
				}
			}

			l1.push_back(car);
			q.pop();

			car.j = -1;
		}
		else if (buff2.sta[1] == 0 && buff2.sta[car.a[car.i]] == 0 && !q.empty()) {
			//车进入二线 
			if (flag)
				printf("%d car come in buff2 at time %d\n", car.id, anstime);

			if (car.a[car.i] == 1 || buff2.sta[car.a[car.i]] == 0 && buff2.sta[car.a[car.i] - 1] == 1 || buff2.sta[car.a[car.i]] == 1 && buff2.sta[car.a[car.i] - 1] == 1) {
				car.now = 1;
				car.next = car.now + 1;
				buff2.sta[1] = 1;
			}
			else {
				if (buff2.sta[car.a[car.i]] == 0 && buff2.sta[car.a[car.i] - 1] == 0) {
					car.now = car.a[car.i];
					car.next = car.now + 1;
					buff2.sta[car.a[car.i]] = 1;
				}
				else if (buff2.sta[car.a[car.i]] == 1 && buff2.sta[car.a[car.i] - 1] == 0) {
					car.now = car.a[car.i] - 1;
					car.next = car.now + 1;
					buff2.sta[car.a[car.i] - 1] = 1;
				}
			}
			l2.push_back(car);
			q.pop();
			//buff2.sta[car.a[car.i]] = 1;
			car.j = -1;
		}
		else {
			//等待，计算时间 
			list<Car>::iterator ite;
			anstime += 2;
			if (!l1.empty())
				for (ite = l1.begin(); ite != l1.end(); ite++)
					(*ite).tim += 2;
			if (!l2.empty())
				for (ite = l2.begin(); ite != l2.end(); ite++)
					(*ite).tim += 2;

			for (ite = l1.begin(); ite != l1.end();) {
				//	if (l1.front().a[(*ite).i] != 0) {
				int k ;	//目标位的索引
				if ((*ite).now == (*ite).a[(*ite).i])
					k = (*ite).i + 1;	//目标位的索引
				else
					k = (*ite).i;
				if ((*ite).tim >= everyTime[(*ite).a[(*ite).i]] || (*ite).now != (*ite).a[(*ite).i]) {

					if (buff1.sta[(*ite).a[k]] == 1 && buff1.sta[(*ite).next] == 1) {
						(*ite).flag = 1;
						ite++;
						continue;
					}
					else if (buff1.sta[(*ite).a[k]] == 0 && buff1.sta[(*ite).next] == 0) {	//移到目标位
						buff1.sta[(*ite).now] = 0;
						(*ite).tim = 0;
						(*ite).flag = 0;
						(*ite).i = k;
						(*ite).now = (*ite).a[(*ite).i];
						(*ite).next = (*ite).now + 1;
						if ((*ite).a[k] != 0)
							buff1.sta[(*ite).a[k]] = 1;
					}
					else if (buff1.sta[(*ite).a[k]] == 0 && buff1.sta[(*ite).next] == 1) {
						(*ite).flag = 1;
						ite++;
						continue;
					}
					else {
						buff1.sta[(*ite).now] = 0;
						(*ite).tim = 0;
						(*ite).flag = 1;	//占位
						if((*ite).now == (*ite).a[(*ite).i])
							(*ite).i += 1;
						if ((*ite).next != 0)
							buff1.sta[(*ite).next] = 1;
						(*ite).now = (*ite).next;
						(*ite).next = (*ite).now + 1;
					}
				}
				if (l1.front().a[(l1.front()).i] == 0) {
					if (flag)
						printf("%d car over from buff1 at time %d\n", (*ite).id, anstime);
					ite++;
					l1.pop_front();

					continue;
				}
				ite++;
			}
			for (ite = l2.begin(); ite != l2.end();) {
				//		if (l2.front().a[(*ite).i] != 0) {
				int k;
				if ((*ite).now == (*ite).a[(*ite).i])
					k = (*ite).i + 1;	//目标位的索引
				else
					k = (*ite).i;
				if ((*ite).tim >= everyTime[(*ite).a[(*ite).i]] || (*ite).now != (*ite).a[(*ite).i]) {
					
					if (buff2.sta[(*ite).a[k]] == 1 && buff2.sta[(*ite).next] == 1) {
						(*ite).flag = 1;
						ite++;
						continue;
					}
					else if (buff2.sta[(*ite).a[k]] == 0 && buff2.sta[(*ite).next] == 0) {	//移到目标位
						buff2.sta[(*ite).now] = 0;
						(*ite).tim = 0;
						(*ite).flag = 0;
						(*ite).i =k;
						(*ite).now = (*ite).a[k];
						(*ite).next = (*ite).now + 1;
						if ((*ite).a[k] != 0)
							buff2.sta[(*ite).a[k]] = 1;
					}
					else if (buff2.sta[(*ite).a[k]] == 0 && buff2.sta[(*ite).next] == 1) {
						(*ite).flag = 1;
						ite++;
						continue;
					}
					else {
						buff2.sta[(*ite).now] = 0;
						(*ite).tim = 0;
						(*ite).flag = 1;	//占位
						if((*ite).now == (*ite).a[(*ite).i])
							(*ite).i += 1;
						if ((*ite).next != 0)
							buff2.sta[(*ite).next] = 1;
						(*ite).now = (*ite).next;
						(*ite).next = (*ite).now + 1;
					}
				}

				//		}
				if (l2.front().a[(l2.front()).i] == 0) {
					if (flag)
						printf("%d car over from buff2 at time %d\n", (*ite).id, anstime);
					ite++;
					l2.pop_front();
					continue;
				}
				ite++;
			}


		}
		//	printf("all time:%d\n", anstime);	
	}
	if (flag)
		printf("all time:%d\n", anstime);
}
int main(int argc, char *argv[]) {

	memset(s, 0, sizeof(s));
	memset(&buff1, 0, sizeof(struct buff1));
	memset(&buff2, 0, sizeof(struct buff2));
	memset(num, 0, sizeof(num));
	memset(ansMin, 0, sizeof(ansMin));
	memset(ansMax, 0, sizeof(ansMax));
	init();
	printf("please input the number of car\n");
	scanf("%d", &carNum);
	for (int i = 1; i <= carNum; i++) {
		//printf("please input cat %d num\n",i);
		cin >> num[i];		//输入每辆车的服务号       ///
							///		Car car;
							///		for (int j = 0; j<3; j++) {
							///			car.a[j] = s[num[i]][j];                   ///
							///			car.id = i;
							//car.tim += everyTime[s[num][j]];
							///		}
							///		q.push(car);
	}
	sort(num + 1, num + carNum + 1);
	//	printf("sequence : ");
	for (int i = 1; i <= carNum; i++) {
		ansMin[i] = num[i];
		ansMax[i] = num[i];
	//	printf("%d ", num[i]);
		Car car;
		for (int j = 0; j<3; j++) {
			car.a[j] = s[num[i]][j];                   ///
			car.id = i;
		}
		q.push(car);
	}
	//	printf("\n");
	doit();
	memset(&buff1, 0, sizeof(struct buff1));
	memset(&buff2, 0, sizeof(struct buff2));
	l1.clear();
	l2.clear();
	while (!q.empty())
		q.pop();
	Min = anstime;
	Max = anstime;
	///	
	while (next_permutation(num + 1, num + carNum + 1)) {
	//	    	printf("sequence : ");
		for (int i = 1; i <= carNum; i++) {
	//					printf("%d ",num[i]);
			Car car;
			for (int j = 0; j<3; j++) {
				car.a[j] = s[num[i]][j];                   ///
				car.id = i;
			}
			q.push(car);

		}
	//			printf("\n");
		doit();
		memset(&buff1, 0, sizeof(struct buff1));
		memset(&buff2, 0, sizeof(struct buff2));
		l1.clear();
		l2.clear();
		if (anstime < Min) {
			for (int i = 1; i <= carNum; i++)
				ansMin[i] = num[i];
		}
		if (anstime > Max) {
			for (int i = 1; i <= carNum; i++)
				ansMax[i] = num[i];
		}
	}
	printf("the min answer:\n");
	for (int i = 1; i <= carNum; i++) {
		printf("%d ", ansMin[i]);
		Car car;
		for (int j = 0; j<3; j++) {
			car.a[j] = s[ansMin[i]][j];                   ///
			car.id = i;
		}
		q.push(car);

	}
	printf("\n");
	flag = 1;
	doit();
	memset(&buff1, 0, sizeof(struct buff1));
	memset(&buff2, 0, sizeof(struct buff2));
	l1.clear();
	l2.clear();
	printf("\nthe max answer:\n");
	for (int i = 1; i <= carNum; i++) {
		printf("%d ", ansMax[i]);
		Car car;
		for (int j = 0; j<3; j++) {
			car.a[j] = s[ansMax[i]][j];                   ///
			car.id = i;
		}
		q.push(car);

	}
	printf("\n");
	doit();
	system("pause");
	return 0;
}


