#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 想定管理表类 - 用于管理想定数据
 *
 * 数据库表：scenario
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - xdmc:      想定名称
 * - jc:        机场
 * - zt:        状态（进行中、草稿、已完成）
 * - xdms:      想定描述
 * - kssj:      开始时间
 * - jssj:      结束时间
 * - ryzddlyl:  人员最大利用率（默认0.00）
 * - zbzddlyl:  装备最大利用率（默认0.00）
 * - rymzl:     人员满足率（默认0.00）
 * - zbmzl:     装备满足率（默认0.00）
 * - sfyc:      算法预测JSON（存储算法预测结果的JSON字符串）
 * - create_time: 创建时间
 * - update_time: 更新时间
 */
class ScenarioModel
{
public:
    /**
     * @brief 获取单例实例
     */
    static ScenarioModel& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new ScenarioModel());
            }
        }
        return *_instance;
    }

    ~ScenarioModel();

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
     *   "xdmc": "2025年10月12日 飞行计划",    // 想定名称（必填）
     *   "jc": "连城机场",                    // 机场（必填）
     *   "zt": "草稿",                        // 状态（进行中/草稿/已完成）
     *   "xdms": "本次训练主要内容...",        // 想定描述（可选）
     *   "kssj": "2025-10-12 08:00:00",      // 开始时间（可选）
     *   "jssj": "2025-10-12 18:00:00"       // 结束时间（可选）
     * }
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",      // 自动生成的唯一ID
     *   "xdmc": "2025年10月12日 飞行计划",
     *   "jc": "连城机场",
     *   "zt": "草稿",
     *   "xdms": "本次训练主要内容...",
     *   "kssj": "2025-10-12 08:00:00",
     *   "jssj": "2025-10-12 18:00:00",
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
     *   "xdmc": "新想定名称",             // 想定名称（选填）
     *   "zt": "进行中",                   // 状态（选填）
     *   "xdms": "更新后的描述"            // 其他需要更新的字段...
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
     *   "xdmc": "2025年10月12日 飞行计划",
     *   "jc": "连城机场",
     *   "zt": "进行中",
     *   "xdms": "本次训练主要内容...",
     *   "kssj": "2025-10-12 08:00:00",
     *   "jssj": "2025-10-12 18:00:00",
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
     *       "xdmc": "2025年10月12日 飞行计划",
     *       "jc": "连城机场",
     *       "zt": "进行中",
     *       "xdms": "本次训练主要内容...",
     *       "kssj": "2025-10-12 08:00:00",
     *       "jssj": "2025-10-12 18:00:00",
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
     * @brief 根据想定名称模糊查询
     *
     * @param name - 想定名称（支持模糊匹配），示例："飞行" 会匹配包含"飞行"的想定名称
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
     * @brief 根据状态查询（精确匹配）
     *
     * @param status - 状态，示例："进行中"、"草稿"、"已完成"
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByStatus(const QString& status);

    /**
     * @brief 根据机场查询（精确匹配）
     *
     * @param airport - 机场，示例："连城机场"、"和田机场"
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByAirport(const QString& airport);

    /**
     * @brief 更新想定的利用率和满足率字段
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "ryzddlyl": 0.85,                 // 人员最大利用率（选填）
     *   "zbzddlyl": 0.92,                 // 装备最大利用率（选填）
     *   "rymzl": 0.78,                    // 人员满足率（选填）
     *   "zbmzl": 0.88                     // 装备满足率（选填）
     * }
     *
     * 说明：
     * - id 字段必须提供
     * - 其他字段可选，只更新提供的字段
     * - update_time 会自动更新
     * - 字段值为浮点数，建议范围 0.00 ~ 1.00
     *
     * @param jsonStr - JSON格式的更新数据（必须包含id字段）
     * @return bool - true: 更新成功, false: 更新失败
     */
    bool updateRateFields(const QString& jsonStr);

    /**
     * @brief 更新算法预测JSON字段
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "sfyc": "{...}"                   // 算法预测JSON字符串（必填）
     * }
     *
     * 说明：
     * - id 字段必须提供
     * - sfyc 为算法预测结果的JSON字符串
     * - update_time 会自动更新
     *
     * @param jsonStr - JSON格式的更新数据（必须包含id和sfyc字段）
     * @return bool - true: 更新成功, false: 更新失败
     */
    bool updatePredictionField(const QString& jsonStr);

private:
    ScenarioModel();
    ScenarioModel(const ScenarioModel&) = delete;
    ScenarioModel& operator=(const ScenarioModel&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& xdmc, const QString& jc,
                             const QString& zt, const QString& xdms,
                             const QString& kssj, const QString& jssj,
                             double ryzddlyl, double zbzddlyl, double rymzl, double zbmzl,
                             const QString& sfyc);

    static std::unique_ptr<ScenarioModel> _instance;
    static std::mutex _mutex;
};
