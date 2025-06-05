#ifndef EVENTRESULTSMANAGER_H
#define EVENTRESULTSMANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include "../../utils.h"
#include "../Core/Result.h"

// 前向声明
class SystemSettings;

class EventResultsManager {
public:
    using iterator = std::map<int, EventResults>::iterator;
    using const_iterator = std::map<int, EventResults>::const_iterator;

    explicit EventResultsManager(SystemSettings& settings);
    
    // 基本操作
    bool addOrUpdate(const EventResults& er);
    void clear();
    void clearForEvent(int eventId);
    
    // 获取方法
    std::optional<utils::Ref<EventResults>> get(int eventId);
    std::optional<utils::RefConst<EventResults>> getConst(int eventId) const;
    const std::map<int, EventResults>& getAll() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // 单位得分管理
    void resetAllUnitScores();
    void addScoreToUnit(int unitId, double score);
    
    // 统计方法
    size_t count() const;
    bool empty() const;
    bool contains(int eventId) const;

private:
    SystemSettings& settings;
    // 内部方法，直接操作比赛结果数据结构
    std::map<int, EventResults>& getResultsMap();
    const std::map<int, EventResults>& getResultsMapConst() const;
};

#endif // EVENTRESULTSMANAGER_H 