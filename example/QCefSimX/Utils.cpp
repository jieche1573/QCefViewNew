#include "Utils.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

const char *const DataBaseINI = "DatabaseConfig.ini";
/**
 * \brief 
 */
std::unique_ptr<Utils> Utils::instance_;
/**
 * \brief 
 */
std::mutex Utils::mutex_;

Utils::Utils()
{
#ifdef NDEBUG
  if (!QFileInfo::exists(DataBaseINI))
#endif // NDEBUG
      {
	writeConfig();
      }
    readConfig();
}

void Utils::writeConfig()
{
    //创建ini配置文件
    QSettings iniFile(DataBaseINI, QSettings::IniFormat);
    //Oracle组开始
    iniFile.beginGroup(("Oracle"));
	//设置键值对
    iniFile.setValue("HostName", ("4.160.5.252"));
    iniFile.setValue("Port", ("1521"));
    iniFile.setValue("DatabaseName", ("ORCL"));
    iniFile.setValue("UserName", ("system"));
    iniFile.setValue("Password", ("orcl"));
    iniFile.endGroup();


    iniFile.beginGroup(("OracleTask"));
    //设置键值对
    iniFile.setValue("HostName", ("4.160.5.252"));
    iniFile.setValue("Port", ("1521"));
    iniFile.setValue("DatabaseName", ("ORCL"));
    iniFile.setValue("UserName", ("system"));
    iniFile.setValue("Password", ("orcl"));
    iniFile.endGroup();

    // MySQL组开始
    iniFile.beginGroup(("MySQL"));
    //设置键值对
    iniFile.setValue("HostName", ("4.160.5.252"));
    iniFile.setValue("Port", ("3306"));
    iniFile.setValue("DatabaseName", ("xjzb"));
    iniFile.setValue("UserName", ("novon"));
    iniFile.setValue("Password", ("123456"));
    iniFile.endGroup();

    // FTPServer组开始
    iniFile.beginGroup(("FTPServer"));
    //设置键值对
    iniFile.setValue("HostName", ("4.160.5.103"));
    iniFile.setValue("Port", ("73"));
    iniFile.setValue("UserName", ("admin"));
    iniFile.setValue("Password", ("123456"));
    iniFile.endGroup();
}

void Utils::readConfig()
{
	//读取键值对
    QSettings iniFile(DataBaseINI, QSettings::IniFormat);

    Oracle_HostName = iniFile.value("Oracle/HostName").toString();
    Oracle_Port = iniFile.value("Oracle/Port").toInt();
    Oracle_DatabaseName = iniFile.value("Oracle/DatabaseName").toString();
    Oracle_UserName = iniFile.value("Oracle/UserName").toString();
    Oracle_Password = iniFile.value("Oracle/Password").toString();

    OracleTask_HostName = iniFile.value("OracleTask/HostName").toString();
    OracleTask_Port = iniFile.value("OracleTask/Port").toInt();
    OracleTask_DatabaseName = iniFile.value("OracleTask/DatabaseName").toString();
    OracleTask_UserName = iniFile.value("OracleTask/UserName").toString();
    OracleTask_Password = iniFile.value("OracleTask/Password").toString();

    MySQL_HostName = iniFile.value("MySQL/HostName").toString();
    MySQL_Port = iniFile.value("MySQL/Port").toInt();
    MySQL_DatabaseName = iniFile.value("MySQL/DatabaseName").toString();
    MySQL_UserName = iniFile.value("MySQL/UserName").toString();
    MySQL_Password = iniFile.value("MySQL/Password").toString();

    FTP_HostName = iniFile.value("FTPServer/HostName").toString();
    FTP_Port = iniFile.value("FTPServer/Port").toInt();
    FTP_UserName = iniFile.value("FTPServer/UserName").toString();
    FTP_Password = iniFile.value("FTPServer/Password").toString();
}
