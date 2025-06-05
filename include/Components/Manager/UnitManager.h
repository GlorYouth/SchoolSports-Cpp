#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include "../../utils.h"
#include "../Core/Unit.h"

// 前向声明
class SystemSettings;

class UnitManager {
public:
    using iterator = std::map<int, Unit>::iterator;
    using const_iterator = std::map<int, Unit>::const_iterator;

    explicit UnitManager(SystemSettings& settings);
    
    // 基本操作
    bool add(const std::string& unitName);
    bool remove(int unitId);
    void clear();
    static void resetIdCounter();
    
    // 获取方法
    std::optional<utils::Ref<Unit>> get(int unitId);
    std::optional<utils::RefConst<Unit>> getConst(int unitId) const;
    const std::map<int, Unit>& getAll() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // 查找方法
    std::optional<utils::Ref<Unit>> findByName(const std::string& name);
    
    // 统计方法
    size_t count() const;
    bool empty() const;
    bool contains(int unitId) const;
    bool contains(const std::string& unitName) const;
    
    // 得分管理
    void resetAllScores();
    void addScore(int unitId, double score);

private:
    SystemSettings& settings;
    // 内部方法，直接操作单位数据结构
    std::map<int, Unit>& getUnitsMap();
    const std::map<int, Unit>& getUnitsMapConst() const;
};

#endif // UNITMANAGER_H 