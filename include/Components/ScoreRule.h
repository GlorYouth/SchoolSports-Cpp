//
// Created by GlorYouth on 2025/6/2.
//

#ifndef SCORERULE_H
#define SCORERULE_H

#include <map>
#include <string>
#include <atomic> // 用于生成唯一ID

/**
 * @brief 定义了比赛项目的计分规则。
 *
 * ScoreRule 类封装了一条计分规则，包括规则的描述、适用的参赛人数范围、
 * 录取的名次数以及每个名次对应的具体分数。
 * ID 由静态原子计数器自动生成，保证唯一性。
 */
class ScoreRule {
private:
    static std::atomic<int> nextId;         // 用于生成唯一的规则ID
    int id;                                 // 规则ID
    std::string description;                // 规则描述 (例如: "超过6人取前5名")
    int minParticipants;                    // 适用此规则的最小参赛人数 (包含)
    int maxParticipants;                    // 适用此规则的最大参赛人数 (包含, 使用 -1 表示无上限)
    int ranksToAward;                       // 录取名次数
    std::map<int, double> scoresForRanks;   // 名次对应的分数 <名次, 分数> (例如: {1:7, 2:5, ...})

public:
    /**
     * @brief ScoreRule 类的构造函数。
     * @param desc 规则的文字描述。
     * @param minP 适用此规则的最小参赛人数（含）。
     * @param maxP 适用此规则的最大参赛人数（含）。若为-1，表示无人数上限。
     * @param ranks 计划奖励的名次数。
     * @param scores 一个map，键为名次（int），值为对应的分数（double）。
     */
    ScoreRule(std::string  desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);

    /**
     * @brief 获取计分规则的唯一ID。
     * @return 返回规则ID。
     */
    [[nodiscard]] int getId() const;
    /**
     * @brief 获取计分规则的文字描述。
     * @return 返回规则描述字符串。
     */
    [[nodiscard]] std::string getDescription() const;

    /**
     * @brief 检查此计分规则是否适用于给定的参赛人数。
     * @param participantCount 实际参赛人数。
     * @return 如果适用，返回 true；否则返回 false。
     */
    [[nodiscard]] bool appliesTo(int participantCount) const;

    /**
     * @brief 获取此规则下计划奖励的名次数。
     * @return 返回录取名次数。
     */
    [[nodiscard]] int getRanksToAward() const;
    /**
     * @brief 根据指定的名次获取对应的分数。
     * @param rank 要查询的名次。
     * @return 如果名次在奖励范围内，则返回对应的分数；否则通常返回0或根据具体实现处理。
     */
    [[nodiscard]] double getScoreForRank(int rank) const;
    /**
     * @brief 获取此规则下所有名次及其对应分数的完整映射。
     * @return 返回一个包含名次到分数映射的常量map引用。
     */
    [[nodiscard]] const std::map<int, double>& getAllScoresForRanks() const;
    /**
     * @brief 获取适用此规则的最小参赛人数。
     * @return 返回最小参赛人数。
     */
    [[nodiscard]] int getMinParticipants() const { return minParticipants; }
    /**
     * @brief 获取适用此规则的最大参赛人数。
     * @return 返回最大参赛人数（-1表示无上限）。
     */
    [[nodiscard]] int getMaxParticipants() const { return maxParticipants; }

    /**
     * @brief 重置用于生成下一个计分规则ID的静态计数器。
     * @param startId 可选参数，指定ID重新开始的初始值，默认为1。
     * @note 主要用于测试目的，以确保每次测试时ID的生成是可预测的。
     */
    static void resetNextId(int startId = 1);
};
#endif //SCORERULE_H
