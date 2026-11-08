#include "mcu.h"


extern int mcu_client;
extern int RES;

sigval_t sig_val;





/* 读取单片机发来的信息 处理1+卡号*/
int Read_mcu(char* codebuf)
{
    char buf[100] = {0};
    char str1[20];
    char str2[20];

    int num;

    //char* temp = NULL;


    if(read(mcu_client,buf,sizeof(buf)) > 0)
    {
        printf("mcu.c read succes %s\n",buf);

        strncpy(str1, buf ,1);
        printf("%s\n",str1);
        num = atoi(str1);
        printf("num = %d\n",num);

        
        strncpy(str2,buf+1,sizeof(buf) - 1);
        printf("str2 = %s\n",str2); 


        //codebuf = malloc(sizeof(str2) + 1); // 分配足够的内存空间

        strncpy(codebuf, str2, sizeof(str2) - 1);  // 复制 str2 到 temp.buf
        printf("codebuf = %s\n",codebuf);
        //free(codebuf);
    }
    else
    {
        //perror("mcu read :");//打印错误信息
        //printf("%d\n",mcu_client);
    }


    return num;

}


void send_singal(char* str)
{
    printf("send_singal\n");
    if(RES == 34) 
    {
        printf("RES = 34\n");
    }

    sig_val.sival_int = 0; //伴随数据-没用的
    sig_val.sival_ptr =str; //伴随数据(其实就是卡号)

    //根据RES状态决定将卡号发送给那个线程(客户端或服务端)
    //34,37是给客户端发送卡号(37-结账或34-选购商品)
    //35,36是给服务端发送卡后(36-添加商品或者35-添加会员)
    if(sigqueue(getpid(),RES,sig_val)==-1)//给当前进程发送信号RES，并且发送数据（卡号）
    {
     perror("send sig error");
     return ;
    }


}



