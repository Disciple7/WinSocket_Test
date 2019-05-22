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
struct funcParam//CreateThreadʱ�������Ҫ�����������Ҫ����һ���ṹ�崫��
{
	int choice;
	SOCKET sockClient;
};

int main()
{
	//�׽��ֳ�ʼ��
	WSADATA wsaData;
	WORD wVersionRequested;
	int SocketInit = 1;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData); //����0���ɹ���������Ǵ�����
	if (err != 0)
	{
		printf("WinSock DLL�汾����Ҫ��n");
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
	//DLL�ж���ɣ���ʼ���׽��֣����õ�ַ�Ͷ˿�
	SOCKADDR_IN addrSrv;
	SOCKET sockClient_Long = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ServerIP);//�趨��Ҫ���ӵķ�������ip��ַ
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4827);//�趨��Ҫ���ӵķ������Ķ˿ڵ�ַ
	int connectResult=connect(sockClient_Long, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//���������������
	if (DEV_MODE)
	{
		if (connectResult == -1)
			cout << "Connection Fucked Up" << endl;
		else if (connectResult == 0)
			cout << "Connect Successfully" << endl;
	}

	int choice = 1;
	if(DEV_MODE)cout << "�����Ӳ���" << endl;
	while (choice)
	{
		cout << "Test: Input Your Choice" << endl;
		cin >> choice;
		//�����߳�
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
	if(DEV_MODE)cout << "�����Ӳ���" << endl;
	choice = 1;
	while (choice)
	{
		cout << "Test: Input Your Choice" << endl;
		cin >> choice;
		//�����߳�
		switch (choice)
		{
		case 0:
		{
			break;
		}
		default:
		{
			SOCKET sockClient_Short = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			int connectResult = connect(sockClient_Short, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//���������������
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
	WSACleanup();//�ر�ͨ��
	return -1;
}

void func1(int choice,SOCKET sockClient_Long)
{
	string choiceStr = std::to_string(choice);
	Send(choiceStr.data() , sockClient_Long);
	char recvBufPtr[MaxSize];
	Rec(recvBufPtr, sockClient_Long);
	printf("%s, Client Get It!\n", recvBufPtr);//д����Щ�ң������ַ��Ĵ�������BUG�����Socket��Thread����˼�ͺ�
}
//������ģʽ��
//�������Ӻ�ֱ������������ɺ�Źر��׽��֡�����˼�⵽���ӹرպ�رն�Ӧ�߳�
void func2(int choice, SOCKET sockClient_Short)
{
	string choiceStr = std::to_string(choice);
	Send(choiceStr.data(), sockClient_Short);
	char recvBufPtr[MaxSize];
	Rec(recvBufPtr, sockClient_Short);
	printf("%s, Client Get It!\n", recvBufPtr);
}
//������ģʽ��
//1.�������Ӻ��½��̵߳ȴ����շ��������ݡ�
//2.�������ݺ���ʾ���ر����ӹر��̡߳�
//3.���뷢�����̽����߳̽������ӣ���������������ݺ�ر����ӹر��̡߳�
//4.Ȼ���ٴν���������̡�
void Send(const char* stringBuffer,SOCKET sockClient)
{
	//�����߳�
	int byte = 0;
	byte = send(sockClient, stringBuffer, strlen(stringBuffer), 0);//�ͻ��˷�������
	if (DEV_MODE)
	{
		if (byte < 0)
		{
			cout << "C Send Err 100" << endl;
		}
		else
		{
			cout << "C Send Success" << endl;//OK��
		}
	}
	return;
}

void Rec(char* recvBuffer,SOCKET sockClient)
{
	char revBuf[MaxSize];
	int byte = 0;
	byte = recv(sockClient, revBuf, MaxSize, 0);//�������ӿͻ��˽�������
	if (byte < 0)
	{
		if(DEV_MODE)cout << "C Recv Err 116" << endl;//ERR��
	}
	else
	{
		revBuf[byte] = '\0';
		strcpy(recvBuffer, revBuf);
		if(DEV_MODE)cout << "C Recv Success" << endl;
	}
	return;
}
//������ģʽ��
//1.�������Ӻ��½��̵߳ȴ����շ��������ݡ�
//2.�������ݺ���ʾ���ر����ӹر��̡߳�
//3.���뷢�����̽����߳̽������ӣ���������������ݺ�ر����ӹر��̡߳�
//4.Ȼ���ٴν���������̡