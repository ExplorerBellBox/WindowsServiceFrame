#include "EServiceFrame.h"
#include <Windows.h>
#include <memory>
#include <string>
using std::make_unique;
using std::unique_ptr;
using std::string;
using std::wstring;


#define DEBUG_BASE_INFO		(std::string(__FILE__) + " : " + std::string(__FUNCTION__) + " : ")
#define DEBUG_OUT(dbgInfo)	(OutputDebugStringA((DEBUG_BASE_INFO + (dbgInfo)).c_str()))

auto ServiceHandleFree = [](SC_HANDLE servicehandle) 
{ 
	if (nullptr != servicehandle)
	{
		CloseServiceHandle(servicehandle);
		servicehandle = nullptr;
	}
};

EServiceFrame::EServiceFrame( )
{

}


EServiceFrame::~EServiceFrame( ) noexcept
{

}

EServiceFrame::ErrorCode EServiceFrame::InstallLocalService(const string & serviceName, const string & serviceDisplayName, const string & exeFileFullPathName)
{
	if (serviceName.empty( ) || serviceDisplayName.empty( ) || exeFileFullPathName.empty( ))
	{
		DEBUG_OUT("argument error");
		return ErrorCode::ArgumentError;
	}
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scm(scm, ServiceHandleFree);
	if (nullptr == scm)
	{
		DEBUG_OUT("OpenSCManagerA failed");
		return ErrorCode::Failed;
	}
	// 尝试创建服务
	auto scv = CreateServiceA(scm, serviceName.c_str( ), serviceDisplayName.c_str( ), SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, exeFileFullPathName.c_str( ), nullptr, nullptr, nullptr, nullptr, nullptr);
	auto errorCode = GetLastError( );
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scv(scv, ServiceHandleFree);
	if (nullptr == scv)
	{
		if (ERROR_SERVICE_EXISTS == errorCode)
		{
			return ErrorCode::AllReadyExist;
		}

		DEBUG_OUT("CreateServiceA failed");
		return ErrorCode::Failed;
	}

	return ErrorCode::Succeed;
}

EServiceFrame::ErrorCode EServiceFrame::UninstallLocalService(const string & serviceName)
{
	if (serviceName.empty( ))
	{
		DEBUG_OUT("argument error");
		return ErrorCode::ArgumentError;
	}
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scm(scm, ServiceHandleFree);
	if (nullptr == scm)
	{
		DEBUG_OUT("OpenSCManagerA failed");
		return ErrorCode::Failed;
	}
	// 尝试打开服务
	auto scv = OpenServiceA(scm, serviceName.c_str( ), SERVICE_ALL_ACCESS | DELETE);
	auto errorCode = GetLastError( );
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scv(scv, ServiceHandleFree);
	if (nullptr == scv)
	{
		if (ERROR_SERVICE_DOES_NOT_EXIST == errorCode)
		{
			return ErrorCode::NotExist;
		}
		
		DEBUG_OUT("OpenServiceA failed");
		return ErrorCode::Failed;
	}
	// 尝试停止服务
	SERVICE_STATUS serviceStatus;
	memset(&serviceStatus, 0, sizeof(ServiceStatus));
	auto controlResult = ControlService(scv, SERVICE_CONTROL_STOP, &serviceStatus);
	errorCode = GetLastError( );
	if ((0 == controlResult) && (ERROR_SERVICE_NOT_ACTIVE != errorCode))
	{
		DEBUG_OUT("ControlService failed");
		return ErrorCode::Failed;
	}
	// 等待服务真正停止
	while (true)
	{
		Sleep(32);
		QueryServiceStatus(scv, &serviceStatus);
		if (SERVICE_STOPPED == serviceStatus.dwCurrentState)
		{
			break;
		}
	}
	// 尝试删除服务
	auto deleteResult = DeleteService(scv);
	if (0 == deleteResult)
	{
		DEBUG_OUT("DeleteService failed");
		return ErrorCode::Failed;
	}

	return ErrorCode::Succeed;
}

EServiceFrame::ErrorCode EServiceFrame::StartLocalService(const string & serviceName)
{
	if (serviceName.empty( ))
	{
		DEBUG_OUT("argument error");
		return ErrorCode::ArgumentError;
	}
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scm(scm, ServiceHandleFree);
	if (nullptr == scm)
	{
		DEBUG_OUT("OpenSCManagerA failed");
		return ErrorCode::Failed;
	}
	// 尝试打开服务
	auto scv = OpenServiceA(scm, serviceName.c_str( ), SERVICE_ALL_ACCESS);
	auto errorCode = GetLastError( );
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scv(scv, ServiceHandleFree);
	if (nullptr == scv)
	{
		if (ERROR_SERVICE_DOES_NOT_EXIST == errorCode)
		{
			return ErrorCode::NotExist;
		}
		
		DEBUG_OUT("OpenServiceA failed");
		return ErrorCode::Failed;
	}
	// 尝试启动服务
	auto startResult = StartServiceA(scv, 0, nullptr);
	errorCode = GetLastError( );
	if ((0 == startResult) && (ERROR_SERVICE_ALREADY_RUNNING != errorCode))
	{
		DEBUG_OUT("StartServiceA failed");
		return ErrorCode::Failed;
	}

	return ErrorCode::Succeed;
}

