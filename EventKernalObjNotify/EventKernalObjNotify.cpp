// EventKernalObjNotify.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

#define MAX_TRY_COUNT 3
#define MAX_REQUESTS 32
#define READ_SIZE 64
HANDLE g_hFile;
HANDLE g_hEvents[MAX_REQUESTS];
OVERLAPPED g_tOverLapped[MAX_REQUESTS];
unsigned char g_arrBuffer[MAX_REQUESTS][1024]={0};

int QueueRequest( int nIndex, DWORD dwLocation, DWORD dwAmount)
{
	BOOL bRet;
	DWORD dwReadNum;
	//����һ��MANUAL�͵�Event����
	g_hEvents[nIndex] = CreateEvent(NULL, TRUE, FALSE, NULL);
	//����Event��������OVERLAPPED�ṹ
	g_tOverLapped[nIndex].hEvent = g_hEvents[nIndex];
	g_tOverLapped[nIndex].Offset = dwLocation;
	//for(int i=0;i<MAX_TRY_COUNT;i++)
	//{
		bRet = ReadFile(g_hFile, g_arrBuffer[nIndex], dwAmount, &dwReadNum, &g_tOverLapped[nIndex]);
	
		if( bRet )//������̶�ȡ�����ݷ�����
			return TRUE;
		DWORD dwErr = GetLastError();
		if( dwErr == ERROR_IO_PENDING )
		{
			return TRUE;
		}

		if( dwErr == ERROR_INVALID_USER_BUFFER || 
			dwErr == ERROR_NOT_ENOUGH_QUOTA ||
			dwErr == ERROR_NOT_ENOUGH_MEMORY )
		{
			Sleep(50);
			//continue;
		}
		//break;
	//}
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int i;
	BOOL bRet;
	//HANDLE hFile;
	DWORD dwReadNum;
	OVERLAPPED tOverLapped;
	unsigned char szBuffer[1024];
	char szFilePath[32] = "C:\\ReadMe.txt";
	g_hFile = CreateFile(szFilePath, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);

	for( i=0; i<MAX_REQUESTS; i++)
	{
		//��ͬһ�ļ��ּ������ְ�overlapped��ʽͬʱ��
        //ע�⿴QueueRequest��������������ģ�ÿ�ζ�16384���顣
		QueueRequest( i, i*64, READ_SIZE);
	}

	//�Ⱥ����в�������
	//������������һ���������ʱ�����Ӧ��Event����ᱻ���
	//while(1)
	//{
		WaitForMultipleObjects(MAX_REQUESTS, g_hEvents, TRUE, INFINITE);
		for (i=0; i<MAX_REQUESTS; i++)
		{
			DWORD dwReadNum;
			bRet = GetOverlappedResult(
				g_hFile,
				&g_tOverLapped[i],
				&dwReadNum,
				FALSE);
		
			if ( TRUE == bRet )
			{
				printf("��%d���˶�:", i);
				for( int j=0; j< dwReadNum; j++)
				{
					printf("%02X", g_arrBuffer[i][j]);
				}
				printf("\n");
				continue;
			}

			CloseHandle(g_tOverLapped[i].hEvent);
		}
		//break;
	//}

    CloseHandle(g_hFile);
    return EXIT_SUCCESS;
}

