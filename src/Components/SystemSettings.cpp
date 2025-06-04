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

SystemSettings::SystemSettings() : athleteMaxEventsAllowed(3), minParticipantsToHoldEvent(4) {
    // 构造函数中可以进行一些初始化
    // initializeDefaultSettings(); // 可以在构造时直接初始化，或者由外部调用
}

// --- 单位管理 ---
bool SystemSettings::addUnit(const std::string& unitName) {
    // 检查单位名是否已存在
    for (const auto &val : units | std::views::values) {
        if (val.getName() == unitName) {
            std::cerr << "错误: 单位名称 '" << unitName << "' 已存在。" << std::endl;
            return false; // 单位名已存在
        }
    }
    Unit newUnit(unitName);
    units.insert({newUnit.getId(), newUnit});
    return true;
}

std::optional<utils::Ref<Unit>> SystemSettings::getUnit(const int unitId) {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<Unit>> SystemSettings::getUnitConst(const int unitId) const {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Unit>& SystemSettings::getAllUnits() const {
    return units;
}

bool SystemSettings::removeUnit(int unitId) {
    // 注意：移除单位前，应处理该单位下的运动员
    // 还需要处理相关的成绩、报名信息等，这里简化处理
    auto unitToRemove = units.find(unitId);
    if (unitToRemove == units.end()) {
        return false; // 单位不存在
    }

    // 移除该单位下的所有运动员
    std::vector<int> athletesToRemove = unitToRemove->second.getAthleteIds();
    // 使用迭代器或基于索引的循环来安全地移除，避免在遍历时修改导致的问题
    for (size_t i = 0; i < athletesToRemove.size(); ++i) {
        removeAthlete(athletesToRemove[i]); // 调用 removeAthlete 来处理相关清理
    }

    return units.erase(unitId) > 0;
}

// 新增：清除所有单位数据
void SystemSettings::clearUnits() {
    // 清除单位前，需要确保所有关联的运动员也被处理
    // 为避免复杂循环依赖和迭代器失效，可以先收集所有单位ID，然后逐个移除
    std::vector<int> unitIds;
    for(const auto& pair : units) {
        unitIds.push_back(pair.first);
    }
    for(int id : unitIds) {
        removeUnit(id); // removeUnit 应该处理其下的运动员
    }
    units.clear(); // 最后确保 map 清空
    // Unit::nextId = 1; // 如果需要重置ID计数器（通常用于测试）
}


// --- 运动员管理 ---
bool SystemSettings::addAthlete(const std::string& name, Gender gender, int unitId) {
    const auto optional_unit = getUnit(unitId);
    if (!optional_unit.has_value()) {
        std::cerr << "错误: 添加运动员失败，单位ID " << unitId << " 不存在。" << std::endl;
        return false; // 单位不存在
    }
    Athlete newAthlete(name, gender, unitId);
    athletes.insert({newAthlete.getId(), newAthlete});
    optional_unit.value().get().addAthleteId(newAthlete.getId()); // 将运动员ID关联到单位
    return true;
}

std::optional<utils::Ref<Athlete>> SystemSettings::getAthlete(const int athleteId) {
    if (const auto it = athletes.find(athleteId); it != athletes.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<Athlete>> SystemSettings::getAthleteConst(const int athleteId) const {
    if (const auto it = athletes.find(athleteId); it != athletes.end()) {
        return it->second;
    }
    return std::nullopt;
}


const std::map<int, Athlete>& SystemSettings::getAllAthletes() const {
    return athletes;
}

bool SystemSettings::removeAthlete(const int athleteId) {
    const auto athleteIt = athletes.find(athleteId);
    if (athleteIt == athletes.end()) {
        return false; // 运动员不存在
    }
    const Athlete& athlete_ref = athleteIt->second;

    // 从所属单位中移除运动员ID
    if (const auto unit = getUnit(athlete_ref.getUnitId())) {
        unit.value().get().removeAthleteId(athleteId);
    }

    // 从所有其报名的项目中移除该运动员的参与记录
    for (const int eventId : athlete_ref.getRegisteredEventIds()) {
        if (auto event = getCompetitionEvent(eventId); event.has_value()) {
            event.value().get().removeParticipant(athleteId);
        }
    }

    // 从成绩记录中移除该运动员的成绩 (如果需要)
    // 这是一个更复杂的操作，可能需要遍历所有EventResults
    // 为简化，此处不直接处理成绩的移除，成绩查询时若运动员不存在则忽略

    return athletes.erase(athleteId) > 0;
}

// 新增：清除所有运动员数据
void SystemSettings::clearAthletes() {
    // 移除运动员会处理其在单位和项目中的关联
    std::vector<int> athleteIds;
    for(const auto& pair : athletes) {
        athleteIds.push_back(pair.first);
    }
    for(int id : athleteIds) {
        removeAthlete(id);
    }
    athletes.clear(); // 最后确保 map 清空
    // Athlete::nextId = 1; // 如果需要重置ID计数器
}

// --- 项目管理 ---
bool SystemSettings::addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq) {
    CompetitionEvent newEvent(eventName, type, genderReq);
    competitionEvents.insert({newEvent.getId(), newEvent});
    return true;
}

// 新增重载：允许直接指定计分规则ID
bool SystemSettings::addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId) {
    CompetitionEvent newEvent(eventName, type, genderReq);
    newEvent.setScoreRuleId(scoreRuleId); // 设置计分规则ID
    competitionEvents.insert({newEvent.getId(), newEvent});
    return true;
}

std::optional<utils::Ref<CompetitionEvent>> SystemSettings::getCompetitionEvent(int eventId) {
    if (const auto it = competitionEvents.find(eventId); it != competitionEvents.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<utils::RefConst<CompetitionEvent>> SystemSettings::getCompetitionEventConst(int eventId) const {
    if (const auto it = competitionEvents.find(eventId); it != competitionEvents.end()) {
        return it->second;
    }
    return std::nullopt;
}

    std::map<int, utils::RefConst<CompetitionEvent>> SystemSettings::getAllCompetitionEventsConst() const {
        auto map = std::map<int, utils::RefConst<CompetitionEvent>>();
        for (const auto& [fst, snd] : competitionEvents){
            map.insert({fst, std::cref(snd)});
        }
        return map;
    }

bool SystemSettings::removeCompetitionEvent(const int eventId) {
    const auto eventIt = competitionEvents.find(eventId);
    if (eventIt == competitionEvents.end()) {
        return false; // 项目不存在
    }
    const CompetitionEvent& event_ref = eventIt->second;

    // 从所有已报名该项目的运动员的报名列表中移除该项目
    for (const int athleteId : event_ref.getParticipantAthleteIds()) {
        if (auto athlete = getAthlete(athleteId); athlete.has_value()) {
            athlete.value().get().unregisterFromEvent(eventId);
        }
    }

    // 移除该项目的成绩记录
    eventResultsMap.erase(eventId);

    return competitionEvents.erase(eventId) > 0;
}

// 新增：清除所有比赛项目数据
void SystemSettings::clearCompetitionEvents() {
    // 移除项目会处理其运动员关联和成绩
    std::vector<int> eventIds;
    for(const auto& pair : competitionEvents) {
        eventIds.push_back(pair.first);
    }
    for(int id : eventIds) {
        removeCompetitionEvent(id);
    }
    competitionEvents.clear(); // 最后确保 map 清空
    // CompetitionEvent::nextId = 1; // 如果需要重置ID计数器
}

// --- 计分规则管理 ---
bool SystemSettings::addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores) {
    ScoreRule newRule(desc, minP, maxP, ranks, scores);
    scoreRules.insert({newRule.getId(), newRule});
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

// 新增：清除所有计分规则
void SystemSettings::clearScoreRules() {
    scoreRules.clear();
    // ScoreRule::nextId = 1; // 如果需要重置ID计数器（通常用于测试，且ScoreRule类需提供此功能）
    std::cout << "所有计分规则已清除。" << std::endl;
}

std::optional<utils::Ref<ScoreRule>> SystemSettings::findAppropriateScoreRule(int participantCount) {
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

void SystemSettings::setMinParticipantsToHoldEvent(int minParticipants) {
     if (minParticipants >= 0) { // 允许为0，表示没有限制，但通常至少为1或更高
        minParticipantsToHoldEvent = minParticipants;
    } else {
        std::cerr << "错误: 项目最少举行人数不能为负值。" << std::endl; // 修正为"负值"
    }
}

int SystemSettings::getMinParticipantsToHoldEvent() const {
    return minParticipantsToHoldEvent;
}

// --- 比赛结果管理 ---
bool SystemSettings::addOrUpdateEventResults(const EventResults& er) {
    if (!competitionEvents.contains(er.getEventId())) {
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
            if (auto athleteOpt = getAthleteConst(result.getAthleteId())) {
                if (auto unitOpt = getUnit(athleteOpt.value().get().getUnitId())) {
                    // 从单位分数中减去此成绩的分数
                    // 假设 Unit::addScore 可以接受负值来扣分
                    unitOpt.value().get().addScore(-result.getPointsAwarded()); 
                }
            }
        }
        eventResultsMap.erase(resultsIt); // 移除项目成绩记录
        // std::cout << "调试: 已清除项目ID " << eventId << " 的成绩及其对单位总分的影响。" << std::endl;
    } else {
        // std::cout << "调试: 项目ID " << eventId << " 没有成绩记录可清除。" << std::endl;
    }

    // 可选：如果项目本身还存在，可能需要重置其某些状态，例如关联的计分规则ID
    // 不过通常清除成绩不意味着项目本身逻辑的重置，除非业务需求如此
    // if (const auto eventOpt = getCompetitionEvent(eventId)) {
    //     eventOpt.value().get().setScoreRuleId(-1); // 假设-1表示未关联或需要重新计算
    // }
}

// 新增：清除所有项目的成绩记录
void SystemSettings::clearAllEventResults() {
    eventResultsMap.clear();
    resetAllUnitScores(); // 清除成绩记录后，所有单位的总分也应重置
    std::cout << "所有比赛结果及单位累计分数已清除。" << std::endl;
}

void SystemSettings::resetAllUnitScores() {
    for (auto &val : units | std::views::values) {
        val.resetScore();
    }
    // std::cout << "所有单位的总分已重置。" << std::endl;
}

// 新增：为指定单位增加分数
void SystemSettings::addScoreToUnit(int unitId, double score) {
    if (const auto unitOpt = getUnit(unitId)) {
        unitOpt.value().get().addScore(score);
        // 可选: 可以在此处添加日志或成功消息，但通常调用方会处理用户反馈
        // std::cout << "成功为单位ID " << unitId << " 增加了 " << score << " 分。" << std::endl;
    } else {
        std::cerr << "错误: 尝试为不存在的单位ID " << unitId << " 加分失败。" << std::endl;
    }
}


// --- 交互操作 (例如报名等) ---
// 新增：为运动员报名项目，并更新项目和运动员的关联
bool SystemSettings::registerAthleteForEvent(int athleteId, int eventId) {
    auto athleteOpt = getAthlete(athleteId);
    auto eventOpt = getCompetitionEvent(eventId);

    if (!athleteOpt) {
        std::cerr << "错误：报名失败，运动员ID " << athleteId << " 未找到。" << std::endl;
        return false;
    }
    if (!eventOpt) {
        std::cerr << "错误：报名失败，项目ID " << eventId << " 未找到。" << std::endl;
        return false;
    }

    Athlete& athlete = athleteOpt.value().get();
    CompetitionEvent& event = eventOpt.value().get();

    // 检查性别是否符合
    if (!event.canAthleteRegister(athlete.getGender())) {
        std::cerr << "错误：运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 的性别不符合项目 " << event.getName() << " (ID: " << eventId << ") 的要求。" << std::endl;
        return false;
    }

    // 检查运动员报名项目数量是否超限
    if (athlete.getRegisteredEventsCount() >= athleteMaxEventsAllowed) {
        std::cerr << "错误：运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 已达到最大报名项目数 (" << athleteMaxEventsAllowed << ")。" << std::endl;
        return false;
    }

    // 尝试将运动员注册到项目中
    if (!athlete.registerForEvent(eventId, athleteMaxEventsAllowed)) {
        // registerForEvent 内部可能已打印错误，或有其他逻辑 (如重复报名)
        // 此处可以根据需要添加额外日志或直接返回其结果
        std::cerr << "运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 报名项目 " << event.getName() << " (ID: " << eventId << ") 失败（运动员侧）。" << std::endl;
        return false;
    }

    // 尝试将项目添加到运动员的参与列表
    if (!event.addParticipant(athleteId)) {
        // 如果 addParticipant 失败，需要回滚运动员的报名操作
        athlete.unregisterFromEvent(eventId);
        std::cerr << "运动员 " << athlete.getName() << " (ID: " << athleteId
                  << ") 报名项目 " << event.getName() << " (ID: " << eventId << ") 失败（项目侧）。" << std::endl;
        return false;
    }

    std::cout << "信息：运动员 " << athlete.getName() << " (ID: " << athleteId
              << ") 成功报名项目 " << event.getName() << " (ID: " << eventId << ")。" << std::endl;
    return true;
}

// 初始化默认设置 (修改后)
void SystemSettings::initializeDefaultSettings() {
    std::cout << "正在初始化系统默认核心设置..." << std::endl;

    // 1. 清空计分规则和比赛结果 (单位、运动员、项目由DataManager::loadSampleData负责，此处不处理)
    scoreRules.clear();
    eventResultsMap.clear();
    // 如果需要重置计分规则ID计数器，可以在此操作，例如：
    // ScoreRule::nextId = 1; // 假设ID从1开始，并且可以重置。请确保这符合您的ID管理策略。
    // **重要**: 重置静态ID计数器 (如 Unit::nextId) 应该在所有相关对象都被清除后，
    // 并且通常在测试或完全重置系统状态时才这样做。对于默认设置，可能只需要确保
    // 默认计分规则的ID是固定的 (如 ID=1)。
    // 如果 ScoreRule 的ID不是从1开始或者不是按顺序，那么 DataManager::loadSampleData 中
    // 对 defaultScoreRuleId = 1 的假设就需要调整，或者这里需要确保ID为1的规则被创建。

    // 2. 设置系统核心参数
    setAthleteMaxEventsAllowed(3); // 每人最多报3项 (来自原逻辑)
    setMinParticipantsToHoldEvent(4); // 不足4人的项目将取消 (来自原逻辑)

    // 3. 添加默认的计分规则 (确保ID为1的规则存在且符合预期)
    // 规则1 (ID将是1，如果ScoreRule::nextId从1开始且未被其他规则占用): 参赛人数 > 6人 (即 >= 7人)，取前5名
    std::map<int, double> scoresRule1 = {{1, 7.0}, {2, 5.0}, {3, 3.0}, {4, 2.0}, {5, 1.0}};
    // 为了确保ID为1，我们需要一种方式来指定ID，或者依赖于ScoreRule的ID生成机制。
    // 假设ScoreRule构造函数不接受ID，且ID由静态计数器生成，则第一个添加的规则ID为1。
    bool rule1Added = addScoreRule("默认规则1: 大于6人取前5名 (7,5,3,2,1)", 7, -1, 5, scoresRule1);
    if (!rule1Added) {
        std::cerr << "严重错误: 无法添加ID为1的默认计分规则！" << std::endl;
        // 可能需要抛出异常或采取其他错误处理措施
    }
    // 您可以检查 getScoreRuleConst(1) 是否有效，以确保ID为1的规则已正确添加。

    // 规则2 (ID将是2): 参赛人数 <= 6人 (且 >= 4人), 取前3名
    std::map<int, double> scoresRule2 = {{1, 5.0}, {2, 3.0}, {3, 2.0}};
    addScoreRule("默认规则2: 4至6人取前3名 (5,3,2)", 4, 6, 3, scoresRule2);
    // 可以根据需要添加更多核心的、不属于"示例数据"的计分规则

    // 4. 重置所有单位分数（如果单位数据此时不应存在，此步骤可能不需要，
    // 或者应在 DataManager::loadSampleData 之后由调用方根据需要执行）
    // resetAllUnitScores(); // 考虑到单位数据由 loadSampleData 加载，此处可能不应重置。

    std::cout << "系统默认核心设置（计分规则、参数）初始化完成。" << std::endl;
    std::cout << "提示：示例单位、运动员、比赛项目需通过 '导入示例数据' 选项加载。" << std::endl;
}

std::vector<utils::RefConst<Athlete>> SystemSettings::getAllAthlesConst() const {
    std::vector<utils::RefConst<Athlete>> refs;
    refs.reserve(athletes.size()); // athletes_ 是 std::map<int, Athlete> m_athletes;
    for (const auto& val : athletes | std::views::values) {
        refs.push_back(std::cref(val)); // 从 pair 中取出 Athlete 对象
    }
    return refs;
}

// --- 赛程锁定与工作流管理实现 ---
void SystemSettings::lockSchedule() {
    this->scheduleLocked = true;
    // 成功锁定赛程后，自动进入运动员报名阶段
    this->currentWorkflowStage_ = WorkflowStage::REGISTRATION_OPEN;
    // std::cout << "[SystemSettings] 赛程已锁定，工作流程进入运动员报名阶段。" << std::endl;
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
