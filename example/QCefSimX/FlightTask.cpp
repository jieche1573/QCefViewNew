#include "FlightTask.h"
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

std::unique_ptr<FlightTask> FlightTask::_instance;
std::mutex FlightTask::_mutex;

FlightTask::FlightTask()
{
    // 初始化数据库表
    initTable();
}

FlightTask::~FlightTask()
{
}

bool FlightTask::initTable()
{
    // 创建飞行任务表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS flight_task (
            id TEXT PRIMARY KEY,
            fjstid TEXT NOT NULL,
            qfsj TEXT NOT NULL,
            jlsj TEXT NOT NULL,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Flight task table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create flight task table";
    }

    return result;
}

QString FlightTask::generateUniqueId()
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

QString FlightTask::addRecord(const QString& jsonStr)
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
    data["fjstid"] = jsonObj["fjstid"].toString();  // 飞机实体ID
    data["qfsj"] = jsonObj["qfsj"].toString();      // 起飞时间
    data["jlsj"] = jsonObj["jlsj"].toString();      // 降落时间
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("flight_task", data);

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

bool FlightTask::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("flight_task", where);
}

bool FlightTask::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("fjstid")) data["fjstid"] = jsonObj["fjstid"].toString();
    if (jsonObj.contains("qfsj")) data["qfsj"] = jsonObj["qfsj"].toString();
    if (jsonObj.contains("jlsj")) data["jlsj"] = jsonObj["jlsj"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("flight_task", data, where);
}

QString FlightTask::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM flight_task WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjstid").toString(),
            query.value("qfsj").toString(),
            query.value("jlsj").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString FlightTask::queryAllRecords()
{
    QString sql = "SELECT * FROM flight_task ORDER BY qfsj ASC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjstid").toString(),
            query.value("qfsj").toString(),
            query.value("jlsj").toString()
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

QString FlightTask::queryByAircraftEntityId(const QString& fjstid)
{
    QString sql = QString("SELECT * FROM flight_task WHERE fjstid = '%1' ORDER BY qfsj ASC").arg(fjstid);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjstid").toString(),
            query.value("qfsj").toString(),
            query.value("jlsj").toString()
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

QString FlightTask::queryByTimeRange(const QString& startTime, const QString& endTime)
{
    QString sql = QString("SELECT * FROM flight_task WHERE qfsj >= '%1' AND qfsj <= '%2' ORDER BY qfsj ASC")
                      .arg(startTime).arg(endTime);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("fjstid").toString(),
            query.value("qfsj").toString(),
            query.value("jlsj").toString()
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

bool FlightTask::deleteByAircraftEntityId(const QString& fjstid)
{
    QString where = QString("fjstid = '%1'").arg(fjstid);
    return SQLite::Instance().remove("flight_task", where);
}

QJsonObject FlightTask::recordToJson(const QString& id, const QString& fjstid,
                                     const QString& qfsj, const QString& jlsj)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["fjstid"] = fjstid;     // 飞机实体ID
    obj["qfsj"] = qfsj;         // 起飞时间
    obj["jlsj"] = jlsj;         // 降落时间
    return obj;
}
