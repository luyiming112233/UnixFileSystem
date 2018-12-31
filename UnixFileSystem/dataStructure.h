#pragma once
#include "constant.h"
/*包含了Unix文件系统的基本数据结构*/
/*超级块*/
struct superBlock
{
	unsigned int blockID;
	unsigned int size;				//磁盘的大小
	unsigned int freeBlock[STACKSIZE];	//记录空闲盘的栈
	unsigned int nextFreeBlock;		//当前空闲盘指针
	unsigned int freeBlockNum;			//空闲盘的个数
	unsigned int freeInode[INODESIZE];	//记录空闲磁盘的栈
	unsigned int freeInodeNum;			//空闲Inode的个数
	unsigned int nextFreeInode;		//当前空闲Inode指针
	unsigned int lastLoginTime;     //上次登录的时间
	unsigned int rootInodeID;       //记录root目录的InodeID 
};
/*inode用于文件储存*/
struct inode
{
	unsigned int inodeID;           //inodeID
	unsigned int mode;              //文件类型:wxr
	unsigned int fileSize;          //文件的大小
	unsigned int fileLink;          //文件的连接数
	char owner[USERNAMESIZE];       //文件所属的用户  
	char group[GROUPNAMESIZE];      //文件所属的用户组
	long int modifyTime;            //修改时间
	long int createTime;            //创建时间   
	int	addr[6];                    //盘块地址，四个直接块号，一个一次间址，一个两次间址
	char fill[28];                  //空白填充，使inode大小为128
};
/*存放在内存中的indeo*/
struct internalInode
{
	inode inode;  //对应的inode
	int usingNum; //该文件的使用数
};
/*目录项结构*/
struct direct
{
	char directName[CATALOGUESIZE]; //文件名
	unsigned int inodeID;           //inodeID
};
/*目录结构*/
struct catalog
{
	int catalogID;                  //目录ID
	struct	direct direct[DIRNUM];  //该目录下的文件
};
/*用户*/
struct user
{
	char userName[USERNAMESIZE];
	char userPassword[PASSWORSIZE];
	char userGroup[GROUPNAMESIZE];
};