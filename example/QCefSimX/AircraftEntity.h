#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 飞机实体表类 - 用于管理飞机实体数据
 *
 * 数据库表：aircraft_entity
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - fjbh:      飞机编号
 * - xdid:      想定ID
 * - jxid:      机型ID
 * - bzdj:      保障等级
 * - create_time: 创建时间
 * - update_time: 更新时间
 */
class AircraftEntity
{
public:
    /**
     * @brief 获取单例实例
     */
    static AircraftEntity& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new AircraftEntity());
            }
        }
        return *_instance;
    }

    ~AircraftEntity();

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
     *   "fjbh": "001",                // 飞机编号（必填）
     *   "xdid": "20251108232312...",  // 想定ID（必填）
     *   "jxid": "20251109144639...",  // 机型ID（必填）
     *   "bzdj": "一级"                 // 保障等级（选填）
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
     *   "fjbh": "002",                    // 飞机编号（选填）
     *   "bzdj": "二级"                     // 其他需要更新的字段...
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
     * @brief 根据飞机编号模糊查询
     * @param code - 飞机编号（支持模糊匹配）
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByCode(const QString& code);

    /**
     * @brief 根据想定ID查询
     * @param xdid - 想定ID
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByScenarioId(const QString& xdid);

    /**
     * @brief 根据机型ID查询
     * @param jxid - 机型ID
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByModelId(const QString& jxid);

    /**
     * @brief 根据保障等级查询
     * @param bzdj - 保障等级
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByLevel(const QString& bzdj);

    /**
     * @brief 根据想定ID和机型ID组合查询
     * @param xdid - 想定ID
     * @param jxid - 机型ID（传入空字符串或"all"表示查询该想定下所有机型）
     * @return QString - 返回匹配记录的JSON字符串
     *
     * 使用场景：
     * - 在指定想定下筛选特定机型的飞机
     * - 支持"全部"选项，传入空字符串或"all"即可查询该想定下所有飞机
     */
    QString queryByScenarioAndModel(const QString& xdid, const QString& jxid);

    /**
     * @brief 根据想定ID删除所有相关飞机实体
     * @param xdid - 想定ID
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteByScenarioId(const QString& xdid);

private:
    AircraftEntity();
    AircraftEntity(const AircraftEntity&) = delete;
    AircraftEntity& operator=(const AircraftEntity&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& fjbh, const QString& xdid,
                             const QString& jxid, const QString& bzdj);

    static std::unique_ptr<AircraftEntity> _instance;
    static std::mutex _mutex;
};
