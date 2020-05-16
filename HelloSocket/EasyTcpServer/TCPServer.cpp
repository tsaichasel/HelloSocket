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
	SOCKET SockSer = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET == SockSer)
		std::cout << "建立SOCKET失败" << std::endl;
	else
		std::cout << "建立SOCKET成功" << std::endl;

	// 2 bind 绑定用于接收客户端连接的网络端口
	sockaddr_in addrSer = {};
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(4567);//host to net unsigned short
	addrSer.sin_addr.S_un.S_addr = INADDR_ANY ;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(SockSer, (sockaddr*)&addrSer, sizeof(addrSer)))
		std::cout << "绑定端口失败" << std::endl;
	else
		std::cout << "绑定端口成功" << std::endl;
	
	// 3 listen 监听网络端口
	if(SOCKET_ERROR == listen(SockSer, 5))
		std::cout << "监听端口失败" << std::endl;
	else
		std::cout << "监听端口成功" << std::endl;
	
	// 4 accpet 等待接收客户端连接
	sockaddr_in ClientAddr{};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET Client = INVALID_SOCKET;
	char SendBuf[] = "你好！我是Server~";

	// 5 send 向客户端发送一条消息
	while (1) {
		Client = accept(SockSer, (sockaddr*)&ClientAddr, &nAddrLen);
		if (INVALID_SOCKET == Client)
			std::cout << "无效的客户端连接" << std::endl;
		std::cout << "新客户端加入 IP = " << inet_ntoa(ClientAddr.sin_addr) << std::endl;
		
		send(Client, SendBuf, sizeof(SendBuf) + 1, 0);
	}

	// 6 关闭套子节
	closesocket(SockSer);

	WSACleanup();
	return 0;
}