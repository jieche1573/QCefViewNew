#include "AircraftModel.h"
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

std::unique_ptr<AircraftModel> AircraftModel::_instance;
std::mutex AircraftModel::_mutex;

AircraftModel::AircraftModel()
{
    // 初始化数据库表
    initTable();
}

AircraftModel::~AircraftModel()
{
}

bool AircraftModel::initTable()
{
    // 创建机型管理表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS aircraft_model (
            id TEXT PRIMARY KEY,
            jxmc TEXT NOT NULL,
            ssjz TEXT,
            ndfxsj INTEGER,
            pjgzjg INTEGER,
            dqwxgzsj INTEGER,
            zqgzsj INTEGER,
            hfgzsj INTEGER,
            ddrysj INTEGER,
            ddzb INTEGER,
            ddbj INTEGER,
            xqjs TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Aircraft model table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create aircraft model table";
    }

    return result;
}

QString AircraftModel::generateUniqueId()
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

QString AircraftModel::addRecord(const QString& jsonStr)
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
    data["jxmc"] = jsonObj["jxmc"].toString();  // 机型名称
    data["ssjz"] = jsonObj["ssjz"].toString();  // 所属机种
    data["ndfxsj"] = jsonObj["ndfxsj"].toInt(); // 年度飞行时间
    data["pjgzjg"] = jsonObj["pjgzjg"].toInt(); // 平均故障间隔
    data["dqwxgzsj"] = jsonObj["dqwxgzsj"].toInt(); // 定期维修工作时间节点
    data["zqgzsj"] = jsonObj["zqgzsj"].toInt(); // 周期工作时间节点
    data["hfgzsj"] = jsonObj["hfgzsj"].toInt(); // 换发工作时间节点
    data["ddrysj"] = jsonObj["ddrysj"].toInt();     // 等待人员时间（分钟）
    data["ddzb"] = jsonObj["ddzb"].toInt();         // 等待装备时间（分钟）
    data["ddbj"] = jsonObj["ddbj"].toInt();         // 等待备件时间（分钟）
    data["xqjs"] = jsonObj["xqjs"].toString();      // 详情介绍
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("aircraft_model", data);

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

bool AircraftModel::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("aircraft_model", where);
}

bool AircraftModel::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("jxmc")) data["jxmc"] = jsonObj["jxmc"].toString();
    if (jsonObj.contains("ssjz")) data["ssjz"] = jsonObj["ssjz"].toString();
    if (jsonObj.contains("ndfxsj")) data["ndfxsj"] = jsonObj["ndfxsj"].toInt();
    if (jsonObj.contains("pjgzjg")) data["pjgzjg"] = jsonObj["pjgzjg"].toInt();
    if (jsonObj.contains("dqwxgzsj")) data["dqwxgzsj"] = jsonObj["dqwxgzsj"].toInt();
    if (jsonObj.contains("zqgzsj")) data["zqgzsj"] = jsonObj["zqgzsj"].toInt();
    if (jsonObj.contains("hfgzsj")) data["hfgzsj"] = jsonObj["hfgzsj"].toInt();
    if (jsonObj.contains("ddrysj")) data["ddrysj"] = jsonObj["ddrysj"].toInt();
    if (jsonObj.contains("ddzb")) data["ddzb"] = jsonObj["ddzb"].toInt();
    if (jsonObj.contains("ddbj")) data["ddbj"] = jsonObj["ddbj"].toInt();
    if (jsonObj.contains("xqjs")) data["xqjs"] = jsonObj["xqjs"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("aircraft_model", data, where);
}

QString AircraftModel::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM aircraft_model WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("jxmc").toString(),
            query.value("ssjz").toString(),
            query.value("ndfxsj").toInt(),
            query.value("pjgzjg").toInt(),
            query.value("dqwxgzsj").toInt(),
            query.value("zqgzsj").toInt(),
            query.value("hfgzsj").toInt(),
            query.value("ddrysj").toInt(),
            query.value("ddzb").toInt(),
            query.value("ddbj").toInt(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString AircraftModel::queryAllRecords()
{
    QString sql = "SELECT * FROM aircraft_model ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("jxmc").toString(),
            query.value("ssjz").toString(),
            query.value("ndfxsj").toInt(),
            query.value("pjgzjg").toInt(),
            query.value("dqwxgzsj").toInt(),
            query.value("zqgzsj").toInt(),
            query.value("hfgzsj").toInt(),
            query.value("ddrysj").toInt(),
            query.value("ddzb").toInt(),
            query.value("ddbj").toInt(),
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
    QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
    auto t1 = QString::fromStdString(jsonString.toStdString());
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString AircraftModel::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM aircraft_model WHERE jxmc LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("jxmc").toString(),
            query.value("ssjz").toString(),
            query.value("ndfxsj").toInt(),
            query.value("pjgzjg").toInt(),
            query.value("dqwxgzsj").toInt(),
            query.value("zqgzsj").toInt(),
            query.value("hfgzsj").toInt(),
            query.value("ddrysj").toInt(),
            query.value("ddzb").toInt(),
            query.value("ddbj").toInt(),
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

QString AircraftModel::queryByType(const QString& type)
{
    QString sql = QString("SELECT * FROM aircraft_model WHERE ssjz = '%1' ORDER BY create_time DESC").arg(type);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("jxmc").toString(),
            query.value("ssjz").toString(),
            query.value("ndfxsj").toInt(),
            query.value("pjgzjg").toInt(),
            query.value("dqwxgzsj").toInt(),
            query.value("zqgzsj").toInt(),
            query.value("hfgzsj").toInt(),
            query.value("ddrysj").toInt(),
            query.value("ddzb").toInt(),
            query.value("ddbj").toInt(),
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

QJsonObject AircraftModel::recordToJson(const QString& id, const QString& jxmc, const QString& ssjz,
                                        int ndfxsj, int pjgzjg, int dqwxgzsj,
                                        int zqgzsj, int hfgzsj, int ddrysj, int ddzb, int ddbj,
                                        const QString& xqjs)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["jxmc"] = jxmc;         // 机型名称
    obj["ssjz"] = ssjz;         // 所属机种
    obj["ndfxsj"] = ndfxsj;     // 年度飞行时间
    obj["pjgzjg"] = pjgzjg;     // 平均故障间隔
    obj["dqwxgzsj"] = dqwxgzsj; // 定期维修工作时间节点
    obj["zqgzsj"] = zqgzsj;     // 周期工作时间节点
    obj["hfgzsj"] = hfgzsj;     // 换发工作时间节点
    obj["ddrysj"] = ddrysj;     // 等待人员时间（分钟）
    obj["ddzb"] = ddzb;         // 等待装备时间（分钟）
    obj["ddbj"] = ddbj;         // 等待备件时间（分钟）
    obj["xqjs"] = xqjs;         // 详情介绍
    return obj;
}
