#include "adim.h"
#include <stdlib.h>

extern clientmes ret;
extern SQLifconfig *MySQL_Handler;

extern int RES;

extern int adim_client;
//信号参数结构体
struct sigaction sig_meb={0};
struct sigaction sig_shopp={0};

void manager_goods_remove(char* str)
{
    char buf[500] = {0};
    //char text_buf[100] = {0};

    // 构造 SQL 删除语句
    sprintf(buf, "DELETE FROM me WHERE pid='%s'", str);

    // 执行 SQL 删除操作
    int result = SQLifconfig_Dml_sql(MySQL_Handler, buf);
    if (result) 
	{
        // 删除成功，记录日志
        //sprintf(text_buf, "删除商品成功(卡号): %s", str);
        //manager_send_log(manager, text_buf);
    } 
	else 
	{
        // 删除失败，记录日志
        //sprintf(text_buf, "删除商品失败(卡号): %s", str);
        //manager_send_log(manager, text_buf);
    }
}

void manager_member_query(char* str)
{
    char buf[500] = {0};
    sprintf(buf, "SELECT id, name, phone, balance FROM users WHERE phone='%s'", str);

    // 执行查询
    char* ret = SQLifconfig_Dql_sql(MySQL_Handler, buf);

    // 检查查询结果
    if (ret == NULL) 
	{
        // 查询失败或无数据
        write(adim_client, "5\n1\n", 5); // 注意：write 的第三个参数是字节数
    } 
	else 
	{
        // 查询成功，拼接结果
        char sendstr[1024] = {0}; // 假设结果不会超过 1024 字节
        strcpy(sendstr, "5\n0\n");
        strcat(sendstr, ret);

        // 发送数据到套接字
        write(adim_client, sendstr, strlen(sendstr));
    }
    free(ret);
}

void manager_member_recharge(char* str)
{
    char pid[9] = {0}; // 用于存储前8位字符
    char balance[100] = {0}; // 用于存储余额部分

    // 截取前8位作为 pid
    strncpy(pid, str, 8);
    pid[8] = '\0'; // 确保字符串以 '\0' 结尾

    // 截取剩余部分作为 balance
    strncpy(balance, str + 8, sizeof(balance) - 1);
    balance[sizeof(balance) - 1] = '\0'; // 确保字符串以 '\0' 结尾

    // 将 balance 转换为整数
    int num = atoi(balance);

    // 构造 SQL 更新语句
    char buf[500] = {0};
    //修改数据库中需要充值的id的金额
    sprintf(buf, "UPDATE users SET balance = balance + %d WHERE id='%s'", num, pid);

    if (SQLifconfig_Dml_sql(MySQL_Handler, buf)) 
	{
        // 更新成功，查询用户信息
        char query_buf[500] = {0};
        sprintf(query_buf, "SELECT id, name, phone, balance FROM users WHERE id='%s'", pid);
        char* sendbuf = SQLifconfig_Dql_sql(MySQL_Handler, query_buf);

        if (sendbuf != NULL) 
		{
            // 在 sendbuf 前插入 "4\n"
            char response[1024] = {0};
            strcpy(response, "4\n");
            strcat(response, sendbuf);

            // 记录日志
            //char text_buf[100] = {0};
            //sprintf(text_buf, "用户: %s 成功充值: %s", pid, balance);
            //manager_send_log(manager, text_buf);

            // 发送数据到套接字
            write(adim_client, response, strlen(response));

            // 释放 sendbuf 内存
            free(sendbuf);
        }
    } 
	else 
	{
        // 更新失败，记录日志
        //char text_buf[100] = {0};
        //sprintf(text_buf, "用户: %s 充值失败", pid);
        //manager_send_log(manager, text_buf);
    }


}



void logout_anything_mem()
{
    if(SQLifconfig_Dml_sql(MySQL_Handler, "TRUNCATE TABLE users"))
	{
		//manager_send_log(manager, "清空用户");
	}
}

void logout_mem(char* str)
{
    char buf[500] = {0};
    //char text_buf[100] = {0};

    // 构造 SQL 删除语句
    sprintf(buf, "DELETE FROM users WHERE id='%s'", str);

    // 执行 SQL 删除操作
    int result = SQLifconfig_Dml_sql(MySQL_Handler, buf);
    if (result) 
	{
        // 删除成功，记录日志
        //sprintf(text_buf, "用户(卡号): %s 成功注销", str);
        //manager_send_log(manager, text_buf);
    } 
	else 
	{
        // 删除失败，记录日志
        //sprintf(text_buf, "用户(卡号): %s 注销失败", str);
        //manager_send_log(manager, text_buf);
    }


}

