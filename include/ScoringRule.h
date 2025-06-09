#ifndef SCORINGRULE_H
#define SCORINGRULE_H

#include <vector>

/**
 * @brief 定义单条计分规则
 */
struct ScoringRule {
    int minParticipants;       // 适用此规则的最低参赛人数
    std::vector<int> scores; // 按名次顺序排列的得分 (例如: 第1名, 第2名, ...)

    /**
     * @brief 默认构造函数，修复vector::resize问题
     */
    ScoringRule() : minParticipants(0) {}

    /**
     * @brief 构造函数
     * @param min_participants 最低参赛人数
     * @param s 得分向量
     */
    ScoringRule(int min_participants, const std::vector<int>& s)
        : minParticipants(min_participants), scores(s) {}
};

#endif // SCORINGRULE_H 