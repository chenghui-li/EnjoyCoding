#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<pthread.h>
#include<time.h>
#include<algorithm>
#include<map>
#include<list>
#include<string>
#include<fstream>
using namespace std;
time_t rawtime;		//获得当前时间
time_t nowtime;		//每隔一秒获得当前时间，做差可以得出0 1 2 ...
#define MAX 50
int progressNum;	//进程PCB的数量
int i;
float max_priority = -1;	//正在运行进程的最大优先级
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;	//互斥锁初始化
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
//PCB结构体
typedef struct myprogress
{
	int pid;			//进程id号
	int begin_time;		//开始时间
	int Sleep;			//挂起标志位，0表示未挂起，1表示挂起
	int end_time;
	int wait_time;		//进程等待时间，在挂起和等待状态会改变
	int to_run_time;	//目标运行时间
	int real_run_time;	//实际已经运行时间
	int is_run;			//运行标志位，0表示等待或阻塞，1表示正在运行
	float priority;	
}myprogress;

//用于list的排序，重载 () 操作符
struct cmp
{
	bool operator()(myprogress a,myprogress b){
		return a.priority>b.priority;
	}
};
FILE *fp;
int max_id = -2;	//目前最大优先级对应的进程的id
int run_id = -1;	//正在运行的进程的id
list<struct myprogress> plist;		//运行/等待队列
list<struct myprogress> pri_list;	//阻塞队列

