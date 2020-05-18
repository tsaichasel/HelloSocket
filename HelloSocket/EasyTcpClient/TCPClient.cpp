#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

enum ForegroundColor
{
	enmCFC_Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
	enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	enmCFC_Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Gray = FOREGROUND_INTENSITY,
	enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Black = 0,
};

enum BackGroundColor
{
	enmCBC_Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
	enmCBC_Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	enmCBC_Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	enmCBC_Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	enmCBC_Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	enmCBC_Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_Black = 0,
};

struct DataPackage {
	int age;
	char name[32];
};

void SetColor(ForegroundColor foreColor, BackGroundColor backColor);


int main() {
	//启动Windows socket 2.x环境
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	//-- 用Socket API建立简易的TCP客户端
	// 1 建立一个socket
	SOCKET SockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (INVALID_SOCKET == SockCli) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- 建立SOCKET失败 ！                      " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- 建立SOCKET成功 ！                      " << std::endl;
	}

	// 2 连接服务器 connect
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nAddrLen = sizeof(sockaddr_in);
	int ret = connect(SockCli, (sockaddr*)&addrSer, nAddrLen);
	if (SOCKET_ERROR == ret) {
		SetColor(enmCFC_Black, enmCBC_Red);
		std::cout << " --- 连接服务器失败 ！                      " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Green);
		std::cout << " --- 连接服务器成功 ！                      " << std::endl;
	}

	while (1) {
		char CmdBuf[128] = {};
		SetColor(enmCFC_HighWhite, enmCBC_Black);
		std::cin >> CmdBuf;
		if (0 == strcmp(CmdBuf, "exit")) {
			SetColor(enmCFC_Red, enmCBC_Yellow);
			std::cout << " --- Client 准备退出                        " << std::endl;
			break;
		}
		else {
			send(SockCli, CmdBuf, 128, 0);
		}

		// 3 接收服务器信息 recv
		char RecvBuf[256] = {};
		int nLen = recv(SockCli, RecvBuf, 256, 0);
		DataPackage* RecvInfo = (DataPackage*)RecvBuf;
		if (nLen > 0) {
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- Server 回应 ：Age = " << RecvInfo->age 
				<< " Years old; Name = " << RecvInfo->name << std::endl;
		}
	}

	// 4 关闭套子节
	closesocket(SockCli);
	SetColor(enmCFC_Red, enmCBC_Yellow);
	std::cout << " --- Client 已经结束                        " << std::endl;
	getchar();
	WSACleanup();
	return 0;
}


void SetColor(ForegroundColor foreColor, BackGroundColor backColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreColor | backColor);
}