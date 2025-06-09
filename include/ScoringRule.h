#ifndef SCORINGRULE_H
#define SCORINGRULE_H

#include <vector>

/**
 * @brief 定义了一个计分规则
 * @details 例如：当参赛人数大于等于 minParticipants 时，
 *          则按照 scores 数组中的分值对前N名进行计分。
 */
struct ScoringRule {
    int minParticipants;       // 规则适用的最少参赛人数
    std::vector<int> scores; // 从第一名开始的计分列表

    /**
     * @brief 默认构造函数，修复vector::resize问题
     */
    ScoringRule() : minParticipants(0) {}

    /**
     * @brief 构造函数
     * @param min_participants 最低参赛人数
     * @param s 得分向量
     */
    ScoringRule(int min_participants, std::vector<int> s)
        : minParticipants(min_participants), scores(std::move(s)) {}
};

#endif // SCORINGRULE_H 