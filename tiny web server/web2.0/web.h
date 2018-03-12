#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXN 4096
#define LISTENQ 1024
#define THREAD_LIMIT 4096     //max num of thread
int THREADNUM = 200;
int ADDTHREADNUM;
int NEWTHREADNUM;
extern char **environ;
typedef struct {

	int rio_fd;
	int rio_cnt;		//unread bytes in internal buf
	char *rio_bufptr;	//next unread byte in internal buf
	char rio_buf[MAXN];	//internal buffer
}rio_t;
typedef struct{
	int *buf;	//buffer arry
	int n;   	//max number of slots
	int front;  //buf[(front+1)%n]is the first item
	int rear;   //buf[(rear)%n] is the last item
	sem_t mutex;//protect accesses to buf
	sem_t slots;//counts available slots
	sem_t items;//counts available items
}sbuf_t;
sbuf_t sbuf;
sem_t ThreadMutex[THREAD_LIMIT];
pthread_t ThreadId[THREAD_LIMIT];
ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n);
ssize_t rio_readnb(rio_t *rp,char *usrbuf,size_t n);
ssize_t rio_writen(int fd,char *usrbuf,size_t n);
int open_listenfd(int port);
ssize_t rio_readlineb(rio_t *rp,char *usrbuf,size_t maxlen);
void rio_readinitb(rio_t *rp,int fd);
void doit(int fd);
void read_requesthdrs(rio_t *rp,char *,int *);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serve_static(int fd,char *filename,int filesize,char *method);
void get_filetype(char *filename,char *filetype);
void serve_dynamic(int fd,char *filename,char *cgiargs);
void clienterror(int fd,const char *cause,const char *errnum,const char *shortmsg,const char *longmsg);
void get_filetype(char *filename,char *filetype);

void sbuf_init(sbuf_t *,int);
void sbuf_deinit(sbuf_t *);
void sbuf_insert(sbuf_t *,int);
int sbuf_remove(sbuf_t *);
void P(sem_t *);
void V(sem_t *);
