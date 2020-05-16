#define WIN32_LEAN_AND_MEAN	
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

int main() {
	//启动Windows socket 2.x环境
	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	//-- 用Socket API建立简易的TCP服务器
	// 1 建立一个socket
	SOCKET SockCli = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (INVALID_SOCKET == SockCli)
		std::cout << "建立SOCKET失败" << std::endl;
	else
		std::cout << "建立SOCKET成功" << std::endl;

	// 2 连接服务器 connect
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int nAddrLen = sizeof(sockaddr_in);
	int ret = connect(SockCli, (sockaddr*)&addrSer, nAddrLen);
	if (SOCKET_ERROR == ret)
		std::cout << "连接服务器失败" << std::endl;
	else
		std::cout << "连接服务器成功" << std::endl;

	// 3 接收服务器信息 recv
	char RecvBuf[256] = {};
	while (1) {
		int nLen = recv(SockCli, RecvBuf, 256, 0);
		if (nLen > 0)
			std::cout << "接收到数据：" << RecvBuf << std::endl;
	}

	// 4 关闭套子节
	closesocket(SockCli);

	WSACleanup();
	return 0;
}