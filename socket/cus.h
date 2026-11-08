#ifndef __CUS_H__
#define __CUS_H__

#include "sql_connect.h"
#include "myserve.h"
#include <signal.h>
#include <string.h>


int custon_read_operate(char* str, size_t str_size);
void cus_wait_mcucode();
void apply_RES37(char* str);

#endif