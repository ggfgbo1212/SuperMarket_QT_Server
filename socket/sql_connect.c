#include "sql_connect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 初始化数据库连接对象
SQLifconfig* SQLifconfig_init() {
    SQLifconfig *config = (SQLifconfig*)malloc(sizeof(SQLifconfig));
    if (!config) return NULL;

    // 初始化成员变量
    config->mysql = NULL;
    config->row = NULL;
    config->result = NULL;
    config->field = NULL;

    // 初始化 MySQL 连接
    config->mysql = mysql_init(NULL);//参数为NULL，函数将分配初始化，并初始化、返回新对象
    if (!config->mysql) {
        printf("Init Error: %s\n", mysql_error(config->mysql));
        free(config);
        return NULL;
    }

	// 强制设置字符集
    if (mysql_options(config->mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4")) {
        printf("Set UTF-8 Error: %s\n", mysql_error(config->mysql));
        mysql_close(config->mysql);
        free(config);
        return NULL;
    }

    return config;
}

// 释放数据库连接对象
void SQLifconfig_destroy(SQLifconfig *config) {
    if (config) {
        if (config->mysql) {
            mysql_close(config->mysql);//关闭数据库连接
        }
        free(config);//释放空间
    }
}

// 初始化数据库连接
bool SQLifconfig_SQL_init(SQLifconfig *config, const char *host, const char *user, const char *pwd, const char *dbname) {
    if (!mysql_real_connect(config->mysql, host, user, pwd, dbname, 3306, NULL, 0)) {
        printf("connect error: %s\n", mysql_error(config->mysql));
        return false;
    }
    return true;
}

// 执行操作 SQL 语句（INSERT/UPDATE/DELETE）
bool SQLifconfig_Dml_sql(SQLifconfig *config, const char *sqlstr) {
    printf("sql: %s\n", sqlstr);

    // 开始事务
    if (mysql_query(config->mysql, "BEGIN")) {
        printf("query_error: %s\n", mysql_error(config->mysql));
        return false;
    }

    // 执行 SQL 语句
    if (mysql_query(config->mysql, sqlstr)) {
        printf("query_error: %s\n", mysql_error(config->mysql));
        return false;
    }

    // 提交事务
    if (mysql_query(config->mysql, "COMMIT")) {
        printf("query_error: %s\n", mysql_error(config->mysql));
        return false;
    }

    return true;
}

// 执行查询 SQL 语句（SELECT）
char* SQLifconfig_Dql_sql(SQLifconfig *config, const char *sqlstr) {
    // 执行 SQL 查询
    if (mysql_query(config->mysql, sqlstr)) {
        printf("query_error: %s\n", mysql_error(config->mysql));
        return NULL;
    }

    // 获取查询结果集
    config->result = mysql_store_result(config->mysql);
    if (!config->result) {
        return NULL; // 无结果或错误
    }

    // 获取字段数量(列数)
    int fieldnum = mysql_num_fields(config->result);

    // 初始化动态缓冲区
    size_t total_len = 1024; // 初始缓冲区大小
    char *buffer = (char*)malloc(total_len);
    if (!buffer) {
        mysql_free_result(config->result);
        return NULL; // 内存分配失败
    }
    buffer[0] = '\0'; // 初始化空字符串

    // 遍历结果集的每一行
    while ((config->row = mysql_fetch_row(config->result))) {
        // 计算当前行所需的总长度
        size_t row_len = 0;
        for (int j = 0; j < fieldnum; j++) {
            row_len += strlen(config->row[j] ? config->row[j] : "NULL") + 1; // 字段值 + 分隔符
        }

        // 动态扩展缓冲区（如果当前缓冲区不足以容纳新行）
        size_t current_len = strlen(buffer);
        if (current_len + row_len + 2 > total_len) {
            total_len += row_len + 2; // 扩展缓冲区大小
            char *new_buf = realloc(buffer, total_len);
            if (!new_buf) {
                free(buffer);
                mysql_free_result(config->result);
                return NULL; // 内存分配失败
            }
            buffer = new_buf;
        }

        // 拼接当前行的字段值
        for (int j = 0; j < fieldnum; j++) {
            const char *val = config->row[j] ? config->row[j] : "NULL"; // 处理 NULL 值
            strcat(buffer, val); // 追加字段值
            strcat(buffer, "#"); // 追加分隔符
        }
        buffer[strlen(buffer)-1] = '\n'; // 将最后一个#替换为换行符
    }

    // 释放结果集内存
    mysql_free_result(config->result);

    // 检查是否有有效数据
    if (strlen(buffer) == 0) {
        free(buffer);
        return NULL; // 无数据
    }

    // 返回格式化后的字符串
    return buffer;
}

