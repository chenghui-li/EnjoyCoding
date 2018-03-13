
#include "mypro.h"
/*
	功能：
		错误信息的打印
*/

void _error(const char *info){
	printf("error : %s\n",info );
	exit(1);
}
//改变输出终端
void change_stdout(const char num[]){
	char ptr1[15] = "/dev/pts/";
	strcat(ptr1,num);
	freopen(ptr1,"w",stdout);
}
//关闭输出终端
void close_stdout(){
	fclose(stdout);
}

//根据id号从运行/就绪队列中查找该PCB，返回其引用
myprogress *find_plist(int id){
	list<struct myprogress>::iterator it;
	for(it = plist.begin();it!=plist.end();it++){
		if((*it).pid == id)
			return &(*it);
	}
	return NULL;

}
//根据id号从阻塞队列中查找该PCB，返回其引用
myprogress *find_pri_list(int id){
	list<struct myprogress>::iterator it;
	for(it = pri_list.begin();it!=pri_list.end();it++){
		if((*it).pid == id)
			return &(*it);
	}
	return NULL;

}
//根据id号从运行/就绪队列中查找该PCB并移除
void Erase_plist(int id){
	list<struct myprogress>::iterator it;
	for(it = plist.begin();it!=plist.end();){
		if((*it).pid == id){
			it = plist.erase(it);
			return ;
		}
		it++;
	}	
}
//根据id号从阻塞队列中查找该PCB并移除
void Erase_pri_list(int id){
	list<struct myprogress>::iterator it;
	for(it = pri_list.begin();it!=pri_list.end();){
		if((*it).pid == id){
			it = pri_list.erase(it);
			return ;
		}
		it++;
	}	
}
//用户输入指令，将指定PCB阻塞，即从运行/就绪队列移动到阻塞队列中
void go_sleep(){
	list<struct myprogress>:: iterator ite;
	for(ite = plist.begin();ite!=plist.end();){
		if((*ite).Sleep == 1){
			(*ite).is_run = 0;
			pri_list.push_back(*ite);	//移动到阻塞队列
			ite = plist.erase(ite);		//从运行/就绪队列移除
			return ;
		}
		else ite++;		//！！！坑点：遍历移除方法

	}
}
//根据id，查找该PCB，停止其正在运行的状态，即is_run由1置0
void stop(int mp){
	myprogress *m = find_plist(mp);
	if(m!= NULL){
	//	m->wait_time = 0;
		m->is_run = 0;		//is_run为1表示其正在运行状态，0表示就绪状态
	}
}
void write_to_file(int id){
	char chr[100];
	memset(chr,0,sizeof(chr));
	myprogress *p = find_plist(id);
	if( p != NULL)
	if((fp = fopen("info.txt","at+")) != NULL){
		fputs("\n\n当前进程： ",fp);
		sprintf(chr,"%d",max_id);
		fputs(chr,fp);
		fputs("\n已运行时间： ",fp);
		sprintf(chr,"%d",p->real_run_time);
		fputs(chr,fp);
		fputs("\n总运行时间： ",fp);
		sprintf(chr,"%d",p->to_run_time);
		fputs(chr,fp);
		fputs("\n已就绪时间： ",fp);
		sprintf(chr,"%d",p->wait_time);
		fputs(chr,fp);
		fputs("\n当前优先级： ",fp);
		sprintf(chr,"%f",p->priority);
		fputs(chr,fp);
		fclose(fp);

	}
	else
		_error("file open");		
}
/*
	功能：
		将制定id的PCB置为运行状态
	参数：
		max_id：当前优先级最大的PCB的id号
		run_id：当前正在运行的PCB的id号，如果没有则为-1
		plist：运行/就绪队列
		real_run_time：PCB已经运行的时间
*/
void running(int max_id){

	//先判断该PCB是否是第一个运行的PCB，如果不是，先将正在运行的PCB停止
	if(run_id != -1)
		stop(run_id);
	run_id = max_id;
	plist.front().real_run_time ++;
	plist.front().is_run = 1;
	avail[run_id]++;
	pthread_cond_signal(&cond_[run_id]);
	//write_to_file();

	//将运行状态输出到编号为1的终端上
	change_stdout("1");		
	printf("\nPID:%d is running",run_id );
	close_stdout();

	//如果该PCB的实际运行时间等于目标运行时间，则将其撤销，注：正在运行的PCB即为当前优先级最大的PCB
	/*if(plist.front().real_run_time == plist.front().to_run_time){

		//将运行结束的信息输出到编号为1的终端上
		change_stdout("1");
		cout<<" "<<plist.front().pid<<" over!";
		close_stdout();

		//将运行完成的PCB从运行/就绪队列中弹出
		plist.pop_front();
	}*/
	return ;
}

