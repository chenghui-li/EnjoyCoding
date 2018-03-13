#include "web.h"
void P(sem_t *s){
	sem_wait(s);
}
void V(sem_t *s){
	sem_post(s);
}
void sbuf_init(sbuf_t *sp,int n){
	sp->buf = calloc(n,sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	ADDTHREADNUM = THREADNUM;
	sem_init(&sp->mutex,0,1);
	sem_init(&sp->slots,0,n);
	sem_init(&sp->items,0,0);
	for(int i = 0;i<THREAD_LIMIT;i++)
		sem_init(&ThreadMutex[i],0,1);
}
int sbuf_isempty(sbuf_t *sp){
	int e;
	P(&sp->mutex);
	e = (sp->front - sp->rear == 0);
	V(&sp->mutex);
	return e;
}
int sbuf_isfull(sbuf_t *sp){
	int f;
	P(&sp->mutex);
	f = (sp->front - sp->rear == sp->n);
	V(&sp->mutex);
	return f;
}
void sbuf_deinit(sbuf_t *sp){
	free(sp->buf);
}
void sbuf_insert(sbuf_t *sp,int connfd){
	P(&sp->slots);
	P(&sp->mutex);
	sp->buf[(++sp->front)%sp->n] = connfd;
	V(&sp->mutex);
	V(&sp->items);
}
int sbuf_remove(sbuf_t *sp){
	P(&sp->items);
	P(&sp->mutex);
	int item = sp->buf[(++sp->rear)%sp->n];
	V(&sp->mutex);
	V(&sp->slots);
	return item;
}
void sigchld_hander(int sig){
	while(waitpid(-1,0,WNOHANG) > 0)
		;
	return;
}
void error_(const char *info){
	printf("%s\n",info );
	exit(1);
}
void *thread_handler(void *arg){
	int tid = *((int *)arg);
	pthread_detach(pthread_self());
	free(arg);
	while(1){
		int connfd = sbuf_remove(&sbuf);
		P(&ThreadMutex[tid]);    //sure every server over
		doit(connfd);
		V(&ThreadMutex[tid]);
		if(close(connfd)<0)
			error_("close");

	}
	return NULL;

}
void *adjustHandler(void *arg){
	while(1){
		if(sbuf_isfull(&sbuf)){
			NEWTHREADNUM = 2*ADDTHREADNUM;
			for(int i = ADDTHREADNUM;i<NEWTHREADNUM;i++){
			
				int *I = (int *)malloc(sizeof(int));//!!!
				*I = i;
				if(pthread_create(&ThreadId[i],NULL,thread_handler,I)<0)
					error_("pthread_create");
				sem_init(&ThreadMutex[i],0,1);
			}
			ADDTHREADNUM = NEWTHREADNUM;
		}
		else if(sbuf_isempty(&sbuf)){
			ADDTHREADNUM = NEWTHREADNUM/2;
			if(ADDTHREADNUM > THREADNUM){
				for(int i = ADDTHREADNUM;i<NEWTHREADNUM;i++){
					P(&ThreadMutex[i]);
					if(pthread_cancel(ThreadId[i]) < 0)
						error_("pthread_cancel");
					V(&ThreadMutex[i]);
				}
				NEWTHREADNUM = ADDTHREADNUM;
			}
		}
	}
}
int main(int argc,char *argv[]){
	int listenfd,connfd,_port;
	char hostname[MAXN],port[MAXN];
	pthread_t  tid;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	if(argc != 2){
		fprintf(stderr,"usage:%s <port>\n",argv[0]);
		exit(1);
	}
	if(signal(SIGCHLD,sigchld_hander) == SIG_ERR){
		error_("signal");
	}
	if(signal(SIGPIPE,SIG_IGN) == SIG_ERR){
		error_("signal");
	}
	_port = atoi(argv[1]);
	if((listenfd = open_listenfd(_port)) <0)
		error_("listenfd");
	for(int i = 0;i< THREADNUM;i++){
		int *I = (int *)malloc(sizeof(int));
		*I = i;
		pthread_create(&ThreadId[i],NULL,thread_handler,I);
		sem_init(&ThreadMutex[i],0,1);
	}
	sbuf_init(&sbuf,100000);
	if(pthread_create(&tid,NULL,adjustHandler,NULL) < 0){
		error_("pthread_create");
	}
	while(1){
		clientlen = sizeof(clientaddr);
		if((connfd = accept(listenfd,(struct sockaddr *)&clientaddr,&clientlen))<0)
			error_("accept");
		getnameinfo((struct sockaddr *)&clientaddr,clientlen,
				hostname,MAXN,
				port,MAXN,0);
//		printf("(%s,%s)\n",hostname,port);
		sbuf_insert(&sbuf,connfd);

	}

}
int open_listenfd(int port){
	int listenfd,optval = 1;
	struct sockaddr_in serveraddr;
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		error_("socket");
	}
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,
		(const void *)&optval,sizeof(int)) < 0)
		error_("setsockopt");
	bzero((char *)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if(bind(listenfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))<0)
		error_("bind");
	if(listen(listenfd,LISTENQ) < 0)
		error_("listen");
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
ssize_t rio_readnb(rio_t *rp,char *usrbuf,size_t n){
	size_t nleft = n;
	ssize_t nread;
	char *bufp = usrbuf;
	while(nleft > 0){
		if((nread = rio_read(rp,bufp,nleft))<0){
			return -1;
		}
		else if(nread ==0)
			break;
		nleft -= nread;
		bufp += nread;
	}
	return (n-nleft);
}
ssize_t rio_readlineb(rio_t *rp,char *usrbuf,size_t maxlen){
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
	//		printf("n1 = %d\n",n );
			if(n == 1)
				return 0;	//no data read;
			else
				break;		//EOF
		}else
	//		printf("n2= %d\n",n );
			return -1;	//error
	}
	*bufp = 0;
