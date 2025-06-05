//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/SystemSettings.h"
#include <iostream> // 用于演示输出
#include <ranges>
#include <string>
#include <vector>
#include <map>
#include "../../include/Components/Result.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"
// Workflow.h 应该通过 SystemSettings.h 包含进来，如果直接使用 WorkflowStage 枚举则需要确保
// #include "../../include/Components/Workflow.h" // 通常不需要重复包含

SystemSettings::SystemSettings() : units(*this), athletes(*this), events(*this), athleteMaxEventsAllowed(3) {
    // 构造函数中可以进行一些初始化
    // initializeDefaultSettings(); // 可以在构造时直接初始化，或者由外部调用
}

void SystemSettings::resetAllIdCounter() {
    units.resetIdCounter();
    athletes.resetIdCounter();
    events.resetIdCounter();
    resetScoreRuleIdCounter();
}

// --- 计分规则管理 ---
bool SystemSettings::addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores) {
    ScoreRule rule(desc, minP, maxP, ranks, scores);
    int ruleId = rule.getId();
    scoreRules.emplace(ruleId, std::move(rule));
    return true; // 目前实现总是成功，未来可能扩展验证等功能
}

bool SystemSettings::addCustomScoreRule(ScoreRule* rule) {
    if (!rule) {
        return false; // 空指针检查
    }
    
    int ruleId = rule->getId();
    scoreRules.emplace(ruleId, std::move(*rule)); // 移动构造进map
    delete rule; // 删除原对象，因为已经被移动到map中
    return true;
}

