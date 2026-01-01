#include "ScenarioModel.h"
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

std::unique_ptr<ScenarioModel> ScenarioModel::_instance;
std::mutex ScenarioModel::_mutex;

ScenarioModel::ScenarioModel()
{
    // 初始化数据库表
    initTable();
}

ScenarioModel::~ScenarioModel()
{
}

bool ScenarioModel::initTable()
{
    // 创建想定管理表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS scenario (
            id TEXT PRIMARY KEY,
            xdmc TEXT NOT NULL,
            jc TEXT,
            zt TEXT,
            xdms TEXT,
            kssj TEXT,
            jssj TEXT,
            ryzddlyl REAL DEFAULT 0.00,
            zbzddlyl REAL DEFAULT 0.00,
            bjzddlyl REAL DEFAULT 0.00,
            rymzl REAL DEFAULT 0.00,
            zbmzl REAL DEFAULT 0.00,
            bjmzl REAL DEFAULT 0.00,
            sfyc TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "Scenario table created/verified successfully";

        // 检查是否需要添加新字段(用于已存在的表)
        QString alterSQL1 = "ALTER TABLE scenario ADD COLUMN ryzddlyl REAL DEFAULT 0.00";
        QString alterSQL2 = "ALTER TABLE scenario ADD COLUMN zbzddlyl REAL DEFAULT 0.00";
        QString alterSQL3 = "ALTER TABLE scenario ADD COLUMN bjzddlyl REAL DEFAULT 0.00";
        QString alterSQL4 = "ALTER TABLE scenario ADD COLUMN rymzl REAL DEFAULT 0.00";
        QString alterSQL5 = "ALTER TABLE scenario ADD COLUMN zbmzl REAL DEFAULT 0.00";
        QString alterSQL6 = "ALTER TABLE scenario ADD COLUMN bjmzl REAL DEFAULT 0.00";

        // 尝试添加新字段(如果字段已存在会失败,但不影响)
        SQLite::Instance().execSQL(alterSQL1);
        SQLite::Instance().execSQL(alterSQL2);
        SQLite::Instance().execSQL(alterSQL3);
        SQLite::Instance().execSQL(alterSQL4);
        SQLite::Instance().execSQL(alterSQL5);
        SQLite::Instance().execSQL(alterSQL6);

        // 添加算法预测字段
        QString alterSQL7 = "ALTER TABLE scenario ADD COLUMN sfyc TEXT";
        SQLite::Instance().execSQL(alterSQL7);
    }
    else
    {
        qDebug() << "Failed to create scenario table";
    }

    return result;
}

QString ScenarioModel::generateUniqueId()
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

QString ScenarioModel::addRecord(const QString& jsonStr)
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
    data["xdmc"] = jsonObj["xdmc"].toString();  // 想定名称
    data["jc"] = jsonObj["jc"].toString();      // 机场
    data["zt"] = jsonObj["zt"].toString();      // 状态
    data["xdms"] = jsonObj["xdms"].toString();  // 想定描述
    data["kssj"] = jsonObj["kssj"].toString();  // 开始时间
    data["jssj"] = jsonObj["jssj"].toString();  // 结束时间
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("scenario", data);

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

bool ScenarioModel::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("scenario", where);
}

bool ScenarioModel::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("xdmc")) data["xdmc"] = jsonObj["xdmc"].toString();
    if (jsonObj.contains("jc")) data["jc"] = jsonObj["jc"].toString();
    if (jsonObj.contains("zt")) data["zt"] = jsonObj["zt"].toString();
    if (jsonObj.contains("xdms")) data["xdms"] = jsonObj["xdms"].toString();
    if (jsonObj.contains("kssj")) data["kssj"] = jsonObj["kssj"].toString();
    if (jsonObj.contains("jssj")) data["jssj"] = jsonObj["jssj"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("scenario", data, where);
}

bool ScenarioModel::updateRateFields(const QString& jsonStr)
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

    if (jsonObj.contains("ryzddlyl")) data["ryzddlyl"] = jsonObj["ryzddlyl"].toDouble();
    if (jsonObj.contains("zbzddlyl")) data["zbzddlyl"] = jsonObj["zbzddlyl"].toDouble();
    if (jsonObj.contains("bjzddlyl")) data["bjzddlyl"] = jsonObj["bjzddlyl"].toDouble();
    if (jsonObj.contains("rymzl")) data["rymzl"] = jsonObj["rymzl"].toDouble();
    if (jsonObj.contains("zbmzl")) data["zbmzl"] = jsonObj["zbmzl"].toDouble();
    if (jsonObj.contains("bjmzl")) data["bjmzl"] = jsonObj["bjmzl"].toDouble();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("scenario", data, where);
}