/*
	参数：
		max_priority：当前最大的优先级
		max_id：最大优先级对应的PCB的id号
*/
void dispatch(){

	//获取最大优先级和其对应的PCB的id号
	max_priority = plist.front().priority;
	max_id = plist.front().pid;

	//将最大优先级和其对应的PCB的id号输出到编号为2的终端
	change_stdout("2");
	cout<<endl<<max_id<<" "<<max_priority;
	close_stdout();	
	running(max_id);
	return ;
}

/*
	功能：
		优先级调度算法的实现，改变就绪状态的PCB和阻塞状态的PCB的优先级
	参数：
		ite：运行/就绪队列和阻塞的迭代器，用来进行遍历
		plist：运行/就绪队列
		pri_list：阻塞队列
		is_run：运行状态，1为运行，0位就绪
		wait_time：PCB已就绪时间
		priority：PCB优先级
		to_run_time：PCB目标运行时间
*/
void HRN(){
	list<struct myprogress>:: iterator ite;
	for(ite = plist.begin();ite!=plist.end();ite++){
		//遍历运行/就绪队列，将每个就绪状态的元素的就绪时间+1，改变其优先级
		if((*ite).is_run == 0){
			(*ite).wait_time++;
			(*ite).priority = ((*ite).to_run_time+(*ite).wait_time)*1.0/(*ite).to_run_time;
		}
	}

	if(!pri_list.empty()){
		//将阻塞队列中的元素按PCB优先级从大到小排列
		pri_list.sort(cmp());
		//遍历阻塞队列，将每个元素的就绪时间+1，改变其优先级
		for(ite = pri_list.begin();ite!=pri_list.end();){
			(*ite).wait_time++;
			(*ite).priority = ((*ite).to_run_time+(*ite).wait_time)*1.0/(*ite).to_run_time;
			ite++;
		}
	}
	
	return ;
}

