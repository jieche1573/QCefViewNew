#include "ProcessT.h"
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

std::unique_ptr<ProcessT> ProcessT::_instance;
std::mutex ProcessT::_mutex;

ProcessT::ProcessT()
{
    // 初始化数据库表
    initTable();
}

ProcessT::~ProcessT()
{
}

bool ProcessT::initTable()
{
    // 创建流程图表
    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS ProcessT_flow (
            id TEXT PRIMARY KEY,
            model_id TEXT NOT NULL,
            name TEXT NOT NULL,
            implement_node TEXT,
            level TEXT,
            graph_data TEXT,
            create_time TEXT,
            update_time TEXT
        )
    )";

    bool result = SQLite::Instance().execSQL(createTableSQL);

    if (result)
    {
        qDebug() << "ProcessT flow table created/verified successfully";
    }
    else
    {
        qDebug() << "Failed to create ProcessT flow table";
    }

    return result;
}

QString ProcessT::generateUniqueId()
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

QString ProcessT::getModelName(const QString& modelId)
{
    QString sql = QString("SELECT jxmc FROM aircraft_model WHERE id = '%1'").arg(modelId);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        return query.value("jxmc").toString();
    }
    return "";
}

QString ProcessT::addRecord(const QString& jsonStr)
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

    // 提取graph_data并转换为字符串
    QJsonObject graphData = jsonObj["graph_data"].toObject();
    QString graphDataStr = QString(QJsonDocument(graphData).toJson(QJsonDocument::Compact));

    // 准备数据
    QMap<QString, QVariant> data;
    data["id"] = id;
    data["model_id"] = jsonObj["model_id"].toString();
    data["name"] = jsonObj["name"].toString();
    data["implement_node"] = jsonObj["implement_node"].toString();
    data["level"] = jsonObj["level"].toString();
    data["graph_data"] = graphDataStr;
    data["create_time"] = currentTime;
    data["update_time"] = currentTime;

    // 插入数据
    bool success = SQLite::Instance().insert("ProcessT_flow", data);

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

bool ProcessT::deleteRecord(const QString& id)
{
    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().remove("ProcessT_flow", where);
}

bool ProcessT::updateRecord(const QString& jsonStr)
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

    if (jsonObj.contains("model_id")) data["model_id"] = jsonObj["model_id"].toString();
    if (jsonObj.contains("name")) data["name"] = jsonObj["name"].toString();
    if (jsonObj.contains("implement_node")) data["implement_node"] = jsonObj["implement_node"].toString();
    if (jsonObj.contains("level")) data["level"] = jsonObj["level"].toString();

    if (jsonObj.contains("graph_data"))
    {
        QJsonObject graphData = jsonObj["graph_data"].toObject();
        QString graphDataStr = QString(QJsonDocument(graphData).toJson(QJsonDocument::Compact));
        data["graph_data"] = graphDataStr;
    }

    data["update_time"] = currentTime;

    QString where = QString("id = '%1'").arg(id);
    return SQLite::Instance().update("ProcessT_flow", data, where);
}

QString ProcessT::queryRecordById(const QString& id)
{
    QString sql = QString("SELECT * FROM ProcessT_flow WHERE id = '%1'").arg(id);
    QSqlQuery query = SQLite::Instance().select(sql);

    if (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
        );

        obj["create_time"] = query.value("create_time").toString();
        obj["update_time"] = query.value("update_time").toString();

        QJsonDocument doc(obj);
        return QString(doc.toJson(QJsonDocument::Compact));
    }

    return "";
}

