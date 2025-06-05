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
            if (subRule.appliesTo(participantCount)) {
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
    // 先判断自身是否适用(非复合规则)
    if (!isCompositeRule) {
        if (appliesTo(participantCount)) {
            return std::cref(*this);  // 返回自身的引用
        } else {
            return std::nullopt;
        }
    }
    
    // 对于复合规则，如果没有子规则，检查自身
    if (subRules.empty()) {
        if (appliesTo(participantCount)) {
            return std::cref(*this);
        } else {
            return std::nullopt;
        }
    }
    
    for (const auto& subRule : subRules) {

        // 检查子规则是否适用

        if (subRule.appliesTo(participantCount)) {
            // 如果子规则不是复合规则，直接返回
            if (!subRule.isCompositeRule) {
                return std::cref(subRule);
            }
            if (!subRule.subRules.empty()) {
                // 如果子规则是复合规则且有子规则，递归查找
                return subRule.getApplicableRule(participantCount);
            }
            // 没有找到适用的规则
            return std::nullopt;
        }
    }
    
    // 没有找到适用的规则
    return std::nullopt;
}

void ScoreRule::addSubRule(ScoreRule &subRule) {
    // 防止添加自身为子规则 - 这会导致循环引用
    if (&subRule == this || subRule.getId() == this->getId()) {
        std::cerr << "错误: 尝试将规则ID " << id << " 添加为自身的子规则，这会导致循环引用。操作已被忽略" << std::endl;
        return;
    }

    // 检查是否已经添加过该子规则，避免重复
    for (const auto& existingRule : subRules) {
        try {
            if (existingRule.getId() == subRule.getId()) {
                std::cerr << "警告: 规则ID " << subRule.getId() << " 已经是规则ID " 
                         << id << " 的子规则，不重复添加" << std::endl;
                return;
            }
        } catch (...) {
            // 忽略无效引用的比较
        }
    }

    // 添加子规则并标记为复合规则
    subRules.push_back(subRule);
    isCompositeRule = true;
    
    std::cout << "已将规则ID " << subRule.getId() << " 添加为规则ID " << id << " 的子规则" << std::endl;
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
