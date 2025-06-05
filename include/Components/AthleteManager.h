#ifndef ATHLETEMANAGER_H
#define ATHLETEMANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include "../utils.h"
#include "Athlete.h"

// 前向声明
class SystemSettings;

// AthleteManager代理类声明
class AthleteManager {
public:
    using iterator = std::map<int, Athlete>::iterator;
    using const_iterator = std::map<int, Athlete>::const_iterator;

    explicit AthleteManager(SystemSettings& settings);
    
    // 基本操作
    bool add(const std::string& name, Gender gender, int unitId);
    bool remove(int athleteId);
    void clear();
    static void resetIdCounter();
    
    // 获取方法
    std::optional<utils::Ref<Athlete>> get(int athleteId);
    std::optional<utils::RefConst<Athlete>> getConst(int athleteId) const;
    const std::map<int, Athlete>& getAll() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // 查找方法
    std::optional<utils::Ref<Athlete>> findByName(const std::string& name);
    std::vector<utils::Ref<Athlete>> findByUnitId(int unitId);
    std::vector<utils::RefConst<Athlete>> getAllConst() const;
    
    // 统计方法
    size_t count() const;
    bool empty() const;
    bool contains(int athleteId) const;
    bool contains(const std::string& name) const;

    // 报名相关
    bool registerForEvent(int athleteId, int eventId);
    bool unregisterFromEvent(int athleteId, int eventId);
    int getMaxEventsAllowed() const;

private:
    SystemSettings& settings;
    // 内部方法，直接操作运动员数据结构
    std::map<int, Athlete>& getAthletesMap();
    const std::map<int, Athlete>& getAthletesMapConst() const;
};

#endif // ATHLETEMANAGER_H 