#include "web.h"
void sigchld_hander(int sig){
	while(waitpid(-1,0,WNOHANG) > 0)
		;
	return;
}
int main(int argc,char *argv[]){
	int listenfd,connfd,_port;
	char hostname[MAXN],port[MAXN];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	if(argc != 2){
		fprintf(stderr,"usage:%s <port>\n",argv[0]);
		exit(1);
	}
	_port = atoi(argv[1]);		//char转int
	listenfd = open_listenfd(_port);
	signal(SIGCHLD,sigchld_hander);		//处理僵死进程
	while(1){
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clientlen);
		if(fork() == 0){
			close(listenfd);
			getnameinfo((struct sockaddr *)&clientaddr,clientlen,
				hostname,MAXN,
				port,MAXN,0);
			printf("accept from (%s , %s)%d\n",hostname,port,listenfd);
			doit(connfd);	//处理HTTP事物
			close(connfd);
			exit(0);
		}

		close(connfd);		//关键！！！父进程关闭连接描述符
	}

}
//
int open_listenfd(int port){
	int listenfd,optval = 1;
	struct sockaddr_in serveraddr;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		return -1;
	}
	//设置与套接字关联的选项
	/*
	int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen);
	参数：  
	sock：将要被设置或者获取选项的套接字。
	level：选项所在的协议层。
	optname：需要访问的选项名。
	optval：对于getsockopt()，指向返回选项值的缓冲。
	对于setsockopt()，指向包含新选项值的缓冲。
	optlen：对于getsockopt()，作为入口参数时，
	选项值的最大长度。作为出口参数时，
	选项值的实际长度。对于setsockopt()，
	现选项的长度。
	*/
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,
		(const void *)&optval,sizeof(int)) < 0)
		return -1;		//closesocket后想重用该socket
	bzero((char *)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))
		return -1;
	if(listen(listenfd,LISTENQ) < 0)
		return -1;
	return listenfd;
/*	struct addrinfo hints,*listp,*p;
	int listenfd,optval = 1;
	memset(&hints,0,sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE|AI_ADDRCONFIG;	//任何IP地址
	hints.ai_flags |=AI_NUMERICSERV;			//使用端口
	getaddrinfo(NULL,port,&hints,&listp);
	//遍历返回的链表，寻找可以bind的项
	for(p = listp;p!=NULL;p = p->ai_next){
		if((listenfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol))<0)
			continue;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));		//配置服务器，使得能够立即重启，终止，开始接收请求
		if(bind(listenfd,p->ai_addr,p->ai_addrlen) == 0)
			break;	//connect successfully
		close(listenfd);	//bind failed,close this and try next
	}
	freeaddrinfo(listp);
	if(p == NULL)
		return -1;
	if(listen(listenfd,LISTENQ)<0){
		close(listenfd);
		return -1;
	}
	return listenfd;*/

}
ssize_t rio_readlineb(rio_t *rp,void *usrbuf,size_t maxlen){
	int n,rc;
	char c,*bufp = usrbuf;
	for(n = 1;n<maxlen;n++){
		if((rc = rio_read(rp,&c,1)) == 1){
			*bufp++ = c;
			if(c == '\n'){
				//n++;
				break;
			}

		}else if(rc == 0){
			if(n == 1)
				return 0;	//no data read;
			else
				break;		//EOF
		}else
			return -1;	//error
	}
	*bufp = 0;
	return n;
}
void rio_readinitb(rio_t *rp,int fd){
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}
void doit(int fd){
	int is_static;		//判断请求内容是静态还是动态
	struct stat sbuf;
	char buf[MAXN],method[MAXN],uri[MAXN],version[MAXN];
	char filename[MAXN],cgiargs[MAXN];
	rio_t rio;		//将一个缓冲区与描述符联系起来
	rio_readinitb(&rio,fd);
	rio_readlineb(&rio,buf,MAXN);
	printf("request header:\n%s",buf);
	sscanf(buf,"%s %s %s",method,uri,version);
	if(strcasecmp(method,"GET")){	//忽略大小写比较字符串
		clienterror(fd,method,"501","not implemented","tiny doesn't implement this method");
		return ;
	}
	read_requesthdrs(&rio);		//读取打印请求头
	is_static = parse_uri(uri,filename,cgiargs);	//判断是静态请求还是动态请求
	if(stat(filename,&sbuf)<0){		//文件不存在
		clienterror(fd,filename,"404","not found","tiny couldn't find this file");
		return ;
	}
	if(is_static){	//静态请求
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
			clienterror(fd,filename,"403","frobidden","tiny couldn't read the file");
			return;
		}
		serve_static(fd,filename,sbuf.st_size);		//**************
	}
	else{	//serve dynamic content
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
			clienterror(fd,filename,"403","forbidden","tiny couldn't run the CGI program");
			return ;
		}
		serve_dynamic(fd,filename,cgiargs);	//******************
	}
	

}
ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n){
	int cnt;
	while(rp->rio_cnt<=0){		//未读字节数
		rp->rio_cnt = read(rp->rio_fd,rp->rio_buf,sizeof(rp->rio_buf));
		if(rp->rio_cnt<0){
			if(errno != EINTR)
				return -1;
		}
		else if(rp->rio_cnt == 0)
			return 0;
		else
			rp->rio_bufptr = rp->rio_buf;
	}
	cnt = n;
	if(rp->rio_cnt<n)
		cnt = rp->rio_cnt;
	memcpy(usrbuf,rp->rio_bufptr,cnt);
	rp->rio_bufptr +=cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}
