// OverlappedDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;
/*
Internal��Ա�������Ա���������Ѵ����I/O����Ĵ����롣
InternalHigh��Ա�����첽I/O������ɵ�ʱ�������Ա���������Ѵ�����ֽ�����
�ڵ������OVERLAPPED�ṹ��ʱ��Microsoft����������Internal��InternalHigh��Ա��������ʵ����
����ʱ������ƣ�Microsoft��ʶ����Щ��Ա��������Ϣ��Կ�����Ա���ã���˰����ǹ����ˡ�
���ǣ�Microsoftû�иı���Щ��Ա�����֣�������Ϊ����ϵͳ��Դ����Ƶ�����õ����ǣ���Microsoft������Ϊ���޸�Դ���롣
*/
#define PAGE_SIZE 0x1000
void Sub_1();//ReadFile �첽����
void Sub_2();//ReadFileEx 
DWORD WINAPI Sub_1ThreadProcedure(LPVOID ParameterData);
DWORD WINAPI Sub_2ThreadProcedure(LPVOID ParameterData);

OVERLAPPED __Overlapped = {0};
unsigned char __BufferData[1024] = {0};

int _tmain(int argc, _TCHAR* argv[])
{
	Sub_1();//�����¼��ں˶���
	return 0;
}

void Sub_1()
{
	BOOL IsOk = FALSE;
	DWORD ReturnLength = 0;
	HANDLE FileHandle = CreateFile(TEXT("ReadMe.txt"), GENERIC_ALL, 
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if( INVALID_HANDLE_VALUE == FileHandle )
	{
		int LastError = GetLastError();
		goto Exit;
	}
	//��һ���첽IO������ɵ�ʱ������������OVERLAPPED�ṹ��hEvent��Ա�Ƿ�ΪNULL��
    //���hEvent��ΪNULL����ô������������SetEvent�������¼���
	//��ʱ�����ʹ���¼����������һ���豸�����Ƿ���ɣ������ǵȴ��豸(�ļ�)����
	__Overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE ThreadHandle = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)Sub_1ThreadProcedure,
		(LPVOID)FileHandle, 0, NULL);
	if( __BufferData == NULL )
	{
		goto Exit;
	}
	IsOk = ReadFile(FileHandle, __BufferData, 4, &ReturnLength,&__Overlapped);
	if( IsOk == FALSE )
	{
		int LastError = GetLastError();
		if( LastError == ERROR_IO_PENDING )
		{
			//�ɹ�
			printf("ERROR_IO_PENDING\r\n");//�ص�I/O���ر�־
		}
	}
	WaitForSingleObject(ThreadHandle, INFINITE);
Exit:
	if( FileHandle != NULL )
	{
		CloseHandle(FileHandle);
		FileHandle = NULL;
	}
	printf("\r\n");
	return;
}

DWORD WINAPI Sub_1ThreadProcedure(LPVOID ParameterData)
{
	HANDLE FileHandle = (HANDLE)ParameterData;
	BOOL IsOk = FALSE;
	DWORD ReturnLength = 0;
	while(1)
	{
		IsOk = WaitForSingleObject( __Overlapped.hEvent, INFINITE);
		IsOk -= WAIT_OBJECT_0;
		if( IsOk == 0 )
		{
			IsOk = GetOverlappedResult(FileHandle, &__Overlapped, &ReturnLength, INFINITE);
			if( IsOk == TRUE )
			{
				int i = 0; 
				for( i=0; i< ReturnLength; i++)
				{
					printf("%02X", __BufferData[i]);

				}
				__Overlapped.Offset += ReturnLength;
				ReadFile(FileHandle, &__BufferData, 4, &ReturnLength, &__Overlapped);
			}
			else
			{
				//�������
				break;
			}
		}
		else 
		{
			return 0;
		}
		
	}
	return 0;
}
