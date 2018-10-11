// OverlappedDemo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;
/*
Internal成员：这个成员用来保存已处理的I/O请求的错误码。
InternalHigh成员：当异步I/O请求完成的时候，这个成员用来保存已传输的字节数。
在当初设计OVERLAPPED结构的时候，Microsoft决定不公开Internal和InternalHigh成员（名副其实）。
随着时间的推移，Microsoft认识到这些成员包含的信息会对开发人员有用，因此把它们公开了。
但是，Microsoft没有改变这些成员的名字，这是因为操作系统的源代码频繁地用到它们，而Microsoft并不想为此修改源代码。
*/
#define PAGE_SIZE 0x1000
void Sub_1();//ReadFile 异步操作
void Sub_2();//ReadFileEx 
DWORD WINAPI Sub_1ThreadProcedure(LPVOID ParameterData);
DWORD WINAPI Sub_2ThreadProcedure(LPVOID ParameterData);

OVERLAPPED __Overlapped = {0};
unsigned char __BufferData[1024] = {0};

int _tmain(int argc, _TCHAR* argv[])
{
	Sub_1();//触发事件内核对象
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
	//当一个异步IO请求完成的时候，驱动程序检查OVERLAPPED结构的hEvent成员是否为NULL。
    //如果hEvent不为NULL，那么驱动程序会调用SetEvent来触发事件。
	//这时候就是使用事件对象来检查一个设备操作是否完成，而不是等待设备(文件)对象。
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
			//成功
			printf("ERROR_IO_PENDING\r\n");//重叠I/O返回标志
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
				//数据完毕
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
