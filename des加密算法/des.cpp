#include <string.h>
#include <bitset>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#define MAXN 100000
using namespace std;
char send[MAXN];
string stabuf;
int sendindex = 0;
static int IP[] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

/* Inverse Initial Permutation Table */
static int PI[] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

/*Expansion table */
static int E[] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

/* Post S-Box permutation */
static int P[] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25
};

/* The S-Box tables */
static int S[8][64] = {{
	/* S1 */
	14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
},{
	/* S2 */
	15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
		3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
		0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
		13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
},{
	/* S3 */
	10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
		13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
		13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
		1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
},{
	/* S4 */
	7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
		13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
		10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
		3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
},{
	/* S5 */
	2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
		14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
		4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
		11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
},{
	/* S6 */
	12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
		10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
		9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
		4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
},{
	/* S7 */
	4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
		13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
		1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
		6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
},{
	/* S8 */
	13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
		1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
		7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
		2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
}};

/* Permuted Choice 1 Table */
static int PC1[] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};
// 密钥置换表，将64位密钥变成56位
int PC_1[] = {57, 49, 41, 33, 25, 17, 9,
			   1, 58, 50, 42, 34, 26, 18,
			  10,  2, 59, 51, 43, 35, 27,
			  19, 11,  3, 60, 52, 44, 36,
			  63, 55, 47, 39, 31, 23, 15,
			   7, 62, 54, 46, 38, 30, 22,
			  14,  6, 61, 53, 45, 37, 29,
			  21, 13,  5, 28, 20, 12,  4}; 

// 压缩置换，将56位密钥压缩成48位子密钥
int PC_2[] = {14, 17, 11, 24,  1,  5,
			   3, 28, 15,  6, 21, 10,
			  23, 19, 12,  4, 26,  8,
			  16,  7, 27, 20, 13,  2,
			  41, 52, 31, 37, 47, 55,
			  30, 40, 51, 45, 33, 48,
			  44, 49, 39, 56, 34, 53,
			  46, 42, 50, 36, 29, 32};
/* Permuted Choice 2 Table */
static int PC2[] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* Iteration Shift Array */
static int iteration_shift[] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

class DES{
private:
	bitset<64> LR,ans;
	bitset<32> L,R,nextL;
	bitset<48> ex,subkey1[16],subkey2[16];
	bitset<64> K1;
	bitset<64> K2;
	char toFile[64];
	bitset<28>C;
	bitset<28>D;
	char num[MAXN];
	char Knum1[16];    //8 bytes keyword1
	char Knum2[16];    //8 bytes keyword2
	int numlen;
	int fp;
public:
	DES(char *_num,char *_Knum1,char *_Knum2){
		memset(num,0,sizeof(num));
		strcpy(num,_num);
		strcpy(Knum1,_Knum1);
		strcpy(Knum2,_Knum2);
		//fp = open("key.txt",O_RDWR|O_CREAT|O_TRUNC,0);
	}
	
	void changeWordToBit();  //char to bitset and save to file
	void changeKeyToBit();
	void polling(bool decodeFlag,int which_key); //use 16 subkeys poll
	void opIP();
	void split();   //64 bits to 2*32 bits
	void opE();   // 32 bits to 48 bits
	void opP();
	void getK(char Knum[16],bitset<64> &K,int which);
	void getSubKey(int j);
	void getnowK(int j,bitset<48> &subkey);
	void init();
	void opK(int i,int which_key);
	void opS();   // 48 to 32
	void opAndOr();
	void incode(bool decodeFlag,int which_key);
	void swap();
	void combination();
	void opPI();
	void print();
	void readWordFromFile();
	~DES(){
	}
};

void DES::init(){
	LR.reset();
	ans.reset();
	L.reset();
	R.reset();
	nextL.reset();
	ex.reset();
	C.reset();
	D.reset();
}
void DES::getSubKey(int j){
	for(int i  = 1;i<=16;i++){
		if(j == 1)
			getnowK(i,subkey1[i-1]);
		else
			getnowK(i,subkey2[i-1]);
	}
}
void DES::changeWordToBit(){   //char to bitset save int file
	numlen = strlen(num);
	int index = 0;
	char toBeSave[70];
	memset(toBeSave,0,sizeof(toBeSave));
	for(int i = 0;i<numlen ;i++){
		int tc = num[i];
		for(int j = 0;j<8;j++){
			if(tc & 1){
				LR.set(index);
			}
			index++;
			tc = tc >> 1;
		}
		if(index > 63 && i<numlen-1){
			//incode();
			//strncpy(toBeSave,LR.to_string().c_str(),64);
			stabuf += LR.to_string();
			//write(fp,toBeSave,strlen(toBeSave));
			index = 0;
			//memset(toBeSave,0,sizeof(toBeSave));
			LR.reset();
		}
	}
	stabuf += LR.to_string();
	//incode();
	//strncpy(toBeSave,LR.to_string().c_str(),64);
	//write(fp,toBeSave,strlen(toBeSave));
	index = 0;
	//close(fp);
}
void DES::changeKeyToBit(){
	getK(Knum1,K1,1);
	init();
	getK(Knum2,K2,2);
	init();
}
void DES::opIP(){
	bitset<64> temp;
	int j = 63; 
	for(int i = 0;i<=63;i++){
		temp[j--] = LR[64 - IP[i]];
	}
	LR = temp;
}

