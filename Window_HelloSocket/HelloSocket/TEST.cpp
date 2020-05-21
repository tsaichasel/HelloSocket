#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>
#include <WinSock2.h>

//#pragma comment(lib,"ws2_32.lib")

int main() {

	WORD Version = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(Version, &wsadata);

	WSACleanup();
	return 0;
}