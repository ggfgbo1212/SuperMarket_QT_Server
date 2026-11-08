#include "cus_thread.h"

int cus_client;

extern int RES;
extern int money;//买单需要的钱


void *cus_thread_function(void *arg)
{
    clientmes ret = *(clientmes*)arg;
    cus_client = ret.client_fd;
    printf("客户端线程创建成功\n");
    cus_wait_mcucode();//等待MCU刷卡，34为选购东西，37为买单


    char operation_str[200] = {0}; // 用于存储操作数据

    while(1)
    {
        memset(operation_str, 0, sizeof(operation_str));
   		int operation = custon_read_operate(operation_str, sizeof(operation_str));
        switch(operation)
        {
            case 1://结算-申请资源，使得RES变成37
                apply_RES37(operation_str);
                printf("一共需要%d\n",money);
            break;
                
            case 2://一定时间不结账就释放资源RES
                if(RES == 37) RES = 34;
            break;

            case 3://接收消费者消费时间以及其它信息
                /*根据电话获得用户的消费记录，将电话之后
                  的数据存入数据库中，限制大小为1000
                  同时将消费记录存入销售记录文件中，限制大小为10000
                 */
            break;
        }



    }

}

void cus_thread_create(clientmes ret)
{
    int pthret;
    pthread_t tid;

    pthret = pthread_create(&tid,NULL,cus_thread_function,(void*)&ret);
    if(pthret != 0)
    {
        printf("create error\n");
    }
    pthread_detach(tid);

}