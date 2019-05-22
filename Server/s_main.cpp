#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <Winsock2.h>
#define MaxSize 1024
#pragma comment(lib,"WS2_32.LIB")

using namespace std;
void state_recv_and_send(SOCKET sockClient);
HANDLE hMutex;

int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	int SockInit=0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData); //返回0，成功，否则就是错误码
	if (err != 0)
	{
		printf("WinSock DLL版本不足要求n");
		SockInit= 0;
	}
	else if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		SockInit= 0;
	}
	if(SockInit!=0)
		SockInit= 1;
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    //htol将主机字节序long型转换为网络字节序
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(4827);                      //htos用来将端口转换成字符，1024以上的数字即可

	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);           //面向连接的可靠性服务SOCK_STRAM
	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)); //将socket绑定到相应地址和端口上
	listen(sockServer, 5);                                  //等待队列中的最大长度为5

	printf("Welcome,the Host %s is running!Now Wating for someone comes in!\n", inet_ntoa(addrServer.sin_addr));

	int len = sizeof(SOCKADDR);
	SOCKADDR_IN addrClient;

	while (1)
	{
		SOCKET sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len); //阻塞调用进程直至新的连接出现
		if (sockClient == INVALID_SOCKET)
		{
			cout << "Acception Fucked Up" << endl;
			Sleep(1000);
			continue;
		}
		cout << "New Client Joins In." << endl;
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)state_recv_and_send, (LPVOID)sockClient, 0, 0);
		if (hThread != NULL)
		{
			CloseHandle(hThread);
			cout << hThread << " has been Closed" << endl;
			Sleep(100);
		}

	}
	WSACleanup();
	return 0;
}
//主线程接收一个connect后创建了子线程hThread，同时自己while运行一次后在accept处阻塞。
//子线程进入处理消息模式。子线程是一个状态机模型。首先服务端和客户端约定一个字节的状态码，根据这个状态码决定服务端进入哪种操作
//再传输对应数据。

void state_recv_and_send(SOCKET sockClient)
{
	while (sockClient!=INVALID_SOCKET)
	{
		char recvBuf[MaxSize];
		string serverString;
		int byte = 0;
		byte = recv(sockClient, recvBuf, MaxSize, 0);//服务器从客户端接受数据
		if (byte < 0)
		{
			cout << "S Recv Err 76" << endl;
		}
		else if (byte == 0)
			cout << "S Recv End" << endl;
		else
		{
			cout << "S Recv Success" << endl;
			recvBuf[byte] = '\0';
			serverString = recvBuf;
			serverString = serverString + " ! Server Get It!";//OK了
			cout << serverString << endl;
		}
		string sendBuf;
		sendBuf = serverString;
		byte = send(sockClient, sendBuf.c_str(), sendBuf.length(), 0);//客户端发送数据
		if (byte < 0)
		{
			cout << "S Send Err 91" << endl;
			closesocket(sockClient);
			return;//连接失败时，及时打断线程复读
		}
		else if (byte == 0)
		{
			cout << "S Send End" << endl;
			closesocket(sockClient);
			return;//连接已关闭时，及时打断线程复读
		}
		else
		{
			cout << "S Send Success" << endl;
		}
	}
	return;
}