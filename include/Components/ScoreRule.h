//
// Created by GlorYouth on 2025/6/2.
//

#ifndef SCORERULE_H
#define SCORERULE_H

#include <map>
#include <string>
#include <atomic> // 用于生成唯一ID

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
    ScoreRule(std::string  desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);

    // 获取规则ID
    [[nodiscard]] int getId() const;
    // 获取规则描述
    [[nodiscard]] std::string getDescription() const;

    // 检查该规则是否适用于给定的参赛人数
    [[nodiscard]] bool appliesTo(int participantCount) const;

    // 获取录取名次数
    [[nodiscard]] int getRanksToAward() const;
    // 根据名次获取分数
    [[nodiscard]] double getScoreForRank(int rank) const;
    // 获取所有名次和分数
    [[nodiscard]] const std::map<int, double>& getAllScoresForRanks() const;
    [[nodiscard]] int getMinParticipants() const { return minParticipants; }
    [[nodiscard]] int getMaxParticipants() const { return maxParticipants; }
};
#endif //SCORERULE_H
