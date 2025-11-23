#pragma once

#include <head.h>
#include "AircraftModel.h"
#include "sqlite.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>

/**
 * @brief AircraftModel 单元测试类（仅头文件）
 *
 * 功能：
 * 1. 检测表是否为空，如果为空则插入模拟测试数据
 * 2. 测试 AircraftModel 的所有接口（增删改查）
 * 3. 服务启动时调用，确保数据库有测试数据
 */
class AircraftModelTest
{
public:
    /**
     * @brief 初始化测试 - 服务启动时调用此方法
     *
     * 功能流程：
     * 1. 检查表是否为空
     * 2. 如果为空，插入3-5条模拟测试记录
     * 3. 运行所有单元测试
     *
     * @return void
     */
    static void initTest()
    {
        qDebug() << "========================================";
        qDebug() << "AircraftModel Unit Test Started";
        qDebug() << "========================================";

        // 检查表是否为空，如果为空则插入模拟数据
        if (isTableEmpty())
        {
            qDebug() << "[INFO] Table is empty, inserting mock data...";
            insertMockData();
        }
        else
        {
            qDebug() << "[INFO] Table already has data, skip inserting mock data";
        }

        // 运行所有测试
        runAllTests();

        qDebug() << "========================================";
        qDebug() << "AircraftModel Unit Test Completed";
        qDebug() << "========================================";
    }

    /**
     * @brief 检查表是否为空
     *
     * @return bool - true: 表为空, false: 表有数据
     */
    static bool isTableEmpty()
    {
        QString sql = "SELECT COUNT(*) FROM aircraft_model";
        QSqlQuery query = SQLite::Instance().select(sql);

        if (query.next())
        {
            int count = query.value(0).toInt();
            qDebug() << "[CHECK] Current record count:" << count;
            return count == 0;
        }

        return true;
    }

