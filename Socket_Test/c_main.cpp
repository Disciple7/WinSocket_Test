#include <Winsock2.h>
#include <iostream>
#include<string>
using namespace std;
#define MaxSize 1024
#define DEV_MODE 1
#pragma comment(lib,"WS2_32.LIB")

const char *ServerIP = "127.0.0.1";

void Rec(char* recvBuffer,SOCKET sockClient);
void Send(const char* sendBuffer,SOCKET sockClient);
void func1(int choice, SOCKET sockClient_Long);
void func2(int choice, SOCKET sockClient_Short);
struct funcParam//CreateThread时，如果需要多个参数，则要构造一个结构体传入
{
	int choice;
	SOCKET sockClient;
};

int main()
{
	//套接字初始化
	WSADATA wsaData;
	WORD wVersionRequested;
	int SocketInit = 1;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData); //返回0，成功，否则就是错误码
	if (err != 0)
	{
		printf("WinSock DLL版本不足要求n");
		SocketInit= 0;
	}
	if (SocketInit!=0&&(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2))
	{
		WSACleanup();
		SocketInit= 0;
	}
	if(SocketInit!=0)
		SocketInit = 1;
	if (SOCKET_ERROR == SocketInit)
		return -1;
	//DLL判断完成，初始化套接字，设置地址和端口
	SOCKADDR_IN addrSrv;
	SOCKET sockClient_Long = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ServerIP);//设定需要连接的服务器的ip地址
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4827);//设定需要连接的服务器的端口地址
	int connectResult=connect(sockClient_Long, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//与服务器进行连接
	if (DEV_MODE)
	{
		if (connectResult == -1)
			cout << "Connection Fucked Up" << endl;
		else if (connectResult == 0)
			cout << "Connect Successfully" << endl;
	}

	int choice = 1;
	if(DEV_MODE)cout << "长连接测试" << endl;
	while (choice)
	{
		cout << "Test: Input Your Choice" << endl;
		cin >> choice;
		//接收线程
		switch (choice)
		{
		case 0:
		{
			break;
		}
		default:
		{
			func1(choice, sockClient_Long);
			break;
		}
		}
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	}
	closesocket(sockClient_Long);
	if(DEV_MODE)cout << "短连接测试" << endl;
	choice = 1;
	while (choice)
	{
		cout << "Test: Input Your Choice" << endl;
		cin >> choice;
		//接收线程
		switch (choice)
		{
		case 0:
		{
			break;
		}
		default:
		{
			SOCKET sockClient_Short = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			int connectResult = connect(sockClient_Short, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//与服务器进行连接
			if (DEV_MODE) 
			{
				if (connectResult == -1)
					cout << "Connection Fucked Up" << endl;
				else if (connectResult == 0)
					cout << "Connect Successfully" << endl;
			}
			func2(choice, sockClient_Short);
			closesocket(sockClient_Short);
			break;
		}
		}
		cin.clear();
		cin.ignore(INT_MAX, '\n');
	}
	WSACleanup();//关闭通信
	return -1;
}

void func1(int choice,SOCKET sockClient_Long)
{
	string choiceStr = std::to_string(choice);
	Send(choiceStr.data() , sockClient_Long);
	char recvBufPtr[MaxSize];
	Rec(recvBufPtr, sockClient_Long);
	printf("%s, Client Get It!\n", recvBufPtr);//写得有些乱，可能字符的处理上有BUG。理解Socket和Thread的意思就好
}
//长连接模式。
//建立连接后，直到所有任务完成后才关闭套接字。服务端检测到连接关闭后关闭对应线程
void func2(int choice, SOCKET sockClient_Short)
{
	string choiceStr = std::to_string(choice);
	Send(choiceStr.data(), sockClient_Short);
	char recvBufPtr[MaxSize];
	Rec(recvBufPtr, sockClient_Short);
	printf("%s, Client Get It!\n", recvBufPtr);
}
//短连接模式。
//1.建立连接后，新建线程等待接收服务器数据。
//2.接收数据后显示，关闭连接关闭线程。
//3.进入发送流程建立线程建立连接，向服务器发送数据后关闭连接关闭线程。
//4.然后再次进入接收流程。
void Send(const char* stringBuffer,SOCKET sockClient)
{
	//发送线程
	int byte = 0;
	byte = send(sockClient, stringBuffer, strlen(stringBuffer), 0);//客户端发送数据
	if (DEV_MODE)
	{
		if (byte < 0)
		{
			cout << "C Send Err 100" << endl;
		}
		else
		{
			cout << "C Send Success" << endl;//OK了
		}
	}
	return;
}

void Rec(char* recvBuffer,SOCKET sockClient)
{
	char revBuf[MaxSize];
	int byte = 0;
	byte = recv(sockClient, revBuf, MaxSize, 0);//服务器从客户端接受数据
	if (byte < 0)
	{
		if(DEV_MODE)cout << "C Recv Err 116" << endl;//ERR了
	}
	else
	{
		revBuf[byte] = '\0';
		strcpy(recvBuffer, revBuf);
		if(DEV_MODE)cout << "C Recv Success" << endl;
	}
	return;
}
//短连接模式。
//1.建立连接后，新建线程等待接收服务器数据。
//2.接收数据后显示，关闭连接关闭线程。
//3.进入发送流程建立线程建立连接，向服务器发送数据后关闭连接关闭线程。
//4.然后再次进入接收流程。