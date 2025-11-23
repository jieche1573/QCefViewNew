#pragma once

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>


class ProcessT
{
public:
    /**
     * @brief 获取单例实例
     */
    static ProcessT& Instance()
    {
      if (!_instance)
        {
            std::lock_guard<std::mutex> lock(_mutex);
        if (!_instance)
            {
              _instance.reset(new ProcessT());
            }
        }
      return *_instance;
    }

    ~ProcessT();

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
     * @brief 添加流程图
     *
     * 入参JSON结构：
     * {
     *   "model_id": "20251109144639678678cc1ae7",  // 机型ID（必填）
     *   "name": "飞行前检查",                      // 流程名称（必填）
     *   "implement_node": "首次起飞前",            // 实施节点（必填）
     *   "level": "一级",                           // 等级（必填）
     *   "graph_data": {                            // 流程图数据（必填）
     *     "nodes": [
     *       {
     *         "id": "start",
     *         "type": "start",
     *         "x": 400,
     *         "y": 80
     *       },
     *       {
     *         "id": "node1",
     *         "type": "person",                    // 有人节点
     *         "x": 400,
     *         "y": 200,
     *         "name": "检查外观",
     *         "personId": "岗位ID",
     *         "personName": "岗位名称",
     *         "prepareTime": 10,                   // 准备时间（分钟）
     *         "workTime": 30,                      // 工作时间（分钟）
     *         "finishTime": 5                      // 收尾时间（分钟）
     *       },
     *       {
     *         "id": "node2",
     *         "type": "personCar",                 // 有人有车节点
     *         "x": 400,
     *         "y": 320,
     *         "name": "加油作业",
     *         "personId": "岗位ID",
     *         "personName": "岗位名称",
     *         "carId": "设备ID",
     *         "carName": "设备名称",
     *         "prepareTime": 15,                   // 准备时间（分钟）
     *         "workTime": 45,                      // 工作时间（分钟）
     *         "finishTime": 10                     // 收尾时间（分钟）
     *       },
     *       {
     *         "id": "end",
     *         "type": "end",
     *         "x": 400,
     *         "y": 500
     *       }
     *     ],
     *     "edges": [
     *       {
     *         "id": "start-node1",
     *         "from": "start",
     *         "to": "node1"
     *       },
     *       {
     *         "id": "node1-node2",
     *         "from": "node1",
     *         "to": "node2"
     *       },
     *       {
     *         "id": "node2-end",
     *         "from": "node2",
     *         "to": "end"
     *       }
     *     ]
     *   }
     * }
     *
     * 节点类型说明：
     * - start: 开始节点（无需时间字段）
     * - end: 结束节点（无需时间字段）
     * - person: 有人节点（需要 personId/personName + prepareTime/workTime/finishTime）
     * - personCar: 有人有车节点（需要 personId/personName/carId/carName + prepareTime/workTime/finishTime）
     *
     * 返回JSON结构：
     * {
     *   "id": "20251108232312345678ABC",           // 自动生成的唯一ID
     *   "model_id": "20251109144639678678cc1ae7",
     *   "model_name": "歼二十",                    // 机型名称（查询得到）
     *   "name": "飞行前检查",
     *   "implement_node": "首次起飞前",
     *   "level": "一级",
     *   "graph_data": {...},
     *   "create_time": "2025-11-08 23:23:12",
     *   "update_time": "2025-11-08 23:23:12"
     * }
     *
     * @param jsonStr - JSON格式的流程图数据
     * @return QString - 成功返回新记录的JSON，失败返回空字符串
     */
    QString addRecord(const QString& jsonStr);

    /**
     * @brief 删除流程图
     *
     * @param id - 流程图ID
     * @return bool - true: 删除成功, false: 删除失败
     */
    bool deleteRecord(const QString& id);