void DES::split(){
	for(int i = 0;i<32;i++){
		R[i] = LR[i];
	}
	for(int i = 32;i<64;i++){
		L[i-32] = LR[i];
	}
	//cout<<"L:"<<L<<endl<<"R:"<<R<<endl;
}
void DES::opE(){
	ex.reset();
	int j = 47;
	for(int i = 0;i<=47;i++){
		ex[j--] = R[32-E[i]];
	}

	//cout<<"ex:"<<ex<<endl;
}
void DES::opK(int i,int witch_key){
	switch (witch_key){
		case 1:
			ex = ex^subkey1[i-1];
			break;
		case 2:
			ex = ex^subkey2[i-1];
			break;
	}
}
void DES::opS(){
	int nowRi = 0;
	for(int i = 0;i<=47;i+=6){
		int group = 7 - i/6;    //which S box
		int beg = i;    //begin
		int end = i+6-1;   //
		int dx = (ex[beg])*pow(2,0)+(ex[end])*pow(2,1);
		int dy = 0;
		for(int k = beg+1;k<end;k++){
			int mi = k - beg-1;
			dy = dy + (ex[k])*pow(2,mi);
		}
		int nownum = S[group][dx*16+dy];
		//cout<<"num:"<<nownum<<endl;
		for(int l = 0;l<4;l++){
			R[nowRi++] = (nownum & 1);
			nownum = nownum >> 1;
		}
	}

}

void DES::opP(){
	bitset<32> temp;
	int j = 31;
	for(int i = 0;i<32;i++){
		
		temp[j--] = R[32-P[i]];
	}
	R = temp;
}

void DES::getK(char Knum[16],bitset<64> &K,int which){
	int klen;
	int c;
	klen = strlen(Knum);
	int keyIndex = 0;
	for(int i = 0;i<klen;i++){
		c = Knum[i];
		for(int j = 0;j<8;j++){
			if(c & 1){
				K.set(keyIndex);
			}
			++keyIndex;
			c = c >> 1;
		}
	}
	keyIndex = 0;
	bitset<56> temp;
	int j = 55;
	for(int i = 0;i<56;i++){
		temp[j--] = K[64-PC1[i]];
	}
	for(int i = 0;i<28;i++){
		D[i] = temp[i];
	}
	for(int i = 28;i<56;i++){
		C[i-28] = temp[i];
	}	
	getSubKey(which);   //get 16 subkey
}
void DES::readWordFromFile(){
	char buff[70];
	memset(buff,0,sizeof(buff));
	int nread = 0;
	string nowWord = "";
	//int fp = open("key.txt",O_RDONLY,0);
	//if(fp < 0){
	////	cout<<"error"<<endl;
	//	return ;
	//}
	for (int i = 0; i < stabuf.size(); i+=64)
	{
		if(i+64 <= stabuf.size()){
			nowWord = stabuf.substr(i,64);
		}
		else{
			nowWord = stabuf.substr(i,stabuf.size() - i);
		}
		bitset<64> temp(nowWord);
		LR = temp;
		incode(true,1);
		incode(false,2);
		incode(true,1);

		incode(false,1);
		incode(true,2);
		incode(false,1);
		print();
	}
	/*while((nread = read(fp,buff,64)) >0){
		//cout<<"read:"<<nread<<endl;
		nowWord = buff;
		bitset<64> temp(nowWord);
		LR = temp;
		incode(true,1);
		incode(false,2);
		incode(true,1);

		incode(false,1);
		incode(true,2);
		incode(false,1);
		print();
	}
	close(fp);*/


}

