#pragma once

#include <string>

class EServiceFrame
{
public:
	enum class ErrorCode : unsigned int { Succeed, Failed, NotExist, AllReadyExist, ArgumentError };

	enum class ServiceStatus : unsigned int
	{ 
		NotExist, Stopped, StartPending, StopPending, Running, 
		ContinuePending, PausePending, Paused, FunctionFailed 
	};

	EServiceFrame(void);

	virtual ~EServiceFrame(void) noexcept;

	static ErrorCode InstallLocalService(const std::string& serviceName, const std::string& serviceDisplayName, const std::string& exeFileFullPathName);

	static ErrorCode UninstallLocalService(const std::string& serviceName);

	static ErrorCode StartLocalService(const std::string& serviceName);

	static ErrorCode StopLocalService(const std::string& serviceName);

	static ErrorCode ReinstallLocalService(const std::string& serviceName, const std::string& serviceDisplayName, const std::string& exeFileFullPathName);

	static ErrorCode RestartLocalService(const std::string& serviceName);

	static ServiceStatus CheckServiceStatus(const std::string& serviceName);
};

