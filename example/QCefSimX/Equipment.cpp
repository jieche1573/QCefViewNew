#include "Equipment.h"
#include "sqlite.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUuid>
#include <QDebug>
#include <QVariant>

std::unique_ptr<Equipment> Equipment::_instance;
std::mutex Equipment::_mutex;

Equipment::Equipment()
{
    // 初始化数据库表
    initTable();
}

Equipment::~Equipment()
{
}

bool Equipment::initTable()
{
    // 创建装备管理表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS equipment (
            id TEXT PRIMARY KEY,
            sbid TEXT NOT NULL,
            sbmc TEXT NOT NULL,
            lx TEXT,
            sfzy TEXT,
            zyjx TEXT,
            xqjs TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Equipment table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create equipment table";
    }

    return result;
}

QString Equipment::generateUniqueId()
{
    // 生成时间字符串 精确到微秒
    QDateTime now = QDateTime::currentDateTime();
    QString timeStr = now.toString("yyyyMMddHHmmsszzz");

    // 获取微秒部分（使用 currentMSecsSinceEpoch 的后3位模拟微秒）
    qint64 msec = now.toMSecsSinceEpoch();
    QString microSec = QString::number(msec % 1000).rightJustified(3, '0');

    // 生成UUID（去掉大括号和横杠）
    QString uuid = QUuid::createUuid().toString().remove("{").remove("}").remove("-").left(6);

    // 组合：202511082323123456 + uuid前6位
    return timeStr + microSec + uuid;
}

QString Equipment::addRecord(const QString& jsonStr)
{
    // 解析JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Invalid JSON format";
        return "";
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 生成唯一ID
    QString id = generateUniqueId();
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QString sfzy = jsonObj["sfzy"].toString();
    QString zyjx = jsonObj["zyjx"].toString();

    // 如果是通用设备，清空专用机型字段
    if (sfzy == "通用设备")
    {
        zyjx = "";
    }

    // 准备数据
    QMap<QString, QVariant> data;
    data["id"] = id;
    data["sbid"] = jsonObj["sbid"].toString();  // 设备ID
    data["sbmc"] = jsonObj["sbmc"].toString();  // 设备名称
    data["lx"] = jsonObj["lx"].toString();      // 类型
    data["sfzy"] = sfzy;                        // 是否专用
    data["zyjx"] = zyjx;                        // 专用机型ID
    data["xqjs"] = jsonObj["xqjs"].toString();  // 详情介绍
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("equipment", data);

    if (success)
    {
        // 返回新创建的记录JSON
        return queryRecordById(id);
    }
    else
    {
        return "";
    }
}

bool Equipment::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("equipment", where);
}

bool Equipment::updateRecord(const QString& jsonStr)
{
    // 解析JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Invalid JSON format";
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();

    // 必须包含id字段
    if (!jsonObj.contains("id"))
    {
        qDebug() << "Missing id field";
        return false;
    }

    QString id = jsonObj["id"].toString();
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // 准备更新数据
    QMap<QString, QVariant> data;

    if (jsonObj.contains("sbid")) data["sbid"] = jsonObj["sbid"].toString();
    if (jsonObj.contains("sbmc")) data["sbmc"] = jsonObj["sbmc"].toString();
    if (jsonObj.contains("lx")) data["lx"] = jsonObj["lx"].toString();

    // 处理专用/通用设备逻辑
    if (jsonObj.contains("sfzy"))
    {
        QString sfzy = jsonObj["sfzy"].toString();
        data["sfzy"] = sfzy;

        // 如果更新为通用设备，清空专用机型字段
        if (sfzy == "通用设备")
        {
            data["zyjx"] = "";
        }
    }

    // 如果sfzy是专用设备且提供了zyjx，才更新zyjx
    if (jsonObj.contains("zyjx"))
    {
        QString sfzy = jsonObj.value("sfzy").toString();
        // 只有当sfzy为专用设备或没有提供sfzy时才更新zyjx
        if (sfzy != "通用设备")
        {
            data["zyjx"] = jsonObj["zyjx"].toString();
        }
    }

    if (jsonObj.contains("xqjs")) data["xqjs"] = jsonObj["xqjs"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("equipment", data, where);
}

QString Equipment::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM equipment WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("sbid").toString(),
            query.value("sbmc").toString(),
            query.value("lx").toString(),
            query.value("sfzy").toString(),
            query.value("zyjx").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString Equipment::queryAllRecords()
{
    QString sql = "SELECT * FROM equipment ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("sbid").toString(),
            query.value("sbmc").toString(),
            query.value("lx").toString(),
            query.value("sfzy").toString(),
            query.value("zyjx").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        array.append(obj);
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString Equipment::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM equipment WHERE sbmc LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("sbid").toString(),
            query.value("sbmc").toString(),
            query.value("lx").toString(),
            query.value("sfzy").toString(),
            query.value("zyjx").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        array.append(obj);
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString Equipment::queryByType(const QString& type)
{
    QString sql = QString("SELECT * FROM equipment WHERE lx = '%1' ORDER BY create_time DESC").arg(type);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("sbid").toString(),
            query.value("sbmc").toString(),
            query.value("lx").toString(),
            query.value("sfzy").toString(),
            query.value("zyjx").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        array.append(obj);
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString Equipment::queryByDedicated(const QString& dedicated)
{
    QString sql = QString("SELECT * FROM equipment WHERE sfzy = '%1' ORDER BY create_time DESC").arg(dedicated);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("sbid").toString(),
            query.value("sbmc").toString(),
            query.value("lx").toString(),
            query.value("sfzy").toString(),
            query.value("zyjx").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        array.append(obj);
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonObject Equipment::recordToJson(const QString& id, const QString& sbid, const QString& sbmc,
                                    const QString& lx, const QString& sfzy, const QString& zyjx,
                                    const QString& xqjs)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["sbid"] = sbid;     // 设备ID
    obj["sbmc"] = sbmc;     // 设备名称
    obj["lx"] = lx;         // 类型
    obj["sfzy"] = sfzy;     // 是否专用
    obj["zyjx"] = zyjx;     // 专用机型ID
    obj["xqjs"] = xqjs;     // 详情介绍
    return obj;
}