    /**
     * @brief 更新流程图
     *
     * 入参JSON结构：
     * {
     *   "id": "20251108232312345678ABC",  // 流程图ID（必填）
     *   "name": "更新后的流程名称",       // 流程名称（选填）
     *   "implement_node": "再次起飞前",   // 实施节点（选填）
     *   "level": "二级",                  // 等级（选填）
     *   "graph_data": {                   // 流程图数据（选填）
     *     "nodes": [
     *       {
     *         "id": "node1",
     *         "type": "person",
     *         "x": 400,
     *         "y": 200,
     *         "name": "检查外观",
     *         "personId": "岗位ID",
     *         "personName": "岗位名称",
     *         "prepareTime": 10,          // 准备时间（分钟）
     *         "workTime": 30,             // 工作时间（分钟）
     *         "finishTime": 5             // 收尾时间（分钟）
     *       }
     *     ],
     *     "edges": [...]
     *   }
     * }
     *
     * 说明：
     * - person 和 personCar 节点必须包含时间字段：prepareTime、workTime、finishTime
     *
     * @param jsonStr - JSON格式的更新数据（必须包含id字段）
     * @return bool - true: 更新成功, false: 更新失败
     */
    bool updateRecord(const QString& jsonStr);

    /**
     * @brief 根据ID查询单条流程图
     *
     * @param id - 流程图ID
     * @return QString - 成功返回记录JSON，失败返回空字符串
     */
    QString queryRecordById(const QString& id);

    /**
     * @brief 查询所有流程图
     *
     * 返回JSON结构：
     * {
     *   "data": [
     *     {
     *       "id": "...",
     *       "model_id": "...",
     *       "model_name": "歼二十",
     *       "name": "飞行前检查",
     *       "implement_node": "首次起飞前",
     *       "level": "一级",
     *       "graph_data": {
     *         "nodes": [
     *           {
     *             "id": "node1",
     *             "type": "person",
     *             "name": "检查外观",
     *             "personId": "...",
     *             "personName": "钳工师",
     *             "prepareTime": 10,
     *             "workTime": 30,
     *             "finishTime": 5
     *           }
     *         ],
     *         "edges": [...]
     *       },
     *       "create_time": "...",
     *       "update_time": "..."
     *     }
     *   ],
     *   "count": 5
     * }
     *
     * @return QString - 返回所有记录的JSON字符串
     */
    QString queryAllRecords();

    /**
     * @brief 根据流程名称模糊查询
     *
     * @param name - 流程名称（支持模糊匹配）
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByName(const QString& name);

    /**
     * @brief 根据机型ID查询流程图
     *
     * @param modelId - 机型ID
     * @return QString - 返回该机型的所有流程图
     */
    QString queryByModelId(const QString& modelId);

    /**
     * @brief 根据实施节点查询
     *
     * @param node - 实施节点（首次起飞前/再次起飞前/最后降落后）
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByNode(const QString& node);

    /**
     * @brief 根据等级查询
     *
     * @param level - 等级（一级/二级/三级/四级）
     * @return QString - 返回匹配记录的JSON字符串
     */
    QString queryByLevel(const QString& level);

    /**
     * @brief 查询使用了指定岗位的所有流程
     *
     * 说明：需要解析graph_data中的节点，查找包含指定personId的流程
     *
     * @param personId - 岗位ID
     * @return QString - 返回使用了该岗位的所有流程
     */
    QString queryByPersonId(const QString& personId);

    /**
     * @brief 查询使用了指定设备的所有流程
     *
     * 说明：需要解析graph_data中的节点，查找包含指定carId的流程
     *
     * @param carId - 设备ID
     * @return QString - 返回使用了该设备的所有流程
     */
    QString queryByCarId(const QString& carId);

private:
    ProcessT();
    ProcessT(const ProcessT&) = delete;
    ProcessT& operator=(const ProcessT&) = delete;

    // 将数据库查询结果转换为JSON对象
    QJsonObject recordToJson(const QString& id, const QString& model_id, const QString& name,
                             const QString& implement_node, const QString& level,
                             const QString& graph_data);

    // 根据机型ID查询机型名称
    QString getModelName(const QString& modelId);


    static std::unique_ptr<ProcessT> _instance;
    static std::mutex _mutex;
};
