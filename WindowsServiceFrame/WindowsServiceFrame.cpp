// WindowsServiceFrame.cpp: 定义控制台应用程序的入口点。
#include "EServiceFrame.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#define DEBUG_BASE_INFO		(std::string(__FILE__) + " : " + std::string(__FUNCTION__) + " : ")

const static string serviceName = "AutoServiceTest";
const static string serviceDisplayName = "AutoServiceTest 测试服务框架";
static SERVICE_STATUS serviceStatus;
static SERVICE_STATUS_HANDLE hStatus = nullptr;
void ServiceMain(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors);
void ControlHandler(DWORD dwControl);
void ServiceFunc(void);


int main(int argc, char** argv)
{
	if (2 == argc)
	{
		EServiceFrame::ErrorCode errorCode = EServiceFrame::ErrorCode::Succeed;
		EServiceFrame service;
		if (!_stricmp(argv[1], "-Install"))
		{
			char exeFileName[256 + 1] = { 0 };
			auto getFileNameResult = GetModuleFileNameA(nullptr, exeFileName, 256);
			exeFileName[256] = '\0';
			errorCode = service.InstallLocalService(serviceName, serviceDisplayName, exeFileName);
		}
		else if (!_stricmp(argv[1], "-Uninstall"))
		{
			errorCode = service.UninstallLocalService(serviceName);
		}
		else if (!_stricmp(argv[1], "-Start"))
		{
			errorCode = service.StartLocalService(serviceName);
		}
		else if (!_stricmp(argv[1], "-Stop"))
		{
			errorCode = service.StopLocalService(serviceName);
		}
		else if (!_stricmp(argv[1], "-Status"))
		{
			errorCode = static_cast<EServiceFrame::ErrorCode>(service.CheckServiceStatus(serviceName));
		}
		else if (!_stricmp(argv[1], "-Reinstall"))
		{
			char exeFileName[256 + 1] = { 0 };
			auto getFileNameResult = GetModuleFileNameA(nullptr, exeFileName, 256);
			exeFileName[256] = '\0';
			errorCode = service.ReinstallLocalService(serviceName, serviceDisplayName, exeFileName);
		}
		else if (!_stricmp(argv[1], "-Restart"))
		{
			errorCode = service.RestartLocalService(serviceName);
		}
		else
		{
			return -1;
		}

		cout << static_cast<UINT>(errorCode) << endl;
		return 0;
	}
	
	SERVICE_TABLE_ENTRYA ServiceTable[2] =
	{
		{ (LPSTR)(serviceName.c_str()), (LPSERVICE_MAIN_FUNCTIONA)ServiceMain },
		{ nullptr, nullptr }
	};

	StartServiceCtrlDispatcherA(ServiceTable);

    return 0;
}

void ServiceMain(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors)
{
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;
	serviceStatus.dwServiceType = SERVICE_WIN32;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwServiceSpecificExitCode = 0;

	hStatus = RegisterServiceCtrlHandlerA(serviceName.c_str(), (LPHANDLER_FUNCTION)ControlHandler);
	if (nullptr == hStatus)
	{
		auto dbgStr = move(DEBUG_BASE_INFO + "RegisterServiceCtrlHandlerA failed, " + serviceName + " start error");
		OutputDebugStringA(dbgStr.c_str( ));
		return;
	}

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &serviceStatus);

	while (SERVICE_RUNNING == serviceStatus.dwCurrentState)
	{
		ServiceFunc( );
		Sleep(3200);
	}
}

void ControlHandler(DWORD dwControl)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		break;

	default:
		break;
	}

	SetServiceStatus(hStatus, &serviceStatus);
}

void ServiceFunc(void)
{
	static int idx = 0;
	auto cmdStr = move(string("cmd /c echo ") + to_string(++idx) + string(" >> c://taskList.txt"));
	system(cmdStr.c_str( ));
	
/*
	Sleep(3200);
	char exeFileName[256 + 1] = { 0 };
	auto getFileNameResult = GetModuleFileNameA(nullptr, exeFileName, 256);
	exeFileName[256] = '\0';
	auto cmdStr = move(string(exeFileName) + " -Restart");
	WinExec(cmdStr.c_str( ), SW_SHOW);
*/
}
