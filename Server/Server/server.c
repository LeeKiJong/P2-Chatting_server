#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h> 
#include <memory.h>
#define BUF_SIZE 100
#define NAME_SIZE 20
#define MAX_CLNT 256


unsigned WINAPI HandleClnt(void* arg);
void SendMsg(char* msg, int len);
void ErrorHandling(char* msg);
void err_quit(char* msg);
void err_display(char* msg);

int clntCnt = 0;


char clientName[MAX_CLNT][NAME_SIZE];
char hclientName[NAME_SIZE];
SOCKET clntSocks[MAX_CLNT];
SOCKADDR_IN clntAdrList[MAX_CLNT];
HANDLE hMutex;
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz;
	HANDLE  hThread;
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	hMutex = CreateMutex(NULL, FALSE, NULL);
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));



	if (bind(hServSock, (SOCKADDR*)& servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");
	while (1) {
		clntAdrSz = sizeof(clntAdr);
		hClntSock = accept(hServSock, (SOCKADDR*)& clntAdr, &clntAdrSz);

		WaitForSingleObject(hMutex, INFINITE);


		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt,
			(void*)& hClntSock, 0, NULL);


		clntSocks[clntCnt] = hClntSock;
		clntAdrList[clntCnt] = clntAdr;
		printf("Connected client IP: %s \n",
			inet_ntoa(clntAdr.sin_addr));
		clntCnt++;
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}
unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0, i;
	char msg[BUF_SIZE];
	char Msgto[NAME_SIZE] = "";
	char msg2[BUF_SIZE] = "";
	char* token = "";




	recv(hClntSock, clientName[clntCnt - 1], sizeof(clientName[clntCnt - 1]), 0);

	for (i = 0; i < clntCnt; i++)
		if (hClntSock == clntSocks[i])
			strcpy(hclientName, clientName[i]);


	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0) {
		int check = 0;
		char dum1[NAME_SIZE];
		strcpy(dum1, msg);
		if (!strcmp(token = strtok(msg, " "), "/to")) {
			Sleep(20);
			if (token = strtok(NULL, " "))
				strcpy(Msgto, token);
			if (token = strtok(NULL, "\n"))
				strcpy(msg2, token);

			for (i = 0; i < clntCnt; i++) {
				if (!strcmp(clientName[i], Msgto)) {
					send(clntSocks[i], hclientName, sizeof(hclientName), 0);
					send(clntSocks[i], "님이 귓속말을 보냈습니다.\n", 40, 0);
					Sleep(20);
					send(clntSocks[i], msg2, sizeof(msg2), 0);
					send(clntSocks[i], "\n", 10, 0);
					Sleep(20);
					check = 1;
				}
			}
			Sleep(20);
			if (check == 0)
			{
				send(hClntSock, Msgto, sizeof(Msgto), 0);
				send(hClntSock, "님은 현재 접속중이지 않습니다.\n---현재 접속중인 사용자 목록---\n", 100, 0);
				for (i = 0; i < clntCnt; i++) {
					send(hClntSock, clientName[i], 100, 0);
					Sleep(20);
					send(hClntSock, "       ", 20, 0);
					Sleep(20);
					send(hClntSock, inet_ntoa(clntAdrList[i].sin_addr), 30, 0);
					Sleep(20);
					send(hClntSock, "\n", 20, 0);
					Sleep(20);
				}
			}
			else {
				strcat(Msgto, "님에게 귓속말을 보냈습니다.");
				send(hClntSock, Msgto, 40, 0);
			}
		}
		else if (!strcmp(token = strtok(msg, " "), "/fileto")) {
			Sleep(20);
			if (token = strtok(NULL, " "))
				strcpy(Msgto, token);
			if (token = strtok(NULL, "\n"))
				strcpy(msg2, token);

			int retval;
			FILE* fp = fopen(msg2, "rb");
			if (fp == NULL) {
				perror("파일 입출력 오류");
				return -1;
			}
			char filename[256];
			ZeroMemory(filename, 256);
			sprintf(filename, msg2);
			retval = send(hClntSock, filename, 256, 0);
			if (retval == SOCKET_ERROR) err_quit("send()");

			// 파일 크기 얻기
			fseek(fp, 0, SEEK_END);      // go to the end of file
			int totalbytes = ftell(fp);  // get the current position

								  // 파일 크기 보내기
			retval = send(hClntSock, (char*)& totalbytes,
				sizeof(totalbytes), 0);
			if (retval == SOCKET_ERROR) err_quit("send()");

			// 파일 데이터 전송에 사용할 변수
			char buf[BUF_SIZE];
			int numread;
			int numtotal = 0;
			rewind(fp); // 파일 포인터를 제일 앞으로 이동
			while (1) {
				numread = fread(buf, 1, BUF_SIZE, fp);
				if (numread > 0) {
					retval = send(hClntSock, buf, numread, 0);
					if (retval == SOCKET_ERROR) {
						err_display("send()");
						break;
					}
					numtotal += numread;
				}
				else if (numread == 0 && numtotal == totalbytes) {
					printf("파일 전송 완료!: %d 바이트\n", numtotal);
					break;
				}
				else {
					perror("파일 입출력 오류");
					break;
				}
			}
			fclose(fp);
			closesocket(hClntSock);
			WSACleanup();
			return 0;
		}
		else if (!strcmp(strtok(msg, "\n"), "/list")) {
			send(hClntSock, "\n---현재 접속중인 사용자 목록---\n", 40, 0);
			for (i = 0; i < clntCnt; i++) {
				send(hClntSock, clientName[i], 100, 0);
				Sleep(20);
				send(hClntSock, "       ", 20, 0);
				Sleep(20);
				send(hClntSock, inet_ntoa(clntAdrList[i].sin_addr), 30, 0);
				Sleep(20);
				send(hClntSock, "\n", 20, 0);
				Sleep(20);
			}
		}
		else {
			SendMsg(dum1, strLen);

		}

	}


	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)   // remove disconnected client
	{
		if (hClntSock == clntSocks[i])
		{
			while (i++ < clntCnt - 1)
				clntSocks[i] = clntSocks[i + 1];
			break;
		}
	}
	clntCnt--;

	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;
}
void SendMsg(char* msg, int len)   // send to all
{
	int i;

	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++) {
		send(clntSocks[i], msg, len, 0);
	}
	ReleaseMutex(hMutex);

}

void ErrorHandling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);

}