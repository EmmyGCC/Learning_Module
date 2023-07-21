#include <string.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
 
 
/***********************************
* 非线性函数
* (&是与,|是或,~是非,^是异或) 
* 
* 这些函数是这样设计的：
*   如果X、Y和Z的对应位是独立和均匀的，
*   那么结果的每一位也应是独立和均匀的。 
* 
* 函数F是按逐位方式操作：如果X，那么Y，否则Z。
* 函数H是逐位奇偶操作符
**********************************/
#define F(x,y,z) ((x & y) | (~x & z))  
#define G(x,y,z) ((x & z) | (y & ~z))  
#define H(x,y,z) (x^y^z)  
#define I(x,y,z) (y ^ (x | ~z))  
 
 
/**************************************
*向左换移(右环移)n个单位
* ************************************/
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))  
 
/****************************************************
* 每次操作对a，b，c和d中的其中三个作一次非线性函数运算
*  F(b,c,d)   G(b,c,d)   H(b,c,d)   I(b,c,d)
*
* 然后将所得结果加上 第四个变量(a)，
* F(b,c,d)+a
*
* 文本的一个子分组(x)
* F(b,c,d)+a+x
* 
* 和一个常数(ac)。
* F(b,c,d)+a+x+ac
*
* 再将所得结果向右环移一个不定的数(s)，
* ROTATE_LEFT( F(b,c,d)+a+x+ac , s )
* 
* 并加上a，b，c或d中之一(b)。
* ROTATE_LEFT( F(b,c,d)+a+x+ac , s )+b
* 
* 最后用该结果取代a，b，c或d中之一(a)。
* a=ROTATE_LEFT( F(b,c,d)+a+x+ac , s )+b
* 
* ***************************************************/
#define FF(a,b,c,d,x,s,ac) { a += F(b,c,d) + x + ac;  a = ROTATE_LEFT(a,s); a += b; }
#define GG(a,b,c,d,x,s,ac) { a += G(b,c,d) + x + ac;  a = ROTATE_LEFT(a,s); a += b; }
#define HH(a,b,c,d,x,s,ac) { a += H(b,c,d) + x + ac;  a = ROTATE_LEFT(a,s); a += b; }
#define II(a,b,c,d,x,s,ac) { a += I(b,c,d) + x + ac;  a = ROTATE_LEFT(a,s); a += b; }
 
 
//储存一个MD5 text信息 
typedef struct  
{  
    unsigned int count[2];    
    //记录当前状态，其数据位数   
 
    unsigned int state[4];    
    //4个数，一共32位 记录用于保存对512bits信息加密的中间结果或者最终结果  
 
    unsigned char buffer[64];
    //一共64字节，512位      
}MD5_CTX;  
 
 
//第一位1 其后若干个0,用于MD5Final函数时的补足
unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  
 
 
//函数声明
void MD5Init(MD5_CTX *context);  
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);  
void MD5Final(MD5_CTX *context,unsigned char digest[16]);  
void MD5Transform(unsigned int state[4],unsigned char block[64]);  
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);  
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);  
 
 
/************************
* 函数功能：初始化一个MD5 text
* 函数参数：MD5 text 指针
* ***********************/
//初始化
void MD5Init(MD5_CTX *context)  
{  
    context->count[0] = 0;  
    context->count[1] = 0;   
    //分别赋固定值  
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;  
    context->state[2] = 0x98BADCFE;  
    context->state[3] = 0x10325476;  
}  
 
 
/************************************************
* 函数功能：对一个MD5 text,把输入的数据进行分组，并进行加密
* 未用到的数据把其储存在MD5 text中。
*
* 参数分析：
* MD5_CTX *context       ：一个MD5 text   
* unsigned char *input   ：新添加的数据  
* unsigned int inputlen  ：新添加数据的长度(字节)
*
***********************************************/
 
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen)  
{  
    unsigned int i = 0,index = 0,partlen = 0;  
 
    //index：当前状态的位数对64取余，其单位是字节
    //也可以写作：  index=(context->count[0]/8)%64
    index = (context->count[0] >> 3) & 0x3F;  
    //partlen:可以补齐64字节的字节数
    partlen = 64 - index;
 
    //下面代码是解决一个unsignde int 无法储存极大数据导致溢出的问题
    //当前位数加上新添加的位数，由于inputlen是以字节为单位，所以其转换为位数
    //相当于context->count[0] += inputlen*8;  
    context->count[0] += inputlen << 3;  
 
    //当其出现溢出的情况时，通过以下操作把两个16位的数连在一块，生成一个
    //32位的二进制数串，从而扩大其储存范围
    if(context->count[0] < (inputlen << 3))
    {
        context->count[1]++;
    }
 
    //该语句可替换为 context->count[1]+=(inputlen<<3)>>32;
    //便于理解
    context->count[1] += inputlen >> 29;  
    //当其输入字节数的大于其可以补足64字节的字节数，进行补足
    if(inputlen >= partlen)  
    {  
        //向buffer中补足partlen个字节，使其到达64字节
        memcpy(&context->buffer[index], input, partlen);
 
        //buffer达到64字节512位，则把其作为一组进行运算
        MD5Transform(context->state, context->buffer);  
 
        //如果输入的数据还可以组成多个64字节，则把其可以组成
        //的作为若干组进行运算
        for(i = partlen;i+64 <= inputlen;i+=64)  
            MD5Transform(context->state, &input[i]);  
 
        //恢复0值，照应 下面 把输入 剩余字节(不能组成64字节组) 储存的操作
        index = 0;          
    }   
    //否则，把输入的数据按顺序放在原来数据后面
    else  
    {  
        i = 0;  
    }  
 
    //放置剩余数据
    memcpy(&context->buffer[index],&input[i],inputlen-i);  
}  
 
 
/*************************************************
* 函数功能：对数据进行补足，并加入数据位数信息，并进一步加密
* 
* 参数分析：
* MD5_CTX *context          :一个MD5 text
* unsigned char digest[16]  :储存加密结果的数组
*************************************************/
 
