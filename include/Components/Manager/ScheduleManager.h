#ifndef SCHEDULEMANAGER_H
#define SCHEDULEMANAGER_H

#include <vector>
#include <string>
#include "../Core/Schedule.h"
#include "../Core/DataContainer.h"

/**
 * @brief 赛程管理器，负责处理与赛程相关的所有操作
 * 
 * ScheduleManager 类提供了一个接口，用于生成、访问和操作赛程数据。
 * 该类遵循Manager设计模式，作为业务逻辑层和数据层之间的桥梁。
 */
class ScheduleManager {
private:
    DataContainer& data; ///< 对中央数据容器的引用

public:
    /**
     * @brief 构造函数
     * @param dataContainer 对数据容器的引用
     */
    explicit ScheduleManager(DataContainer& dataContainer);

    /**
     * @brief 生成赛程表
     * @return 是否成功生成
     */
    bool generateSchedule();

    /**
     * @brief 获取赛程条目列表
     * @return 赛程条目的常量引用
     */
    const std::vector<ScheduleEntry>& getScheduleEntries() const;

    /**
     * @brief 打印赛程表
     */
    void printSchedule() const;

    /**
     * @brief 获取赛程表内容的字符串表示
     * @return 格式化的赛程表字符串
     */
    std::string getScheduleContentAsString() const;

    /**
     * @brief 验证赛程表的有效性
     * @return 赛程表是否有效
     */
    bool validateSchedule() const;
};

#endif // SCHEDULEMANAGER_H 