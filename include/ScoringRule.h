#ifndef SCORINGRULE_H
#define SCORINGRULE_H

#include <string>
#include <vector>

/**
 * @brief 子规则，定义了特定人数范围内的计分方式
 */
struct SubRule {
    int minParticipants;      // 适用的最少参赛人数
    std::vector<int> scores;  // 从第一名开始的计分列表
    
    SubRule() : minParticipants(0) {}
    SubRule(int min_participants, std::vector<int> s)
        : minParticipants(min_participants), scores(std::move(s)) {}
};

/**
 * @brief 完整的计分规则，包含多个子规则
 * @details 根据参赛人数选择适用的子规则
 */
struct ScoringRule {
    std::string ruleName;               // 规则名称
    int minParticipantsRequired;        // 项目举办所需的最少参赛人数
    std::vector<SubRule> subRules;      // 子规则列表
    
    /**
     * @brief 默认构造函数
     */
    ScoringRule() : minParticipantsRequired(0) {}
    
    /**
     * @brief 构造函数
     * @param name 规则名称
     * @param min_participants_required 项目举办所需的最少参赛人数
     * @param sub_rules 子规则列表
     */
    ScoringRule(std::string name, int min_participants_required, std::vector<SubRule> sub_rules)
        : ruleName(std::move(name)), minParticipantsRequired(min_participants_required), 
          subRules(std::move(sub_rules)) {}

    /**
     * @brief 根据参赛人数选择适用的子规则
     * @param participants 参赛人数
     * @return 适用的子规则，如果没有适用的子规则则返回空指针
     */
    const SubRule* getSubRuleForParticipants(int participants) const {
        // 如果参赛人数不足最低要求，返回nullptr
        if (participants < minParticipantsRequired)
            return nullptr;
        
        // 从第一个子规则开始检查，找到第一个适用的子规则
        for (const auto& subRule : subRules) {
            if (participants >= subRule.minParticipants)
                return &subRule;
        }
        
        // 如果没有找到适用的子规则（通常不会发生，因为应该有一个兜底的子规则）
        return nullptr;
    }
};

#endif // SCORINGRULE_H 