#include "Position.h"
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

std::unique_ptr<Position> Position::_instance;
std::mutex Position::_mutex;

Position::Position()
{
    // 初始化数据库表
    initTable();
}

Position::~Position()
{
}

bool Position::initTable()
{
    // 创建岗位管理表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS position (
            id TEXT PRIMARY KEY,
            zymc TEXT NOT NULL,
            gwbh TEXT NOT NULL,
            syyx TEXT,
            gw TEXT,
            nlxs INTEGER,
            gzxl REAL,
            dnsz TEXT,
            xqjs TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Position table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create position table";
    }

    return result;
}

QString Position::generateUniqueId()
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

QString Position::addRecord(const QString& jsonStr)
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
    data["zymc"] = jsonObj["zymc"].toString();  // 专业名称
    data["gwbh"] = jsonObj["gwbh"].toString();  // 岗位编号
    data["syyx"] = jsonObj["syyx"].toString();  // 适用机型
    data["gw"] = jsonObj["gw"].toString();      // 岗位
    data["nlxs"] = jsonObj["nlxs"].toInt();     // 能力系数
    data["gzxl"] = jsonObj["gzxl"].toDouble();  // 工作效率
    data["dnsz"] = jsonObj["dnsz"].toString();  // 多能设置
    data["xqjs"] = jsonObj["xqjs"].toString();  // 详情介绍
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("position", data);

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

bool Position::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("position", where);
}

bool Position::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("zymc")) data["zymc"] = jsonObj["zymc"].toString();
    if (jsonObj.contains("gwbh")) data["gwbh"] = jsonObj["gwbh"].toString();
    if (jsonObj.contains("syyx")) data["syyx"] = jsonObj["syyx"].toString();
    if (jsonObj.contains("gw")) data["gw"] = jsonObj["gw"].toString();
    if (jsonObj.contains("nlxs")) data["nlxs"] = jsonObj["nlxs"].toInt();
    if (jsonObj.contains("gzxl")) data["gzxl"] = jsonObj["gzxl"].toDouble();
    if (jsonObj.contains("dnsz")) data["dnsz"] = jsonObj["dnsz"].toString();
    if (jsonObj.contains("xqjs")) data["xqjs"] = jsonObj["xqjs"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("position", data, where);
}

QString Position::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM position WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("zymc").toString(),
            query.value("gwbh").toString(),
            query.value("syyx").toString(),
            query.value("gw").toString(),
            query.value("nlxs").toInt(),
            query.value("gzxl").toDouble(),
            query.value("dnsz").toString(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString Position::queryAllRecords()
{
    QString sql = "SELECT * FROM position ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("zymc").toString(),
            query.value("gwbh").toString(),
            query.value("syyx").toString(),
            query.value("gw").toString(),
            query.value("nlxs").toInt(),
            query.value("gzxl").toDouble(),
            query.value("dnsz").toString(),
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

QString Position::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM position WHERE zymc LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("zymc").toString(),
            query.value("gwbh").toString(),
            query.value("syyx").toString(),
            query.value("gw").toString(),
            query.value("nlxs").toInt(),
            query.value("gzxl").toDouble(),
            query.value("dnsz").toString(),
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

QString Position::queryByPosition(const QString& position)
{
    QString sql = QString("SELECT * FROM position WHERE gw = '%1' ORDER BY create_time DESC").arg(position);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("zymc").toString(),
            query.value("gwbh").toString(),
            query.value("syyx").toString(),
            query.value("gw").toString(),
            query.value("nlxs").toInt(),
            query.value("gzxl").toDouble(),
            query.value("dnsz").toString(),
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

QString Position::queryByModel(const QString& model)
{
    QString sql = QString("SELECT * FROM position WHERE syyx = '%1' ORDER BY create_time DESC").arg(model);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("zymc").toString(),
            query.value("gwbh").toString(),
            query.value("syyx").toString(),
            query.value("gw").toString(),
            query.value("nlxs").toInt(),
            query.value("gzxl").toDouble(),
            query.value("dnsz").toString(),
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

QJsonObject Position::recordToJson(const QString& id, const QString& zymc, const QString& gwbh,
                                   const QString& syyx, const QString& gw, int nlxs,
                                   double gzxl, const QString& dnsz, const QString& xqjs)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["zymc"] = zymc;     // 专业名称
    obj["gwbh"] = gwbh;     // 岗位编号
    obj["syyx"] = syyx;     // 适用机型
    obj["gw"] = gw;         // 岗位
    obj["nlxs"] = nlxs;     // 能力系数
    obj["gzxl"] = gzxl;     // 工作效率
    obj["dnsz"] = dnsz;     // 多能设置
    obj["xqjs"] = xqjs;     // 详情介绍
    return obj;
}