ssize_t rio_writen(int fd,void *usrbuf,size_t n){
	size_t nleft = n;	//ul，待写入的字节数
	ssize_t nwritten;	//l，实际写入的字节数
	char *bufp = usrbuf;
	while(nleft >0){
		if((nwritten = write(fd,bufp,nleft))<=0){
			if(errno == EINTR)
				nwritten = 0;
			else 
				return -1;
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}
void clienterror(int fd,char *cause,char *errnum,char *shortmsg,char *longmsg){
	char buf[MAXN],body[MAXN];
	//build the http response body
	sprintf(body,"<html><title>tiny error</title>");
	sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
	sprintf(body,"%s%s:%s\r\n",body,errnum,shortmsg);
	sprintf(body,"%s<p>%s:%s\r\n",body,longmsg,cause);
	sprintf(body,"%s<hr><em>the tiny web server</em>\r\n",body);
	
	//print the http response
	sprintf(buf,"HTTP/1.0 %s %s \r\n",errnum,shortmsg);
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-type: text/html\r\n");
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-length: %d\r\n\r\n",(int)strlen(body));
	rio_writen(fd,buf,strlen(buf));
	rio_writen(fd,body,strlen(body));

}
//输出请求报头
void read_requesthdrs(rio_t *rp){
	char buf[MAXN];
	rio_readlineb(rp,buf,MAXN);
	while(strcmp(buf,"\r\n")){	//未读到空行
		rio_readlineb(rp,buf,MAXN);
		printf("%s",buf);
	}
	return ;
}
int parse_uri(char *uri,char *filename,char *cgiargs){
	char *ptr;
	if(!strstr(uri,"cgi-bin")){	//static content
		strcpy(cgiargs,"");
		strcpy(filename,".");
		strcat(filename,uri);
		if(uri[strlen(uri)-1] == '/')
			strcat(filename,"index.html");//默认访问的主页
		return 1;		//静态内容
	}
	else{	//dynamic content
		ptr = index(uri,'?');		//？分离参数与函数名，&分离参数名
		if(ptr){
			strcpy(cgiargs,ptr+1);	//提取参数
			*ptr = '\0';
		}
		else
			strcpy(cgiargs,"");		//请求的参数
		strcpy(filename,".");
		strcat(filename,uri);
		return 0;		//动态内容
		
	}
}

void serve_static(int fd,char *filename,int filesize){
	int srcfd;
	char *srcp,filetype[MAXN],buf[MAXN];

	//向客户端发送响应头
	get_filetype(filename,filetype);
	sprintf(buf,"HTTP/1.1 200 OK\r\n");
	sprintf(buf,"%sServer:tiny wib Server \r\n",buf);
	sprintf(buf,"%sConnection:close\r\n",buf);
	sprintf(buf,"%sContent-length: %d\r\n",buf,filesize);
	sprintf(buf,"%sContent-type: %s\r\n\r\n",buf,filetype);
	rio_writen(fd,buf,strlen(buf));		//响应报文头
	printf("response headers:\n%s",buf);

	//将一个文件或者其它对象映射到进程的地址空间
	srcfd = open(filename,O_RDONLY,0);
	srcp = mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);		//内存映射
	close(srcfd);
	rio_writen(fd,srcp,filesize);
	munmap(srcp,filesize);
}

/*
     {".css", "text/css"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".ico", "image/x-icon"},
    {".js", "application/javascript"},
    {".pdf", "application/pdf"},
    {".mp4", "video/mp4"},
    {".png", "image/png"},
    {".svg", "image/svg+xml"},
    {".xml", "text/xml"},
*/
void get_filetype(char *filename,char *filetype){
	if(strstr(filename,".html"))
		strcpy(filetype,"text/html");
	else if(strstr(filename,".gif"))
		strcpy(filetype,"image/gif");
	else if(strstr(filename,".png"))
		strcpy(filetype,"image/png");
	else if(strstr(filename,".jpg"))
		strcpy(filetype,"image/jpeg");
	else if(strstr(filename,".css"))
		strcpy(filetype,"text/css");
	else if(strstr(filename,".js"))
		strcpy(filetype,"application/javascript");
	else if(strstr(filename,".mp4"))
		strcpy(filetype,"video/mp4");
	else if(strstr(filename,".pdf"))
		strcpy(filetype,"application/pdf");
	else
		strcpy(filetype,"text/plain");
}
void serve_dynamic(int fd,char *filename,char *cgiargs){
	char buf[MAXN],*emptylist[] = {NULL};

	//返回响应头
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"server: tiny web server\r\n");
	rio_writen(fd,buf,strlen(buf));
	if(fork()==0){
		setenv("QUERY_STRING",cgiargs,1);
		dup2(fd,STDOUT_FILENO);
		execve(filename,emptylist,environ);		//运行CGI程序
	}
	wait(NULL);
}
