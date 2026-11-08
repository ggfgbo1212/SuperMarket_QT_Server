#ifndef __ADIM_H__
#define __ADIM_H__

#include "sql_connect.h"
#include "myserve.h"
#include <signal.h>
#include <string.h>

typedef struct {
    int num;
    char* buf;

}qtmes;

void manager_goods_remove(char* str);

void manager_member_query(char* str);

void manager_member_recharge(char* str);

void logout_anything_mem();

void logout_mem(char* str);
void shopp_Init();
void mem_init();

//qtmes Adim_Read();
int iszhuce(char* str);
void mem_register(char* str);
//等待MCU线程信息（RES）为35-添加会员，36-添加商品
void wait_mcucode();

int manager_read_operate(char* str, size_t str_size);
void shop_register(char* str);




#endif