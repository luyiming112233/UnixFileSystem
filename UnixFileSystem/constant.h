#pragma once
/*定义了Unix文件系统中使用到的常量*/

#define STACKSIZE 20 //空块组的大小
#define INODESIZE 20 //空INODE组的大小
#define USERNAMESIZE 25  //用户名大小
#define GROUPNAMESIZE 25 //组名大小
#define PASSWORSIZE 20  //密码大小
#define CATALOGUESIZE 14 //目录名大小
#define BLOCKINODESNUM 248 //设定248个块用于存储节点,同时也是存储文件块的起点
#define BLOCKFILESNUM 1800 //设定1800个块用于存储文件
#define SUPERBLACKSTART 512 //超级块存储位置的起点
#define BLOCKSIZE 512 //块的大小
#define DIRNUM 16 //目录下文件的个数
#define INODESIZE 128 //INODE的大小,实际为100
#define INODENUM 980 //INODE设置的个数，最大为984
#define NODIRECT 1000//用于存入目录的inodeID，表示没有目录