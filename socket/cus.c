#include "cus.h"

extern int cus_client;
extern int RES;

//信号参数结构体
extern struct sigaction sig_meb;
extern struct sigaction sig_shopp;

extern SQLifconfig *MySQL_Handler;

int money;//买单需要的钱

void customer_final_checkout(char* str)
{
    char buf[1024];
    sprintf(buf, "select name,phone,balance  from users where id='%s'", str);

     //执行查询
    char* ret = SQLifconfig_Dql_sql(MySQL_Handler, buf);//查看数据库是否有这个对应卡号的会员卡ret = name#phone#balance#ff这种类型

     //检查查询结果
    if (ret == NULL || strcmp(ret, "0") == 0) {
        write(cus_client, "4\n", 2); // 查询失败或无数据
        return;
    }

    // 解析查询结果：name#phone#balance
    char name[100] = {0};
    char phone[12] = {0}; // 假设电话是11位
    char balance[100] = {0};

    // 使用 strtok 分割字符串
    char* token = strtok(ret, "#");
    if (token) strncpy(name, token, sizeof(name) - 1);

    token = strtok(NULL, "#");
    if (token) strncpy(phone, token, sizeof(phone) - 1);

    token = strtok(NULL, "#");
    if (token) strncpy(balance, token, sizeof(balance) - 1);

    printf("ret = %s\n",ret);

    // 将 balance 转换为整数
    int num = atoi(balance);

    // 检查余额是否足够
    if (num < money) 
	{
        char response[1024] = {0};
        sprintf(response, "5\n%s#%s#%s", name, phone, balance);
        write(cus_client, response, strlen(response));
        return;
    }

    // 更新余额
    char buf_up[100] = {0};
    sprintf(buf_up, "UPDATE users SET balance = balance - %d WHERE id='%s'", money,str);
    if (SQLifconfig_Dml_sql(MySQL_Handler, buf_up)) {
        // 更新成功，发送消费成功消息
        printf("消费成功\n");
        char sendbuf[100] = {0};
        sprintf(sendbuf, "3\n%s#%s#%d\n", name, phone, num - money);
        write(cus_client, sendbuf, strlen(sendbuf));

        // 调用 mcu 的 mcu_send_to_mcu 函数,给stm32下位机发送数据,开启舵机出货
        //if (g_mcu != NULL) mcu_send_to_mcu(g_mcu);
    }


    // 释放资源
    if(RES == 37)  RES = 34;

}

//申请资源RES= 37
void apply_RES37(char* str)
{
    printf("apply_RES37\n");
    // 检查 RES 是否等于 34
    if (RES != 34) {
        write(cus_client, "2\n", 2); 
        return;
    }

    // 解析字符串中的整数（total）
    char* endptr;
    money = strtol(str, &endptr, 10);
    //printf("一共需要%d\n",money);
    if (endptr == str) {
        // 如果解析失败，写入错误信息并返回
        write(cus_client, "2\n", 2);
        return;
    }

    // 更新 RES 的值
    RES = 37;

    // 写入成功信息
    write(cus_client, "6\n", 2);
}

/* 根据传进来的卡号在数据库中寻找对应的商品 */
void cust_sigal_functoin_add(int sig, siginfo_t *info,void *sd)
{
    printf("我要买东西 cust_sigal_functoin_add\n");

    sigval_t sig_val = info->si_value;

    char buf[1024];
    printf("**************%s\n",(char*)sig_val.sival_ptr);
    sprintf(buf,"select * from me where pid='%s'",(char*)sig_val.sival_ptr);
    
    //寻找数据库是否有对应的商品
	char *result = SQLifconfig_Dql_sql(MySQL_Handler, buf);
	if(result)//找到了
	{
		char response[1028]; // 假设结果不会超过 1028 字节
        strcpy(response, "1\n");
        strcat(response, result);
        write(cus_client, response, strlen(response));

	}
	else//no找到
	{
		write(cus_client, "4\n", 2);
        printf("没有这个商品\n");
	}


}

void cust_sigal_functoin_checkout(int sig, siginfo_t *info,void *sd)
{
    printf("我要买单 cust_sigal_functoin_checkout\n");

    char *str = (char*)info->si_value.sival_ptr;
    printf("%s\n",str);
    customer_final_checkout(str);
    money = 0;

    RES=34;//释放资源

}

void cus_wait_mcucode()
{
   sig_meb.sa_sigaction = cust_sigal_functoin_add;
   sig_meb.sa_flags = SA_SIGINFO;
   sigaction(34,&sig_meb,NULL); //RES为34，客户端选取商品

   sig_shopp.sa_sigaction = cust_sigal_functoin_checkout;
   sig_shopp.sa_flags = SA_SIGINFO;
   sigaction(37,&sig_shopp,NULL); //RES为37，客户买单
}


/* 将第一个字符串转为数字，其他保存在参数中 */
int custon_read_operate(char* str, size_t str_size) 
{
    char buf[1000] = {0};
    ssize_t bytes_read = read(cus_client, buf, sizeof(buf) - 1);
    if (bytes_read <= 0) return 0;
    buf[bytes_read] = '\0';

    int num = (buf[0] - '0');//将第一个字符串转为数字
    printf("mcu operation code:%d\n", num);

    // 关键修复：使用 str_size 控制复制长度
    strncpy(str, buf + 1, str_size - 1);
    str[str_size - 1] = '\0';

    return num;


}