void MD5Final(MD5_CTX *context,unsigned char digest[16])  
{  
    unsigned int index = 0,padlen = 0;  
 
    //bits： 8个字节，64位
    unsigned char bits[8];  
 
    //index:对64取余结果
    index = (context->count[0] >> 3) & 0x3F;  
    //因为要填充满足使其位长对512求余的结果等于448（56位）
    //所以当其所剩余的数小于56字节，则填充56-index字节，
    //否则填充120-index字节
    //这里padlen代表其所需填充的字节
    padlen = (index < 56)?(56-index):(120-index);  
 
    //然后，在这个结果后面附加一个以64位二进制表示的填充前数据长度。
    //把填充前数据数据长度转换后放到bit字符数组中
    MD5Encode(bits,context->count,8);
 
    //根据已经存储好的数组PADDING，在信息的后面填充一个1和无数个0，
    //直到满足上面的条件时才停止用0对信息的填充
    //其填充后进行了一系列的加密操作，其定剩余48个字节
    MD5Update(context,PADDING,padlen);  
 
    //在最后添加进8个字节的数据长度信息，最后凑成一组，进行一次加密处理
    MD5Update(context,bits,8);  
 
    //把最终得到的加密信息变成字符输出，共16字节
    MD5Encode(digest,context->state,16);  
}  
 
 
/**********************************************************
* 函数功能：利用位操作，按1->4方式把数字分解成字符
*
* 参数分析：
* unsigned char  *output ：输出的字符的数组
* unsigned int   *input  ：输入数字的数组
* unsigned int   len     : 输入数字数组的长度（单位：位） 
* *********************************************************/
 
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
    {  
        //这里& 0xFF为取后8位
        //i代表数字数组下标
        //j代表字符数组下标
        //把数字的8、8-16、16-24、24-32分别赋值给字符
        output[j] = input[i] & 0xFF;    
        output[j+1] = (input[i] >> 8) & 0xFF;  
        output[j+2] = (input[i] >> 16) & 0xFF;  
        output[j+3] = (input[i] >> 24) & 0xFF;  
        i++;  
        j+=4;  
    }  
}  
 
 
/**********************************************************
* 函数功能：利用位操作，按4->1方式把字符合成数字
*
* 参数分析：
* unsigned int  *output ：输出的数字的数组
* unsigned char *input  ：输入字符的数组
* unsigned int  len     : 输入字符的长度 （单位：位）
* *********************************************************/
 
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)  
{  
    unsigned int i = 0,j = 0;  
    while(j < len)  
    {  
        //利用位操作，把四个单位为1字节的字符，合成一个单位为4字节的数字
        //因为FF GG HH II和非线性函数都只能对数字进行处理
        //第一个字符占前8位，第二个占8-16位，第三个占16-24位，第四个占
        //24-32位。
        //i代表数字数组下标
        //j代表字符数组下标
        output[i] = (input[j]) |  
            (input[j+1] << 8) |  
            (input[j+2] << 16) |  
            (input[j+3] << 24);  
        i++;  
        j+=4;   
    }  
}  
 
 
/**************************************************************
* 函数功能：对512位的block数据进行加密，并把加密结果存入state数组中
* 对512位信息(即block字符数组)进行一次处理，每次处理包括四轮
*state[4]：md5结构中的state[4]，用于保存对512bits信息加密的中间结果或者最终结果
* block[64]：欲加密的512bits信息或其中间数据
***************************************************************/
void MD5Transform(unsigned int state[4], unsigned char block[64])  
{  
    //a b c d继承上一个加密的结果，所以其具有继承性
    unsigned int a = state[0];  
    unsigned int b = state[1];  
    unsigned int c = state[2];  
    unsigned int d = state[3];  
 
    //这里只需用到16个，我把原来的unsiged int x[64]  改为了 x[16]
    unsigned int x[16];  
 
    //把字符转化成数字，便于运算
    MD5Decode(x,block,64);  
 
 
    //具体函数方式固定，不再赘述
 
    /*************第一轮******************/
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478);   
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);   
    FF(c, d, a, b, x[ 2], 17, 0x242070db);   
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);   
 
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf);   
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a);   
    FF(c, d, a, b, x[ 6], 17, 0xa8304613);   
    FF(b, c, d, a, x[ 7], 22, 0xfd469501);   
 
    FF(a, b, c, d, x[ 8], 7, 0x698098d8);   
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);   
    FF(c, d, a, b, x[10], 17, 0xffff5bb1);   
    FF(b, c, d, a, x[11], 22, 0x895cd7be);   
 
    FF(a, b, c, d, x[12], 7, 0x6b901122);   
    FF(d, a, b, c, x[13], 12, 0xfd987193);   
    FF(c, d, a, b, x[14], 17, 0xa679438e);   
    FF(b, c, d, a, x[15], 22, 0x49b40821);   
 
 
    /*************第二轮*****************/
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562);   
    GG(d, a, b, c, x[ 6], 9, 0xc040b340);   
    GG(c, d, a, b, x[11], 14, 0x265e5a51);   
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);   
 
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d);   
    GG(d, a, b, c, x[10], 9,  0x2441453);   
    GG(c, d, a, b, x[15], 14, 0xd8a1e681);   
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);   
 
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6);   
    GG(d, a, b, c, x[14], 9, 0xc33707d6);   
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);   
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed);   
 
    GG(a, b, c, d, x[13], 5, 0xa9e3e905);   
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8);   
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9);   
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);   
 
 
    /*************第三轮*****************/
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942);   
    HH(d, a, b, c, x[ 8], 11, 0x8771f681);   
    HH(c, d, a, b, x[11], 16, 0x6d9d6122);   
    HH(b, c, d, a, x[14], 23, 0xfde5380c);   
 
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44);   
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);   
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);   
    HH(b, c, d, a, x[10], 23, 0xbebfbc70);   
 
    HH(a, b, c, d, x[13], 4, 0x289b7ec6);   
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);   
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);   
    HH(b, c, d, a, x[ 6], 23,  0x4881d05);   
 
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039);   
    HH(d, a, b, c, x[12], 11, 0xe6db99e5);   
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8);   
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);   
 
 
 
    /*************第四轮******************/
    II(a, b, c, d, x[ 0], 6, 0xf4292244);   
    II(d, a, b, c, x[ 7], 10, 0x432aff97);   
    II(c, d, a, b, x[14], 15, 0xab9423a7);   
    II(b, c, d, a, x[ 5], 21, 0xfc93a039);   
 
    II(a, b, c, d, x[12], 6, 0x655b59c3);   
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);   
    II(c, d, a, b, x[10], 15, 0xffeff47d);   
    II(b, c, d, a, x[ 1], 21, 0x85845dd1);   
 
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f);   
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0);   
    II(c, d, a, b, x[ 6], 15, 0xa3014314);   
    II(b, c, d, a, x[13], 21, 0x4e0811a1);   
 
    II(a, b, c, d, x[ 4], 6, 0xf7537e82);   
    II(d, a, b, c, x[11], 10, 0xbd3af235);   
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);   
    II(b, c, d, a, x[ 9], 21, 0xeb86d391);   
 
 
    //更换原来的结果
    state[0] += a;  
    state[1] += b;  
    state[2] += c;  
    state[3] += d;  
}
 
 
int main(int argc, char *argv[])  
{  
    MD5_CTX md5;  //定义一个MD5 text
    MD5Init(&md5);//初始化
    int i;
    //unsigned char encrypt[] ="admin";//要加密内容
    //加密结果：21232f297a57a5a743894a0e4a801fc3
 
    unsigned char encrypt[1000];//要加密内容
    printf("请输入要加密的字符串：");
    gets((char *)encrypt);
 
    unsigned char decrypt[16]; //加密结果
    MD5Update(&md5, encrypt, strlen((char *)encrypt));//进行初步分组加密
 
    MD5Final(&md5,decrypt);   //进行后序的补足，并加密 
 
    printf("加密前:%s\n加密后16位:",encrypt);  
    for(i=4;i<12;i++)  
    {  
        printf("%02x",decrypt[i]);
    }  
 
    printf("\n加密前:%s\n加密后32位:",encrypt);  
    for(i=0;i<16;i++)  
    {  
        printf("%02x",decrypt[i]);
    }
    printf("\n");
 
    return 0;  
}  