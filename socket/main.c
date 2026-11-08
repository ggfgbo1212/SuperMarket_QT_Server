#include "sql_connect.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <pthread.h>
#include "myserve.h"

#include "adim_thread.h"
#include "mcu_thread.h"
#include "cus_thread.h"


//#define M_IP "192.168.254.128"
int RES = 34;

int i = 0;
clientmes ret;

SQLifconfig *MySQL_Handler;












int main() 
{
    //socket;
    int serve_fd = socket_create(AF_INET, SOCK_STREAM, 0);

    // 初始化数据库连接对象
    MySQL_Handler = SQLifconfig_init();
    if (!MySQL_Handler) {
        printf("初始化数据库失败\n");
        return -1;
    }
    // 连接数据库                                                               //数据库名字
    if (!SQLifconfig_SQL_init(MySQL_Handler, "127.0.0.1", "root", "123456", "shopping")) {
        printf("连接数据库失败\n");
        SQLifconfig_destroy(MySQL_Handler);
        return -1;
    }
    while(1)
    {
        
        ret = socket_accept(serve_fd);

        char* data = inet_ntoa(ret.s_addr.sin_addr);
        printf("%s\n",data);

        /* wait mess to create thread */
        char read_data[100] = {0};
        int red = read(ret.client_fd, read_data, sizeof(read_data));
        if(red <= 0)
        {
            //printf("read error\n");
        }

        int connect_flag = atoi(read_data);
        //printf("%d\n",connect_flag);

        switch(connect_flag)
        {
            case 100101://adim connect服务端线程
                //pthret = pthread_create(&tid,NULL,adim_thread_function,(void*)&ret);
                Adim_thread_create(ret);
            break;

            case 100111://custom connect客户端线程
                cus_thread_create(ret);
            break;

            case 101001://MCU connectMCU端线程
                mcu_thread_create(ret);
            break;
        }

    }
    
    

    

    //

    



  //执行插入操作                         //表名
  const char *insert_sql = "INSERT INTO users(id, name,phone,balance) VALUES('888','hxy','666',100)";
  if (SQLifconfig_Dml_sql(MySQL_Handler, insert_sql)) {// 执行操作 SQL 语句（INSERT/UPDATE/DELETE）
      printf("插入成功\n");
  } else {
      printf("插入失败\n");
  }

  insert_sql = "INSERT INTO me(pid, pname,price,brand) VALUES('888','hxy','666',100)";
  if (SQLifconfig_Dml_sql(MySQL_Handler, insert_sql)) {// 执行操作 SQL 语句（INSERT/UPDATE/DELETE）
      printf("插入成功\n");
  } else {
      printf("插入失败\n");
  }


  // 执行查询操作                        //表名     
  const char *select_sql = "SELECT * FROM users";
  char *result = SQLifconfig_Dql_sql(MySQL_Handler, select_sql);// 执行查询 SQL 语句（SELECT）
  if (result) {
      printf("查询结果:\n%s", result);
      free(result); // 必须手动释放内存
  } else {
      printf("查询失败或无结果\n");
  }

  select_sql = "SELECT * FROM me";
  result = SQLifconfig_Dql_sql(MySQL_Handler, select_sql);// 执行查询 SQL 语句（SELECT）
  if (result) {
      printf("查询结果:\n%s", result);
      free(result); // 必须手动释放内存
  } else {
      printf("查询失败或无结果\n");
  }

 
  



  while(1)
  {

  }

    
    //释放数据库连接对象
    SQLifconfig_destroy(MySQL_Handler);

    close(serve_fd);
    return 0;
}



