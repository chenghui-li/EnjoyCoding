#include "myIO.h"
#include "mysocket.h"
ssize_t readn(int fd,void *vptr,size_t n){
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft>0){
		if((nread = read(fd,ptr,nleft)) < 0){
			if(errno == EINTR)
				nread = 0;
			else
				return -1;
		}else if(nread == 0)
			break;     //EOF
		nleft -= nread;
		ptr += nread;
	}
	return (n-nleft);
}
ssize_t writen(int fd,const void *vptr,size_t n){
    size_t nleft;       //unsigned int
	ssize_t nwriten;    //int
	const char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft > 0){
		if((nwriten = write(fd,ptr,nleft)) <= 0){
			if(nwriten <0 && errno == EINTR)
				nwriten = 0;
			else
				return -1;

		}
		nleft -= nwriten;
		ptr += nwriten;
	}
	return (n);
}
ssize_t readline(int fd,void *vptr,size_t maxlen){
	ssize_t n,rc;
	char c,*ptr;
	ptr = vptr;
	for(n = 1;n<maxlen;n++){
		again:
		if((rc = read(fd,&c,1)) == 1){
			*ptr++ = c;
			if(c == '\n')
				break;

		}else if(rc == 0){
			*ptr = 0;
			return (n-1);
		}else{
			if(errno == EINTR)
				goto again;
			return -1;
		}

	
	}
	*ptr = 0;
	return (n);
}
