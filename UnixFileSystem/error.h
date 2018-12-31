#pragma once
#define FAIL 0//失败操作
#define OK 1//成功操作
#define ERROR_NODISK 2 //找不到磁盘
#define ERROR_LOADSUPERBLACKFAIL 3 //载入超级块失败
#define ERROR_SAVESUPERBLACKFAIL 4 //存入超级块失败
#define ERROR_SAVEINODEFAIL 5//存入inode失败
#define ERROR_BLOCKREADFAIL 6//读入BLOCK失败
#define ERROR_BLOCKWRITEFAIL 7//写入BLOCK失败
#define ERROR_BLOCKNOFREE 8//没有空闲的BLOCK
