// client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <Winsock2.h>
#include <iostream>
#include <Ws2tcpip.h>
#pragma comment(lib,"WS2_32.LIB")
using namespace std;

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#define SOCK_RDM        4               /* reliably-delivered message */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */
/*
* Socket address, internet style.
*/
typedef struct sockaddr_in SOCKADDR_IN; /* Microsoft Windows Extended data types */

int SocketInit();
#define MaxSize 1024



const char *SeverIp = "127.0.0.1";

HANDLE hMutex;

void Send(SOCKET sockClient)
{
	char sendBuf[MaxSize];

	while (1)
	{
		WaitForSingleObject(hMutex, INFINITE);
		//printf("Please input your choice.\n");
		gets_s(sendBuf);
		int byte = 0;
		byte = send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);;//服务器从客户端接受数据
		if (byte <= 0)
			break;

		Sleep(1000);
		ReleaseMutex(hMutex);

	}
	closesocket(sockClient);//关闭socket,一次通信完毕
}

void Rec(SOCKET sockClient)
{

	char revBuf[MaxSize];

	while (1)
	{
		WaitForSingleObject(hMutex, INFINITE);

		int n = 0;
		n = recv(sockClient, revBuf, MaxSize, 0);//服务器从客户端接受数据
		revBuf[n] = '\0';
		if (n <= 0)
			break;

		printf("%s", revBuf);

		Sleep(1000);
		ReleaseMutex(hMutex);

	}
	closesocket(sockClient);//关闭socket,一次通信完毕
}


#pragma comment(lib,"WS2_32.LIB")


int SocketInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData); //返回0，成功，否则就是错误码

	if (err != 0)
	{
		printf("WinSock DLL版本不足要求n");
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return 0;
	}

	return 1;
}

int main()
{

	if (SOCKET_ERROR == SocketInit())
		return -1;

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == -1) {
		cout << "socket创建失败 : " << endl;
		exit(-1);
	}
	cout << "创建成功:" << endl;
	SOCKADDR_IN addrSrv;
	//inet_pton(AF_INET, SeverIp, (void *)&addrSrv.sin_addr.S_un.S_addr);
	addrSrv.sin_addr.S_un.S_addr = inet_addr(SeverIp);//设定需要连接的服务器的ip地址
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6666);//设定需要连接的服务器的端口地址
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(addrSrv));//与服务器进行连接

	while (1) {

		HANDLE hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Rec, (LPVOID)sockClient, 0, 0);

		if (hThread1 != NULL)
		{
			CloseHandle(hThread1);
		}
		
		HANDLE hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Send, (LPVOID)sockClient, 0, 0);

		if (hThread2 != NULL)
		{
			CloseHandle(hThread2);
		}

		//Sleep(1000);

	}


	getchar();

	WSACleanup();
	return 0;
}
