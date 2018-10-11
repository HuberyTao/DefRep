// DeviceKernalObjNotify.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bRet;
	HANDLE hFile;
	DWORD dwReadNum;
	OVERLAPPED tOverLapped;
	unsigned char szBuffer[1024];
	char szFilePath[32] = "C:\\ReadMe.txt";
	hFile = CreateFile(szFilePath, 
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);

	memset(&tOverLapped, 0, sizeof(tOverLapped));
	tOverLapped.Offset = 1;
	bRet = ReadFile(hFile, szBuffer, 4, &dwReadNum, &tOverLapped);
	if( bRet )
	{
		//�ļ�������
	}
	else
	{
		if(GetLastError() == ERROR_IO_PENDING )
		{
			//��������ERROR_IO_PENDING,����ζ�Ŷ��ļ��Ĳ������ڽ����С�
			//�Ⱥ�ֱ���ļ����ꡣ
			while(1)
			{
				WaitForSingleObject( hFile, INFINITE);
				bRet = GetOverlappedResult( hFile, &tOverLapped, &dwReadNum, FALSE);
				if ( TRUE == bRet )
				{
					for( int i=0; i< dwReadNum; i++)
					{
						printf("%02X", szBuffer[i]);
					}
					tOverLapped.Offset += dwReadNum;
					ReadFile(hFile, szBuffer, 4, &dwReadNum, &tOverLapped);
				}
				else
				{
					break;
				}
			}	
			
		}
		else
		{
			//����
		}

	}
	CloseHandle(hFile);
	return EXIT_SUCCESS;
	return 0;
}

