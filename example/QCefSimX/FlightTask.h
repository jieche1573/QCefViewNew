#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 飞行任务表类 - 用于管理飞行任务数据
 *
 * 数据库表：flight_task
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - fjstid:    飞机实体ID
 * - qfsj:      起飞时间
 * - jlsj:      降落时间
 * - create_time: 创建时间
 * - update_time: 更新时间
 */
class FlightTask
{
public:
    /**
     * @brief 获取单例实例
     */
    static FlightTask& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new FlightTask());
            }
        }
        return *_instance;
    }

    ~FlightTask();

    /**
     * @brief 初始化数据库表
     * @return bool - true: 成功, false: 失败
     */
    bool initTable();

    /**
     * @brief 生成唯一ID
     * @return QString - 格式：时间戳精确到微秒 + UUID前6位
     */
    static QString generateUniqueId();

    /**
     * @brief 添加记录
     *
     * 入参JSON结构：
     * {
     *   "fjstid": "20251108232312...",    // 飞机实体ID（必填）
     *   "qfsj": "2025-10-12 08:00:00",    // 起飞时间（必填）
     *   "jlsj": "2025-10-12 10:30:00"     // 降落时间（必填）
     * }
     *
     * @param jsonStr - JSON格式的记录数据
     * @return QString - 成功返回新记录的JSON，失败返回空字符串
     */
    QString addRecord(const QString& jsonStr);

    /**
     * @brief 删除记录
     * @param id - 记录ID
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteRecord(const QString& id);

    /**
     * @brief 更新记录
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "qfsj": "2025-10-12 08:30:00",    // 起飞时间（选填）
     *   "jlsj": "2025-10-12 11:00:00"     // 降落时间（选填）
     * }
     *
     * @param jsonStr - JSON格式的更新数据（必须包含id字段）
     * @return bool - true: 更新成功, false: 更新失败
     */
    bool updateRecord(const QString& jsonStr);

    /**
     * @brief 根据ID查询单条记录
     * @param id - 记录ID
     * @return QString - 成功返回记录JSON，失败返回空字符串
     */
    QString queryRecordById(const QString& id);

    /**
     * @brief 查询所有记录
     * @return QString - 返回所有记录的JSON字符串
     */
    QString queryAllRecords();

    /**
     * @brief 根据飞机实体ID查询所有飞行任务
     * @param fjstid - 飞机实体ID
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByAircraftEntityId(const QString& fjstid);

    /**
     * @brief 根据时间范围查询飞行任务
     * @param startTime - 开始时间
     * @param endTime - 结束时间
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByTimeRange(const QString& startTime, const QString& endTime);

    /**
     * @brief 根据飞机实体ID删除所有相关飞行任务
     * @param fjstid - 飞机实体ID
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteByAircraftEntityId(const QString& fjstid);

private:
    FlightTask();
    FlightTask(const FlightTask&) = delete;
    FlightTask& operator=(const FlightTask&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& fjstid,
                             const QString& qfsj, const QString& jlsj);

    static std::unique_ptr<FlightTask> _instance;
    static std::mutex _mutex;
};
