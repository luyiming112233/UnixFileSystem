// UnixFileSystem.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include"UnixFileSystem.h"
#include <iostream>
#include<string>

using namespace std;

int main()
{
	//begin1();
	//begin2();
	startUp();
	inodeDisplay(currentInode);
	
	if (login()) {
		terminal();
	}	
}
