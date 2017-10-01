// ServiceTest.cpp: 定义控制台应用程序的入口点。

#include <Windows.h>
#include <iostream>
#include <string>
#include <memory>
using namespace std;
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Kernel32")

#define DEBUG_BASE_INFO		(std::string(__FILE__) + " : " + std::string(__FUNCTION__) + " : ") 

enum ServiceStatus : unsigned int
{
	ServiceStatus_NotExist = 0,
	ServiceStatus_Stopped = 1,
	ServiceStatus_StartPending = 2,
	ServiceStatus_StopPending = 3,
	ServiceStatus_Running = 4,
	ServiceStatus_ContinuePending = 5,
	ServiceStatus_PausePending = 6,
	ServiceStatus_Paused = 7,
	ServiceStatus_FunctionFailed = 8
};


ServiceStatus CheckServiceStatus(const string& serviceName);

int main()
{
//	auto result = CheckServiceStatus("ADSafeSvc");


	system("pause");
    return 0;
}



ServiceStatus CheckServiceStatus(const string& serviceName)
{
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
	if (nullptr == scm)
	{
		auto dbgStr = move(DEBUG_BASE_INFO + "OpenSCManagerA filed");
		OutputDebugStringA(dbgStr.c_str( ));
		return ServiceStatus::ServiceStatus_FunctionFailed;
	}
	// 查询服务
	LPENUM_SERVICE_STATUSA lpServices = nullptr;
	DWORD cbBufSize = 0;
	DWORD pcbBytesNeeded = 0;
	DWORD lpServicesReturned = 0;
	DWORD lpResumeHandle = 0;
	// 第一次调用 EnumServicesStatusA 应该总是执行失败，从传出参数中获取缓存大小
	auto enumResult = EnumServicesStatusA(scm, SERVICE_WIN32, SERVICE_STATE_ALL, lpServices, cbBufSize, &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	auto errorCode = GetLastError( );
	if ((0 != enumResult) || (ERROR_MORE_DATA != errorCode))
	{
		auto dbgStr = move(DEBUG_BASE_INFO + "EnumServicesStatusA filed");
		OutputDebugStringA(dbgStr.c_str( ));
		CloseServiceHandle(scm);
		return ServiceStatus::ServiceStatus_FunctionFailed;
	}
	// 从传出参数获取缓存大小，并重新调用 EnumServicesStatusA
	cbBufSize = pcbBytesNeeded;
	auto tmpbuf = make_unique<BYTE[]>(cbBufSize);
	lpServices = (LPENUM_SERVICE_STATUSA)(tmpbuf.get( ));
	// 再次调用 EnumServicesStatusA 应该成功，若不成功，则返回错误
	enumResult = EnumServicesStatusA(scm, SERVICE_WIN32, SERVICE_STATE_ALL, lpServices, cbBufSize, &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	CloseServiceHandle(scm);
	if (0 == enumResult)
	{
		auto dbgStr = move(DEBUG_BASE_INFO + "EnumServicesStatusA filed");
		OutputDebugStringA(dbgStr.c_str( ));
		return ServiceStatus::ServiceStatus_FunctionFailed;
	}
	// 查询服务
	for (DWORD i = lpResumeHandle; i < lpServicesReturned; ++i)
	{
		if (serviceName == lpServices->lpServiceName)
		{
			return static_cast<ServiceStatus>(lpServices->ServiceStatus.dwCurrentState);
		}

		++lpServices;
	}

	return ServiceStatus::ServiceStatus_NotExist;
}
