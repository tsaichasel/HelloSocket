#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include<iomanip>
#include <thread>
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

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader {
	short DataLength;
	short Cmd;
};

struct Login : public DataHeader {
	Login() {
		DataLength = sizeof(Login);
		Cmd = CMD_LOGIN;
	}
	char UserName[32];
	char PassWord[32];
};

struct LoginResult : public DataHeader {
	LoginResult() {
		DataLength = sizeof(LoginResult);
		Cmd = CMD_LOGIN_RESULT;
		Result = 0;
	}
	int Result;
};

struct Logout : public DataHeader {
	Logout() {
		DataLength = sizeof(Logout);
		Cmd = CMD_LOGOUT;
	}
	char UserName[32];
};

struct LogoutResult : public DataHeader {
	LogoutResult() {
		DataLength = sizeof(LogoutResult);
		Cmd = CMD_LOGOUT_RESULT;
		Result = 0;
	}
	int Result;
};

struct NewUserJion : public DataHeader {
	NewUserJion() {
		DataLength = sizeof(NewUserJion);
		Cmd = CMD_NEW_USER_JOIN;
		SockID = 0;
	}
	int SockID;
};

void SetColor(ForegroundColor foreColor, BackGroundColor backColor);

int ClientProcessor(SOCKET Client) {
	// 5 recv 接收客户端数据
	//缓冲区 （注：recv函数会一直接收直到返回值为 < 0 阻塞在recv这里）
	char szRecv[1024] = {};
	int RecvLen = recv(Client, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (RecvLen <= 0) {
		SetColor(enmCFC_Black, enmCBC_HighWhite);
		std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 已经退出              " << std::endl;
		return -1;
	}

	// 6 send 处理客户端数据
	switch (header->Cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			LoginResult* loginresult = (LoginResult*)szRecv;
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_LOGIN_RESULT ：" << loginresult->Cmd
				<< " Result = " << loginresult->Result << " 长度 ：" << loginresult->DataLength << std::endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			LogoutResult* logoutresult = (LogoutResult*)szRecv;
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_LOGOUT_RESULT ：" << logoutresult->Cmd
				<< " Result = " << logoutresult->Result << " 长度 ：" << logoutresult->DataLength << std::endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			NewUserJion* userJion = (NewUserJion*)szRecv;
			SetColor(enmCFC_HighWhite, enmCBC_Black);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_NEW_USER_JOIN ：" << userJion->Cmd
				<< " 新Client Socket = " << userJion->SockID << " 长度 ：" << userJion->DataLength << std::endl;
		}
		break;
	}
}

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
		std::cout << " --- 建立SOCKET失败 ！                              " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Cyan);
		std::cout << " --- 建立SOCKET成功 ！                              " << std::endl;
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
		std::cout << " --- 连接服务器失败 ！                              " << std::endl;
	}
	else {
		SetColor(enmCFC_Black, enmCBC_Green);
		std::cout << " --- 连接服务器成功 ！                              " << std::endl;
	}

	while (1) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(SockCli, &fdRead);
		timeval t{ 1,0 };
		int ret = select(SockCli + 1, &fdRead, nullptr, nullptr, &t);
		if (ret < 0) {
			SetColor(enmCFC_Red, enmCBC_Yellow);
			std::cout << " --- Client select 结束1                            " << std::endl;
			break;
		}

		if (FD_ISSET(SockCli, &fdRead)) {
			FD_CLR(SockCli, &fdRead);
			if (-1 == ClientProcessor(SockCli)) {
				SetColor(enmCFC_Red, enmCBC_Yellow);
				std::cout << " --- Client select 结束2                            " << std::endl;
				break;
			}
		}

		SetColor(enmCFC_Black, enmCBC_Yellow);
		std::cout << " --- 现在Client Socket = " << std::setw(4) << std::setfill('0')
			<< SockCli << " 处理自己的事情        " << std::endl;
	}

	// 4 关闭套子节
	closesocket(SockCli);
	SetColor(enmCFC_Red, enmCBC_Yellow);
	std::cout << " --- Client 已经结束                                " << std::endl;
	SetColor(enmCFC_HighWhite, enmCBC_Black);
	getchar();
	WSACleanup();
	return 0;
}

void SetColor(ForegroundColor foreColor, BackGroundColor backColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreColor | backColor);
}