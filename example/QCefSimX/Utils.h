#pragma once

#include <mutex>
#include <qfileinfo.h>
#include <QVariant>

class Utils 
{
	static std::unique_ptr<Utils> instance_;
	static std::mutex mutex_;
public:
	static QString rootPath;
	static Utils& Instance() {
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]() {
			instance_.reset(new Utils);
		});

		return *instance_;
	}
private:
	Utils();

	static void writeConfig();
	void readConfig();
public:

public:
	QString Oracle_HostName;
	int     Oracle_Port{};
	QString Oracle_DatabaseName;
	QString Oracle_UserName;
	QString Oracle_Password;

    QString OracleTask_HostName;
    int OracleTask_Port{};
    QString OracleTask_DatabaseName;
    QString OracleTask_UserName;
    QString OracleTask_Password;

    QString MySQL_HostName;
    int     MySQL_Port{};
    QString MySQL_DatabaseName;
    QString MySQL_UserName;
    QString MySQL_Password;

    QString FTP_HostName;
    int FTP_Port{};
    QString FTP_UserName;
    QString FTP_Password;

    QString Login_BDNM;

};

