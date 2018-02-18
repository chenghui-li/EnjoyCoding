#include "mysocket.h"
#include "myIO.h"
#define MAXN 1024

#define SERVPORT 1234
#define LISTENNUM 1024
void error_(const char *info){
	printf("error : %s\n",info);
	exit(1);
}
void doit(int connfd){
    ssize_t n;
	char buf[MAXN];
	bzero(&buf,sizeof(buf));
again:
	while((n = read(connfd,buf,MAXN))>0)
		if(writen(connfd,buf,n)!=n)
			error_("writen");
	if(n<0 && errno == EINTR)
		goto again;
	else if(n<0)
		error_("read");
}
void handler(int signal){
	pid_t pid;
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0)
		printf("child %d terminated\n",pid);
	return ;
}
int main(int argc,char *argv[]){
	int listenfd,connfd;
	struct sockaddr_in cliaddr,servaddr;
	pid_t childpid;
	socklen_t clilen;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		error_("socket");
	bzero(&servaddr,sizeof(servaddr));
	bzero(&cliaddr,sizeof(cliaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(2345);
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
		error_("bind");
	if(listen(listenfd,LISTENNUM) == -1)
		error_("listen");
	if(signal(SIGCHLD,handler) == SIG_ERR)
		error_("signal");
	while(1){
        clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen)) == -1)
			if(errno == EINTR)
				continue;
			else
				error_("accept");

		if((childpid = fork()) == -1)
			error_("fork");
		else if(childpid == 0){
			if(close(listenfd) == -1)
				error_("close in child");
			doit(connfd);
			if(close(connfd) == -1)
				error_("close in child");
			exit(0);
		}else
			if(close(connfd) == -1)
				error_("close in parent");
	}
	if(close(listenfd) == -1){
		error_("listenfd close");
	}

	exit(0);
}
