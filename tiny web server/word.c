#include "web.h"  
char *word[16] = {
		"abandon",
		"aboard",
		"absolute",
		"absolutely",
		"absorb",
		"abstract",
		"abundant",
		"abuse",
		"academic",
		"accelerate",
		"access",
		"accidental",
		"accommodate",
		"accommodation",
		"accompany",
		"accomplish"
};
char *chn[16] = {
	"/ ə’bændən/ vt.丢弃;放弃，抛弃",
	"/ ə’bɔ:d/ ad.在船(车)上;上船",
	"/ ‘æbsəlu:t/ a.绝对的;纯粹的",
	"/ ‘æbsəlu:tli/ ad.完全地;绝对地",
	"/ əb’sɔ:b/ vt.吸收;使专心",
	"/ ’æbstrækt/ n.摘要",
	"/ ə’bΛndənt/ a.丰富的;大量的",
	"/ ə’bju:z, ə’bju:s/ vt.滥用;虐待 n.滥用",
	"/ ækə’demik/ a.学院的;学术的",
	"/ æk’seləreit/ vt.(使)加快;促进",
	"/ ‘ækses/ n.接近;通道，入口",
	"/ æksi’dentl/ a.偶然的;非本质的",
	"/ ə’kɔmədeit/ vt.容纳;供应，供给",
	"/ ə,kɔmə’deiʃən/ n.招待设备;预定铺位",
	"/ ə’kΛmpəni/ vt.陪伴，陪同;伴随",
	"/ ə’kɔmpliʃ/ vt.达到(目的);完成"
};
int main(){
	char *buff,buff1[1024],buff2[1024],content[8192];
	char *p;
	bzero(&buff1,sizeof(buff1));
	bzero(&buff2,sizeof(buff2));
	bzero(&content,sizeof(content));
	if((buff =getenv("QUERY_STRING")) != NULL){
		p = strchr(buff,'=');
		strcpy(buff1,p+1);
		for(int i = 0;i<16;i++){
			if(strcmp(buff1,word[i]) == 0){
				strcpy(buff2,chn[i]);
				break;
			}
		}
	}
	if(buff2[0] == '\0')
		strcpy(buff2,"not find\0");
	//sprintf(content,"QUERY_STRING=%s\r\n",buff1);
	//sprintf(content,"<h1>the word you find is %s<h1>\r\n",buff1);
	sprintf(content,"<h1>%s<h1>\r\n",buff2);
	printf("Connection: close\r\n");
	printf("Content-length: %d\r\n",(int)strlen(content));
	printf("Content-type:text/html;charset=utf-8\r\n\r\n");
	printf("%s",content);
	fflush(stdout);
	exit(0);
}
