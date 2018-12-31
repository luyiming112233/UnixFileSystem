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

//电脑磁盘
FILE * Disk;
//磁盘路径
const char *path = "Disk.txt";
//超级块
superBlock mySuperBlock;
//根节点
inode* root;
//根文件，进入文件系统的起点
direct rootDirect;
//用户文件节点,存储用户信息
inode userInode;
//内存中的inode
internalInode interInode[INODENUM];
//当前登录用户
user activeUser;
//当前的目录节点
inode *currentInode;
//当前文件夹
direct currentDirect;
//当前目录
catalog currentCata;
//记录文件访问路径
vector<direct> directPath;

void inodeDisplay(inode* myInode);
void cataDisplay(catalog *cata);

/*在每组第一个块的位置处记录*/
int DiskInitialize() {
	char a = (char)(0);
	cout << a << endl;
	const char *name = "00";
	if ((Disk = fopen(path, "r+")) == NULL) {
		cout << "文件打开失败！" << endl;
	}
	for (int j = 0; j < 65536; j++) {
		for (int i = 0; i < 16; i++) {
			fwrite(&a, sizeof(char), 1, Disk);
		}
	}
	fclose(Disk);
	return 0;
}

/*载入超级块*/
int loadSuperBlock() {
	Disk = fopen(path, "r+");
	if (Disk == NULL)
	{
		return ERROR_NODISK;
	}
	//从文件开头（SEEK_SET）偏移SUPERBLACKSTART,然后进行读操作
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);
	if (fread(&mySuperBlock, sizeof(mySuperBlock), 1, Disk) == sizeof(mySuperBlock))
		return ERROR_LOADSUPERBLACKFAIL;
	return OK;
}
/*存入超级块*/
int saveSuperBlock() {
	if(Disk == NULL)
		return ERROR_NODISK;
	//从文件开头（SEEK_SET）偏移SUPERBLACKSTART,然后进行写入操作
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);
	if (fwrite(&mySuperBlock, sizeof(mySuperBlock), 1, Disk) != 1) {
		return ERROR_SAVESUPERBLACKFAIL;
	}
	fflush(Disk);
	return OK;
}
/*输出超级块存储的信息*/
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
/*存入inode*/
int saveInode(inode *ino) {
	//inode点的起始位置为超级块结尾下一行和当前inodeID*inodeSize
	int inodeStart = SUPERBLACKSTART +BLOCKSIZE +sizeof(inode) * ino->inodeID;
	//记录修改时间
	time_t timer;
	time(&timer);
	ino->modifyTime = timer;
	//将inode内容写入Disk
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
/*超级块初始化*/
int initialize(){
	if (Disk == NULL)
	{
		return ERROR_NODISK;
	}
	fseek(Disk, SUPERBLACKSTART, SEEK_SET);				//定位到超级块起始位置
	fread(&mySuperBlock, sizeof(superBlock), 1, Disk);

	//初始化freeBlock数组
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
/*初始化磁盘，在每组开始的块中记录下一组空闲块的块号*/
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
/*block读入*/
int blockRead(void * _Buf, unsigned short int blockID, int offset, int size, int count = 1)
{
	long int pos;
	int ret;
	//根据blockID和偏移量（offset）定位到要读取的位置
	pos = blockID * BLOCKSIZE + offset;
	fseek(Disk, pos, SEEK_SET);
	ret = fread(_Buf, size, count, Disk);
	fflush(Disk);
	if (ret != count)
		return ERROR_BLOCKREADFAIL;
	return OK;
}
/*block写入*/
int blockWrite(void * _Buf, unsigned short int blockID, int offset, int size, int count = 1)
{
	long int pos;
	int ret;
	//根据blockID和偏移量（offset）定位到要写入的位置
	pos = blockID * BLOCKSIZE + offset;
	fseek(Disk, pos, SEEK_SET);
	ret = fwrite(_Buf, size, count, Disk);
	fflush(Disk);
	if (ret != count)
		return ERROR_BLOCKWRITEFAIL;
	return OK;
}
/*初始化块信息，从空块栈中取到一个新的空块ID，即申请到了空间*/
int blockInitialize() {
	unsigned int blockID;
	if (mySuperBlock.nextFreeBlock <= 0)
		return ERROR_BLOCKNOFREE;
	if (mySuperBlock.nextFreeBlock == 1) {
		//得到空块的ID
		mySuperBlock.nextFreeBlock--;
		blockID = mySuperBlock.freeBlock[mySuperBlock.nextFreeBlock];
		//更新freeBlock
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
/*释放block资源*/
int blockFree(int blockID) {
	//若nextFreeBlock等于20，即这一组空块已经满了
	//将这组空块的号写入这个新的空块
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
/*将int类型的mode转化为字串*/
string transMode(int mode) {
	string smode = "";
	//前三位用于判断文件权限，后面几位用于判断文件的类型
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
		//判断读权限
		if (num >= 4) {
			num -= 4;
			smode = smode + "r";
		}
		else
			smode = smode + "-";
		//判断写权限
		if (num >= 2) {
			num -= 2;
			smode = smode + "w";
		}
		else
			smode = smode + "-";
		//判断执行权限
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
/*将long int 类型的时间戳转化为日期*/
string getTime(long int time) {
	time_t timer;
	//测试时发现时间慢8小时
	timer = time+28800;
	tm *p;
	p = gmtime(&timer);
	char s[80];
	strftime(s,80, "%Y-%m-%d %H:%M:%S", p);
	return s;
}
/*输出inode的信息*/
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
/*通过inodeID获得inode*/
struct inode * inodeGet(int ino)
{
	int ipos = 0;
	int ret = 0;
	//先在内存中查找是否有这个inode，如果有usingNum++,并返回
	if (interInode[ino].usingNum != 0) {
		interInode[ino].usingNum++;
		//cout << "inode-" << ino << "已经存在  " << interInode[ino].usingNum << endl;
		return &interInode[ino].inode;
	}
	//磁盘未载入
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
	if (interInode[ino].inode.fileLink == 0)		//这个文件刚创建
	{
		cout << "ino: " << ino << "  刚刚创建！" << endl;
		interInode[ino].inode.fileLink= 1;
		interInode[ino].inode.fileSize = 0;
		time_t timer;
		time(&timer);
		interInode[ino].inode.createTime = timer;
		mySuperBlock.freeInodeNum--;
		interInode[ino].inode.inodeID = ino;//inodeID被初始化好的
		interInode[ino].usingNum = 0;
		saveInode(&interInode[ino].inode);
	}
	interInode[ino].usingNum++;
	//inodeDisplay(&interInode[ino].inode);
	//返回inode引用
	return &interInode[ino].inode;
}
/*输出内存中所有的inode*/
void displayInternalInode() {
	cout << "------------------输出内存中的Inode------------------" << endl;
	for (int ino = 0; ino < INODENUM; ino++) {
		if (interInode[ino].usingNum != 0) {
			interInode[ino].usingNum++;
			cout << "inode-" << ino << "已经存在  " << interInode[ino].usingNum << endl;
			inodeDisplay(&interInode[ino].inode);
		}
	}
}
/*申请一个Inode资源*/
inode* inodeInitialize() {
	//当前组没有空的inode，需要从存储空间寻找
	if (mySuperBlock.nextFreeInode == 0) {
		int num = 0;
		inode temp;
		//inode的区域在0号块和1号块（超级块）之后
		fseek(Disk, SUPERBLACKSTART + BLOCKSIZE, SEEK_SET);
		for (int i = 0; i < INODENUM; i++) {
			fread(&temp, sizeof(inode), 1, Disk);
			if (temp.fileLink == 0) {
				mySuperBlock.freeInode[num] = i;
				mySuperBlock.nextFreeInode++;
				num++;
			}
			//找到一组空的INODE组
			if (num == INODESIZE)
				break;
		}
	}
	//遍历所有的INODE都没有找到空的INODE
	if (mySuperBlock.nextFreeInode == 0)
		return NULL;
	mySuperBlock.nextFreeInode--;
	saveSuperBlock();
	cout << "freeInode: " << mySuperBlock.freeInode[mySuperBlock.nextFreeInode] << endl;
	return inodeGet(mySuperBlock.freeInode[mySuperBlock.nextFreeInode]);
}
/*初始化一个目录*/
void cataGet(catalog* cata) {
	//cata->catalogID = -1;
	for (int i = 0; i < DIRNUM; i++) {
		cata->direct[i].inodeID = NODIRECT;
		strcpy(cata->direct[i].directName, "");
	}
}
/*将目录存储*/
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
/*输出目录信息*/
void cataDisplay(catalog *cata) {
	cout << "catalogID: " << cata->catalogID << endl;
	for (int i = 0; i < DIRNUM; i++) {
		cout << i << " " << cata->direct[i].inodeID << " " << cata->direct[i].directName << endl;
	}
}
/*从Disk读入一个目录*/
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
/*拷贝目录*/
void cataCopy(catalog *copy, catalog *becopy) {
	copy->catalogID = becopy->catalogID;
	for (int i = 0; i < DIRNUM; i++) {
		copy->direct[i].inodeID = becopy->direct[i].inodeID;
		strcpy(copy->direct[i].directName, becopy->direct[i].directName);
	}
}
/*拷贝目录项*/
void directCopy(direct *copy, direct*becopy) {
	copy->inodeID = becopy->inodeID;
	strcpy(copy->directName, becopy->directName);
}
/*输出目录项信息*/
void directDisplay(direct *dir) {
	cout << "----------------------------" << endl;
	cout << "dirName: " << dir->directName << endl;
	cout << "dirInodeID: " << dir->inodeID << endl;
	cout << "----------------------------" << endl;
}
/*登录功能*/
int login() {
	cout << "-------------进入login---------------" << endl;
	catalog cata;
	//读入目录文件
	cataRead(&cata, root->addr[0]);
	cataCopy(&currentCata, &cata);
	userInode = *inodeGet(cata.direct[0].inodeID);
	int usernum = userInode.fileSize / sizeof(user);
	//读入所有用户
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
			//当前用户登录
			activeUser = users[i];
			cout << "welcome!"<<tempuser << endl;
			return OK;
		}
	}
	return 0;
}
/*启动系统*/
void startUp() {
	//载入超级块
	loadSuperBlock();
	
	//初始化内存中的inode
	for (int i = 0; i < INODENUM; i++) {
		interInode[i].usingNum = 0;
	}
	//读入root
	root = inodeGet(mySuperBlock.rootInodeID);
	strcpy(rootDirect.directName, "root");
	rootDirect.inodeID = root->inodeID;
	//设置当前文件为root
	currentDirect = rootDirect;
	//设置当前节点为root
	currentInode = root;
	directPath.push_back(currentDirect);
	//当前目录的设置放在login()函数中
}
/*测试函数*/
void Test() {
	cataDisplay(&currentCata);
}
/*显示文件目录*/
void Ls() {
	cout << ". .. ";
	for (int i = 0; i < DIRNUM; i++) {
		if(currentCata.direct[i].inodeID!=NODIRECT)
		    cout <<currentCata.direct[i].directName<< " ";
	}
	cout << endl;
}
/*改变文件权限*/
void Chmod() {
	cout << "改变文件权限" << endl;
}
/*改变文件的拥有者*/
void Chown() {
	cout << "改变文件的拥有者" << endl;
}
/*改变文件所属组*/
void Chgrp() {
	cout << "改变文件所属组" << endl;
}
/*显示当前目录*/
void Pwd() {
	cout << "显示当前目录" << endl;
}
/*改变当前目录*/
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
	//将目录路径放入向量中
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
	//如果最后一个为/，在这里会读出一个空字符串，报错
	temp = path.substr(start, path.length() - start);
	p.push_back(temp);
	if (temp == "") {
		cout << "FAIL:Path can't end up whit '/'." << endl;
		return FAIL;
	}
	//输出输入的path信息
	/*for (int i = 0; i < p.size(); i++) {
		cout << "#" << p[i] << "#" << endl;
	}*/
	if (p.size() == 0) {
		cout << "FAIL:No path." << endl;
		return FAIL;
	}
	//第一个是/，说明从根目录开始
	if (p[0] == "/") {
		//TODO:将之前的inode释放
		tempPath.clear();
		tempPath.push_back(directPath[0]);
		tempInode = root;
		cataRead(&tempCata, tempInode->addr[0]);
		directCopy(&tempDirect, &rootDirect);
	}
	//第一个是..，说明从上级目录开始
	else if (p[0] == "..") {
		if (tempPath.size() < 2) {
			cout << "FAIL:No parent catalog." << endl;
			return FAIL;
		}
		//TODO:释放删除的inode
		tempPath.pop_back();
		tempInode = inodeGet(tempPath[tempPath.size() - 1].inodeID);
		cataRead(&tempCata, tempInode->addr[0]);
		directCopy(&tempDirect, &tempPath[tempPath.size() - 1]);
	}
	//第一个是.，不用改变
	else if(p[0] == "."){
		//do nothing
	}
	//第一个是目录项
	else {
		cout << "第一次进入目录项" << endl;
		for (dir = 0; dir < DIRNUM; dir++) {
			//cout << dir << " " << tempCata.direct[dir].directName << endl;
			if (strcmp(tempCata.direct[dir].directName, p[0].data()) == 0) {
				if (tempCata.direct[dir].inodeID == NODIRECT) {
					cout << "FAIL:It is invalid directory." << endl;
					return FAIL;
				}
				//先使用当前目录存入direct，读取inode，然后根据inode信息读入新的catalog
				tempPath.push_back(tempCata.direct[dir]);
				directDisplay(&tempCata.direct[dir]);
				directCopy(&tempDirect, &tempCata.direct[dir]);
				tempInode = inodeGet(tempCata.direct[dir].inodeID);
				cataRead(&tempCata, tempInode->addr[0]);
				break;
			}
		}
		//表示无法在当前目录下找到这个目录项
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
		//如果当前为/，跳过
		cout << "当前为： " << p[start] << endl;
		if (p[start] != "/") {
			cout << "进入" << endl;
			for (dir = 0; dir < DIRNUM; dir++) {
				cout << dir << " " << tempCata.direct[dir] .inodeID<<" "<<tempCata.direct[dir].directName << endl;
				if (strcmp(tempCata.direct[dir].directName, p[start].data()) == 0) {
					if (tempCata.direct[dir].inodeID == NODIRECT) {
						cout << "FAIL:It is invalid directory." << endl;
						return FAIL;
					}
					//先使用当前目录存入direct，读取inode，然后根据inode信息读入新的catalog
					tempPath.push_back(tempCata.direct[dir]);
					directDisplay(&tempCata.direct[dir]);
					directCopy(&tempDirect, &tempCata.direct[dir]);
					tempInode = inodeGet(tempCata.direct[dir].inodeID);
					cataRead(&tempCata, tempInode->addr[0]);
					break;
				}
			}
			//表示无法在当前目录下找到这个目录项
			if (dir == DIRNUM) {
				cout << "FAIL:It is invalid directory." << endl;
				return FAIL;
			}
		}
	}
	//运行到此处说明路径正确，将tempInode和tempCata赋值到currentInode和currentCata
	currentInode = tempInode;
	cataCopy(&currentCata, &tempCata);
	directCopy(&currentDirect, &tempDirect);
	directPath = tempPath;
	return 0;
}
/*创建子目录*/
int Mkdir(string directName) {
	cout << "输出当前INODE的信息" << endl;
	inodeDisplay(currentInode);
	//判断当前文件是catalog
	if (currentInode->mode / 1000 != 0) {
		cout << "FAIL:It is not in a catalog." << endl;
		return FAIL;
	}
	//寻找目录中有空的direct
	int emptyDir;
	for (emptyDir = 0; emptyDir < DIRNUM; emptyDir++) {
		if (currentCata.direct[emptyDir].inodeID == NODIRECT) {
			break;
		}
	}
	//判断是否有重名的文件
	for (int i = 0; i < DIRNUM; i++) {
		if (strcmp(currentCata.direct[i].directName, directName.data()) == 0) {
			cout << "FAIL:" << directName << " is exist in current catalog." << endl;
			return FAIL;
		}
	}
	//目录已满
	if (emptyDir == DIRNUM) {
		cout << "FAIL:The catalog is full." << endl;
		return FAIL;
	}
	//为子目录申请一个inode
	inode *dirInode = inodeInitialize();
	dirInode->mode = 644;
	strcpy(dirInode->owner, activeUser.userName);
	strcpy(dirInode->group, activeUser.userGroup);
	//在当前目录下新建子目录的direct
	currentCata.direct[emptyDir].inodeID = dirInode->inodeID;
	strcpy(currentCata.direct[emptyDir].directName, directName.data());
	//为子目录新建一个目录catalog
	catalog dirCata;
	cataGet(&dirCata);
	dirCata.catalogID = dirInode->inodeID;
	//申请一块block用于存储目录
	dirInode->addr[0] = blockInitialize();
	//将inode存入磁盘
	saveInode(dirInode);
	//将子目录存入磁盘
	saveCatalog(&dirCata, dirInode->addr[0], 0);
	//将当前目录存入磁盘
	saveCatalog(&currentCata, currentInode->addr[0], 0);
	return OK;
}
/*删除子目录*/
void Rmdir() {
	cout << "删除子目录" << endl;
}
/*文件创建屏蔽码*/
void Umask() {
	cout << "文件创建屏蔽码" << endl;
}
/*改变文件名*/
void Mv() {
	cout << "改变文件名" << endl;
}
/*文件拷贝*/
void Cp() {
	cout << "文件拷贝" << endl;
}
/*文件删除*/
void Rm() {
	cout << "文件删除" << endl;
}
/*建立文件联接*/
void Ln() {
	cout << "建立文件联接" << endl;
}
/*连接显示文件内容*/
void Cat() {
	cout << "连接显示文件内容" << endl;
}
/*修改用户口令*/
int Passwd() {
	//cout << "修改用户口令" << endl;
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
	//通过userInode节点从Disk读取用户信息
	int usernum = userInode.fileSize / sizeof(user);
	user* users = (struct user*)calloc(usernum, sizeof(struct user));
	blockRead(users, userInode.addr[0], 0, sizeof(user), usernum);
	//更新密码
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
/*命令运行程序*/
int terminal(){
	string command, dictTag,userTag,username;
	vector<string> historyCom;
	bool run = true;
	while (run) {
		//根据目录判读目录标记
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
//初始化函数1:完成磁盘的格式化，添加两个用户
void begin1() {
	DiskInitialize();
	Disk = fopen(path, "r+");
	initialize();
	initializeDisk();
	
	//初始化内存中的inode
	for (int i = 0; i < INODENUM; i++) {
		interInode[i].usingNum = 0;
	}
	
	//添加user目录，并添加两个用户
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
	//修改
	//blockWrite(&rootCata, root->addr[0], 0, sizeof(rootCata), 1);
	saveCatalog(&rootCata, root->addr[0], 0);
	saveInode(userInode);
	strcpy(root->group, "root");
	strcpy(root->owner, "root");
	root->mode = 644;
	saveInode(root);
	//在根目录添加几个基本的目录	

	//操作过超级块后需要立刻存储
	mySuperBlock.rootInodeID = root->inodeID;
	saveSuperBlock();
	fclose(Disk);	
}
//初始化函数2:在根目录添加几个基本文件夹
void begin2() {
	startUp();
	catalog cata;
	//读入目录文件
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

