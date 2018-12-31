#pragma once
#include "constant.h"
/*������Unix�ļ�ϵͳ�Ļ������ݽṹ*/
/*������*/
struct superBlock
{
	unsigned int blockID;
	unsigned int size;				//���̵Ĵ�С
	unsigned int freeBlock[STACKSIZE];	//��¼�����̵�ջ
	unsigned int nextFreeBlock;		//��ǰ������ָ��
	unsigned int freeBlockNum;			//�����̵ĸ���
	unsigned int freeInode[INODESIZE];	//��¼���д��̵�ջ
	unsigned int freeInodeNum;			//����Inode�ĸ���
	unsigned int nextFreeInode;		//��ǰ����Inodeָ��
	unsigned int lastLoginTime;     //�ϴε�¼��ʱ��
	unsigned int rootInodeID;       //��¼rootĿ¼��InodeID 
};
/*inode�����ļ�����*/
struct inode
{
	unsigned int inodeID;           //inodeID
	unsigned int mode;              //�ļ�����:wxr
	unsigned int fileSize;          //�ļ��Ĵ�С
	unsigned int fileLink;          //�ļ���������
	char owner[USERNAMESIZE];       //�ļ��������û�  
	char group[GROUPNAMESIZE];      //�ļ��������û���
	long int modifyTime;            //�޸�ʱ��
	long int createTime;            //����ʱ��   
	int	addr[6];                    //�̿��ַ���ĸ�ֱ�ӿ�ţ�һ��һ�μ�ַ��һ�����μ�ַ
	char fill[28];                  //�հ���䣬ʹinode��СΪ128
};
/*������ڴ��е�indeo*/
struct internalInode
{
	inode inode;  //��Ӧ��inode
	int usingNum; //���ļ���ʹ����
};
/*Ŀ¼��ṹ*/
struct direct
{
	char directName[CATALOGUESIZE]; //�ļ���
	unsigned int inodeID;           //inodeID
};
/*Ŀ¼�ṹ*/
struct catalog
{
	int catalogID;                  //Ŀ¼ID
	struct	direct direct[DIRNUM];  //��Ŀ¼�µ��ļ�
};
/*�û�*/
struct user
{
	char userName[USERNAMESIZE];
	char userPassword[PASSWORSIZE];
	char userGroup[GROUPNAMESIZE];
};