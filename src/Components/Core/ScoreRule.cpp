

#include "../../../include/Components/Core/ScoreRule.h"
#include <utility>
#include <iostream>
#include <optional>

std::atomic<int> ScoreRule::nextId(1); // 初始化静态成员, ID从1开始

ScoreRule::ScoreRule(std::string desc, int minP, int maxP, int ranks, const std::map<int, double>& scores, bool isComposite)
    : description(std::move(desc)), minParticipants(minP), maxParticipants(maxP), ranksToAward(ranks), 
      scoresForRanks(scores), isCompositeRule(isComposite) {
    id = nextId++; // 分配唯一ID
}

ScoreRule::~ScoreRule() {
    subRules.clear();
}

int ScoreRule::getId() const {
    return id;
}

std::string ScoreRule::getDescription() const {
    return description;
}

bool ScoreRule::appliesTo(int participantCount) const {
    // 如果是复合规则，只要有任一子规则适用即可
    if (isCompositeRule && !subRules.empty()) {
        for (const auto subRule : subRules) {
            if (subRule.get().appliesTo(participantCount)) {
                return true;
            }
        }
        return false; // 没有任何子规则适用
    }
    
    // 非复合规则或无子规则，使用自身的条件判断
    const bool minMet = participantCount >= minParticipants;
    const bool maxMet = (maxParticipants == -1) ? true : (participantCount <= maxParticipants); // -1 表示无上限
    return minMet && maxMet;
}

std::optional<utils::RefConst<ScoreRule>> ScoreRule::getApplicableRule(int participantCount) const {
    // 如果是复合规则，查找适用的子规则
    if (isCompositeRule && !subRules.empty()) {
        for (const auto subRule : subRules) {
            if (subRule.get().appliesTo(participantCount)) {
                return subRule.get();
            }
        }
    }
    
    // 如果没有适用的子规则或者不是复合规则，检查自身是否适用
    if (appliesTo(participantCount)) {
        return std::cref(*this);  // 返回对自身的引用
    }
    
    // 没有找到任何适用的规则
    return std::nullopt;
}

void ScoreRule::addSubRule(ScoreRule &subRule) {
    subRules.push_back(std::ref(subRule));
    isCompositeRule = true;
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

// 实现创建复合规则的静态工厂方法
ScoreRule ScoreRule::createCompositeRule(const std::string& desc, int minP, int maxP) {
    // 创建一个空的分数映射表，因为复合规则本身不直接使用分数
    std::map<int, double> emptyScores;
    // 创建并返回一个复合规则，最后参数true表示这是一个复合规则
    return ScoreRule(desc, minP, maxP, 0, emptyScores, true);
}
