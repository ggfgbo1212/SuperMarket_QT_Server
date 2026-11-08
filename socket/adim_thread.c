#include "adim_thread.h"


extern int RES;

int adim_client;


void *adim_thread_function(void *arg)
{
    clientmes ret = *(clientmes*)arg;
    adim_client = ret.client_fd;
    printf("服务端线程创建成功\n");

    //等待MCU线程信息（RES）为35-添加会员，36-添加商品
    wait_mcucode();
    char operation_str[200] = {0}; // 用于存储操作数据

    while(1)
    {
        
        memset(operation_str, 0, sizeof(operation_str));
   		int operation = manager_read_operate(operation_str, sizeof(operation_str));
        

        switch(operation)
        {
            case 1://点击扫描卡号按键，向服务端发送"01"
                if(RES == 34)
                {
                    RES = 36;
                    write(ret.client_fd, "2\n1\n",strlen("2\n1\n"));
                    //write(ret.client_fd, "hight",sizeof("hight"));
                }
                else
                {
                    write(ret.client_fd, "10\n",strlen("10\n"));
                }
            break;
            case 12://点击请求资源按键，向服务端发送"12"
                if(RES == 34)
                {
                    RES = 35;
                    write(ret.client_fd, "1\n1\n",strlen("1\n1\n"));
                    //write(ret.client_fd, "hight",sizeof("hight"));
                }
                else
                {
                    write(ret.client_fd, "10\n",strlen("10\n"));
                }
            break;
            case 2://zhuce botton
                mem_register(operation_str);
            break;
            case 7://根据卡号在数据库中查看是否已经被注册为用户或商品：
                shop_register(operation_str);//operation_str-"编号名称$价格&品牌"
            break;
            case 10://发送 "9\n"+ 商品信息-更新商品信息
                shopp_Init();
            break;
            case 11://发送 "8\n"+ 会员信息
                mem_init();
            break;
            case 5://注销卡
                logout_mem(operation_str);
            break;
            case 6://删除全部会员信息
                logout_anything_mem();
            break;
            case 3://充值。接收到卡号+金额
                manager_member_recharge(operation_str);
            break;
            case 4://查询会员信息，接收到电话号码
                manager_member_query(operation_str);
            break;
            case 8://删除商品
                manager_goods_remove(operation_str);
            break;
        }

    }
    
    
}


void Adim_thread_create(clientmes ret)
{
    int pthret;
    pthread_t tid;

    pthret = pthread_create(&tid,NULL,adim_thread_function,(void*)&ret);
    if(pthret != 0)
    {
        printf("create error\n");
    }
    pthread_detach(tid);

}