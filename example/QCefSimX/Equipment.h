#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 装备管理表类 - 用于管理装备/设备数据
 *
 * 数据库表：equipment
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - sbid:      设备ID
 * - sbmc:      设备名称
 * - lx:        类型（装备/设备）
 * - sfzy:      是否专用（通用设备/专用设备）
 * - zyjx:      专用机型ID（关联aircraft_model表的id，通用设备时为空）
 * - xqjs:      详情介绍
 * - create_time: 创建时间
 * - update_time: 更新时间
 *
 * 注意：
 * - 当sfzy为"专用设备"时，zyjx存储机型ID
 * - 当sfzy为"通用设备"时，zyjx字段为空
 * - 只返回机型ID，前端需要显示机型名称时应调用AircraftModel::queryRecordById()查询
 */
class Equipment
{
public:
    /**
     * @brief 获取单例实例
     */
    static Equipment& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new Equipment());
            }
        }
        return *_instance;
    }

    ~Equipment();

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
     *   "sbid": "001",                 // 设备ID（必填）
     *   "sbmc": "电源车",              // 设备名称（必填）
     *   "lx": "装备",                  // 类型：装备/设备（必填）
     *   "sfzy": "通用设备/专用设备",   // 是否专用（必填）
     *   "zyjx": "20251109144639678678cc1ae7",  // 专用机型ID（专用设备时必填，通用设备时忽略）
     *   "xqjs": "详情介绍"             // 详情介绍（可选）
     * }
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",      // 自动生成的唯一ID
     *   "sbid": "001",
     *   "sbmc": "电源车",
     *   "lx": "装备",
     *   "sfzy": "专用设备",
     *   "zyjx": "20251109144639678678cc1ae7",  // 专用机型ID
     *   "zyjx_name": "歼二十",                 // 专用机型名称（查询得到）
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
     *   "sbmc": "新设备名称",             // 设备名称（选填）
     *   "lx": "设备",                     // 类型（选填）
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
     *   "sbid": "001",
     *   "sbmc": "电源车",
     *   "lx": "装备",
     *   "sfzy": "专用设备",
     *   "zyjx": "20251109144639678678cc1ae7",  // 专用机型ID
     *   "zyjx_name": "歼二十",                 // 专用机型名称
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
     *       "sbid": "001",
     *       "sbmc": "电源车",
     *       "lx": "装备",
     *       "sfzy": "专用设备",
     *       "zyjx": "20251109144639678678cc1ae7",  // 专用机型ID
     *       "zyjx_name": "歼二十",                 // 专用机型名称
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
     * @brief 根据设备名称模糊查询
     *
     * @param name - 设备名称（支持模糊匹配），示例："电源" 会匹配 "电源车"、"电源设备"等
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
     * @brief 根据类型查询（精确匹配）
     *
     * @param type - 类型，示例："装备"、"设备"
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

    /**
     * @brief 根据是否专用查询（精确匹配）
     *
     * @param dedicated - 是否专用，示例："通用设备"、"专用设备"
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByDedicated(const QString& dedicated);

private:
    Equipment();
    Equipment(const Equipment&) = delete;
    Equipment& operator=(const Equipment&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& sbid, const QString& sbmc,
                             const QString& lx, const QString& sfzy, const QString& zyjx,
                             const QString& xqjs);

    static std::unique_ptr<Equipment> _instance;
    static std::mutex _mutex;
};