EServiceFrame::ErrorCode EServiceFrame::StopLocalService(const string & serviceName)
{
	if (serviceName.empty( ))
	{
		DEBUG_OUT("argument error");
		return ErrorCode::ArgumentError;
	}
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scm(scm, ServiceHandleFree);
	if (nullptr == scm)
	{
		DEBUG_OUT("OpenSCManagerA failed");
		return ErrorCode::Failed;
	}
	// 尝试打开服务
	auto scv = OpenServiceA(scm, serviceName.c_str( ), SERVICE_ALL_ACCESS);
	auto errorCode = GetLastError( );
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scv(scv, ServiceHandleFree);
	if (nullptr == scv)
	{
		if (ERROR_SERVICE_DOES_NOT_EXIST == errorCode)
		{
			return ErrorCode::NotExist;
		}
		
		DEBUG_OUT("OpenServiceA failed");
		return ErrorCode::Failed;
	}
	// 尝试停止服务
	SERVICE_STATUS serviceStatus;
	auto controlResult = ControlService(scv, SERVICE_CONTROL_STOP, &serviceStatus);
	errorCode = GetLastError( );
	if ((0 == controlResult) && (ERROR_SERVICE_NOT_ACTIVE != errorCode))
	{
		DEBUG_OUT("ControlService failed");
		return ErrorCode::Failed;
	}

	return ErrorCode::Succeed;
}

EServiceFrame::ErrorCode EServiceFrame::ReinstallLocalService(const string & serviceName, const string & serviceDisplayName, const string & exeFileFullPathName)
{
	auto uninstallResult = UninstallLocalService(serviceName);
	if ((ErrorCode::Succeed != uninstallResult) && (ErrorCode::NotExist != uninstallResult))
	{
		return uninstallResult;
	}

	while (true)
	{
		Sleep(128);
		auto checkResult = CheckServiceStatus(serviceName);
		if (ServiceStatus::NotExist == checkResult)
		{
			break;
		}
		else if (ServiceStatus::FunctionFailed == checkResult)
		{
			return ErrorCode::Failed;
		}
	}

	return InstallLocalService(serviceName, serviceDisplayName, exeFileFullPathName);
}

EServiceFrame::ErrorCode EServiceFrame::RestartLocalService(const string & serviceName)
{
	auto stopResult = StopLocalService(serviceName);
	if (ErrorCode::Succeed != stopResult)
	{
		return stopResult;
	}

	while (true)
	{
		Sleep(64);
		auto checkResult = CheckServiceStatus(serviceName);
		if (ServiceStatus::Stopped == checkResult)
		{
			break;
		}
		else if (ServiceStatus::FunctionFailed == checkResult)
		{
			return ErrorCode::Failed;
		}
	}

	return StartLocalService(serviceName);
}

EServiceFrame::ServiceStatus EServiceFrame::CheckServiceStatus(const string & serviceName)
{
	if (serviceName.empty())
	{
		DEBUG_OUT("argument error");
		return ServiceStatus::NotExist;
	}
	// 打开服务管理器
	auto scm = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ENUMERATE_SERVICE);
	unique_ptr<SC_HANDLE__, decltype(ServiceHandleFree)> auto_scm(scm, ServiceHandleFree);
	if (nullptr == scm)
	{
		DEBUG_OUT("OpenSCManagerA failed");
		return ServiceStatus::FunctionFailed;
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
		DEBUG_OUT("EnumServicesStatusA failed");
		return ServiceStatus::FunctionFailed;
	}
	// 从传出参数获取缓存大小
	cbBufSize = pcbBytesNeeded;
	auto tmpbuf = make_unique<BYTE[]>(cbBufSize);
	lpServices = (LPENUM_SERVICE_STATUSA)(tmpbuf.get( ));
	// 再次调用 EnumServicesStatusA 应该成功，若不成功，则返回错误
	enumResult = EnumServicesStatusA(scm, SERVICE_WIN32, SERVICE_STATE_ALL, lpServices, cbBufSize, &pcbBytesNeeded, &lpServicesReturned, &lpResumeHandle);
	if (0 == enumResult)
	{
		DEBUG_OUT("EnumServicesStatusA failed");
		return ServiceStatus::FunctionFailed;
	}
	// 遍历，查询服务
	for (DWORD i = lpResumeHandle; i < lpServicesReturned; ++i)
	{
		if (serviceName == lpServices->lpServiceName)
		{
			return static_cast<ServiceStatus>(lpServices->ServiceStatus.dwCurrentState);
		}

		++lpServices;
	}

	return ServiceStatus::NotExist;
}






