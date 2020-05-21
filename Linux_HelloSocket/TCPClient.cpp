#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
	#include <Windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
#include <iostream>
#include<iomanip>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

 
const int F_BLACK = 0x01;	// 000001
const int F_RED = 0x02;		// 000010
const int F_GREEN = 0x03;	// 000011
const int F_YELLOW = 0x04;	// 000100
const int F_BLUE = 0x05;	// 000101
const int F_PURPLE = 0x06;	// 000110
const int F_WHITE = 0x07;	// 000111
 
const int B_BLACK = 0x08;	// 001000
const int B_RED = 0x10;		// 010000
const int B_GREEN = 0x18;	// 011000
const int B_BROWN = 0x80;	// 100000
const int B_BLUE = 0x88;	// 101000
const int B_WHITE = 0x90;	// 110000
 
bool set_color(int fcolor, int bcolor)
{
	bool ret = true;
	int fore = fcolor%8;	//取color的后3位
	int back = (bcolor/8)*8;	//将color的后3位清空 (即取前3位)
	switch (fore)
	{
	case F_BLACK:std::cout<<"\033[30m";break;
	case F_RED:std::cout<<"\033[31m";break;
	case F_GREEN:std::cout<<"\033[32m";break;
	case F_YELLOW:std::cout<<"\033[33m";break;
	case F_BLUE:std::cout<<"\033[34m";break;
	case F_PURPLE:std::cout<<"\033[35m";break;
	case F_WHITE:std::cout<<"\033[37m";break;
	default:ret = false;
	}

	// switch (back)
	// {
	// case B_BLACK:std::cout<<"\033[40m";break;
	// case B_RED:std::cout<<"\033[41m";break;
	// case B_GREEN:std::cout<<"\033[42m";break;
	// case B_BROWN:std::cout<<"\033[43m";break;
	// case B_BLUE:std::cout<<"\033[44m";break;
	// case B_WHITE:std::cout<<"\033[47m";break;
	// default:ret = false;
	// }
	return ret;
}


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


int ClientProcessor(SOCKET Client) {
	// 5 recv 接收客户端数据
	//缓冲区 （注：recv函数会一直接收直到返回值为 < 0 阻塞在recv这里）
	char szRecv[1024] = {};
	int RecvLen = recv(Client, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (RecvLen <= 0) {
		set_color(F_WHITE, B_BLACK);
		std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 已经退出              " << std::endl;
		return -1;
	}

	// 6 send 处理客户端数据
	switch (header->Cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			LoginResult* loginresult = (LoginResult*)szRecv;
			set_color(F_WHITE, B_BLACK);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_LOGIN_RESULT ：" << loginresult->Cmd
				<< " Result = " << loginresult->Result << " 长度 ：" << loginresult->DataLength << std::endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			LogoutResult* logoutresult = (LogoutResult*)szRecv;
			set_color(F_WHITE, B_BLACK);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_LOGOUT_RESULT ：" << logoutresult->Cmd
				<< " Result = " << logoutresult->Result << " 长度 ：" << logoutresult->DataLength << std::endl;
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(Client, szRecv + sizeof(DataHeader), header->DataLength - sizeof(DataHeader), 0);
			NewUserJion* userJion = (NewUserJion*)szRecv;
			set_color(F_WHITE, B_BLACK);
			std::cout << " --- 收到Server Socket = " << std::setw(4) << std::setfill('0') << Client << " 请求 CMD_NEW_USER_JOIN ：" << userJion->Cmd
				<< " 新Client Socket = " << userJion->SockID << " 长度 ：" << userJion->DataLength << std::endl;
		}
		break;
	}
}

bool g_run = true;
void cmdThread(SOCKET SockCli) {
	while (true) {
		// 3 输入请求命令
		char CmdBuf[128] = {};
		set_color(F_WHITE, B_BLACK);
		std::cin >> CmdBuf;

		// 4 处理请求命令
		if (0 == strcmp(CmdBuf, "exit")) {
			set_color(F_RED, B_BLACK);
			std::cout << " --- Client 正在退出                        " << std::endl;
			g_run = false;
			break;
		}
		else if (0 == strcmp(CmdBuf, "login")) {
			// 5 向服务器发送命令
			Login login;
			login.Cmd = CMD_LOGIN;
			login.DataLength = sizeof(login);
			strcpy(login.UserName, "Tsai");
			strcpy(login.PassWord, "TsaiPassword");
			send(SockCli, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(CmdBuf, "logout")) {
			// 5 向服务器发送命令
			Logout logout;
			logout.Cmd = CMD_LOGOUT;
			logout.DataLength = sizeof(logout);
			strcpy(logout.UserName, "Tsai");
			send(SockCli, (const char*)&logout, sizeof(logout), 0);
		}
		else {
			set_color(F_RED, B_BLACK);
			std::cout << " --- Client 输入命令不支持                  " << std::endl;
		}
	}
}

int main() {
#ifdef _WIN32
	//启动Windows socket 2.x环境
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);
#endif
	//-- 用Socket API建立简易的TCP客户端
	// 1 建立一个socket
	SOCKET SockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (INVALID_SOCKET == SockCli) {
		set_color(F_RED, B_BLACK);
		std::cout << " --- 建立SOCKET失败 ！                              " << std::endl;
	}
	else {
		set_color(F_WHITE, B_BLACK);
		std::cout << " --- 建立SOCKET成功 ！                              " << std::endl;
	}

	// 2 连接服务器 connect
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
#ifdef _WIN32
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	addrSer.sin_addr.s_addr = inet_addr("192.168.63.1");
#endif
	int nAddrLen = sizeof(sockaddr_in);
	int ret = connect(SockCli, (sockaddr*)&addrSer, nAddrLen);
	if (SOCKET_ERROR == ret) {
		set_color(F_RED, B_BLACK);
		std::cout << " --- 连接服务器失败 ！                              " << std::endl;
	}
	else {
		set_color(F_WHITE, B_BLACK);
		std::cout << " --- 连接服务器成功 ！                              " << std::endl;
	}

	// 创建一个线程用于客户端发送请求
	std::thread t1(cmdThread, SockCli);
	t1.detach();

	//采用全局变量判断是否线程退出(后期使用条件变量condition_variable - notify_one)
	while (g_run) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(SockCli, &fdRead);
		timeval t{ 1,0 };
		int ret = select(SockCli + 1, &fdRead, nullptr, nullptr, &t);
		if (ret < 0) {
			set_color(F_RED, B_BLACK);
			std::cout << " --- Client 36select 结束1                            " << std::endl;
			break;
		}

		if (FD_ISSET(SockCli, &fdRead)) {
			FD_CLR(SockCli, &fdRead);
			if (-1 == ClientProcessor(SockCli)) {
				set_color(F_RED, B_BLACK);
				std::cout << " --- Client select 结束2                            " << std::endl;
				break;
			}
		}

		//set_color(STDOUT_FILENO, (Color)31);
		//std::cout << " --- 现在Client Socket = " << std::setw(4) << std::setfill('0')
		//	<< SockCli << " 处理自己的事情        " << std::endl;
		set_color(F_WHITE, B_BLACK);
	}

	// 4 关闭套子节
#ifdef _WIN32
	closesocket(SockCli);
	WSACleanup();
#else
	close(SockCli);
#endif
	set_color(F_RED, B_BLACK);
	std::cout << " --- Client 已经结束                                " << std::endl;
	set_color(F_WHITE, B_BLACK);
	getchar();
	return 0;
}