void mem_init()
{
    char* ret= SQLifconfig_Dql_sql(MySQL_Handler,"select * from users ");
    if(ret != NULL)//避免没有会员信息，引起段错误
    {
         // 在 ret 前插入 "9\n"
        char buffer[1024] = {0}; // 假设结果不会超过 1024 字节
        strcpy(buffer, "8\n");
        strcat(buffer, ret);

        // 发送数据到套接字
        write(adim_client, buffer, strlen(buffer));
        free(ret);
    }
    

}

void shopp_Init() 
{
    char* ret= SQLifconfig_Dql_sql(MySQL_Handler,"select * from me ");
    if(ret != NULL)//避免商品没信息返回空指针，防止段错误
    {
         // 在 ret 前插入 "9\n"
        char buffer[1024] = {0}; // 假设结果不会超过 1024 字节
        strcpy(buffer, "9\n");
        strcat(buffer, ret);

        // 发送数据到套接字
        write(adim_client, buffer, strlen(buffer));
    }
    
	free(ret);
} 

/* Read QTSreve mes adivide num, str */
int manager_read_operate(char* str, size_t str_size) 
{
	/* 新修改的
	 */
	char buf[200] = {0};
    ssize_t bytes_read = read(adim_client, buf, sizeof(buf) - 1);
    if (bytes_read <= 0) {
        return 0; // 读取失败或连接关闭
    }
    buf[bytes_read] = '\0'; // 确保字符串终止

    // 1. 检查数据长度是否足够
    if (bytes_read < 2) {
        str[0] = '\0';      // 清空输出缓冲区
        return -1;          // 返回错误码（数据不足）
    }

    // 2. 提取前两个字符作为操作码
    int num = (buf[0] - '0') * 10 + (buf[1] - '0'); // 直接计算两位数
    printf("manager operation code: %d\n", num);

    // 3. 安全复制剩余数据
    size_t remaining_len = bytes_read - 2; // 剩余数据长度
    size_t copy_len = (remaining_len < str_size - 1) ? remaining_len : (str_size - 1);
    memcpy(str, buf + 2, copy_len);       // 从第三个字符开始复制
    str[copy_len] = '\0';                 // 确保终止符

    return num;
}


int iszhuce(char* id)
{
    char buf1[500] = {0};
    sprintf(buf1, "SELECT pname FROM me WHERE pid='%s'", id);

    // 执行第一个查询
    char* ret1 = SQLifconfig_Dql_sql(MySQL_Handler, buf1);

    char buf2[500] = {0};
    sprintf(buf2, "SELECT name FROM users WHERE id='%s'", id);

    // 执行第二个查询
    char* ret2 = SQLifconfig_Dql_sql(MySQL_Handler, buf2);

    // 检查查询结果
    if (ret1 == NULL && ret2 == NULL) {
		free(ret1);
		free(ret2);
        printf("no重复\n");
        return 0; // 返回 false
    } else {
		free(ret1);
		free(ret2);
        printf("重复\n");
        return 1; // 返回 true
    }
}





void admin_sigal_functoin_meb(int sig, siginfo_t *info,void *sd)  //35
{
    printf("admin_sigal_functoin_meb\n");
    sigval_t sig_val = info->si_value;//接收卡号
    //char buf[50]={0};
    //sprintf(buf,"%s",(char*)sig_val.sival_ptr);
    //printf("%s\n",buf);
    char buf[50]={0};
    sprintf(buf,"1\n0\n%s",(char*)sig_val.sival_ptr);
    write(adim_client,buf,sizeof(buf));//将卡号发送给QT服务端用于注册会员

    RES=34;   //释放资源
}


void admin_sigal_functoin_shopp(int sig, siginfo_t *info,void *sd) //36
{
   printf("admin_sigal_functoin_shopp\n");
   sigval_t sig_val = info->si_value;

   char buf[50]={0};
   sprintf(buf,"2\n0\n%s",(char*)sig_val.sival_ptr);//2进入case2,0用于验证成功添加商品
   write(adim_client,buf,sizeof(buf));//将卡号发送给QT服务端用于添加商品
   RES=34;  //释放资源
}

