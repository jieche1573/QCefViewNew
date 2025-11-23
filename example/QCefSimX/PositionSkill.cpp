#include "PositionSkill.h"
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

std::unique_ptr<PositionSkill> PositionSkill::_instance;
std::mutex PositionSkill::_mutex;

PositionSkill::PositionSkill()
{
    // 初始化数据库表
    initTable();
}

PositionSkill::~PositionSkill()
{
}

bool PositionSkill::initTable()
{
    // 创建岗位多能表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS position_skill (
            id TEXT PRIMARY KEY,
            gw_id TEXT NOT NULL,
            jx_id TEXT NOT NULL,
            nlxs REAL NOT NULL,
            create_time TEXT,
            update_time TEXT,
            UNIQUE(gw_id, jx_id)
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Position skill table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create position skill table";
    }

    return result;
}

QString PositionSkill::generateUniqueId()
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

QString PositionSkill::getPositionName(const QString& gwId)
{
    QString sql = QString("SELECT gw FROM position WHERE id = '%1'").arg(gwId);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        return query.value("gw").toString();
    }
    return "";
}

QString PositionSkill::getAircraftName(const QString& jxId)
{
    QString sql = QString("SELECT jxmc FROM aircraft_model WHERE id = '%1'").arg(jxId);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        return query.value("jxmc").toString();
    }
    return "";
}

QString PositionSkill::addRecord(const QString& jsonStr)
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
    data["gw_id"] = jsonObj["gw_id"].toString();      // 岗位ID
    data["jx_id"] = jsonObj["jx_id"].toString();      // 机型ID
    data["nlxs"] = jsonObj["nlxs"].toDouble();        // 能力系数
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("position_skill", data);

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

bool PositionSkill::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("position_skill", where);
}

bool PositionSkill::updateRecord(const QString& jsonStr)
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

    // 通常只更新能力系数
    if (jsonObj.contains("nlxs")) data["nlxs"] = jsonObj["nlxs"].toDouble();

    // 如果需要更新关联关系，也可以更新这两个字段
    if (jsonObj.contains("gw_id")) data["gw_id"] = jsonObj["gw_id"].toString();
    if (jsonObj.contains("jx_id")) data["jx_id"] = jsonObj["jx_id"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("position_skill", data, where);
}

QString PositionSkill::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM position_skill WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QString gwId = query.value("gw_id").toString();
        QString jxId = query.value("jx_id").toString();

        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            gwId,
            jxId,
            query.value("nlxs").toDouble(),
            getPositionName(gwId),
            getAircraftName(jxId)
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString PositionSkill::queryAllRecords()
{
    QString sql = "SELECT * FROM position_skill ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QString gwId = query.value("gw_id").toString();
        QString jxId = query.value("jx_id").toString();

        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            gwId,
            jxId,
            query.value("nlxs").toDouble(),
            getPositionName(gwId),
            getAircraftName(jxId)
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

QString PositionSkill::queryByPositionId(const QString& gwId)
{
    QString sql = QString("SELECT * FROM position_skill WHERE gw_id = '%1' ORDER BY create_time DESC").arg(gwId);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QString jxId = query.value("jx_id").toString();

        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            gwId,
            jxId,
            query.value("nlxs").toDouble(),
            getPositionName(gwId),
            getAircraftName(jxId)
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

QString PositionSkill::queryByAircraftId(const QString& jxId)
{
    QString sql = QString("SELECT * FROM position_skill WHERE jx_id = '%1' ORDER BY create_time DESC").arg(jxId);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QString gwId = query.value("gw_id").toString();

        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            gwId,
            jxId,
            query.value("nlxs").toDouble(),
            getPositionName(gwId),
            getAircraftName(jxId)
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

QString PositionSkill::batchAddRecords(const QString& jsonStr)
{
    // 解析JSON
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "Invalid JSON format";
        QJsonObject result;
        result["success"] = false;
        result["count"] = 0;
        result["message"] = "无效的JSON格式";
        return QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
    }

    QJsonObject jsonObj = jsonDoc.object();
    QString gwId = jsonObj["gw_id"].toString();
    QJsonArray skills = jsonObj["skills"].toArray();

    int successCount = 0;
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    for (const QJsonValue& skill : skills)
    {
        QJsonObject skillObj = skill.toObject();

        // 生成唯一ID
        QString id = generateUniqueId();

        // 准备数据
        QMap<QString, QVariant> data;
        data["id"] = id;
        data["gw_id"] = gwId;
        data["jx_id"] = skillObj["jx_id"].toString();
        data["nlxs"] = skillObj["nlxs"].toDouble();
        data["create_time"] = currentTime;
        data["update_time"] = currentTime;

        // 插入数据
        bool success = SQLite::Instance().insert("position_skill", data);
        if (success)
        {
            successCount++;
        }
    }

    QJsonObject result;
    result["success"] = (successCount == skills.size());
    result["count"] = successCount;
    result["message"] = QString("成功添加%1条记录").arg(successCount);

    return QString(QJsonDocument(result).toJson(QJsonDocument::Compact));
}

bool PositionSkill::deleteByPositionId(const QString& gwId)
{
    QString where = QString("gw_id = '%1'").arg(gwId);
    return SQLite::Instance().remove("position_skill", where);
}

QJsonObject PositionSkill::recordToJson(const QString& id, const QString& gw_id, const QString& jx_id,
                                        double nlxs, const QString& gw_name, const QString& jx_name)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["gw_id"] = gw_id;         // 岗位ID
    obj["jx_id"] = jx_id;         // 机型ID
    obj["nlxs"] = nlxs;           // 能力系数
    obj["gw_name"] = gw_name;     // 岗位名称
    obj["jx_name"] = jx_name;     // 机型名称
    return obj;
}
