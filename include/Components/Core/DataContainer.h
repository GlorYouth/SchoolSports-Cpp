#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "Unit.h"
#include "Athlete.h"
#include "CompetitionEvent.h"
#include "ScoreRule.h"
#include "Result.h"
#include "Workflow.h"

/**
 * @brief 统一管理所有系统底层数据的容器类
 * 
 * DataContainer类作为单一数据存储点，包含系统所有核心数据。
 * 所有Manager类通过此类访问数据，备份和恢复也仅针对此类操作，
 * 确保数据的完整性和一致性。
 */
class DataContainer {
public:
    // 核心数据结构
    std::map<int, Unit> unitsMap;                   // 参赛单位
    std::map<int, Athlete> athletesMap;             // 运动员
    std::map<int, CompetitionEvent> eventsMap;      // 比赛项目
    std::map<int, ScoreRule> rulesMap;              // 计分规则
    std::map<int, EventResults> resultsMap;         // 比赛结果
    std::set<std::string> venues;                   // 比赛场地
    
    // 系统配置和状态
    WorkflowStage currentWorkflowStage;             // 当前工作流阶段
    int athleteMaxEventsAllowed;                    // 每名运动员最多可报项目数
    bool scheduleLocked;                            // 赛程是否锁定
    std::string morningSessionStart;                // 上午比赛开始时间
    std::string morningSessionEnd;                  // 上午比赛结束时间
    std::string afternoonSessionStart;              // 下午比赛开始时间
    std::string afternoonSessionEnd;                // 下午比赛结束时间
    
    /**
     * @brief 构造函数，初始化默认值
     */
    DataContainer();
    
    /**
     * @brief 清空所有数据，重置到初始状态
     */
    void clear();
    
    /**
     * @brief 序列化所有数据到输出流
     * @param outStream 输出流
     * @return 序列化是否成功
     */
    bool serialize(std::ostream& outStream) const;
    
    /**
     * @brief 从输入流反序列化所有数据
     * @param inStream 输入流
     * @return 反序列化是否成功
     */
    bool deserialize(std::istream& inStream);
    
    /**
     * @brief 创建当前数据的快照副本
     * @return 返回数据快照
     */
    DataContainer createSnapshot() const;
    
    /**
     * @brief 从快照恢复数据
     * @param snapshot 数据快照
     */
    void restoreFromSnapshot(const DataContainer& snapshot);
};

#endif // DATACONTAINER_H 