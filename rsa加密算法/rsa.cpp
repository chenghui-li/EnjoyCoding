#include <cstdio>
#include <vector>
#include <cstdint>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
using namespace std;
const int maxn = 100005;
bool flag[100005];
vector<int> primeNum;
// 循环右移
static uint32_t ROTR(uint32_t w, uint32_t n) {
    return (w >> n) | (w << (32 - n));
}

// 右移
static uint32_t SHR(uint32_t w, uint32_t n) {
    return w >> n;
}

// 6个逻辑函数
static uint32_t Conditional(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ ((~x) & z);
}

static uint32_t Majority(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

static uint32_t BSIG0(uint32_t x) {
    return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
}

static uint32_t BSIG1(uint32_t x) {
    return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
}

static uint32_t SSIG0(uint32_t x) {
    return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3);
}

static uint32_t SSIG1(uint32_t x) {
    return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10);
}

static uint32_t A, B, C, D, E, F, G, H;

static uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void initCache() {
    A = 0x6A09E667;
    B = 0xBB67AE85;
    C = 0x3C6EF372;
    D = 0xA54FF53A;
    E = 0x510E527F;
    F = 0x9B05688C;
    G = 0x1F83D9AB;
    H = 0x5BE0CD19;
}

// 位填充
void pad(std::vector<uint8_t> &message) {
    uint64_t len = message.size() * 8;
    message.push_back(0x80);
    while ((message.size() * 8 % 512) != 448) {
        message.push_back(0x00);
    }
    for (int i = 0; i < 8; ++i) {
        message.push_back((len >> ((7 - i) * 8)) & 0xff);
    }
}

// 将整数转换为大端序，SHA256字节序为大端序，X86架构一般为小端序
void bigEndian(uint32_t x, uint8_t bytes[4]) {
    for (int i = 0; i < 4; ++i) {
        bytes[3 - i] = (x & 0xff);
        x >>= 8;
    }
}

// 计算msg（512位）的摘要（256位），存入digest
void digest(uint8_t msg[64], uint8_t digest[32]) {
    uint32_t W[64];
    for (int i = 0; i < 16; ++i) {
        W[i] = 0;
        for (int j = 0; j < 4; ++j) {
            W[i] = (W[i] << 8) + msg[i * 4 + j];
        }
    }

    for (int i = 16; i < 64; ++i) {
        W[i] = SSIG1(W[i - 2]) + W[i - 7] + SSIG0(W[i - 15]) + W[i - 16];
    }

    uint32_t a = A, b = B, c = C, d = D, e = E, f = F, g = G, h = H, t1, t2;
    for (int i = 0; i < 64; ++i) {
        t1 = h + BSIG1(e) + Conditional(e, f, g) + K[i] + W[i];
        t2 = BSIG0(a) + Majority(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    A = (A + a) & 0xffffffff;
    B = (B + b) & 0xffffffff;
    C = (C + c) & 0xffffffff;
    D = (D + d) & 0xffffffff;
    E = (E + e) & 0xffffffff;
    F = (F + f) & 0xffffffff;
    G = (G + g) & 0xffffffff;
    H = (H + h) & 0xffffffff;
    bigEndian(A, digest);
    bigEndian(B, digest + 4);
    bigEndian(C, digest + 8);
    bigEndian(D, digest + 12);
    bigEndian(E, digest + 16);
    bigEndian(F, digest + 20);
    bigEndian(G, digest + 24);
    bigEndian(H, digest + 28);
}

void sha256(std::vector<uint8_t>& message, uint8_t buffer[32]) {
    pad(message);
    initCache();
    for (int i = 0; i < message.size(); i += 64) {
        digest(message.data() + i, buffer);
    }
}
void getPrimeNum(){
    for(int i = 0;i<maxn;i++)
        flag[i] = true;
    flag[0] = 0;
    flag[1] = 0;
    for (int i = 2; i <= maxn; ++i)
    {
        if(flag[i] == true)
        for(int j = 2*i;j<=maxn;j+=i){
            flag[j] = 0;
        }

    }
    for(int i = 2;i<=maxn;i++){
        if(flag[i] == true){
           // printf("%d ",i);
            primeNum.push_back(i);
        }
    }
    //cout<<count<<endl;   //9593个素数
}
int extgcd(int a, int64_t b,int &x, int &y){
    if(b == 0){
        x = 1,y = 0;
        return a;
    }
    else{
        int d = extgcd(b, a % b, y, x);
        y -= (a / b) * x;
        return d;
    }
}
int quickCal(int a,int b,int64_t mod){
    long long  ans = 1;
    long long now = a;
    while(b){
        if(b & 1){
            ans = ans * now % mod;
        }
        b = b >> 1;
        now = now * now % mod;
    }
    return ans;
}
int main() {
    //string 
    char input[100005];
    char send[100005];
    memset(input,0,sizeof(input));
    memset(send,0,sizeof(send));
    //cin>>input;
    char *buff;
    char *word;
    if((buff = getenv("QUERY_STRING")) != NULL){
        word = strchr(buff,'=');
        strcpy(input,word+1);
    }
    uint8_t byte;
    std::vector<uint8_t> message;
    getPrimeNum();
    for(int i = 0;i<strlen(input);i++) {
        byte = input[i];
        message.push_back(byte);
    }
    uint8_t buffer[32];
    sha256(message, buffer);
    //for (int i = 0; i < 32; ++i) 
    //    printf("%02x\n",buffer[i]);
    int index1 = 123,index2 = 321;
    int p = primeNum[index1],q = primeNum[index2];
    int64_t n =  p*q ;
    int64_t phn = (p-1)*(q-1);
    //vector<int>::iterator low = lower_bound(primeNum.begin(),primeNum.end(),phn);
    int e = primeNum[2];
    int x,y;
    extgcd(e,phn,x,y);
    //cout<<x<<endl;
    //printf("%d %d %lld %lld %lld\n",p,q,n,phn,e );
    int d = x;    //（n,d)私钥,(n,e)公钥
    //sprintf(send,"n:%lld<br>d:%d<br>%d<br>",n,d,e);

    //cout<<d<<endl<<e<<endl;
    string now = "";
    for (int i = 0; i < 32; ++i)
    {
        /* code */
        now += to_string(buffer[i]);
        now += " ";

    }
    string nownum = "";
    for (int i = 0; i < 32; ++i) { 
        //int now = buffer[i];
        //sprintf(send,"<h1><h1>" ,buffer[i]);
        int send1 = quickCal(buffer[i],d,n);
       // cout<<"send: "<<send<<endl;
        int recve = quickCal(send1,e,n);
        nownum += to_string(recve);
        nownum += " ";
    }
    sprintf(send,"<h1>原摘要：%s<br>解密后摘要：%s<br><h1>",now.c_str(),nownum.c_str() );
    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n",(int)strlen(send));
    printf("Content-type:text/html;charset=utf-8\r\n\r\n");
    printf("%s",send);
    fflush(stdout);
    return 0;
}