std::optional<utils::Ref<ScoreRule>> SystemSettings::getScoreRule(int ruleId) {
    if (const auto it = scoreRules.find(ruleId); it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<ScoreRule>> SystemSettings::getScoreRuleConst(const int ruleId) const {
    if (const auto it = scoreRules.find(ruleId); it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}


const std::map<int, ScoreRule>& SystemSettings::getAllScoreRules() const {
    return scoreRules;
}

// 新增：重置计分规则ID计数器
void SystemSettings::resetScoreRuleIdCounter() {
    ScoreRule::resetNextId(1);
}

// 修改：清除所有计分规则
void SystemSettings::clearScoreRules() {
    scoreRules.clear();
    resetScoreRuleIdCounter(); // 重置ID计数器
    std::cout << "所有计分规则已清除。" << std::endl;
}

std::optional<utils::Ref<ScoreRule>> SystemSettings::findAppropriateScoreRule(int participantCount) {
    // 新版本：优先使用默认复合规则（ID=1）中的适用子规则
    auto mainRuleOpt = getScoreRule(1);
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
    for (auto &val : scoreRules | std::views::values) {
        if (val.appliesTo(participantCount)) {
            return val;
        }
    }
    
    std::cerr << "警告: 未找到适用于参赛人数 " << participantCount << " 的计分规则。" << std::endl;
    return std::nullopt; // 没有找到合适的规则
}

// --- 系统参数设置 ---
void SystemSettings::setAthleteMaxEventsAllowed(int maxEvents) {
    if (maxEvents > 0) {
        athleteMaxEventsAllowed = maxEvents;
    } else {
        std::cerr << "错误: 运动员最大参赛项目数必须大于0。" << std::endl;
    }
}

int SystemSettings::getAthleteMaxEventsAllowed() const {
    return athleteMaxEventsAllowed;
}

// --- 比赛结果管理 ---
bool SystemSettings::addOrUpdateEventResults(const EventResults& er) {
    if (!events.contains(er.getEventId())) {
        std::cerr << "错误: 无法为不存在的项目ID " << er.getEventId() << " 添加成绩。" << std::endl;
        return false;
    }
    eventResultsMap.insert_or_assign(er.getEventId(), er); // 插入或更新
    return true;
}

std::optional<utils::Ref<EventResults>> SystemSettings::getEventResults(int eventId) {
    if (const auto it = eventResultsMap.find(eventId); it != eventResultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<EventResults>> SystemSettings::getEventResultsConst(int eventId) const {
    if (const auto it = eventResultsMap.find(eventId); it != eventResultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, EventResults>& SystemSettings::getAllEventResults() const {
    return eventResultsMap;
}

// 清除特定项目的所有成绩，并尝试撤销已加到单位的总分
void SystemSettings::clearResultsForEvent(const int eventId) {
    if (const auto resultsIt = eventResultsMap.find(eventId); resultsIt != eventResultsMap.end()) {
        const EventResults& results_ref = resultsIt->second;
        for (const auto& result : results_ref.getResultsList()) {
            if (auto athleteOpt = athletes.getConst(result.getAthleteId())) {
                if (auto unitOpt = units.get(athleteOpt.value().get().getUnitId())) {
                    // 从单位分数中减去此成绩的分数
                    // 假设 Unit::addScore 可以接受负值来扣分
                    unitOpt.value().get().addScore(-result.getPointsAwarded()); 
                }
            }
        }
        eventResultsMap.erase(resultsIt); // 移除项目成绩记录
    }
}

// 新增：清除所有项目的成绩记录
void SystemSettings::clearAllEventResults() {
    eventResultsMap.clear();
    resetAllUnitScores(); // 清除成绩记录后，所有单位的总分也应重置
    std::cout << "所有比赛结果及单位累计分数已清除。" << std::endl;
}

void SystemSettings::resetAllUnitScores() {
    units.resetAllScores();
}

void SystemSettings::addScoreToUnit(int unitId, double score) {
    units.addScore(unitId, score);
}

// --- 交互操作 (例如报名等) ---
// 初始化默认设置 (修改后)
void SystemSettings::initializeDefaultSettings() {
    std::cout << "正在初始化系统默认核心设置..." << std::endl;

    // 1. 清空计分规则和比赛结果
    scoreRules.clear();
    eventResultsMap.clear();

    // 2. 设置系统核心参数
    setAthleteMaxEventsAllowed(3); // 每人最多报3项 (来自原逻辑)
    // 移除setMinParticipantsToHoldEvent设置，这个逻辑将由具体的计分规则决定

    // 3. 添加默认的复合计分规则（ID为1）
    ScoreRule::resetNextId(1); // 确保第一个规则的ID为1
    
    // 创建主规则（包含子规则）
    std::map<int, double> dummyScores; // 主规则不直接使用分数，而是通过子规则
    bool rule1Added = addScoreRule("默认规则: 根据参赛人数动态选择计分方案", 4, -1, 0, dummyScores);
    
    if (!rule1Added) {
        std::cerr << "严重错误: 无法添加ID为1的默认计分规则！" << std::endl;
        return;
    }
    
    // 获取主规则（ID为1）
    auto mainRuleOpt = getScoreRule(1);
    if (!mainRuleOpt.has_value()) {
        std::cerr << "严重错误: 无法获取刚刚创建的主规则（ID=1）！" << std::endl;
        return;
    }
    
    // 子规则1: 参赛人数 > 6人，取前5名 (7,5,3,2,1)
    std::map<int, double> scoresRule1 = {{1, 7.0}, {2, 5.0}, {3, 3.0}, {4, 2.0}, {5, 1.0}};
    auto* subRule1 = new ScoreRule("子规则1: 大于6人取前5名 (7,5,3,2,1)", 7, -1, 5, scoresRule1);
    
    // 子规则2: 参赛人数 <= 6人且 >= 4人，取前3名 (5,3,2)
    std::map<int, double> scoresRule2 = {{1, 5.0}, {2, 3.0}, {3, 2.0}};
    auto* subRule2 = new ScoreRule("子规则2: 4至6人取前3名 (5,3,2)", 4, 6, 3, scoresRule2);
    
    // 将子规则添加到主规则
    mainRuleOpt.value().get().addSubRule(subRule1);
    mainRuleOpt.value().get().addSubRule(subRule2);
    
    std::cout << "系统默认规则（ID=1）初始化完成，包含两个子规则：" << std::endl;
    std::cout << " - " << subRule1->getDescription() << std::endl;
    std::cout << " - " << subRule2->getDescription() << std::endl;
    std::cout << " - 注意：不足4人的项目将被自动取消" << std::endl;  // 添加说明，最小人数限制现在由规则决定

    std::cout << "系统默认核心设置（计分规则、参数）初始化完成。" << std::endl;
    std::cout << "提示：示例单位、运动员、比赛项目需通过 '导入示例数据' 选项加载。" << std::endl;
}

// --- 赛程锁定与工作流管理实现 ---
void SystemSettings::lockSchedule() {
    this->scheduleLocked = true;
    // 成功锁定赛程后，自动进入运动员报名阶段
    this->currentWorkflowStage_ = WorkflowStage::REGISTRATION_OPEN;
    // 注意：锁定赛程时需要生成赛程，但具体逻辑由外部ScheduleController实现
    // 这里我们仅改变状态，赛程生成将在调用lockSchedule的地方处理
}

void SystemSettings::unlockSchedule() {
    this->scheduleLocked = false;
    // 解锁赛程后，自动退回项目设置阶段
    this->currentWorkflowStage_ = WorkflowStage::SETUP_EVENTS;
    // std::cout << "[SystemSettings] 赛程已解锁，工作流程退回项目设置阶段。" << std::endl;
}

bool SystemSettings::isScheduleLocked() const {
    return this->scheduleLocked;
}

WorkflowStage SystemSettings::getCurrentWorkflowStage() const {
    return this->currentWorkflowStage_;
}

bool SystemSettings::setWorkflowStage(WorkflowStage newStage) {
    this->currentWorkflowStage_ = newStage;
    // std::cout << "[SystemSettings] 工作流程阶段已设置为: " << static_cast<int>(newStage) << std::endl;
    return true;
}

// --- 场地管理 ---
bool SystemSettings::addVenue(const std::string& venueName) {
    // 添加场地，若已存在则返回false
    return venues.insert(venueName).second;
}
bool SystemSettings::removeVenue(const std::string& venueName) {
    // 移除场地，若不存在则返回false
    return venues.erase(venueName) > 0;
}
const std::set<std::string>& SystemSettings::getAllVenues() const {
    return venues;
}

// --- 上午/下午时间段管理 ---
void SystemSettings::setMorningSession(const std::string& start, const std::string& end) {
    morningSessionStart = start;
    morningSessionEnd = end;
}
void SystemSettings::setAfternoonSession(const std::string& start, const std::string& end) {
    afternoonSessionStart = start;
    afternoonSessionEnd = end;
}
std::pair<std::string, std::string> SystemSettings::getMorningSession() const {
    return {morningSessionStart, morningSessionEnd};
}
std::pair<std::string, std::string> SystemSettings::getAfternoonSession() const {
    return {afternoonSessionStart, afternoonSessionEnd};
}
