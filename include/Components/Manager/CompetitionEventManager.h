#ifndef COMPETITIONEVENTMANAGER_H
#define COMPETITIONEVENTMANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include "../../utils.h"
#include "../Core/CompetitionEvent.h"

// 前向声明
class SystemSettings;

class CompetitionEventManager {
public:
    using iterator = std::map<int, CompetitionEvent>::iterator;
    using const_iterator = std::map<int, CompetitionEvent>::const_iterator;

    explicit CompetitionEventManager(SystemSettings& settings);
    
    // 基本操作
    int add(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId);
    bool remove(int eventId);
    void clear();
    static void resetIdCounter();
    
    // 获取方法
    std::optional<utils::Ref<CompetitionEvent>> get(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<CompetitionEvent>> getConst(int eventId) const;
    [[nodiscard]] std::map<int, utils::RefConst<CompetitionEvent>> getAllConst() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    
    // 查找方法
    std::optional<utils::Ref<CompetitionEvent>> findByName(const std::string& name);
    [[nodiscard]] std::vector<utils::RefConst<CompetitionEvent>> findByType(EventType type) const;
    [[nodiscard]] std::vector<utils::RefConst<CompetitionEvent>> findByGenderRequirement(Gender gender) const;
    [[nodiscard]] std::vector<utils::RefConst<CompetitionEvent>> findActive() const;
    
    // 统计方法
    [[nodiscard]] size_t count() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool contains(int eventId) const;
    [[nodiscard]] bool contains(const std::string& eventName) const;

private:
    SystemSettings& settings;
    // 内部方法，直接操作赛事数据结构
    std::map<int, CompetitionEvent>& getEventsMap();
    [[nodiscard]] const std::map<int, CompetitionEvent>& getEventsMapConst() const;
};

#endif // COMPETITIONEVENTMANAGER_H 