bool ScenarioModel::updatePredictionField(const QString& jsonStr)
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

    if (jsonObj.contains("sfyc")) data["sfyc"] = jsonObj["sfyc"].toString();

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("scenario", data, where);
}

QString ScenarioModel::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM scenario WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("xdmc").toString(),
            query.value("jc").toString(),
            query.value("zt").toString(),
            query.value("xdms").toString(),
            query.value("kssj").toString(),
            query.value("jssj").toString(),
            query.value("ryzddlyl").toDouble(),
            query.value("zbzddlyl").toDouble(),
            query.value("bjzddlyl").toDouble(),
            query.value("rymzl").toDouble(),
            query.value("zbmzl").toDouble(),
            query.value("bjmzl").toDouble(),
            query.value("sfyc").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString ScenarioModel::queryAllRecords()
{
    QString sql = "SELECT * FROM scenario ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("xdmc").toString(),
            query.value("jc").toString(),
            query.value("zt").toString(),
            query.value("xdms").toString(),
            query.value("kssj").toString(),
            query.value("jssj").toString(),
            query.value("ryzddlyl").toDouble(),
            query.value("zbzddlyl").toDouble(),
            query.value("bjzddlyl").toDouble(),
            query.value("rymzl").toDouble(),
            query.value("zbmzl").toDouble(),
            query.value("bjmzl").toDouble(),
            query.value("sfyc").toString()
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

QString ScenarioModel::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM scenario WHERE xdmc LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("xdmc").toString(),
            query.value("jc").toString(),
            query.value("zt").toString(),
            query.value("xdms").toString(),
            query.value("kssj").toString(),
            query.value("jssj").toString(),
            query.value("ryzddlyl").toDouble(),
            query.value("zbzddlyl").toDouble(),
            query.value("bjzddlyl").toDouble(),
            query.value("rymzl").toDouble(),
            query.value("zbmzl").toDouble(),
            query.value("bjmzl").toDouble(),
            query.value("sfyc").toString()
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

QString ScenarioModel::queryByStatus(const QString& status)
{
    QString sql = QString("SELECT * FROM scenario WHERE zt = '%1' ORDER BY create_time DESC").arg(status);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("xdmc").toString(),
            query.value("jc").toString(),
            query.value("zt").toString(),
            query.value("xdms").toString(),
            query.value("kssj").toString(),
            query.value("jssj").toString(),
            query.value("ryzddlyl").toDouble(),
            query.value("zbzddlyl").toDouble(),
            query.value("bjzddlyl").toDouble(),
            query.value("rymzl").toDouble(),
            query.value("zbmzl").toDouble(),
            query.value("bjmzl").toDouble(),
            query.value("sfyc").toString()
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

QString ScenarioModel::queryByAirport(const QString& airport)
{
    QString sql = QString("SELECT * FROM scenario WHERE jc = '%1' ORDER BY create_time DESC").arg(airport);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("xdmc").toString(),
            query.value("jc").toString(),
            query.value("zt").toString(),
            query.value("xdms").toString(),
            query.value("kssj").toString(),
            query.value("jssj").toString(),
            query.value("ryzddlyl").toDouble(),
            query.value("zbzddlyl").toDouble(),
            query.value("bjzddlyl").toDouble(),
            query.value("rymzl").toDouble(),
            query.value("zbmzl").toDouble(),
            query.value("bjmzl").toDouble(),
            query.value("sfyc").toString()
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

QJsonObject ScenarioModel::recordToJson(const QString& id, const QString& xdmc, const QString& jc,
                                        const QString& zt, const QString& xdms,
                                        const QString& kssj, const QString& jssj,
                                        double ryzddlyl, double zbzddlyl, double bjzddlyl,
                                        double rymzl, double zbmzl, double bjmzl,
                                        const QString& sfyc)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["xdmc"] = xdmc;     // 想定名称
    obj["jc"] = jc;         // 机场
    obj["zt"] = zt;         // 状态
    obj["xdms"] = xdms;     // 想定描述
    obj["kssj"] = kssj;     // 开始时间
    obj["jssj"] = jssj;     // 结束时间
    obj["ryzddlyl"] = ryzddlyl;  // 人员最大利用率
    obj["zbzddlyl"] = zbzddlyl;  // 装备最大利用率
    obj["bjzddlyl"] = bjzddlyl;  // 备件最大利用率
    obj["rymzl"] = rymzl;        // 人员满足率
    obj["zbmzl"] = zbmzl;        // 装备满足率
    obj["bjmzl"] = bjmzl;        // 备件满足率
    obj["sfyc"] = sfyc;          // 算法预测JSON
    return obj;
}
