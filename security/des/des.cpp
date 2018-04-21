#include "des.h"
void DES::change(){   //long long to char
	long long temp = num;
	long long tempk = Knum;
	for(int i = 64;i>=1;i--){
		LR[i] = (temp & 1) + '0';
		temp = temp >> 1;
		K[i] = (tempk & 1)+'0';
		tempk = tempk >>  1;
	}

}

void DES::opIP(){
	char temp[70];
	memset(temp,0,sizeof(temp));
	for(int i = 1;i<=64;i++){
		temp[i] = LR[IP[i-1]];
	}
	strcpy(LR,temp);
}

void DES::split(){
	strncpy(L+1,LR+1,32);
	strncpy(R+1,LR+33,32);
}
void DES::opE(){
	for(int i = 1;i<=48;i++){
		ex[i] = R[E[i-1]];
	}
}
void DES::opK(){
	for(int i = 1;i<=48;i++){
		if(ex[i] != nowK[i])
			ex[i] = '1';
		else
			ex[i] = '0';
	}
}
void DES::opS(){
	int nowRi = 1;
	for(int i = 1;i<=48;i+=6){
		int group = i/6;    //which S box
		int beg = i;    //begin
		int end = i+6-1;   //
		int dx = (ex[end] - '0')*pow(2,0)+(ex[beg]-'0')*pow(2,1);
		int dy = 0;
		for(int k = end-1;k>beg;k--){
			int mi = end-1-k;
			dy = dy + (ex[k]-'0')*pow(2,mi);
		}
		int nownum = S[group][dx*16+dy];
		for(int l = 3;l>=0;l--){
			R[nowRi+l] = (nownum & 1)+'0';
			nownum = nownum >> 1;
		}
		nowRi += 4;
	}

}

void DES::opP(){
	char temp[35];
	memset(temp,0,sizeof(temp));
	for(int i = 1;i<=32;i++){
		
		temp[i] = R[P[i-1]];
	}
	temp[33] = '\0';
	strcpy(R,temp);
}
void DES::getK(){
	char temp[60];
	memset(temp,0,sizeof(temp));
	for(int i = 1;i<=56;i++){
		temp[i] = K[PC1[i-1]];

	}
	strncpy(C+1,temp+1,28);
	strncpy(D+1,temp+29,28);
	C[29] = '\0';
	D[29] = '\0';
}
void DES::getnowK(int j){
	int step = 2;
	char temp[60];
	if(j == 1 || j== 9 || j == 2 || j == 16)
		step = 1;
	for(int i = 27;i>=0;i--){
		temp[(i-step+28)%28+1] = C[i+1];
	}
	strncpy(C+1,temp+1,28);
	C[29] = '\0';
	for(int i = 27;i>=0;i--){
		temp[(i-step+28)%28+1] = D[i+1];
	}
	strncpy(D+1,temp+1,28);
	D[29] = '\0';
	strcpy(temp+1,C+1);
	strcpy(temp+29,D+1);
	for(int i = 1;i<=48;i++){
		nowK[i] = temp[PC2[i-1]];
	}
}
void DES::opAndOr(){
	for(int i = 1;i<=32;i++){
		if(L[i] != R[i])
			R[i] = '1';
		else
			R[i] = '0';
	}
	R[33] = '\0';
	strcpy(L,nextL);
}

void DES::F(){
	change();
	getK();
	opIP();
	split();     //split 64 to L0 and R0
	for(int i = 1;i<=16;i++){
		strcpy(nextL,R);
		opE();
		getnowK(i);
		opK();
		opS();
		opP();
		opAndOr();
		strcpy(L,nextL);
	}
	swap();
	opPI();
	print();
}

void DES::swap(){
	strcpy(nextL,R);
	strcpy(R,L);
	strcpy(L,nextL);
}
void DES::combination(){
	strncpy(LR+1,L+1,32);
	strncpy(LR+33,R+1,32);
	LR[65] = '\0';
}
void DES::opPI(){
	for(int i = 1;i<=64;i++){
		ans[i] = LR[PI[i-1]];
	}
	ans[65] = '\0';
}
void DES::print(){
	cout<<"the last ans is :"<<endl;
	cout<<ans+1<<endl;
}