//等待MCU线程信息（RES）为35-添加会员，36-添加商品
void wait_mcucode()
{
   sig_meb.sa_sigaction = admin_sigal_functoin_meb;
   sig_meb.sa_flags = SA_SIGINFO;
   sigaction(35,&sig_meb,NULL); //如果当前RES为35，就跳转admin_sigal_functoin_meb-添加会员

   sig_shopp.sa_sigaction = admin_sigal_functoin_shopp;
   sig_shopp.sa_flags = SA_SIGINFO;
   sigaction(36,&sig_shopp,NULL); //如果当前RES为36，就跳转admin_sigal_functoin_shopp-添加商品
}


void shop_register(char* str)
{
    char cod[9] = {0}; // 用于存储前8位字符
    char name[100] = {0}; // 用于存储商品名称
    char price[100] = {0}; // 用于存储商品价格
    char pp[100] = {0}; // 用于存储商品品牌

    // 截取前8位作为 cod
    strncpy(cod, str, 8);
    cod[8] = '\0'; // 确保字符串以 '\0' 结尾

    // 检查是否已经注册
    if (iszhuce(cod)) {
        write(adim_client, "6\n0\n", 5); // 注意：write 的第三个参数是字节数
        return;
    }

    // 查找分隔符 '$' 和 '&' 的位置
    const char* a1 = strchr(str + 8, '$'); // 从第8位开始查找 '$'
    const char* a2 = strchr(str + 8, '&'); // 从第8位开始查找 '&'
    printf("a1=%s a2=%s\n",a1,a2);

    if (a1 == NULL || a2 == NULL) {
        // 如果分隔符未找到，返回错误
        write(adim_client, "6\n0\n", 5);
        return;
    }

    // 截取 name
    strncpy(name, str + 8, a1 - (str + 8));
    name[a1 - (str + 8)] = '\0'; // 确保字符串以 '\0' 结尾

    // 截取 price
    strncpy(price, a1 + 1, a2 - (a1 + 1));
    price[a2 - (a1 + 1)] = '\0'; // 确保字符串以 '\0' 结尾

    // 截取 pinpai
    strncpy(pp, a2 + 1, strlen(a2 + 1));
    pp[strlen(a2 + 1)] = '\0'; // 确保字符串以 '\0' 结尾

    printf("str=%s,cod=%s,name=%s,price=%s,pp=%s\n",str,cod,name,price,pp);



  char buf[500]={0};
  sprintf(buf,"insert INTO me(pid,pname,price,brand) values  ('%s','%s','%s','%s')",cod,name,price,pp);
  if(SQLifconfig_Dml_sql(MySQL_Handler,buf))
  {
     //sprintf(text_buf,"添加商品(卡号): %s    价格: %s",name.c_str(),price.c_str());
     //set_send_rizhi(text_buf);

     write(adim_client,"6\n1\n",10);
  }
  else
  {
     //sprintf(text_buf,"添加商品(卡号): %s 失败",name.c_str());
     //set_send_rizhi(text_buf);

     write(adim_client,"6\n0\n",10);
  }
}




void mem_register(char* str)//str-people mess
{
    printf("enter mem_register\n");
    char code[9];
    char number[12];
    char name[100];

    strncpy(code, str, 8);//tiqu kahao
    if(iszhuce(code))//使用卡号检测是否已经注册
    {
       write(adim_client,"3\n0\n",10);
       return;
    }

    // 截取后11位作为 number
    strncpy(number, str + strlen(str) - 11, 11);
    number[11] = '\0'; // 确保字符串以 '\0' 结尾


    // 截取中间部分作为 nname
    strncpy(name, str + 8, strlen(str) - 11 - 8);
    name[strlen(str) - 11 - 8] = '\0'; // 确保字符串以 '\0' 结尾

 

    printf("注册成功str=%s code=%s num=%s name=%s\n",str, code,number,name);


    // 构造 SQL 插入语句
    char buf[500] = {0};
    sprintf(buf, "INSERT INTO users(id, name, phone, balance) VALUES ('%s', '%s', '%s', '0')", code, name, number);

    // 执行 SQL 插入操作
    if (SQLifconfig_Dml_sql(MySQL_Handler, buf)) {
        // 插入成功
        char text_buf[100] = {0};
        sprintf(text_buf, "用户:%s 号码:%s 卡号:%s 成功注册", name, number, code);
        //manager_send_log(manager, text_buf);

        write(adim_client, "3\n1\n", 5);
    } else {
        // 插入失败
        char text_buf[100] = {0};
        sprintf(text_buf, "用户:%s 号码:%s 注册失败,稍后再试", name, number);
        //manager_send_log(manager, text_buf);

        write(adim_client, "3\n0\n", 5);
    }

    
}
