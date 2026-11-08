#include "mcu_thread.h"

int mcu_client;



void* mcu_thread_function(void* arg)
{
    clientmes ret = *(clientmes*)arg;
    mcu_client = ret.client_fd;
    printf("MCU线程创建成功\n");


    //write(mcu_client, "mcu test\n",sizeof("mcu test\n"));

    while(1)
    {
        char codebuf[20];
		//接收单片机发来的信息，将前面第一个数字转为int返回出来，剩余(卡号)存放在codebuf中
        int data = Read_mcu(codebuf);
        printf("data=%d codebuf=%s\n",data,codebuf);
        switch(data)
        {
            case 1://单片机发送1+卡号过来了
                send_singal(codebuf);//接收到单片机发来的卡号，存放在信息里，使用信号通知
            break;
        }
        
    }

}


void mcu_thread_create(clientmes ret)
{
    int pthret;
    pthread_t tid;

    pthret = pthread_create(&tid,NULL,mcu_thread_function,(void*)&ret);
    if(pthret != 0)
    {
        printf("create error\n");
    }
    pthread_detach(tid);

}


