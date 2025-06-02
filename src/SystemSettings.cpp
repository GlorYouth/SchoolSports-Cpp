//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/SystemSettings.h"
#include <iostream> // 用于演示输出
#include <ranges>

SystemSettings::SystemSettings() : athleteMaxEventsAllowed(3), minParticipantsToHoldEvent(4) {
    // 构造函数中可以进行一些初始化
    // 例如，加载默认配置或从文件加载配置
}

// --- 单位管理 ---
bool SystemSettings::addUnit(const std::string& unitName) {
    // 检查单位名是否已存在 (简单实现，实际可能需要更鲁棒的检查)
    for (const auto &val: units | std::views::values) {
        if (val.getName() == unitName) {
            return false; // 单位名已存在
        }
    }
    Unit newUnit(unitName);
    units.insert({newUnit.getId(), newUnit});
    return true;
}

std::optional<std::reference_wrapper<Unit>> SystemSettings::getUnit(const int unitId) {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const Unit>> SystemSettings::getUnitConst(const int unitId) const {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Unit>& SystemSettings::getAllUnits() const {
    return units;
}

bool SystemSettings::removeUnit(int unitId) {
    // 注意：移除单位前，应处理该单位下的运动员 (例如：将其移至“未分配”或报错)
    // 此处为简化实现
    return units.erase(unitId) > 0;
}


// --- 运动员管理 ---
bool SystemSettings::addAthlete(const std::string& name, Gender gender, int unitId) {
    const auto optional_unit = getUnit(unitId);
    if (!optional_unit.has_value()) {
        return false; // 单位不存在
    }
    Athlete newAthlete(name, gender, unitId);
    athletes.insert({newAthlete.getId(), newAthlete});
    optional_unit.value().get().addAthleteId(newAthlete.getId()); // 将运动员ID关联到单位
    return true;
}

std::optional<std::reference_wrapper<Athlete>> SystemSettings::getAthlete(const int athleteId) {
    if (const auto it = athletes.find(athleteId); it != athletes.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Athlete>& SystemSettings::getAllAthletes() const {
    return athletes;
}

bool SystemSettings::removeAthlete(const int athleteId) {
    const auto athlete = getAthlete(athleteId);
    if (!athlete.has_value()) return false;
    const auto athlete_ref = athlete.value().get();

    // 从所属单位中移除
    if (const auto unit = getUnit(athlete_ref.getUnitId())) {
        unit.value().get().removeAthleteId(athleteId);
    }
    // 从所有其报名的项目中移除
    for (const int eventId : athlete_ref.getRegisteredEventIds()) {
        if (auto event = getCompetitionEvent(eventId); event.has_value()) {
            event.value().get().removeParticipant(athleteId);
        }
    }
    return athletes.erase(athleteId) > 0;
}

// --- 项目管理 ---
bool SystemSettings::addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq) {
    CompetitionEvent newEvent(eventName, type, genderReq);
    competitionEvents.insert({newEvent.getId(), newEvent});
    return true;
}

std::optional<std::reference_wrapper<CompetitionEvent>> SystemSettings::getCompetitionEvent(int eventId) {
    const auto it = competitionEvents.find(eventId);
    if (it != competitionEvents.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, CompetitionEvent>& SystemSettings::getAllCompetitionEvents() const {
    return competitionEvents;
}

bool SystemSettings::removeCompetitionEvent(const int eventId) {
    // 注意：移除项目前，应处理已报名该项目的运动员 (例如：从运动员的报名列表中移除)
    // 此处为简化实现
    const auto event = getCompetitionEvent(eventId);
    if (!event) return false;

    for (const int athleteId : event.value().get().getParticipantAthleteIds()) {
        if (auto athlete = getAthlete(athleteId); athlete.has_value()) {
            athlete.value().get().unregisterFromEvent(eventId);
        }
    }
    return competitionEvents.erase(eventId) > 0;
}

// --- 计分规则管理 ---
bool SystemSettings::addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores) {
    ScoreRule newRule(desc, minP, maxP, ranks, scores);
    scoreRules.insert({newRule.getId(), newRule});
    return true;
}

std::optional<std::reference_wrapper<ScoreRule>> SystemSettings::getScoreRule(int ruleId) {
    if (const auto it = scoreRules.find(ruleId); it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const ScoreRule>> SystemSettings::getScoreRuleConst(const int ruleId) const {
    auto it = scoreRules.find(ruleId);
    if (it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, ScoreRule>& SystemSettings::getAllScoreRules() const {
    return scoreRules;
}

std::optional<std::reference_wrapper<ScoreRule>> SystemSettings::findAppropriateScoreRule(int participantCount) {
    for (auto &val: scoreRules | std::views::values) {
        if (val.appliesTo(participantCount)) {
            return val;
        }
    }
    return std::nullopt; // 没有找到合适的规则
}

// --- 系统参数设置 ---
void SystemSettings::setAthleteMaxEventsAllowed(int maxEvents) {
    athleteMaxEventsAllowed = maxEvents;
}

int SystemSettings::getAthleteMaxEventsAllowed() const {
    return athleteMaxEventsAllowed;
}

void SystemSettings::setMinParticipantsToHoldEvent(int minParticipants) {
    minParticipantsToHoldEvent = minParticipants;
}

int SystemSettings::getMinParticipantsToHoldEvent() const {
    return minParticipantsToHoldEvent;
}

// 初始化默认设置 (示例)
void SystemSettings::initializeDefaultSettings() {
    std::cout << "正在初始化系统默认设置..." << std::endl;

    // 1. 添加参赛单位
    addUnit("计算机学院");
    addUnit("外国语学院");
    addUnit("体育学院");

    // 2. 添加比赛项目
    addCompetitionEvent("男子100米", EventType::TRACK, Gender::MALE);
    addCompetitionEvent("女子100米", EventType::TRACK, Gender::FEMALE);
    addCompetitionEvent("男子跳高", EventType::FIELD, Gender::MALE);
    addCompetitionEvent("女子铅球", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子4x100米接力", EventType::TRACK, Gender::MALE); // 示例接力项目

    // 3. 设置运动员参赛项目限制
    setAthleteMaxEventsAllowed(3); // 每人最多报3项

    // 4. 设置计分规则
    // 规则1: 参赛人数 > 6人，取前5名
    std::map<int, double> scores1 = {{1, 7.0}, {2, 5.0}, {3, 3.0}, {4, 2.0}, {5, 1.0}};
    addScoreRule("超过6人取前5名", 7, -1, 5, scores1); // -1 表示无人数上限

    // 规则2: 参赛人数 <= 6人 (且 >= 4人，因为不足4人取消), 取前3名
    std::map<int, double> scores2 = {{1, 5.0}, {2, 3.0}, {3, 2.0}};
    // 注意minParticipantsToHoldEvent是4，所以这里minP可以设为4
    addScoreRule("4-6人取前3名", getMinParticipantsToHoldEvent(), 6, 3, scores2);

    // 5. 项目成立的最小参赛人数
    setMinParticipantsToHoldEvent(4); // 不足4人的项目将取消

    std::cout << "系统默认设置初始化完成。" << std::endl;
}