/*
	功能：
		线程创建函数，实时改变每个PCB的运行状态，优先级，并进行调度
	参数：
		lock：互斥锁
	函数：
		dispatch() 
		HRN()

*/
void *fork_progress1(void *arg){

	while(1){
		//int i = *(int *)arg;
		
		if(plist.empty()){
			if(pri_list.empty()){		//如果运行/就绪队列和阻塞队列均为空，则表示所有进程都运行完毕
				change_stdout("3");
				cout<<"\nover!\n";
				close_stdout();
				break ;
			}
			else{						//如果阻塞队列不为空，则自动将阻塞队列的最大优先级的元素移动到运行/就绪队列
				pthread_mutex_lock(&lock);
				plist.push_back(pri_list.front());
				pri_list.pop_front();
				pthread_mutex_unlock(&lock);
			}
			
		}
		pthread_mutex_lock(&lock);
		plist.sort(cmp());

		dispatch();		//让优先级最大的PCB处于运行状态
		HRN();			//改变就绪和挂起进程的优先级
		
		pthread_mutex_unlock(&lock);
		sleep(1);
		
	}

	//pthread_exit(NULL);
}
void *fork_progress(void *arg){
	int pidNum = *(int *)(arg);
	free(arg);
  while(1){
		pthread_mutex_lock(&lock);
	while(avail[pidNum] == 0){
		if(pthread_cond_wait(&cond_[pidNum],&lock) != 0){
			_error("pthread_cond_wait");
		}
	}
	write_to_file(pidNum);
	while(avail[pidNum] > 0)
		avail[pidNum]--;
	pthread_mutex_unlock(&lock);
	if(plist.front().real_run_time == plist.front().to_run_time){

		//将运行结束的信息输出到编号为1的终端上
		change_stdout("1");
		cout<<" "<<plist.front().pid<<" over!";
		close_stdout();

		//将运行完成的PCB从运行/就绪队列中弹出
		plist.pop_front();
	}
  }
	

}
int main(int argc,char *argv[]){
	//
	pthread_t tid[MAX];
	//存储管理线程的id
	pthread_t pid;
	memset(avail,0,sizeof(avail));
	while(1){
		//在控制终端显示提示符 >>>
		change_stdout("0");
		printf(">>>");
		close_stdout();
		string in;				//从键盘输入的字符串
		getline(cin,in);		//从cin中读到字符串in中
		
		int flag = 0;			//判断控制线程创建是否成功
		const char *inn;		//in（string类型）转为inn（char *类型）
		int progressNum = 0;	//创建的进程PCB数量
		lock = PTHREAD_MUTEX_INITIALIZER;	//互斥锁初始化

		if(in[0] == 'n')		//输入new num 的处理
		{	
				inn = in.c_str();						//string转char[]
				int num;								//输入的num
				sscanf(inn,"new %d",&num);
				srand((unsigned)time(NULL));			//以时间为准重置随机数种子
				time(&rawtime);							//获取当前时间，存储到rawtime中
				
				for (i = 0; i < num; ++i)
				{
					cond_[i] = PTHREAD_COND_INITIALIZER;
					myprogress PCB;
					time(&nowtime);						//获取当前时间，存储在nowtime中
					PCB.pid = i;						//PCB的id号
					PCB.begin_time = nowtime - rawtime;	//PCB开始时间
					PCB.to_run_time = rand()%15+1;		//随机1-15的目标运行时间
					PCB.priority = rand()%3;			//随机0-2的优先级
					if(i == 0)
						PCB.is_run = 1;					//0号进程PCB直接运行
					else
						PCB.is_run = 0;					//否则先就绪，需判断优先级
					plist.push_back(PCB);				//将创建的PCB插入到运行/就绪队列
					//在编号为3的终端打印该PCB的信息
					change_stdout("3");
					printf("\nPID :%d begin_time:%d ",i,PCB.begin_time);
					printf("run_time:%d priority:%f",PCB.to_run_time,PCB.priority );
					close_stdout();
					//！！！！！未完待续...
					int *I = new int(i);
					if((flag = pthread_create(&tid[i],NULL,fork_progress,I)) != 0){
						_error("creat thread");
					}
					//创建控制线程，实时改变每个PCB的运行状态，优先级，并进行调度
					if(i == 0)
						if((flag = pthread_create(&pid,NULL,fork_progress1,NULL)) != 0){
							_error("creat thread");
						}
					sleep(1);
				} 		
		}
		//输入sleep n ：将id为n的进程PCB挂起
		else if(in[0] == 's'){
			inn = in.c_str();
			int num;
			sscanf(inn,"sleep %d",&num);
			myprogress *m = find_plist(num);	//查找该id号对应的PCB，将引用赋给m指针，即可用m直接操作该PCB
			if(m == NULL){
				change_stdout("0");
				printf("pid %d can't sleep!\n",num );
				close_stdout();
				continue;
			}
			pthread_mutex_lock(&lock);
			m->Sleep = 1;						//将PCB的挂起标志位置1。0表示非挂起，即在运行/阻塞队列中，1表示挂起，将该PCB放到阻塞队列中
			change_stdout("1");					//将挂起信息输出到编号为1的终端上
			go_sleep();							//挂起函数
			printf(" %d sleep\n",num );
			close_stdout();
			pthread_mutex_unlock(&lock);
			
			continue;
		}
		//输入wake n：将id为n的进程PCB唤醒，即从阻塞队列移动到运行/就绪队列
		else if(in[0] == 'w'){
			inn = in.c_str();
			int num;
			sscanf(inn,"wake %d",&num);
			myprogress *m = find_pri_list(num);		//查找该id号对应的PCB，将引用赋给m指针，即可用m直接操作该PCB
			if(m == NULL){
				change_stdout("0");
				printf("pid %d can't wake!\n",num );
				close_stdout();
				continue;
			}			
			pthread_mutex_lock(&lock);
			m->Sleep = 0;							//挂起标志位置0，表示非挂起
			plist.push_back(*m);					//将插入到运行/就绪队列
			Erase_pri_list(num);					//从阻塞队列中移除该PCB
			change_stdout("1");		
			printf(" %d wake\n",num );
			close_stdout();	
			pthread_mutex_unlock(&lock);
			continue;
		}
		//输入exit n：撤销id为n的进程PCB
		else if(in[0] == 'e'){
			inn = in.c_str();
			int num;
			sscanf(inn,"exit %d",&num);	
			myprogress *m = find_plist(num);
			if(m == NULL){
				change_stdout("0");
				printf("pid %d can't exit!\n",num );
				close_stdout();
				continue;
			}			
			pthread_mutex_lock(&lock);
			Erase_plist(num);						//从运行/就绪队列中将该PCB移除
			change_stdout("1");
			printf(" %d exit\n",num );				//打印提示信息
			close_stdout();				
			pthread_mutex_unlock(&lock);
		}
		
	}
	return 0;

}