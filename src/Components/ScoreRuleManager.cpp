#include "../../include/Components/ScoreRuleManager.h"
#include "../../include/Components/SystemSettings.h"
#include "../../include/utils.h"
#include <ranges>

ScoreRuleManager::ScoreRuleManager(SystemSettings& settings) : settings(settings) {}

// 内部访问方法
std::map<int, ScoreRule>& ScoreRuleManager::getRulesMap() {
    return settings._scoreRules;
}

const std::map<int, ScoreRule>& ScoreRuleManager::getRulesMapConst() const {
    return settings._scoreRules;
}

// 基本操作
bool ScoreRuleManager::add(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores) {
    ScoreRule rule(desc, minP, maxP, ranks, scores);
    int ruleId = rule.getId();
    getRulesMap().emplace(ruleId, std::move(rule));
    return true; // 目前实现总是成功，未来可能扩展验证等功能
}

bool ScoreRuleManager::addCustom(ScoreRule* rule) {
    if (!rule) {
        return false; // 空指针检查
    }
    
    int ruleId = rule->getId();
    getRulesMap().emplace(ruleId, std::move(*rule)); // 移动构造进map
    delete rule; // 删除原对象，因为已经被移动到map中
    return true;
}

void ScoreRuleManager::clear() {
    getRulesMap().clear();
    resetIdCounter(); // 重置ID计数器
    std::cout << "所有计分规则已清除。" << std::endl;
}

void ScoreRuleManager::resetIdCounter() {
    ScoreRule::resetNextId(1);
}

// 获取方法
std::optional<utils::Ref<ScoreRule>> ScoreRuleManager::get(int ruleId) {
    auto& rulesMap = getRulesMap();
    if (const auto it = rulesMap.find(ruleId); it != rulesMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<ScoreRule>> ScoreRuleManager::getConst(int ruleId) const {
    const auto& rulesMap = getRulesMapConst();
    if (const auto it = rulesMap.find(ruleId); it != rulesMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, ScoreRule>& ScoreRuleManager::getAll() const {
    return getRulesMapConst();
}

// 迭代器支持
ScoreRuleManager::iterator ScoreRuleManager::begin() {
    return getRulesMap().begin();
}

ScoreRuleManager::iterator ScoreRuleManager::end() {
    return getRulesMap().end();
}

ScoreRuleManager::const_iterator ScoreRuleManager::begin() const {
    return getRulesMapConst().begin();
}

ScoreRuleManager::const_iterator ScoreRuleManager::end() const {
    return getRulesMapConst().end();
}

// 查找方法
std::optional<utils::Ref<ScoreRule>> ScoreRuleManager::findAppropriate(int participantCount) {
    // 优先使用默认复合规则（ID=1）中的适用子规则
    auto mainRuleOpt = get(1);
    if (mainRuleOpt.has_value()) {
        ScoreRule& mainRule = mainRuleOpt.value().get();
        if (mainRule.isComposite()) {
            const ScoreRule* applicableSubRule = mainRule.getApplicableRule(participantCount);
            if (applicableSubRule && applicableSubRule->appliesTo(participantCount)) {
                // 这里有个问题：applicableSubRule是指向子规则的指针，但返回值需要是主规则的引用包装
                // 为兼容现有接口，我们返回主规则的引用
                return mainRuleOpt;
            }
        } else if (mainRule.appliesTo(participantCount)) {
            return mainRuleOpt;
        }
    }
    
    // 向后兼容：如果默认复合规则不可用或不适用，则搜索所有规则
    const auto& rulesMap = getRulesMapConst();
    for (auto &val : rulesMap | std::views::values) {
        if (val.appliesTo(participantCount)) {
            // 找到匹配的规则，返回非const引用
            return get(val.getId());
        }
    }
    
    std::cerr << "警告: 未找到适用于参赛人数 " << participantCount << " 的计分规则。" << std::endl;
    return std::nullopt; // 没有找到合适的规则
}

// 统计方法
size_t ScoreRuleManager::count() const {
    return getRulesMapConst().size();
}

bool ScoreRuleManager::empty() const {
    return getRulesMapConst().empty();
}

bool ScoreRuleManager::contains(int ruleId) const {
    return getRulesMapConst().find(ruleId) != getRulesMapConst().end();
} 