//	printf("n3 = %d\n",n );
	return n;
}
void rio_readinitb(rio_t *rp,int fd){
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

void doit(int fd){
	int is_static;
	struct stat sbuf;
	char buf[MAXN],method[MAXN] ,uri[MAXN],version[MAXN];
	char requestBody[MAXN];
	char filename[MAXN],cgiargs[MAXN];
	rio_t rio;
	int contentLen = -1;
	rio_readinitb(&rio,fd);
	rio_readlineb(&rio,buf,MAXN);
	bzero(requestBody,sizeof(requestBody));
//	printf("request header:\n%s",buf);
	sscanf(buf,"%s %s %s",method,uri,version);
	if(!(strcasecmp(method,"GET") == 0 || strcasecmp(method,"POST") == 0 || strcasecmp(method,"HEAD") == 0)){
		clienterror(fd,method,"501","not implemented","tiny doesn't implement this method");
		return ;
	}
	read_requesthdrs(&rio,method,&contentLen);		//*******************
	if(strcasecmp(method,"POST") == 0){
	//	printf("Content-length:%d\n", contentLen);

		//int this line the last char is not \r\n
		rio_readnb(&rio,requestBody,contentLen);   		
	}

	is_static = parse_uri(uri,filename,cgiargs);	//*************
	if(stat(filename,&sbuf)<0){
		clienterror(fd,filename,"404","not found","tiny couldn't find this file");
		return ;
	}
	if(is_static){	//server static content
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
			clienterror(fd,filename,"403","frobidden","tiny couldn't read the file");
			return;
		}
		serve_static(fd,filename,sbuf.st_size,method);		//**************
	}
	else{	//serve dynamic content
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
			clienterror(fd,filename,"403","forbidden","tiny couldn't run the CGI program");
			return ;
		}
		if(strcasecmp(method,"GET") == 0)
			serve_dynamic(fd,filename,cgiargs);	//******************
		else
			serve_dynamic(fd,filename,requestBody);
	}
	

}
ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n){
	int cnt;
	while(rp->rio_cnt<=0){
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
void Im_rio_writen(int fd,char *usrbuf,size_t n){
	if(rio_writen(fd,usrbuf,n) != n){
		if(errno == EPIPE){
			fprintf(stderr, "EPIPE error" );
		}
		fprintf(stderr, "%s\n",strerror(errno) );
		error_("EPIPE");
	}
}
ssize_t rio_writen(int fd,char *usrbuf,size_t n){
	size_t nleft = n;
	ssize_t nwritten;
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
void clienterror(int fd,const char *cause,const char *errnum,const char *shortmsg,const char *longmsg){
	char buf[MAXN],body[MAXN];
	//build the http response body
	sprintf(body,"<html><title>tiny error</title>");
	sprintf(body,"%s<body bgcolor=""ffffff"">\r\n",body);
	sprintf(body,"%s%s:%s\r\n",body,errnum,shortmsg);
	sprintf(body,"%s<p>%s:%s\r\n",body,longmsg,cause);
	sprintf(body,"%s<hr><em>the tiny web server</em>\r\n",body);
	
	//print the http response
	sprintf(buf,"HTTP/1.0 %s %s \r\n",errnum,shortmsg);
	Im_rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-type: text/html\r\n");
	Im_rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"Content-length: %d\r\n\r\n",(int)strlen(body));
	Im_rio_writen(fd,buf,strlen(buf));
	Im_rio_writen(fd,body,strlen(body));

}
void read_requesthdrs(rio_t *rp,char *method,int *contentLen){
	char buf[MAXN];
	do{
		rio_readlineb(rp,buf,MAXN);
		if(strcasecmp(method,"POST") == 0 && 
			strncasecmp(buf,"Content-length:",15) == 0){
			*contentLen = atoi(buf+16);
		}
	}while(strcmp(buf,"\r\n"));
	return ;
}
int parse_uri(char *uri,char *filename,char *cgiargs){
	char *ptr;
	if(!strstr(uri,"cgi-bin")){	//static content
		strcpy(cgiargs,"");
		strcpy(filename,".");    //under current folder
		strcat(filename,uri);
		if(uri[strlen(uri)-1] == '/')
			strcat(filename,"index.html");
		return 1;
	}
	else{	//dynamic content
		ptr = index(uri,'?');
		if(ptr){
			strcpy(cgiargs,ptr+1);    //get the parameter list
			*ptr = '\0';
		}
		else
			strcpy(cgiargs,"");   //no parameter
		strcpy(filename,".");
		strcat(filename,uri);
		return 0;
		
	}
}

void serve_static(int fd,char *filename,int filesize,char *method){
	int srcfd;
	char *srcp,filetype[MAXN],buf[MAXN];

	//send responce to client
	get_filetype(filename,filetype);
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	sprintf(buf,"%sServer:tiny wib Server \r\n",buf);
	sprintf(buf,"%sConnection:close\r\n",buf);
	sprintf(buf,"%sContent-length: %d\r\n",buf,filesize);
	sprintf(buf,"%sContent-type: %s\r\n\r\n",buf,filetype);
	Im_rio_writen(fd,buf,strlen(buf));
	//send response body to client
	if(strcasecmp(method,"GET") == 0){
		srcfd = open(filename,O_RDONLY,0);
		srcp = (char *)mmap(0,filesize,PROT_READ,MAP_PRIVATE,srcfd,0);
		close(srcfd);
		if(strcasecmp(method,"GET") == 0)
			Im_rio_writen(fd,srcp,filesize);
		munmap(srcp,filesize);
	}

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
	bzero(buf,sizeof(buf));
	
	//return first part of http response
	sprintf(buf,"HTTP/1.0 200 OK\r\n");

	Im_rio_writen(fd,buf,strlen(buf));
	sprintf(buf,"server: tiny web server\r\n");
	Im_rio_writen(fd,buf,strlen(buf));
	if(fork()==0){
		if(signal(SIGPIPE,SIG_DFL) == SIG_ERR){
			error_("signal SIGPIPE");
		}
		setenv("QUERY_STRING",cgiargs,1);

		printf("%s   %s\n",filename,cgiargs );
		dup2(fd,STDOUT_FILENO);
		execve(filename,emptylist,environ);		//run CGI program
		error_("execve");
	}
	
}
