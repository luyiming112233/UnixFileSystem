#pragma once
/*������Unix�ļ�ϵͳ��ʹ�õ��ĳ���*/

#define STACKSIZE 20 //�տ���Ĵ�С
#define INODESIZE 20 //��INODE��Ĵ�С
#define USERNAMESIZE 25  //�û�����С
#define GROUPNAMESIZE 25 //������С
#define PASSWORSIZE 20  //�����С
#define CATALOGUESIZE 14 //Ŀ¼����С
#define BLOCKINODESNUM 248 //�趨248�������ڴ洢�ڵ�,ͬʱҲ�Ǵ洢�ļ�������
#define BLOCKFILESNUM 1800 //�趨1800�������ڴ洢�ļ�
#define SUPERBLACKSTART 512 //������洢λ�õ����
#define BLOCKSIZE 512 //��Ĵ�С
#define DIRNUM 16 //Ŀ¼���ļ��ĸ���
#define INODESIZE 128 //INODE�Ĵ�С,ʵ��Ϊ100
#define INODENUM 980 //INODE���õĸ��������Ϊ984
#define NODIRECT 1000//���ڴ���Ŀ¼��inodeID����ʾû��Ŀ¼