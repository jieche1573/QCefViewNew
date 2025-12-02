#include "SparePart.h"
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

std::unique_ptr<SparePart> SparePart::_instance;
std::mutex SparePart::_mutex;

SparePart::SparePart()
{
    // 初始化数据库表
    initTable();
}

SparePart::~SparePart()
{
}

bool SparePart::initTable()
{
    // 创建备件管理表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS spare_part (
            id TEXT PRIMARY KEY,
            bjmc TEXT NOT NULL,
            bjbh TEXT NOT NULL,
            syyx TEXT,
            gzl REAL,
            pjxfsj INTEGER,
            pjghsj INTEGER,
            xqjs TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Spare part table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create spare part table";
    }

    return result;
}

QString SparePart::generateUniqueId()
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

QString SparePart::addRecord(const QString& jsonStr)
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
    data["bjmc"] = jsonObj["bjmc"].toString();       // 备件名称
    data["bjbh"] = jsonObj["bjbh"].toString();       // 备件编号
    data["syyx"] = jsonObj["syyx"].toString();       // 适用机型
    data["gzl"] = jsonObj["gzl"].toDouble();         // 故障率
    data["pjxfsj"] = jsonObj["pjxfsj"].toInt();      // 平均修复时间
    data["pjghsj"] = jsonObj["pjghsj"].toInt();      // 平均更换时间
    data["xqjs"] = jsonObj["xqjs"].toString();       // 详情介绍
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("spare_part", data);

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

bool SparePart::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("spare_part", where);
}

bool SparePart::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("bjmc")) data["bjmc"] = jsonObj["bjmc"].toString();
    if (jsonObj.contains("bjbh")) data["bjbh"] = jsonObj["bjbh"].toString();
    if (jsonObj.contains("syyx")) data["syyx"] = jsonObj["syyx"].toString();
    if (jsonObj.contains("gzl")) data["gzl"] = jsonObj["gzl"].toDouble();
    if (jsonObj.contains("pjxfsj")) data["pjxfsj"] = jsonObj["pjxfsj"].toInt();
    if (jsonObj.contains("pjghsj")) data["pjghsj"] = jsonObj["pjghsj"].toInt();
    if (jsonObj.contains("xqjs")) data["xqjs"] = jsonObj["xqjs"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("spare_part", data, where);
}

QString SparePart::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM spare_part WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("bjmc").toString(),
            query.value("bjbh").toString(),
            query.value("syyx").toString(),
            query.value("gzl").toDouble(),
            query.value("pjxfsj").toInt(),
            query.value("pjghsj").toInt(),
            query.value("xqjs").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString SparePart::queryAllRecords()
{
    QString sql = "SELECT * FROM spare_part ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("bjmc").toString(),
            query.value("bjbh").toString(),
            query.value("syyx").toString(),
            query.value("gzl").toDouble(),
            query.value("pjxfsj").toInt(),
            query.value("pjghsj").toInt(),
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

QString SparePart::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM spare_part WHERE bjmc LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("bjmc").toString(),
            query.value("bjbh").toString(),
            query.value("syyx").toString(),
            query.value("gzl").toDouble(),
            query.value("pjxfsj").toInt(),
            query.value("pjghsj").toInt(),
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

QString SparePart::queryByModel(const QString& model)
{
    QString sql = QString("SELECT * FROM spare_part WHERE syyx = '%1' ORDER BY create_time DESC").arg(model);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("bjmc").toString(),
            query.value("bjbh").toString(),
            query.value("syyx").toString(),
            query.value("gzl").toDouble(),
            query.value("pjxfsj").toInt(),
            query.value("pjghsj").toInt(),
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

QJsonObject SparePart::recordToJson(const QString& id, const QString& bjmc, const QString& bjbh,
                                    const QString& syyx, double gzl, int pjxfsj,
                                    int pjghsj, const QString& xqjs)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["bjmc"] = bjmc;         // 备件名称
    obj["bjbh"] = bjbh;         // 备件编号
    obj["syyx"] = syyx;         // 适用机型
    obj["gzl"] = gzl;           // 故障率
    obj["pjxfsj"] = pjxfsj;     // 平均修复时间
    obj["pjghsj"] = pjghsj;     // 平均更换时间
    obj["xqjs"] = xqjs;         // 详情介绍
    return obj;
}
