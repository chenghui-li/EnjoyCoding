#include "table.h"
#include<string.h>
class DES{
private:
	char LR[70];
	char L[35],R[35],nextL[35];
	char ex[50];
	char K[70];
	char ans[70];
	char C[30];
	char D[30];
	char nowK[50];
	long long num;
	long long Knum;
public:
	DES(long long _num,long long _Knum):num(_num),Knum(_Knum){
		memset(LR,0,sizeof(LR));	
		memset(L,0,sizeof(L));
		memset(R,0,sizeof(R));
		memset(ex,0,sizeof(ex));
		memset(C,0,sizeof(C));
		memset(D,0,sizeof(D));
		memset(K,0,sizeof(K));
		memset(nextL,0,sizeof(nextL));
		memset(nowK,0,sizeof(nowK));

	}
	
	void change();  //long long to char
	void opIP();
	void split();   //64 bits to 2*32 bits
	void opE();   // 32 bits to 48 bits
	void opP();
	void getK();
	void getnowK(int j);
	void opK();
	void opS();   // 48 to 32
	void opAndOr();
	void F();
	void swap();
	void combination();
	void opPI();
	void print();
};
void getnum(long long &,long long &);