void DES::getnowK(int j,bitset<48> &subkey){
	int step = 2;
	bitset<56>temp;
	bitset<48> nowK;
	if(j == 1 || j== 9 || j == 2 || j == 16)
		step = 1;
	for(int i = 0;i<=27;i++){
		temp[(i+step)%28] = C[i];
	}
	for(int i = 0;i<28;i++){
		C[i] = temp[i];
	}
	temp.reset();
	for(int i = 0;i<=27;i++){
		temp[(i+step)%28] = D[i];
	}
	for(int i = 0;i<28;i++){
		D[i] = temp[i];
	}
	temp.reset();
	int k = 0;
	for(int i = 0;i<28;i++){
		temp[k++] = D[i];
	}	
	for(int i = 0;i<28;i++){
		temp[k++] = C[i];
	}
	
	//cout<<"C:"<<C<<endl<<"D:"<<D<<endl;
	k = 47;
	for(int i = 0;i<=47;i++){
		nowK[k--] = temp[56-PC2[i]];
	}
	subkey = nowK;
	//cout<<j<<": "<<nowK<<endl;
}
void DES::opAndOr(){
	for(int i = 0;i<=31;i++){
		R[i] = R[i]^L[i];
	}
	L = nextL;
}
void DES::polling(bool decodeFlag,int witch_key){
	for(int i = 1;i<=16;i++){
		nextL = R;
		opE();
		if(decodeFlag == false)   //incode
			opK(17-i,witch_key);
		else
			opK(i,witch_key);   //decode
		opS();
		opP();
		opAndOr();
		L = nextL;
	}
}
void DES::incode(bool decodeFlag,int witch_key){
	opIP();
	split();     //split 64 to L0 and R0
	polling(decodeFlag,witch_key);
	swap();
	combination();
	opPI();
}

void DES::swap(){
	nextL = R;
	R = L;
	L = nextL;
}
void DES::combination(){
	int j = 0;
	for(int i = 0;i<32;i++){
		LR[j++] = R[i];
	}
	for(int i = 0;i<32;i++){
		LR[j++] = L[i];
	}

}
void DES::opPI(){
	//cout<<"before PI:"<<LR<<endl;
	int j =63;
	for(int i = 0;i<=63;i++){
		ans[j--] = LR[64-PI[i]];
	}
	LR = ans;

}
void DES::print(){
	/*for(int i = 0;i<64;i++){
		toFile[63-i] = ans[i] + '0';
	}
	write(fp,toFile,64);*/
	//cout<<"after PI:"<<ans<<endl;
	int now = 0;
	for(int i = 0;i<=63;i++){
		now += ans[i]*pow(2,i%8);
		if(i % 8 == 7){

			send[sendindex++] = now;
			now = 0;
		}
		
	}
}

int main(){
 char  mynum[MAXN] ;//= "abcdefghijklmnopqrstuvwxyz";
	char *buff;
	char *word,*key1,*key2;
	memset(send,0,sizeof(send));
	 char _key1[16],_key2[16] ;
	//cin>>mynum>>_key1>>_key2;
	if((buff = getenv("QUERY_STRING")) != NULL){
		word = strchr(buff,'=');
		key1 = strchr(buff,'&');
		bcopy(word+1,mynum,(int)(key1-word-1));
		key1 = strchr(key1,'=');
		key2 = strchr(key1+1,'&');
		bcopy(key1+1,_key1,(int)(key2-key1-1));
		key2 = strchr(key2,'=');
		strcpy(_key2,key2+1);
	}
	char toclient[MAXN];
	memset(toclient,0,sizeof(toclient));
	//cout<<"out:"<<mynum<<" "<<_key1<<" "<<_key2<<endl;
	
	DES obj(mynum,_key1,_key2);
	//obj.change();
	obj.init();
	obj.changeWordToBit();    //word to bitset
	obj.changeKeyToBit();
	obj.readWordFromFile();
	//sprintf(toclient,"\r\ndes解密后：%s\n",send);

	sprintf(toclient,"<h1>明文：%s<br>密钥1：%s<br>密钥2：%s<br><br>des解密后：%s<br><h1>",mynum,_key1,_key2,send);
	printf("Connection: close\r\n");
	printf("Content-length: %d\r\n",(int)strlen(toclient));
	printf("Content-type:text/html;charset=utf-8\r\n\r\n");
	printf("%s\n",toclient );
	fflush(stdout);
	exit(0);
	//obj.SaveCiphertextToFile();
	//obj.print();    //save cipherkey to file
	//obj.incode(false,1);
	//obj.incode(true,2);
	//obj.incode(false,1);
	//obj.print();    //get old word

}
