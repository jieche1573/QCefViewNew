#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 机型管理表类 - 用于管理飞机机型数据
 *
 * 数据库表：aircraft_model
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - jxmc:      机型名称
 * - ssjz:      所属机种
 * - ndfxsj:    年度飞行时间
 * - pjgzjg:    平均故障间隔
 * - dqwxgzsj:  定期维修工作时间节点
 * - zqgzsj:    周期工作时间节点
 * - hfgzsj:    换发工作时间节点
 * - ddrysj:    等待人员时间（分钟）
 * - ddzb:      等待装备时间（分钟）
 * - ddbj:      等待备件时间（分钟）
 * - xqjs:      详情介绍
 * - create_time: 创建时间
 * - update_time: 更新时间
 */
class AircraftModel
{
public:
    /**
     * @brief 获取单例实例
     */
    static AircraftModel& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new AircraftModel());
            }
        }
        return *_instance;
    }

    ~AircraftModel();

    /**
     * @brief 初始化数据库表
     * @return bool - true: 成功, false: 失败
     */
    bool initTable();

    /**
     * @brief 生成唯一ID
     * @return QString - 格式：时间戳精确到微秒 + UUID前6位
     *                   示例："20251108232312345678ABC"
     */
    static QString generateUniqueId();

    /**
     * @brief 添加记录
     *
     * 入参JSON结构：
     * {
     *   "jxmc": "歼二十",           // 机型名称（必填）
     *   "ssjz": "战斗机",           // 所属机种（必填）
     *   "ndfxsj": 1500,            // 年度飞行时间（整数）
     *   "pjgzjg": 1300,            // 平均故障间隔（整数）
     *   "dqwxgzsj": 2000,          // 定期维修工作时间节点（整数）
     *   "zqgzsj": 1800,            // 周期工作时间节点（整数）
     *   "hfgzsj": 3000,            // 换发工作时间节点（整数）
     *   "ddrysj": 10,              // 等待人员时间-分钟（整数）
     *   "ddzb": 5,                 // 等待装备时间-分钟（整数）
     *   "ddbj": 15,                // 等待备件时间-分钟（整数）
     *   "xqjs": "详情介绍"         // 详情介绍（可选）
     * }
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",      // 自动生成的唯一ID
     *   "jxmc": "歼二十",
     *   "ssjz": "战斗机",
     *   "ndfxsj": 1500,
     *   "pjgzjg": 1300,
     *   "dqwxgzsj": 2000,
     *   "zqgzsj": 1800,
     *   "hfgzsj": 3000,
     *   "ddrysj": 10,
     *   "ddzb": 5,
     *   "ddbj": 15,
     *   "xqjs": "详情介绍",
     *   "create_time": "2025-11-08 23:23:12",  // 自动生成
     *   "update_time": "2025-11-08 23:23:12"   // 自动生成
     * }
     *
     * @param jsonStr - JSON格式的记录数据
     * @return QString - 成功返回新记录的JSON，失败返回空字符串
     */
    QString addRecord(const QString& jsonStr);

    /**
     * @brief 删除记录
     *
     * @param id - 记录ID，示例："20251108232312345678ABC"
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteRecord(const QString& id);

    /**
     * @brief 更新记录
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "jxmc": "新机型名称",             // 机型名称（选填）
     *   "fxsj": 1600,                     // 飞行时间（选填）
     *   "xqjs": "更新后的详情"            // 其他需要更新的字段...
     * }
     *
     * 说明：
     * - id 字段必须提供
     * - 其他字段可选，只更新提供的字段
     * - update_time 会自动更新
     *
     * @param jsonStr - JSON格式的更新数据（必须包含id字段）
     * @return bool - true: 更新成功, false: 更新失败
     */
    bool updateRecord(const QString& jsonStr);

    /**
     * @brief 根据ID查询单条记录
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",
     *   "jxmc": "歼二十",
     *   "ssjz": "战斗机",
     *   "ndfxsj": 1500,
     *   "pjgzjg": 1300,
     *   "dqwxgzsj": 2000,
     *   "zqgzsj": 1800,
     *   "hfgzsj": 3000,
     *   "ddrysj": 10,
     *   "ddzb": 5,
     *   "ddbj": 15,
     *   "xqjs": "详情介绍",
     *   "create_time": "2025-11-08 23:23:12",
     *   "update_time": "2025-11-08 23:23:12"
     * }
     *
     * @param id - 记录ID
     * @return QString - 成功返回记录JSON，失败返回空字符串
     */
    QString queryRecordById(const QString& id);

    /**
     * @brief 查询所有记录
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "jxmc": "歼二十",
     *       "ssjz": "战斗机",
     *       "ndfxsj": 1500,
     *       "pjgzjg": 1300,
     *       "dqwxgzsj": 2000,
     *       "zqgzsj": 1800,
     *       "hfgzsj": 3000,
     *       "ddrysj": 10,
     *       "ddzb": 5,
     *       "ddbj": 15,
     *       "xqjs": "详情介绍",
     *       "create_time": "2025-11-08 23:23:12",
     *       "update_time": "2025-11-08 23:23:12"
     *     },
     *     { ... }  // 更多记录
     *   ],
     *   "count": 5  // 记录总数
     * }
     *
     * @return QString - 返回所有记录的JSON字符串
     */
    QString queryAllRecords();

    /**
     * @brief 根据机型名称模糊查询
     *
     * @param name - 机型名称（支持模糊匹配），示例："十" 会匹配 "十"、"十一"、"十二"等
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByName(const QString& name);

    /**
     * @brief 根据所属机种查询（精确匹配）
     *
     * @param type - 所属机种，示例："战斗机"、"轰炸机"、"运输机"
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByType(const QString& type);

private:
    AircraftModel();
    AircraftModel(const AircraftModel&) = delete;
    AircraftModel& operator=(const AircraftModel&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& jxmc, const QString& ssjz,
                             int ndfxsj, int pjgzjg, int dqwxgzsj,
                             int zqgzsj, int hfgzsj, int ddrysj, int ddzb, int ddbj,
                             const QString& xqjs);

    static std::unique_ptr<AircraftModel> _instance;
    static std::mutex _mutex;
};