    /**
     * @brief 插入模拟测试数据（5条记录）
     *
     * 模拟数据包括不同类型的战斗机数据
     *
     * @return void
     */
    static void insertMockData()
    {
        qDebug() << "[INSERT] Inserting 5 mock records...";

        // 模拟记录1：十
        QString mock1 = R"({
            "jxmc": "十",
            "ssjz": "战斗机",
            "nd": 1000,
            "fxsj": 1500,
            "pjgzjg": 1300,
            "dqwxgzsj": 2000,
            "djhfgzsj": 0,
            "ddrysj": 10,
            "ddzb": 5,
            "ddbj": 15,
            "xqjs": "第四代战斗机，具有超音速巡航能力"
        })";

        // 模拟记录2：十一
        QString mock2 = R"({
            "jxmc": "十一",
            "ssjz": "战斗机",
            "nd": 1000,
            "fxsj": 1500,
            "pjgzjg": 1300,
            "dqwxgzsj": 2000,
            "djhfgzsj": 0,
            "ddrysj": 10,
            "ddzb": 5,
            "ddbj": 15,
            "xqjs": "多用途战斗机，海空通用型号"
        })";

        // 模拟记录3：十二
        QString mock3 = R"({
            "jxmc": "十二",
            "ssjz": "战斗机",
            "nd": 1000,
            "fxsj": 1500,
            "pjgzjg": 1300,
            "dqwxgzsj": 2000,
            "djhfgzsj": 0,
            "ddrysj": 10,
            "ddzb": 5,
            "ddbj": 15,
            "xqjs": "轻型多用途战斗机，机动性能优异"
        })";

        // 模拟记录4：轰六
        QString mock4 = R"({
            "jxmc": "轰六",
            "ssjz": "轰炸机",
            "nd": 800,
            "fxsj": 1200,
            "pjgzjg": 1000,
            "dqwxgzsj": 1800,
            "djhfgzsj": 0,
            "ddrysj": 15,
            "ddzb": 10,
            "ddbj": 20,
            "xqjs": "中型轰炸机，可携带核武器和常规武器"
        })";

        // 模拟记录5：运二十
        QString mock5 = R"({
            "jxmc": "运二十",
            "ssjz": "运输机",
            "nd": 600,
            "fxsj": 1000,
            "pjgzjg": 800,
            "dqwxgzsj": 1500,
            "djhfgzsj": 0,
            "ddrysj": 20,
            "ddzb": 15,
            "ddbj": 25,
            "xqjs": "大型运输机，可运输重型装备和人员"
        })";

        // 插入所有模拟记录
        QString result1 = AircraftModel::Instance().addRecord(mock1);
        QString result2 = AircraftModel::Instance().addRecord(mock2);
        QString result3 = AircraftModel::Instance().addRecord(mock3);
        QString result4 = AircraftModel::Instance().addRecord(mock4);
        QString result5 = AircraftModel::Instance().addRecord(mock5);

        qDebug() << "[SUCCESS] Inserted 5 mock records";
        qDebug() << "  - Record 1:" << (!result1.isEmpty() ? "OK" : "FAILED");
        qDebug() << "  - Record 2:" << (!result2.isEmpty() ? "OK" : "FAILED");
        qDebug() << "  - Record 3:" << (!result3.isEmpty() ? "OK" : "FAILED");
        qDebug() << "  - Record 4:" << (!result4.isEmpty() ? "OK" : "FAILED");
        qDebug() << "  - Record 5:" << (!result5.isEmpty() ? "OK" : "FAILED");
    }

    /**
     * @brief 运行所有单元测试
     *
     * 测试项目：
     * 1. 查询所有记录
     * 2. 根据ID查询单条记录
     * 3. 按机型名称查询
     * 4. 按所属机种查询
     * 5. 添加新记录
     * 6. 更新记录
     * 7. 删除记录
     *
     * @return void
     */
    static void runAllTests()
    {
        qDebug() << "\n[TEST] Running all unit tests...\n";

        testQueryAllRecords();
        testQueryRecordById();
        testQueryByName();
        testQueryByType();
        testAddRecord();
        testUpdateRecord();
        testDeleteRecord();

        qDebug() << "\n[COMPLETE] All tests finished\n";
    }

    /**
     * @brief 测试：查询所有记录
     *
     * 接口：queryAllRecords()
     *
     * 入参：无
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",     // 唯一ID（时间戳+UUID）
     *       "jxmc": "十",                        // 机型名称
     *       "ssjz": "战斗机",                    // 所属机种
     *       "nd": 1000,                          // 年度（整数）
     *       "fxsj": 1500,                        // 飞行时间（整数）
     *       "pjgzjg": 1300,                      // 平均故障间隔（整数）
     *       "dqwxgzsj": 2000,                    // 定期维修工作时间节点（整数）
     *       "djhfgzsj": 0,                       // 定检/换发工作时间节点（整数）
     *       "ddrysj": 10,                        // 等待人员时间（分钟）
     *       "ddzb": 5,                           // 等待装备时间（分钟）
     *       "ddbj": 15,                          // 等待备件时间（分钟）
     *       "xqjs": "第四代战斗机，具有超音速巡航能力",  // 详情介绍
     *       "create_time": "2025-11-08 23:23:12", // 创建时间
     *       "update_time": "2025-11-08 23:23:12"  // 更新时间
     *     },
     *     { ... }  // 更多记录
     *   ],
     *   "count": 5  // 记录总数
     * }
     */
    static void testQueryAllRecords()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 1] queryAllRecords()";
        qDebug() << "入参：无";
        qDebug() << "返回：JSON字符串（包含所有记录）";
        qDebug() << "----------------------------------------";

        QString result = AircraftModel::Instance().queryAllRecords();

        qDebug() << "返回结果：";
        qDebug().noquote() << result;

        // 解析JSON验证
        QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            int count = obj["count"].toInt();
            qDebug() << "[RESULT] 查询成功，共" << count << "条记录";
        }
        else
        {
            qDebug() << "[ERROR] JSON解析失败";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：根据ID查询单条记录
     *
     * 接口：queryRecordById(id)
     *
     * 入参：QString id - 记录的唯一ID
     * 示例："20251108232312345678ABC"
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",     // 唯一ID
     *   "jxmc": "十",                        // 机型名称
     *   "ssjz": "战斗机",                    // 所属机种
     *   "nd": 1000,                          // 年度
     *   "fxsj": 1500,                        // 飞行时间
     *   "pjgzjg": 1300,                      // 平均故障间隔
     *   "dqwxgzsj": 2000,                    // 定期维修工作时间节点
     *   "djhfgzsj": 0,                       // 定检/换发工作时间节点
     *   "ddrysj": 10,                        // 等待人员时间（分钟）
     *   "ddzb": 5,                           // 等待装备时间（分钟）
     *   "ddbj": 15,                          // 等待备件时间（分钟）
     *   "xqjs": "第四代战斗机，具有超音速巡航能力",  // 详情介绍
     *   "create_time": "2025-11-08 23:23:12", // 创建时间
     *   "update_time": "2025-11-08 23:23:12"  // 更新时间
     * }
     *
     * 如果未找到记录，返回空字符串 ""
     */
    static void testQueryRecordById()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 2] queryRecordById(id)";
        qDebug() << "入参：QString id - 记录ID";
        qDebug() << "返回：JSON字符串（单条记录）";
        qDebug() << "----------------------------------------";

        // 先获取第一条记录的ID
        QString allRecords = AircraftModel::Instance().queryAllRecords();
        QJsonDocument doc = QJsonDocument::fromJson(allRecords.toUtf8());
        QString testId;

        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            QJsonArray arr = obj["data"].toArray();
            if (!arr.isEmpty())
            {
                testId = arr[0].toObject()["id"].toString();
            }
        }

        if (!testId.isEmpty())
        {
            qDebug() << "测试ID：" << testId;
            QString result = AircraftModel::Instance().queryRecordById(testId);

            qDebug() << "返回结果：";
            qDebug().noquote() << result;

            if (!result.isEmpty())
            {
                qDebug() << "[RESULT] 查询成功";
            }
            else
            {
                qDebug() << "[ERROR] 查询失败，未找到记录";
            }
        }
        else
        {
            qDebug() << "[SKIP] 没有可用的测试数据";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：按机型名称查询
     *
     * 接口：queryByName(name)
     *
     * 入参：QString name - 机型名称（支持模糊查询）
     * 示例："十"  （会匹配："十"、"十一"、"十二"等）
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "jxmc": "十",
     *       "ssjz": "战斗机",
     *       "nd": 1000,
     *       "fxsj": 1500,
     *       "pjgzjg": 1300,
     *       "dqwxgzsj": 2000,
     *       "djhfgzsj": 0,
     *       "ddrysj": 10,
     *       "ddzb": 5,
     *       "ddbj": 15,
     *       "xqjs": "第四代战斗机，具有超音速巡航能力",
     *       "create_time": "2025-11-08 23:23:12",
     *       "update_time": "2025-11-08 23:23:12"
     *     },
     *     { ... }  // 其他匹配的记录
     *   ],
     *   "count": 3  // 匹配的记录数
     * }
     */
    static void testQueryByName()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 3] queryByName(name)";
        qDebug() << "入参：QString name - 机型名称（模糊查询）";
        qDebug() << "返回：JSON字符串（匹配的记录列表）";
        qDebug() << "----------------------------------------";

        QString testName = "十";
        qDebug() << "测试名称：" << testName;

        QString result = AircraftModel::Instance().queryByName(testName);

        qDebug() << "返回结果：";
        qDebug().noquote() << result;

        QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            int count = obj["count"].toInt();
            qDebug() << "[RESULT] 查询成功，找到" << count << "条匹配记录";
        }
        else
        {
            qDebug() << "[ERROR] JSON解析失败";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：按所属机种查询
     *
     * 接口：queryByType(type)
     *
     * 入参：QString type - 所属机种（精确匹配）
     * 示例："战斗机"、"轰炸机"、"运输机"
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "jxmc": "十",
     *       "ssjz": "战斗机",
     *       "nd": 1000,
     *       "fxsj": 1500,
     *       "pjgzjg": 1300,
     *       "dqwxgzsj": 2000,
     *       "djhfgzsj": 0,
     *       "ddrysj": 10,
     *       "ddzb": 5,
     *       "ddbj": 15,
     *       "xqjs": "第四代战斗机，具有超音速巡航能力",
     *       "create_time": "2025-11-08 23:23:12",
     *       "update_time": "2025-11-08 23:23:12"
     *     },
     *     { ... }  // 其他同机种的记录
     *   ],
     *   "count": 3  // 匹配的记录数
     * }
     */
    static void testQueryByType()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 4] queryByType(type)";
        qDebug() << "入参：QString type - 所属机种（精确匹配）";
        qDebug() << "返回：JSON字符串（匹配的记录列表）";
        qDebug() << "----------------------------------------";

        QString testType = "战斗机";
        qDebug() << "测试机种：" << testType;

        QString result = AircraftModel::Instance().queryByType(testType);

        qDebug() << "返回结果：";
        qDebug().noquote() << result;

        QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            int count = obj["count"].toInt();
            qDebug() << "[RESULT] 查询成功，找到" << count << "条匹配记录";
        }
        else
        {
            qDebug() << "[ERROR] JSON解析失败";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：添加新记录
     *
     * 接口：addRecord(jsonStr)
     *
     * 入参：QString jsonStr - JSON格式的记录数据
     * 入参JSON结构：
     * {
     *   "jxmc": "歼二十",           // 机型名称（必填）
     *   "ssjz": "战斗机",           // 所属机种（必填）
     *   "nd": 1000,                // 年度（整数）
     *   "fxsj": 1500,              // 飞行时间（整数）
     *   "pjgzjg": 1300,            // 平均故障间隔（整数）
     *   "dqwxgzsj": 2000,          // 定期维修工作时间节点（整数）
     *   "djhfgzsj": 0,             // 定检/换发工作时间节点（整数）
     *   "ddrysj": 10,              // 等待人员时间-分钟（整数）
     *   "ddzb": 5,                 // 等待装备时间-分钟（整数）
     *   "ddbj": 15,                // 等待备件时间-分钟（整数）
     *   "xqjs": "详情介绍文本"     // 详情介绍（可选）
     * }
     *
     * 返回JSON结构：包含新创建的完整记录（含自动生成的ID和时间戳）
     * {
     *   "id": "20251108232312345678ABC",      // 自动生成的唯一ID
     *   "jxmc": "歼二十",
     *   "ssjz": "战斗机",
     *   "nd": 1000,
     *   "fxsj": 1500,
     *   "pjgzjg": 1300,
     *   "dqwxgzsj": 2000,
     *   "djhfgzsj": 0,
     *   "ddrysj": 10,
     *   "ddzb": 5,
     *   "ddbj": 15,
     *   "xqjs": "详情介绍文本",
     *   "create_time": "2025-11-08 23:23:12",  // 自动生成的创建时间
     *   "update_time": "2025-11-08 23:23:12"   // 自动生成的更新时间
     * }
     *
     * 如果添加失败，返回空字符串 ""
     */
    static void testAddRecord()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 5] addRecord(jsonStr)";
        qDebug() << "入参：QString jsonStr - JSON格式的记录数据";
        qDebug() << "返回：JSON字符串（新创建的完整记录）";
        qDebug() << "----------------------------------------";

        QString testJson = R"({
            "jxmc": "测试机型",
            "ssjz": "测试机种",
            "nd": 999,
            "fxsj": 888,
            "pjgzjg": 777,
            "dqwxgzsj": 666,
            "djhfgzsj": 555,
            "ddrysj": 30,
            "ddzb": 20,
            "ddbj": 10,
            "xqjs": "这是一条测试记录，用于单元测试"
        })";

        qDebug() << "入参JSON：";
        qDebug().noquote() << testJson;

        QString result = AircraftModel::Instance().addRecord(testJson);

        qDebug() << "返回结果：";
        qDebug().noquote() << result;

        if (!result.isEmpty())
        {
            QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8());
            if (doc.isObject())
            {
                QString newId = doc.object()["id"].toString();
                qDebug() << "[RESULT] 添加成功，新记录ID：" << newId;

                // 保存ID供后续测试使用
                lastCreatedId = newId;
            }
        }
        else
        {
            qDebug() << "[ERROR] 添加失败";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：更新记录
     *
     * 接口：updateRecord(jsonStr)
     *
     * 入参：QString jsonStr - JSON格式的更新数据（必须包含id字段）
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "jxmc": "新机型名称",             // 机型名称（选填，只更新需要改的字段）
     *   "fxsj": 1600,                     // 飞行时间（选填）
     *   "xqjs": "更新后的详情介绍"        // 详情介绍（选填）
     *   // 其他需要更新的字段...
     * }
     *
     * 说明：
     * - id 字段必须提供
     * - 其他字段可选，只需提供要更新的字段
     * - 未提供的字段保持原值不变
     * - update_time 会自动更新为当前时间
     *
     * 返回：bool
     * - true:  更新成功
     * - false: 更新失败（记录不存在或JSON格式错误）
     */
    static void testUpdateRecord()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 6] updateRecord(jsonStr)";
        qDebug() << "入参：QString jsonStr - JSON格式的更新数据（必须包含id）";
        qDebug() << "返回：bool - 更新是否成功";
        qDebug() << "----------------------------------------";

        if (lastCreatedId.isEmpty())
        {
            qDebug() << "[SKIP] 没有可用的测试ID";
            qDebug() << "";
            return;
        }

        QString updateJson = QString(R"({
            "id": "%1",
            "jxmc": "测试机型-已更新",
            "fxsj": 9999,
            "xqjs": "这条记录已被更新"
        })").arg(lastCreatedId);

        qDebug() << "入参JSON：";
        qDebug().noquote() << updateJson;

        bool result = AircraftModel::Instance().updateRecord(updateJson);

        qDebug() << "返回结果：" << (result ? "true" : "false");

        if (result)
        {
            qDebug() << "[RESULT] 更新成功";

            // 验证更新结果
            QString updatedRecord = AircraftModel::Instance().queryRecordById(lastCreatedId);
            qDebug() << "更新后的记录：";
            qDebug().noquote() << updatedRecord;
        }
        else
        {
            qDebug() << "[ERROR] 更新失败";
        }

        qDebug() << "";
    }

    /**
     * @brief 测试：删除记录
     *
     * 接口：deleteRecord(id)
     *
     * 入参：QString id - 要删除的记录ID
     * 示例："20251108232312345678ABC"
     *
     * 返回：bool
     * - true:  删除成功
     * - false: 删除失败（记录不存在）
     *
     * 说明：
     * - 删除操作不可恢复，请谨慎使用
     * - 删除后可通过 queryRecordById 验证是否删除成功（应返回空字符串）
     */
    static void testDeleteRecord()
    {
        qDebug() << "----------------------------------------";
        qDebug() << "[TEST 7] deleteRecord(id)";
        qDebug() << "入参：QString id - 要删除的记录ID";
        qDebug() << "返回：bool - 删除是否成功";
        qDebug() << "----------------------------------------";

        if (lastCreatedId.isEmpty())
        {
            qDebug() << "[SKIP] 没有可用的测试ID";
            qDebug() << "";
            return;
        }

        qDebug() << "要删除的ID：" << lastCreatedId;

        bool result = AircraftModel::Instance().deleteRecord(lastCreatedId);

        qDebug() << "返回结果：" << (result ? "true" : "false");

        if (result)
        {
            qDebug() << "[RESULT] 删除成功";

            // 验证删除结果
            QString deletedRecord = AircraftModel::Instance().queryRecordById(lastCreatedId);
            qDebug() << "验证删除（应为空）：" << (deletedRecord.isEmpty() ? "确认已删除" : "删除失败");
        }
        else
        {
            qDebug() << "[ERROR] 删除失败";
        }

        qDebug() << "";
    }

private:
    static inline QString lastCreatedId; // 保存最后创建的记录ID，供更新和删除测试使用
};
