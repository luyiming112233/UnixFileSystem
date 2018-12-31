#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<time.h>
#include"dataStructure.h"
#include"constant.h"
#include"error.h"

using namespace std;

//���Դ���
FILE * Disk;
//����·��
const char *path = "Disk.txt";
//������
superBlock mySuperBlock;
//���ڵ�
inode* root;
//���ļ��������ļ�ϵͳ�����
direct rootDirect;
//�û��ļ��ڵ�,�洢�û���Ϣ
inode userInode;
//�ڴ��е�inode
internalInode interInode[INODENUM];
//��ǰ��¼�û�
user activeUser;
//��ǰ��Ŀ¼�ڵ�
inode *currentInode;
//��ǰ�ļ���
direct currentDirect;
//��ǰĿ¼
catalog currentCata;
//��¼�ļ�����·��
vector<direct> directPath;

void inodeDisplay(inode* myInode);
void cataDisplay(catalog *cata);

/*��ÿ���һ�����λ�ô���¼*/
int DiskInitialize() {
	char a = (char)(0);
	cout << a << endl;
	const char *name = "00";
	if ((Disk = fopen(path, "r+")) == NULL) {
		cout << "�ļ���ʧ�ܣ�" << endl;
	}
	for (int j = 0; j < 65536; j++) {
		for (int i = 0; i < 16; i++) {
			fwrite(&a, sizeof(char), 1, Disk);
		}
	}
	fclose(Disk);
	return 0;
}

