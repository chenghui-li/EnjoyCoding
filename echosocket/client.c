#include "mysocket.h"
#include "myIO.h"
#define MAXN 1024
void error_(const char *info){
	printf("error : %s\n",info);
	exit(1);
}
void doit(int sockfd){
	char send[MAXN],recv[MAXN];
	while(fgets(send,MAXN,stdin)!=NULL){
		writen(sockfd,send,strlen(send));
		if(readline(sockfd,recv,MAXN) == 0)
			error_("readline");
		fputs(recv,stdout);
	}
	return;
}
int main(int argc,char *argv[]){
	int sockfd;
	struct sockaddr_in servaddr;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
		error_("socket");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(2345);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))
== -1)
	{
		close(sockfd);
		error_("connect");
	}	
	doit(sockfd);
	exit(0);
}
