#ifndef SCORERULEMANAGER_H
#define SCORERULEMANAGER_H

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <iostream>
#include "../utils.h"
#include "ScoreRule.h"

// 前向声明
class SystemSettings;

class ScoreRuleManager {
public:
    using iterator = std::map<int, ScoreRule>::iterator;
    using const_iterator = std::map<int, ScoreRule>::const_iterator;

    explicit ScoreRuleManager(SystemSettings& settings);
    
    // 基本操作
    bool add(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    bool addCustom(ScoreRule* rule);
    void clear();
    static void resetIdCounter();
    
    // 获取方法
    std::optional<utils::Ref<ScoreRule>> get(int ruleId);
    std::optional<utils::RefConst<ScoreRule>> getConst(int ruleId) const;
    const std::map<int, ScoreRule>& getAll() const;
    
    // 迭代器支持
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // 查找方法
    std::optional<utils::Ref<ScoreRule>> findAppropriate(int participantCount);
    
    // 统计方法
    size_t count() const;
    bool empty() const;
    bool contains(int ruleId) const;

private:
    SystemSettings& settings;
    // 内部方法，直接操作计分规则数据结构
    std::map<int, ScoreRule>& getRulesMap();
    const std::map<int, ScoreRule>& getRulesMapConst() const;
};

#endif // SCORERULEMANAGER_H 