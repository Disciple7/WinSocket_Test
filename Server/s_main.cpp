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
	err = WSAStartup(wVersionRequested, &wsaData); //����0���ɹ���������Ǵ�����
	if (err != 0)
	{
		printf("WinSock DLL�汾����Ҫ��n");
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
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);    //htol�������ֽ���long��ת��Ϊ�����ֽ���
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(4827);                      //htos�������˿�ת�����ַ���1024���ϵ����ּ���

	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, 0);           //�������ӵĿɿ��Է���SOCK_STRAM
	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)); //��socket�󶨵���Ӧ��ַ�Ͷ˿���
	listen(sockServer, 5);                                  //�ȴ������е���󳤶�Ϊ5

	printf("Welcome,the Host %s is running!Now Wating for someone comes in!\n", inet_ntoa(addrServer.sin_addr));

	int len = sizeof(SOCKADDR);
	SOCKADDR_IN addrClient;

	while (1)
	{
		SOCKET sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len); //�������ý���ֱ���µ����ӳ���
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
//���߳̽���һ��connect�󴴽������߳�hThread��ͬʱ�Լ�while����һ�κ���accept��������
//���߳̽��봦����Ϣģʽ�����߳���һ��״̬��ģ�͡����ȷ���˺Ϳͻ���Լ��һ���ֽڵ�״̬�룬�������״̬���������˽������ֲ���
//�ٴ����Ӧ���ݡ�

void state_recv_and_send(SOCKET sockClient)
{
	while (sockClient!=INVALID_SOCKET)
	{
		char recvBuf[MaxSize];
		string serverString;
		int byte = 0;
		byte = recv(sockClient, recvBuf, MaxSize, 0);//�������ӿͻ��˽�������
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
			serverString = serverString + " ! Server Get It!";//OK��
			cout << serverString << endl;
		}
		string sendBuf;
		sendBuf = serverString;
		byte = send(sockClient, sendBuf.c_str(), sendBuf.length(), 0);//�ͻ��˷�������
		if (byte < 0)
		{
			cout << "S Send Err 91" << endl;
			closesocket(sockClient);
			return;//����ʧ��ʱ����ʱ����̸߳���
		}
		else if (byte == 0)
		{
			cout << "S Send End" << endl;
			closesocket(sockClient);
			return;//�����ѹر�ʱ����ʱ����̸߳���
		}
		else
		{
			cout << "S Send Success" << endl;
		}
	}
	return;
}