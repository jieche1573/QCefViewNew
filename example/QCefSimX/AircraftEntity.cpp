#include "AircraftEntity.h"
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

std::unique_ptr<AircraftEntity> AircraftEntity::_instance;
std::mutex AircraftEntity::_mutex;

AircraftEntity::AircraftEntity()
{
    // 初始化数据库表
    initTable();
}

AircraftEntity::~AircraftEntity()
{
}

bool AircraftEntity::initTable()
{
    // 创建飞机实体表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS aircraft_entity (
            id TEXT PRIMARY KEY,
            fjbh TEXT NOT NULL,
            xdid TEXT NOT NULL,
            jxid TEXT NOT NULL,
            bzdj TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Aircraft entity table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create aircraft entity table";
    }

    return result;
}

QString AircraftEntity::generateUniqueId()
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

QString AircraftEntity::addRecord(const QString& jsonStr)
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

    // 准备数据
    QMap<QString, QVariant> data;
    data["id"] = id;
    data["fjbh"] = jsonObj["fjbh"].toString();  // 飞机编号
    data["xdid"] = jsonObj["xdid"].toString();  // 想定ID
    data["jxid"] = jsonObj["jxid"].toString();  // 机型ID
    data["bzdj"] = jsonObj["bzdj"].toString();  // 保障等级
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("aircraft_entity", data);

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

bool AircraftEntity::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("aircraft_entity", where);
}

bool AircraftEntity::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("fjbh")) data["fjbh"] = jsonObj["fjbh"].toString();
    if (jsonObj.contains("xdid")) data["xdid"] = jsonObj["xdid"].toString();
    if (jsonObj.contains("jxid")) data["jxid"] = jsonObj["jxid"].toString();
    if (jsonObj.contains("bzdj")) data["bzdj"] = jsonObj["bzdj"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("aircraft_entity", data, where);
}

QString AircraftEntity::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM aircraft_entity WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString AircraftEntity::queryAllRecords()
{
    QString sql = "SELECT * FROM aircraft_entity ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

QString AircraftEntity::queryByCode(const QString& code)
{
    QString sql = QString("SELECT * FROM aircraft_entity WHERE fjbh LIKE '%%1%' ORDER BY create_time DESC").arg(code);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

QString AircraftEntity::queryByScenarioId(const QString& xdid)
{
    QString sql = QString("SELECT * FROM aircraft_entity WHERE xdid = '%1' ORDER BY create_time DESC").arg(xdid);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

QString AircraftEntity::queryByModelId(const QString& jxid)
{
    QString sql = QString("SELECT * FROM aircraft_entity WHERE jxid = '%1' ORDER BY create_time DESC").arg(jxid);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

QString AircraftEntity::queryByLevel(const QString& bzdj)
{
    QString sql = QString("SELECT * FROM aircraft_entity WHERE bzdj = '%1' ORDER BY create_time DESC").arg(bzdj);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

QString AircraftEntity::queryByScenarioAndModel(const QString& xdid, const QString& jxid)
{
    QString sql;

    // 如果机型ID为空或为"all"，则查询该想定下所有飞机
    if (jxid.isEmpty() || jxid.toLower() == "all")
    {
        sql = QString("SELECT * FROM aircraft_entity WHERE xdid = '%1' ORDER BY create_time DESC").arg(xdid);
    }
    else
    {
        // 否则查询指定想定和机型的飞机
        sql = QString("SELECT * FROM aircraft_entity WHERE xdid = '%1' AND jxid = '%2' ORDER BY create_time DESC")
                  .arg(xdid).arg(jxid);
    }

    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjbh").toString(),
            query.value("xdid").toString(),
            query.value("jxid").toString(),
            query.value("bzdj").toString()
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

bool AircraftEntity::deleteByScenarioId(const QString& xdid)
{
    QString where = QString("xdid = '%1'").arg(xdid);
    return SQLite::Instance().remove("aircraft_entity", where);
}

QJsonObject AircraftEntity::recordToJson(const QString& id, const QString& fjbh, const QString& xdid,
                                        const QString& jxid, const QString& bzdj)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["fjbh"] = fjbh;     // 飞机编号
    obj["xdid"] = xdid;     // 想定ID
    obj["jxid"] = jxid;     // 机型ID
    obj["bzdj"] = bzdj;     // 保障等级
    return obj;
}
