#include "des.h"
int main(){
	long long num;
	long long _K;
	getnum(num,_K);
	DES obj(num,_K);
	
	//obj.change();
	obj.F();
	//obj.opIP();
	//obj.split();
	//obj.opE();

}
void getnum(long long &num,long long &_K){
	cin>>num>>_K;
}
