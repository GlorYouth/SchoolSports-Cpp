//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/ScoreRule.h"
#include <utility>

std::atomic<int> ScoreRule::nextId(1); // 初始化静态成员, ID从1开始

ScoreRule::ScoreRule(std::string  desc, int minP, int maxP, int ranks, const std::map<int, double>& scores)
    : description(std::move(desc)), minParticipants(minP), maxParticipants(maxP), ranksToAward(ranks), scoresForRanks(scores) {
    id = nextId++; // 分配唯一ID
}

int ScoreRule::getId() const {
    return id;
}

std::string ScoreRule::getDescription() const {
    return description;
}

bool ScoreRule::appliesTo(int participantCount) const {
    const bool minMet = participantCount >= minParticipants;
    const bool maxMet = (maxParticipants == -1) ? true : (participantCount <= maxParticipants); // -1 表示无上限
    return minMet && maxMet;
}

int ScoreRule::getRanksToAward() const {
    return ranksToAward;
}

double ScoreRule::getScoreForRank(int rank) const {
    if (const auto it = scoresForRanks.find(rank); it != scoresForRanks.end()) {
        return it->second;
    }
    return 0.0; // 未找到该名次的得分，返回0
}

const std::map<int, double>& ScoreRule::getAllScoresForRanks() const {
    return scoresForRanks;
}

// 实现静态方法：重置ID计数器
void ScoreRule::resetNextId(int startId) {
    nextId.store(startId);
}
