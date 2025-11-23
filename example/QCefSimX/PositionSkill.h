#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 岗位多能表类 - 用于管理岗位与机型的关联关系
 *
 * 数据库表：position_skill
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - gw_id:     岗位ID（关联position表的id）
 * - jx_id:     机型ID（关联aircraft_model表的id）
 * - nlxs:      能力系数（0.0-1.0之间）
 * - create_time: 创建时间
 * - update_time: 更新时间
 *
 * 关系说明：
 * - 一个岗位可以关联多个机型（一对多）
 * - 每个关联记录包含能力系数，表示该岗位对该机型的维修能力
 * - 例如：机械岗位001 可以维修 JJ1(能力系数0.8)、JJ2(能力系数0.6)、JJ3(能力系数0.3)
 */
class PositionSkill
{
public:
    /**
     * @brief 获取单例实例
     */
    static PositionSkill& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new PositionSkill());
            }
        }
        return *_instance;
    }

    ~PositionSkill();

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
     * @brief 添加岗位多能记录
     *
     * 入参JSON结构：
     * {
     *   "gw_id": "20251110103015678678cc1ae7",    // 岗位ID（必填）
     *   "jx_id": "20251109144639678678cc1ae7",    // 机型ID（必填）
     *   "nlxs": 0.8                                // 能力系数（必填，0.0-1.0）
     * }
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",           // 自动生成的唯一ID
     *   "gw_id": "20251110103015678678cc1ae7",
     *   "jx_id": "20251109144639678678cc1ae7",
     *   "nlxs": 0.8,
     *   "gw_name": "钳工师",                       // 岗位名称（查询得到）
     *   "jx_name": "歼二十",                       // 机型名称（查询得到）
     *   "create_time": "2025-11-08 23:23:12",      // 自动生成
     *   "update_time": "2025-11-08 23:23:12"       // 自动生成
     * }
     *
     * @param jsonStr - JSON格式的记录数据
     * @return QString - 成功返回新记录的JSON，失败返回空字符串
     */
    QString addRecord(const QString& jsonStr);

    /**
     * @brief 删除岗位多能记录
     *
     * @param id - 记录ID，示例："20251108232312345678ABC"
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteRecord(const QString& id);

    /**
     * @brief 更新岗位多能记录
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 要更新的记录ID（必填）
     *   "nlxs": 0.9                       // 能力系数（选填）
     * }
     *
     * 说明：
     * - id 字段必须提供
     * - 通常只更新能力系数，不建议更新gw_id和jx_id
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
     *   "gw_id": "20251110103015678678cc1ae7",
     *   "jx_id": "20251109144639678678cc1ae7",
     *   "nlxs": 0.8,
     *   "gw_name": "钳工师",
     *   "jx_name": "歼二十",
     *   "create_time": "2025-11-08 23:23:12",
     *   "update_time": "2025-11-08 23:23:12"
     * }
     *
     * @param id - 记录ID
     * @return QString - 成功返回记录JSON，失败返回空字符串
     */
    QString queryRecordById(const QString& id);

    /**
     * @brief 查询所有岗位多能记录
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "gw_id": "20251110103015678678cc1ae7",
     *       "jx_id": "20251109144639678678cc1ae7",
     *       "nlxs": 0.8,
     *       "gw_name": "钳工师",
     *       "jx_name": "歼二十",
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
     * @brief 根据岗位ID查询该岗位的所有机型能力
     *
     * @param gwId - 岗位ID
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "gw_id": "20251110103015678678cc1ae7",
     *       "jx_id": "20251109144639678678cc1ae7",
     *       "nlxs": 0.8,
     *       "gw_name": "钳工师",
     *       "jx_name": "歼二十",
     *       "create_time": "2025-11-08 23:23:12",
     *       "update_time": "2025-11-08 23:23:12"
     *     },
     *     { ... }  // 该岗位可以维修的其他机型
     *   ],
     *   "count": 3  // 该岗位可以维修的机型数量
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByPositionId(const QString& gwId);

    /**
     * @brief 根据机型ID查询所有能维修该机型的岗位
     *
     * @param jxId - 机型ID
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "20251108232312345678ABC",
     *       "gw_id": "20251110103015678678cc1ae7",
     *       "jx_id": "20251109144639678678cc1ae7",
     *       "nlxs": 0.8,
     *       "gw_name": "钳工师",
     *       "jx_name": "歼二十",
     *       "create_time": "2025-11-08 23:23:12",
     *       "update_time": "2025-11-08 23:23:12"
     *     },
     *     { ... }  // 其他能维修该机型的岗位
     *   ],
     *   "count": 5  // 能维修该机型的岗位数量
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByAircraftId(const QString& jxId);

    /**
     * @brief 批量添加岗位多能记录（一次性为一个岗位添加多个机型能力）
     *
     * 入参JSON结构：
     * {
     *   "gw_id": "20251110103015678678cc1ae7",    // 岗位ID（必填）
     *   "skills": [
     *     {
     *       "jx_id": "20251109144639678678cc1ae7",
     *       "nlxs": 0.8
     *     },
     *     {
     *       "jx_id": "20251109144639684684086dfc",
     *       "nlxs": 0.6
     *     }
     *   ]
     * }
     *
     * 返回JSON结构：
     * {
     *   "success": true,
     *   "count": 2,
     *   "message": "成功添加2条记录"
     * }
     *
     * @param jsonStr - JSON格式的批量数据
     * @return QString - 成功返回结果JSON，失败返回错误信息
     */
    QString batchAddRecords(const QString& jsonStr);

    /**
     * @brief 根据岗位ID删除该岗位的所有机型能力
     *
     * @param gwId - 岗位ID
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteByPositionId(const QString& gwId);

private:
    PositionSkill();
    PositionSkill(const PositionSkill&) = delete;
    PositionSkill& operator=(const PositionSkill&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& gw_id, const QString& jx_id,
                             double nlxs, const QString& gw_name, const QString& jx_name);

    // 根据岗位ID查询岗位名称
    QString getPositionName(const QString& gwId);

    // 根据机型ID查询机型名称
    QString getAircraftName(const QString& jxId);

    static std::unique_ptr<PositionSkill> _instance;
    static std::mutex _mutex;
};
