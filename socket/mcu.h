#ifndef __MCU_H__
#define __MCU_H__

#include "myserve.h"

#include <string.h>
#include <signal.h>
#include "sql_connect.h"


int Read_mcu(char* codebuf);
void send_singal(char* str);

#endif