#ifndef __SQL_CONNECT_H
#define __SQL_CONNECT_H

#include <mysql.h>
#include <stdbool.h>

// 数据库连接结构体
typedef struct {
    MYSQL *mysql;       // MySQL 连接对象
    MYSQL_ROW row;      // 当前行数据
    MYSQL_RES *result;  // 查询结果集
    MYSQL_FIELD *field; // 字段信息
} SQLifconfig;

// 初始化数据库连接
SQLifconfig* SQLifconfig_init();

// 释放数据库连接
void SQLifconfig_destroy(SQLifconfig *config);

// 初始化数据库连接
bool SQLifconfig_SQL_init(SQLifconfig *config, const char *host, const char *user, const char *pwd, const char *dbname);

// 执行查询 SQL 语句（SELECT）
char* SQLifconfig_Dql_sql(SQLifconfig *config, const char *sqlstr);

// 执行操作 SQL 语句（INSERT/UPDATE/DELETE）
bool SQLifconfig_Dml_sql(SQLifconfig *config, const char *sqlstr);

#endif