QString ProcessT::queryAllRecords()
{
    QString sql = "SELECT * FROM ProcessT_flow ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
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

QString ProcessT::queryByName(const QString& name)
{
    QString sql = QString("SELECT * FROM ProcessT_flow WHERE name LIKE '%%1%' ORDER BY create_time DESC").arg(name);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
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

QString ProcessT::queryByModelId(const QString& modelId)
{
    QString sql = QString("SELECT * FROM ProcessT_flow WHERE model_id = '%1' ORDER BY create_time DESC").arg(modelId);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
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

QString ProcessT::queryByNode(const QString& node)
{
    QString sql = QString("SELECT * FROM ProcessT_flow WHERE implement_node = '%1' ORDER BY create_time DESC").arg(node);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
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

QString ProcessT::queryByLevel(const QString& level)
{
    QString sql = QString("SELECT * FROM ProcessT_flow WHERE level = '%1' ORDER BY create_time DESC").arg(level);
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QJsonObject obj = recordToJson(
            query.value("id").toString(),
            query.value("model_id").toString(),
            query.value("name").toString(),
            query.value("implement_node").toString(),
            query.value("level").toString(),
            query.value("graph_data").toString()
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

QString ProcessT::queryByPersonId(const QString& personId)
{
    QString sql = "SELECT * FROM ProcessT_flow ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QString graphDataStr = query.value("graph_data").toString();

        // 解析graph_data查找是否包含指定personId
        QJsonDocument graphDoc = QJsonDocument::fromJson(graphDataStr.toUtf8());
        if (!graphDoc.isNull() && graphDoc.isObject())
        {
            QJsonObject graphObj = graphDoc.object();
            QJsonArray nodes = graphObj["nodes"].toArray();

            bool found = false;
            for (const QJsonValue& nodeVal : nodes)
            {
                QJsonObject node = nodeVal.toObject();
                if (node.contains("personId") && node["personId"].toString() == personId)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                QJsonObject obj = recordToJson(
                    query.value("id").toString(),
                    query.value("model_id").toString(),
                    query.value("name").toString(),
                    query.value("implement_node").toString(),
                    query.value("level").toString(),
                    graphDataStr
                );

                obj["create_time"] = query.value("create_time").toString();
                obj["update_time"] = query.value("update_time").toString();

                array.append(obj);
            }
        }
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QString ProcessT::queryByCarId(const QString& carId)
{
    QString sql = "SELECT * FROM ProcessT_flow ORDER BY create_time DESC";
    QSqlQuery query = SQLite::Instance().select(sql);

    QJsonArray array;
    while (query.next())
    {
        QString graphDataStr = query.value("graph_data").toString();

        // 解析graph_data查找是否包含指定carId
        QJsonDocument graphDoc = QJsonDocument::fromJson(graphDataStr.toUtf8());
        if (!graphDoc.isNull() && graphDoc.isObject())
        {
            QJsonObject graphObj = graphDoc.object();
            QJsonArray nodes = graphObj["nodes"].toArray();

            bool found = false;
            for (const QJsonValue& nodeVal : nodes)
            {
                QJsonObject node = nodeVal.toObject();
                if (node.contains("carId") && node["carId"].toString() == carId)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                QJsonObject obj = recordToJson(
                    query.value("id").toString(),
                    query.value("model_id").toString(),
                    query.value("name").toString(),
                    query.value("implement_node").toString(),
                    query.value("level").toString(),
                    graphDataStr
                );

                obj["create_time"] = query.value("create_time").toString();
                obj["update_time"] = query.value("update_time").toString();

                array.append(obj);
            }
        }
    }

    QJsonObject root;
    root["data"] = array;
    root["count"] = array.size();

    QJsonDocument doc(root);
    return QString(doc.toJson(QJsonDocument::Compact));
}

QJsonObject ProcessT::recordToJson(const QString& id, const QString& model_id, const QString& name,
                                  const QString& implement_node, const QString& level,
                                  const QString& graph_data)
{
    QJsonObject obj;
    obj["id"] = id;
    obj["model_id"] = model_id;
    obj["model_name"] = getModelName(model_id);
    obj["name"] = name;
    obj["implement_node"] = implement_node;
    obj["level"] = level;

    // 将graph_data字符串转换回JSON对象
    QJsonDocument graphDoc = QJsonDocument::fromJson(graph_data.toUtf8());
    if (!graphDoc.isNull() && graphDoc.isObject())
    {
        obj["graph_data"] = graphDoc.object();
    }
    else
    {
        obj["graph_data"] = QJsonObject();
    }

    return obj;
}