/*���볬����*/
int loadSuperBlock() {
	Disk = fopen(path, "r+");
	if (Disk == NULL)
	{
		return ERROR_NODISK;
	}
	//���ļ���ͷ��SEEK_SET��ƫ��SUPERBLACKSTART,Ȼ����ж�����
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);
	if (fread(&mySuperBlock, sizeof(mySuperBlock), 1, Disk) == sizeof(mySuperBlock))
		return ERROR_LOADSUPERBLACKFAIL;
	return OK;
}
/*���볬����*/
int saveSuperBlock() {
	if(Disk == NULL)
		return ERROR_NODISK;
	//���ļ���ͷ��SEEK_SET��ƫ��SUPERBLACKSTART,Ȼ�����д�����
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);
	if (fwrite(&mySuperBlock, sizeof(mySuperBlock), 1, Disk) != 1) {
		return ERROR_SAVESUPERBLACKFAIL;
	}
	fflush(Disk);
	return OK;
}
/*���������洢����Ϣ*/
void displaySuperBlock(){
	cout << "blockID: " << mySuperBlock.blockID << endl;
	cout << "size: " << mySuperBlock.size << endl;
	cout << "freeBlock: " << endl;
	for (int i = 0; i < mySuperBlock.nextFreeBlock; i++)
		cout << mySuperBlock.freeBlock[i] << " ";
	cout << endl;
	cout << "freeInode: " << endl;
	for (int i = 0; i < mySuperBlock.nextFreeInode; i++)
		cout << mySuperBlock.freeInode[i] << " ";
	cout << endl;
	cout << "nextFreeInode: " << mySuperBlock.nextFreeInode << endl;
	cout << "nextFreeBlock: " << mySuperBlock.nextFreeBlock << endl;
	cout << "freeBlockNum: " << mySuperBlock.freeBlockNum << endl;
	cout << "freeInodeNum: " << mySuperBlock.freeInodeNum << endl;
	cout << "rootInodeID: " << mySuperBlock.rootInodeID << endl;
}
/*����inode*/
int saveInode(inode *ino) {
	//inode�����ʼλ��Ϊ�������β��һ�к͵�ǰinodeID*inodeSize
	int inodeStart = SUPERBLACKSTART +BLOCKSIZE +sizeof(inode) * ino->inodeID;
	//��¼�޸�ʱ��
	time_t timer;
	time(&timer);
	ino->modifyTime = timer;
	//��inode����д��Disk
	fseek(Disk, inodeStart, SEEK_SET);
	fwrite(&ino->inodeID, sizeof(ino->inodeID), 1, Disk);
	fwrite(&ino->mode, sizeof(ino->mode), 1, Disk);
	fwrite(&ino->fileSize, sizeof(ino->fileSize), 1, Disk);
	fwrite(&ino->fileLink, sizeof(ino->fileLink), 1, Disk);
	fwrite(&ino->owner, sizeof(ino->owner), 1, Disk);
	fwrite(&ino->group, sizeof(ino->group), 1, Disk);
	fwrite(&ino->modifyTime, sizeof(ino->modifyTime), 1, Disk);
	fwrite(&ino->createTime, sizeof(ino->createTime), 1, Disk);
	fwrite(&ino->addr, sizeof(ino->addr), 1, Disk);
	fwrite(&ino->fill, sizeof(ino->fill), 1, Disk);
	/*if (fwrite(&ino, sizeof(inode), 1, Disk)!=1) {
		return ERROR_SAVEINODEFAIL;
	}*/
	fflush(Disk);
	return OK;
}
/*�������ʼ��*/
int initialize(){
	if (Disk == NULL)
	{
		return ERROR_NODISK;
	}
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);				//��λ����������ʼλ��
	fread(&mySuperBlock, sizeof(superBlock), 1, Disk);

	//��ʼ��freeBlock����
	for (int i = 0; i < STACKSIZE; i++)
		mySuperBlock.freeBlock[i] = BLOCKINODESNUM + i;
	mySuperBlock.nextFreeBlock = STACKSIZE;
	mySuperBlock.size = 512 * 2048;
	mySuperBlock.freeInodeNum = INODENUM;

	for (int i = 0; i < STACKSIZE; i++)
	{
		mySuperBlock.freeInode[i] = i;
	}
	mySuperBlock.nextFreeInode = STACKSIZE;
	mySuperBlock.freeBlockNum = BLOCKFILESNUM;
	saveSuperBlock();
	//fclose(Disk);
    return OK;
}
/*��ʼ�����̣���ÿ�鿪ʼ�Ŀ��м�¼��һ����п�Ŀ��*/
int initializeDisk() {
	if (Disk == NULL) {
		return ERROR_NODISK;
	}
	fseek(Disk, BLOCKINODESNUM * BLOCKSIZE, SEEK_SET);
	unsigned int group[STACKSIZE];
	for (int i = 0; i < STACKSIZE; i++) {
		group[i] = i + BLOCKINODESNUM;
	}
	for (int i = 0; i < 89; i++) {
		for (int j = 0; j < STACKSIZE; j++)
			group[j] += STACKSIZE;
		fseek(Disk, (BLOCKINODESNUM + i * STACKSIZE)*BLOCKSIZE, SEEK_SET);
		fwrite(group, sizeof(unsigned int), STACKSIZE, Disk);
	}
	return OK;
}
/*block����*/
int blockRead(void * _Buf, unsigned short int blockID, int offset, int size, int count = 1)
{
	long int pos;
	int ret;
	//����blockID��ƫ������offset����λ��Ҫ��ȡ��λ��
	pos = blockID * BLOCKSIZE + offset;
	fseek(Disk, pos, SEEK_SET);
	ret = fread(_Buf, size, count, Disk);
	fflush(Disk);
	if (ret != count)
		return ERROR_BLOCKREADFAIL;
	return OK;
}
/*blockд��*/
int blockWrite(void * _Buf, unsigned short int blockID, int offset, int size, int count = 1)
{
	long int pos;
	int ret;
	//����blockID��ƫ������offset����λ��Ҫд���λ��
	pos = blockID * BLOCKSIZE + offset;
	fseek(Disk, pos, SEEK_SET);
	ret = fwrite(_Buf, size, count, Disk);
	fflush(Disk);
	if (ret != count)
		return ERROR_BLOCKWRITEFAIL;
	return OK;
}
/*��ʼ������Ϣ���ӿտ�ջ��ȡ��һ���µĿտ�ID�������뵽�˿ռ�*/
int blockInitialize() {
	unsigned int blockID;
	if (mySuperBlock.nextFreeBlock <= 0)
		return ERROR_BLOCKNOFREE;
	if (mySuperBlock.nextFreeBlock == 1) {
		//�õ��տ��ID
		mySuperBlock.nextFreeBlock--;
		blockID = mySuperBlock.freeBlock[mySuperBlock.nextFreeBlock];
		//����freeBlock
		fseek(Disk, mySuperBlock.freeBlock[0] * BLOCKSIZE, SEEK_SET);
		int ret = fread(mySuperBlock.freeBlock, sizeof(unsigned int), STACKSIZE, Disk);
		mySuperBlock.nextFreeBlock = ret;
		return blockID;
	}
	mySuperBlock.freeBlockNum--;
	mySuperBlock.nextFreeBlock--;
	saveSuperBlock();
	blockID = mySuperBlock.freeBlock[mySuperBlock.nextFreeBlock];
	return blockID;
}
/*�ͷ�block��Դ*/
int blockFree(int blockID) {
	//��nextFreeBlock����20������һ��տ��Ѿ�����
	//������տ�ĺ�д������µĿտ�
	if (mySuperBlock.nextFreeBlock == 20) {
		blockWrite(&mySuperBlock.freeBlock, blockID, 0, sizeof(unsigned int), 20);
		mySuperBlock.nextFreeBlock = 1;
		mySuperBlock.freeBlock[0] = blockID;
	}
	else {
		mySuperBlock.freeBlock[mySuperBlock.nextFreeBlock] = blockID;
		mySuperBlock.nextFreeBlock++;
	}
	mySuperBlock.freeBlockNum++;
	saveSuperBlock();
	return OK;
}
/*��int���͵�modeת��Ϊ�ִ�*/
string transMode(int mode) {
	string smode = "";
	//ǰ��λ�����ж��ļ�Ȩ�ޣ����漸λ�����ж��ļ�������
	int type = mode / 1000;
	int authority = mode % 1000;
	switch (type)
	{
	case 0:smode = smode + "cata "; break;
	case 1:smode = smode + "txt "; break;
	case 2:smode = smode + "doc "; break;
	case 3:smode = smode + "cpp "; break;
	default: smode = smode + "file "; break;
	}
	int div = 100;
	for (int i = 0; i < 3; i++)
	{
		int num = authority / div;
		authority = authority % div;
		int a[3] = { 0 };
		//�ж϶�Ȩ��
		if (num >= 4) {
			num -= 4;
			smode = smode + "r";
		}
		else
			smode = smode + "-";
		//�ж�дȨ��
		if (num >= 2) {
			num -= 2;
			smode = smode + "w";
		}
		else
			smode = smode + "-";
		//�ж�ִ��Ȩ��
		if (num >= 1) {
			num -= 1;
			smode = smode + "x";
		}
		else
			smode = smode + "-";
		smode = smode + " ";
		div /= 10;
	}
	return smode;
}
/*��long int ���͵�ʱ���ת��Ϊ����*/
string getTime(long int time) {
	time_t timer;
	//����ʱ����ʱ����8Сʱ
	timer = time+28800;
	tm *p;
	p = gmtime(&timer);
	char s[80];
	strftime(s,80, "%Y-%m-%d %H:%M:%S", p);
	return s;
}
/*���inode����Ϣ*/
void inodeDisplay(inode* myInode) {
	cout << "-----------show inode-----------" << endl;
	cout << "inodeID: " << myInode->inodeID << endl;
	cout << "mode: " << transMode(myInode->mode) << endl;
	cout << "fileSize: " << myInode->fileSize << endl;
	cout << "fileLink: " << myInode->fileLink << endl;
	cout << "owner: " << myInode->owner << endl;
	cout << "group: " << myInode->group << endl;
	cout << "modifyTime: " << getTime(myInode->modifyTime) << endl;
	cout << "createTime: " << getTime(myInode->createTime) << endl;
	cout << "addr[0]: " << myInode->addr[0] << endl;
	cout << "----------- end inode-----------" << endl;
}
/*ͨ��inodeID���inode*/
struct inode * inodeGet(int ino)
{
	int ipos = 0;
	int ret = 0;
	//�����ڴ��в����Ƿ������inode�������usingNum++,������
	if (interInode[ino].usingNum != 0) {
		interInode[ino].usingNum++;
		//cout << "inode-" << ino << "�Ѿ�����  " << interInode[ino].usingNum << endl;
		return &interInode[ino].inode;
	}
	//����δ����
	if (Disk == NULL)
		return NULL;
	ipos = SUPERBLACKSTART + BLOCKSIZE + ino * sizeof(inode);
	fseek(Disk, ipos, SEEK_SET);
	fread(&interInode[ino].inode.inodeID, sizeof(interInode[ino].inode.inodeID), 1, Disk);
	fread(&interInode[ino].inode.mode, sizeof(interInode[ino].inode.mode), 1, Disk);
	fread(&interInode[ino].inode.fileSize, sizeof(interInode[ino].inode.fileSize), 1, Disk);
	fread(&interInode[ino].inode.fileLink, sizeof(interInode[ino].inode.fileLink), 1, Disk);
	fread(&interInode[ino].inode.owner, sizeof(interInode[ino].inode.owner), 1, Disk);
	fread(&interInode[ino].inode.group, sizeof(interInode[ino].inode.group), 1, Disk);
	fread(&interInode[ino].inode.modifyTime, sizeof(interInode[ino].inode.modifyTime), 1, Disk);
	fread(&interInode[ino].inode.createTime, sizeof(interInode[ino].inode.createTime), 1, Disk);
	fread(&interInode[ino].inode.addr, sizeof(interInode[ino].inode.addr), 1, Disk);
	fread(&interInode[ino].inode.fill, sizeof(interInode[ino].inode.fill), 1, Disk);
	fflush(Disk);
	//if (ret != 1)
	//	return NULL;
	if (interInode[ino].inode.fileLink == 0)		//����ļ��մ���
	{
		cout << "ino: " << ino << "  �ոմ�����" << endl;
		interInode[ino].inode.fileLink= 1;
		interInode[ino].inode.fileSize = 0;
		time_t timer;
		time(&timer);
		interInode[ino].inode.createTime = timer;
		mySuperBlock.freeInodeNum--;
		interInode[ino].inode.inodeID = ino;//inodeID����ʼ���õ�
		interInode[ino].usingNum = 0;
		saveInode(&interInode[ino].inode);
	}
	interInode[ino].usingNum++;
	//inodeDisplay(&interInode[ino].inode);
	//����inode����
	return &interInode[ino].inode;
}
/*����ڴ������е�inode*/
void displayInternalInode() {
	cout << "------------------����ڴ��е�Inode------------------" << endl;
	for (int ino = 0; ino < INODENUM; ino++) {
		if (interInode[ino].usingNum != 0) {
			interInode[ino].usingNum++;
			cout << "inode-" << ino << "�Ѿ�����  " << interInode[ino].usingNum << endl;
			inodeDisplay(&interInode[ino].inode);
		}
	}
}
/*����һ��Inode��Դ*/
inode* inodeInitialize() {
	//��ǰ��û�пյ�inode����Ҫ�Ӵ洢�ռ�Ѱ��
	if (mySuperBlock.nextFreeInode == 0) {
		int num = 0;
		inode temp;
		//inode��������0�ſ��1�ſ飨�����飩֮��
		fseek(Disk, SUPERBLACKSTART + BLOCKSIZE, SEEK_SET);
		for (int i = 0; i < INODENUM; i++) {
			fread(&temp, sizeof(inode), 1, Disk);
			if (temp.fileLink == 0) {
				mySuperBlock.freeInode[num] = i;
				mySuperBlock.nextFreeInode++;
				num++;
			}
			//�ҵ�һ��յ�INODE��
			if (num == INODESIZE)
				break;
		}
	}
	//�������е�INODE��û���ҵ��յ�INODE
	if (mySuperBlock.nextFreeInode == 0)
		return NULL;
	mySuperBlock.nextFreeInode--;
	saveSuperBlock();
	cout << "freeInode: " << mySuperBlock.freeInode[mySuperBlock.nextFreeInode] << endl;
	return inodeGet(mySuperBlock.freeInode[mySuperBlock.nextFreeInode]);
}
/*��ʼ��һ��Ŀ¼*/
void cataGet(catalog* cata) {
	//cata->catalogID = -1;
	for (int i = 0; i < DIRNUM; i++) {
		cata->direct[i].inodeID = NODIRECT;
		strcpy(cata->direct[i].directName, "");
	}
}
/*��Ŀ¼�洢*/
int saveCatalog(catalog *cata,unsigned short int blockID,int offset) {
	int off = offset;
	blockWrite(&cata->catalogID, blockID, off, sizeof(cata->catalogID), 1);
	off += sizeof(cata->catalogID);
	for (int i = 0; i < DIRNUM; i++) {
		if (cata->direct[i].inodeID != NODIRECT) {
			blockWrite(&cata->direct[i].inodeID, blockID, off, sizeof(cata->direct[i].inodeID), 1);
			off += sizeof(cata->direct[i].inodeID);
			blockWrite(&cata->direct[i].directName, blockID, off, sizeof(cata->direct[i].directName), 1);
			off += sizeof(cata->direct[i].directName);
		}
	}
	return 0;
}
/*���Ŀ¼��Ϣ*/
void cataDisplay(catalog *cata) {
	cout << "catalogID: " << cata->catalogID << endl;
	for (int i = 0; i < DIRNUM; i++) {
		cout << i << " " << cata->direct[i].inodeID << " " << cata->direct[i].directName << endl;
	}
}
/*��Disk����һ��Ŀ¼*/
void cataRead(catalog *cata,unsigned short int blockID) {
	cataGet(cata);
	int off = 0;
	blockRead(&cata->catalogID, blockID, off, sizeof(cata->catalogID));
	off += sizeof(cata->catalogID);
	for (int i = 0; i < DIRNUM; i++) {
		blockRead(&cata->direct[i].inodeID, blockID, off, sizeof(cata->direct[i].inodeID));
		off += sizeof(cata->direct[i].inodeID);
		blockRead(&cata->direct[i].directName, blockID, off, sizeof(cata->direct[i].directName));
		off += sizeof(cata->direct[i].directName);
		if (cata->direct[i].inodeID == 0) {
			cata->direct[i].inodeID = NODIRECT;
		}
	}
}
/*����Ŀ¼*/
void cataCopy(catalog *copy, catalog *becopy) {
	copy->catalogID = becopy->catalogID;
	for (int i = 0; i < DIRNUM; i++) {
		copy->direct[i].inodeID = becopy->direct[i].inodeID;
		strcpy(copy->direct[i].directName, becopy->direct[i].directName);
	}
}
/*����Ŀ¼��*/
void directCopy(direct *copy, direct*becopy) {
	copy->inodeID = becopy->inodeID;
	strcpy(copy->directName, becopy->directName);
}
/*���Ŀ¼����Ϣ*/
void directDisplay(direct *dir) {
	cout << "----------------------------" << endl;
	cout << "dirName: " << dir->directName << endl;
	cout << "dirInodeID: " << dir->inodeID << endl;
	cout << "----------------------------" << endl;
}
/*��¼����*/
int login() {
	cout << "-------------����login---------------" << endl;
	catalog cata;
	//����Ŀ¼�ļ�
	cataRead(&cata, root->addr[0]);
	cataCopy(&currentCata, &cata);
	userInode = *inodeGet(cata.direct[0].inodeID);
	int usernum = userInode.fileSize / sizeof(user);
	//���������û�
	struct user* users = (struct user*)calloc(usernum, sizeof(struct user));
	blockRead(users, userInode.addr[0], 0, sizeof(user),usernum);
	string user, password,tempuser,temppass;
	cout << "username: ";
	cin >> user;
	cout << "password: ";
	cin >> password;
	for (int i = 0; i < usernum; i++) {
		tempuser = users[i].userName;
		temppass = users[i].userPassword;
		if (user.compare(tempuser)==0 && password.compare(temppass)==0) {
			//��ǰ�û���¼
			activeUser = users[i];
			cout << "welcome!"<<tempuser << endl;
			return OK;
		}
	}
	return 0;
}
/*����ϵͳ*/
void startUp() {
	//���볬����
	loadSuperBlock();
	
	//��ʼ���ڴ��е�inode
	for (int i = 0; i < INODENUM; i++) {
		interInode[i].usingNum = 0;
	}
	//����root
	root = inodeGet(mySuperBlock.rootInodeID);
	strcpy(rootDirect.directName, "root");
	rootDirect.inodeID = root->inodeID;
	//���õ�ǰ�ļ�Ϊroot
	currentDirect = rootDirect;
	//���õ�ǰ�ڵ�Ϊroot
	currentInode = root;
	directPath.push_back(currentDirect);
	//��ǰĿ¼�����÷���login()������
}
/*���Ժ���*/
void Test() {
	cataDisplay(&currentCata);
}
/*��ʾ�ļ�Ŀ¼*/
void Ls() {
	cout << ". .. ";
	for (int i = 0; i < DIRNUM; i++) {
		if(currentCata.direct[i].inodeID!=NODIRECT)
		    cout <<currentCata.direct[i].directName<< " ";
	}
	cout << endl;
}
/*�ı��ļ�Ȩ��*/
void Chmod() {
	cout << "�ı��ļ�Ȩ��" << endl;
}
/*�ı��ļ���ӵ����*/
void Chown() {
	cout << "�ı��ļ���ӵ����" << endl;
}
/*�ı��ļ�������*/
void Chgrp() {
	cout << "�ı��ļ�������" << endl;
}
/*��ʾ��ǰĿ¼*/
void Pwd() {
	cout << "��ʾ��ǰĿ¼" << endl;
}
/*�ı䵱ǰĿ¼*/
int Cd(string path) {
	vector<string> p;
	vector<direct> tempPath = directPath;
	int start = 0,dir;
	string temp;
	catalog tempCata;
	cataCopy(&tempCata, &currentCata);
	inode* tempInode = currentInode;
	direct tempDirect = currentDirect;
	directCopy(&tempDirect, &currentDirect);
	//��Ŀ¼·������������
	for (int i = 0; i < path.length(); i++) {
		if (path[i] == '/') {
			if (start != i) {
				temp = path.substr(start, i - start);
				p.push_back(temp);
			}
			start = i + 1;
			p.push_back("/");
		}
	}
	//������һ��Ϊ/������������һ�����ַ���������
	temp = path.substr(start, path.length() - start);
	p.push_back(temp);
	if (temp == "") {
		cout << "FAIL:Path can't end up whit '/'." << endl;
		return FAIL;
	}
	//��������path��Ϣ
	/*for (int i = 0; i < p.size(); i++) {
		cout << "#" << p[i] << "#" << endl;
	}*/
	if (p.size() == 0) {
		cout << "FAIL:No path." << endl;
		return FAIL;
	}
	//��һ����/��˵���Ӹ�Ŀ¼��ʼ
	if (p[0] == "/") {
		//TODO:��֮ǰ��inode�ͷ�
		tempPath.clear();
		tempPath.push_back(directPath[0]);
		tempInode = root;
		cataRead(&tempCata, tempInode->addr[0]);
		directCopy(&tempDirect, &rootDirect);
	}
	//��һ����..��˵�����ϼ�Ŀ¼��ʼ
	else if (p[0] == "..") {
		if (tempPath.size() < 2) {
			cout << "FAIL:No parent catalog." << endl;
			return FAIL;
		}
		//TODO:�ͷ�ɾ����inode
		tempPath.pop_back();
		tempInode = inodeGet(tempPath[tempPath.size() - 1].inodeID);
		cataRead(&tempCata, tempInode->addr[0]);
		directCopy(&tempDirect, &tempPath[tempPath.size() - 1]);
	}
	//��һ����.�����øı�
	else if(p[0] == "."){
		//do nothing
	}
	//��һ����Ŀ¼��
	else {
		cout << "��һ�ν���Ŀ¼��" << endl;
		for (dir = 0; dir < DIRNUM; dir++) {
			//cout << dir << " " << tempCata.direct[dir].directName << endl;
			if (strcmp(tempCata.direct[dir].directName, p[0].data()) == 0) {
				if (tempCata.direct[dir].inodeID == NODIRECT) {
					cout << "FAIL:It is invalid directory." << endl;
					return FAIL;
				}
				//��ʹ�õ�ǰĿ¼����direct����ȡinode��Ȼ�����inode��Ϣ�����µ�catalog
				tempPath.push_back(tempCata.direct[dir]);
				directDisplay(&tempCata.direct[dir]);
				directCopy(&tempDirect, &tempCata.direct[dir]);
				tempInode = inodeGet(tempCata.direct[dir].inodeID);
				cataRead(&tempCata, tempInode->addr[0]);
				break;
			}
		}
		//��ʾ�޷��ڵ�ǰĿ¼���ҵ����Ŀ¼��
		if (dir == DIRNUM) {
			cout << "FAIL:It is invalid directory." << endl;
			return FAIL;
		}
	}
	for (start = 1; start < p.size(); start++) {
		if (p[start] == "/" && p[start-1]=="/") {
			cout << "FAIL:Wrong path." << endl;
			return FAIL;
		}
		//�����ǰΪ/������
		cout << "��ǰΪ�� " << p[start] << endl;
		if (p[start] != "/") {
			cout << "����" << endl;
			for (dir = 0; dir < DIRNUM; dir++) {
				cout << dir << " " << tempCata.direct[dir] .inodeID<<" "<<tempCata.direct[dir].directName << endl;
				if (strcmp(tempCata.direct[dir].directName, p[start].data()) == 0) {
					if (tempCata.direct[dir].inodeID == NODIRECT) {
						cout << "FAIL:It is invalid directory." << endl;
						return FAIL;
					}
					//��ʹ�õ�ǰĿ¼����direct����ȡinode��Ȼ�����inode��Ϣ�����µ�catalog
					tempPath.push_back(tempCata.direct[dir]);
					directDisplay(&tempCata.direct[dir]);
					directCopy(&tempDirect, &tempCata.direct[dir]);
					tempInode = inodeGet(tempCata.direct[dir].inodeID);
					cataRead(&tempCata, tempInode->addr[0]);
					break;
				}
			}
			//��ʾ�޷��ڵ�ǰĿ¼���ҵ����Ŀ¼��
			if (dir == DIRNUM) {
				cout << "FAIL:It is invalid directory." << endl;
				return FAIL;
			}
		}
	}
	//���е��˴�˵��·����ȷ����tempInode��tempCata��ֵ��currentInode��currentCata
	currentInode = tempInode;
	cataCopy(&currentCata, &tempCata);
	directCopy(&currentDirect, &tempDirect);
	directPath = tempPath;
	return 0;
}
/*������Ŀ¼*/
int Mkdir(string directName) {
	cout << "�����ǰINODE����Ϣ" << endl;
	inodeDisplay(currentInode);
	//�жϵ�ǰ�ļ���catalog
	if (currentInode->mode / 1000 != 0) {
		cout << "FAIL:It is not in a catalog." << endl;
		return FAIL;
	}
	//Ѱ��Ŀ¼���пյ�direct
	int emptyDir;
	for (emptyDir = 0; emptyDir < DIRNUM; emptyDir++) {
		if (currentCata.direct[emptyDir].inodeID == NODIRECT) {
			break;
		}
	}
	//�ж��Ƿ����������ļ�
	for (int i = 0; i < DIRNUM; i++) {
		if (strcmp(currentCata.direct[i].directName, directName.data()) == 0) {
			cout << "FAIL:" << directName << " is exist in current catalog." << endl;
			return FAIL;
		}
	}
	//Ŀ¼����
	if (emptyDir == DIRNUM) {
		cout << "FAIL:The catalog is full." << endl;
		return FAIL;
	}
	//Ϊ��Ŀ¼����һ��inode
	inode *dirInode = inodeInitialize();
	dirInode->mode = 644;
	strcpy(dirInode->owner, activeUser.userName);
	strcpy(dirInode->group, activeUser.userGroup);
	//�ڵ�ǰĿ¼���½���Ŀ¼��direct
	currentCata.direct[emptyDir].inodeID = dirInode->inodeID;
	strcpy(currentCata.direct[emptyDir].directName, directName.data());
	//Ϊ��Ŀ¼�½�һ��Ŀ¼catalog
	catalog dirCata;
	cataGet(&dirCata);
	dirCata.catalogID = dirInode->inodeID;
	//����һ��block���ڴ洢Ŀ¼
	dirInode->addr[0] = blockInitialize();
	//��inode�������
	saveInode(dirInode);
	//����Ŀ¼�������
	saveCatalog(&dirCata, dirInode->addr[0], 0);
	//����ǰĿ¼�������
	saveCatalog(&currentCata, currentInode->addr[0], 0);
	return OK;
}
/*ɾ����Ŀ¼*/
void Rmdir() {
	cout << "ɾ����Ŀ¼" << endl;
}
/*�ļ�����������*/
void Umask() {
	cout << "�ļ�����������" << endl;
}
/*�ı��ļ���*/
void Mv() {
	cout << "�ı��ļ���" << endl;
}
/*�ļ�����*/
void Cp() {
	cout << "�ļ�����" << endl;
}
/*�ļ�ɾ��*/
void Rm() {
	cout << "�ļ�ɾ��" << endl;
}
/*�����ļ�����*/
void Ln() {
	cout << "�����ļ�����" << endl;
}
/*������ʾ�ļ�����*/
void Cat() {
	cout << "������ʾ�ļ�����" << endl;
}
/*�޸��û�����*/
int Passwd() {
	//cout << "�޸��û�����" << endl;
	string password, newPass, rePass,activePass = activeUser.userPassword;
	cout << "Please enter your password : ";
	cin >> password;
	cout << "-----" << activePass << "-----" << endl;
	cout << "-----" <<password<< "-----" << endl;
	if (activePass != password) {
		cout << "FAIL:Your password is incorrect." << endl;
		return FAIL;
	}
	cout << "Please enter your new password : ";
	cin >> newPass;
	cout << "Please enter your new password again : ";
	cin >> rePass;
	if (newPass != rePass) {
		cout << "FAIL:The two password entries are different." << endl;
		return FAIL;
	}
	//ͨ��userInode�ڵ��Disk��ȡ�û���Ϣ
	int usernum = userInode.fileSize / sizeof(user);
	user* users = (struct user*)calloc(usernum, sizeof(struct user));
	blockRead(users, userInode.addr[0], 0, sizeof(user), usernum);
	//��������
	for (int i = 0; i < usernum; i++) {
		if (strcmp(users[i].userName, activeUser.userName)==0) {
			strcpy(users[i].userPassword, newPass.data());
			break;
		}
	}
	cout << userInode.fileSize << endl;
	for (int i = 0; i < usernum; i++) {
		blockWrite(&users[i], userInode.addr[0], i*sizeof(user), sizeof(user), 1);
		//cout << "user: " << users[i].userName << " pass: " << users[i].userPassword << endl;
	}
	
}
/*�������г���*/
int terminal(){
	string command, dictTag,userTag,username;
	vector<string> historyCom;
	bool run = true;
	while (run) {
		//����Ŀ¼�ж�Ŀ¼���
		if (currentInode->inodeID == mySuperBlock.rootInodeID)
			dictTag = "/";
		else if (currentDirect.directName == activeUser.userName)
			dictTag = "~";
		else
			dictTag = currentDirect.directName;
		username = activeUser.userName;
		userTag = (username == "root" ? "#" : "$");
		cout << "[" << activeUser.userName << "@localhost " << dictTag << "]" << userTag << " ";
		cin >> command;
		historyCom.push_back(command);
		if (command == "ls") {
			Ls();
		}
		else if (command == "chmod") {
			Chmod();
		}
		else if (command == "chown") {
			Chown();
		}
		else if (command == "chgrp") {
			Chgrp();
		}
		else if (command == "pwd") {
			Pwd();
		}
		else if (command == "cd") {
			string path;
			cin >> path;
			Cd(path);
		}
		else if (command == "mkdir") {
			string directName;
			cin >> directName;
			Mkdir(directName);
		}
		else if (command == "umask") {
			Umask();
		}
		else if (command == "mv") {
			Mv();
		}
		else if (command == "cp") {
			Cp();
		}
		else if (command == "rm") {
			Rm();
		}
		else if (command == "ln") {
			Ln();
		}
		else if (command == "cat") {
			Cat();
		}
		else if (command == "passwd") {
			Passwd();
		}
		else if (command == "test") {
			Test();
		}
		else if (command == "quit") {
			run = false;
			cout << "quit system" << endl;
		}
		else if (command == "superblock") {
			displaySuperBlock();
		}
	}
	return 0;
}
//��ʼ������1:��ɴ��̵ĸ�ʽ������������û�
void begin1() {
	DiskInitialize();
	Disk = fopen(path, "r+");
	initialize();
	initializeDisk();
	
	//��ʼ���ڴ��е�inode
	for (int i = 0; i < INODENUM; i++) {
		interInode[i].usingNum = 0;
	}
	
	//���userĿ¼������������û�
	root = inodeInitialize();

	root->addr[0] = blockInitialize();
	catalog rootCata;
	cout << sizeof(rootCata) << endl;
	cataGet(&rootCata);
	rootCata.catalogID = 100;
	inode *userInode = inodeInitialize();
	userInode->addr[0] = blockInitialize();
	strcpy(rootCata.direct[0].directName, "user");

	rootCata.direct[0].inodeID = userInode->inodeID;
	user myuser[2];
	strcpy(myuser[0].userGroup, "root");
	strcpy(myuser[0].userName, "root");
	strcpy(myuser[0].userPassword, "root");
	strcpy(myuser[1].userGroup, "group1");
	strcpy(myuser[1].userName, "lym");
	strcpy(myuser[1].userPassword, "lym");
	userInode->fileSize = sizeof(myuser);

	blockWrite(&myuser, userInode->addr[0], 0, sizeof(myuser), 1);
	//�޸�
	//blockWrite(&rootCata, root->addr[0], 0, sizeof(rootCata), 1);
	saveCatalog(&rootCata, root->addr[0], 0);
	saveInode(userInode);
	strcpy(root->group, "root");
	strcpy(root->owner, "root");
	root->mode = 644;
	saveInode(root);
	//�ڸ�Ŀ¼��Ӽ���������Ŀ¼	

	//���������������Ҫ���̴洢
	mySuperBlock.rootInodeID = root->inodeID;
	saveSuperBlock();
	fclose(Disk);	
}
//��ʼ������2:�ڸ�Ŀ¼��Ӽ��������ļ���
void begin2() {
	startUp();
	catalog cata;
	//����Ŀ¼�ļ�
	cataRead(&cata, root->addr[0]);
	cataCopy(&currentCata, &cata);
	userInode = *inodeGet(cata.direct[0].inodeID);
	int usernum = userInode.fileSize / sizeof(user);
	struct user* users = (struct user*)calloc(usernum, sizeof(struct user));
	blockRead(users, userInode.addr[0], 0, sizeof(user), usernum);
	strcpy(activeUser.userName, users[0].userName);
	strcpy(activeUser.userGroup, users[0].userGroup);
	Mkdir("home");
	Mkdir("dev");
	Mkdir("bin");
}

