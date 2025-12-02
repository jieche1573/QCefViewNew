#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>

/**
 * @brief 备件管理表类 - 用于管理备件数据
 *
 * 数据库表：spare_part
 * 字段说明：
 * - id:        唯一ID（时间戳精确到微秒 + UUID前6位）
 * - bjmc:      备件名称
 * - bjbh:      备件编号
 * - syyx:      适用机型
 * - gzl:       故障率
 * - pjxfsj:    平均修复时间（分钟）
 * - pjghsj:    平均更换时间（分钟）
 * - xqjs:      详情介绍
 * - create_time: 创建时间
 * - update_time: 更新时间
 */
class SparePart
{
public:
    /**
     * @brief 获取单例实例
     */
    static SparePart& Instance()
    {
        if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (!_instance)
            {
                _instance.reset(new SparePart());
            }
        }
        return *_instance;
    }

    ~SparePart();

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
     *   "bjmc": "轮胎",                 // 备件名称（必填）
     *   "bjbh": "001",                  // 备件编号（必填）
     *   "syyx": "十",                   // 适用机型（必填）
     *   "gzl": 0.8,                     // 故障率（小数）
     *   "pjxfsj": 30,                   // 平均修复时间-分钟（整数）
     *   "pjghsj": 10,                   // 平均更换时间-分钟（整数）
     *   "xqjs": "详情介绍"              // 详情介绍（可选）
     * }
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",      // 自动生成的唯一ID
     *   "bjmc": "轮胎",
     *   "bjbh": "001",
     *   "syyx": "十",
     *   "gzl": 0.8,
     *   "pjxfsj": 30,
     *   "pjghsj": 10,
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
     *   "bjmc": "新备件名称",             // 备件名称（选填）
     *   "gzl": 0.9,                       // 故障率（选填）
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
     *   "bjmc": "轮胎",
     *   "bjbh": "001",
     *   "syyx": "十",
     *   "gzl": 0.8,
     *   "pjxfsj": 30,
     *   "pjghsj": 10,
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
     *       "bjmc": "轮胎",
     *       "bjbh": "001",
     *       "syyx": "十",
     *       "gzl": 0.8,
     *       "pjxfsj": 30,
     *       "pjghsj": 10,
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
     * @brief 根据备件名称模糊查询
     *
     * @param name - 备件名称（支持模糊匹配），示例："轮" 会匹配 "轮胎"等
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
     * @brief 根据适用机型查询（精确匹配）
     *
     * @param model - 适用机型，示例："十"、"十一"、"十二"
     *
     * 返回JSON结构：
     * {
     *   "data": [ ... ],  // 匹配的记录数组
     *   "count": 3        // 匹配的记录数
     * }
     *
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByModel(const QString& model);

private:
    SparePart();
    SparePart(const SparePart&) = delete;
    SparePart& operator=(const SparePart&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& bjmc, const QString& bjbh,
                             const QString& syyx, double gzl, int pjxfsj,
                             int pjghsj, const QString& xqjs);

    static std::unique_ptr<SparePart> _instance;
    static std::mutex _mutex;
};
