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
#define MAXN 8192
#define LISTENQ 1024
extern char **environ;
typedef struct {

	int rio_fd;
	int rio_cnt;		//unread bytes in internal buf
	char *rio_bufptr;	//next unread byte in internal buf
	char rio_buf[MAXN];	//internal buffer
}rio_t;
ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n);
ssize_t rio_writen(int fd,void *usrbuf,size_t n);
int open_listenfd(int port);
ssize_t rio_readlineb(rio_t *rp,void *usrbuf,size_t maxlen);
void rio_readinitb(rio_t *rp,int fd);
void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serve_static(int fd,char *filename,int filesize);
void get_filetype(char *filename,char *filetype);
void serve_dynamic(int fd,char *filename,char *cgiargs);
void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg);
void get_filetype(char *filename,char *filetype);

