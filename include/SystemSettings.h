//
// Created by GlorYouth on 2025/6/2.
//

#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H


#include <vector>
#include <string>
#include <map>
#include <optional>

#include "Unit.h"
#include "CompetitionEvent.h"
#include "Athlete.h"
#include "ScoreRule.h"

// 系统设置类，负责管理全局配置信息
class SystemSettings {
private:
    std::map<int, Unit> units;                      // 参赛单位列表 <UnitId, Unit>
    std::map<int, CompetitionEvent> competitionEvents; // 比赛项目列表 <EventId, CompetitionEvent>
    std::map<int, Athlete> athletes;                // 运动员列表 <AthleteId, Athlete>
    std::map<int, ScoreRule> scoreRules;            // 计分规则列表 <RuleId, ScoreRule>

    int athleteMaxEventsAllowed;                    // 运动员允许参加的最多项目数
    int minParticipantsToHoldEvent;                 // 项目成立的最小参赛人数

public:
    SystemSettings();

    // --- 单位管理 ---
    bool addUnit(const std::string& unitName);
    std::optional<std::reference_wrapper<Unit>> getUnit(int unitId);
    [[nodiscard]] std::optional<std::reference_wrapper<const Unit>> getUnitConst(int unitId) const;
    [[nodiscard]] const std::map<int, Unit>& getAllUnits() const;
    bool removeUnit(int unitId); // 需要考虑移除单位时，其下运动员的处理

    // --- 运动员管理 ---
    bool addAthlete(const std::string& name, Gender gender, int unitId);
    std::optional<std::reference_wrapper<Athlete>> getAthlete(int athleteId);
    [[nodiscard]] const std::map<int, Athlete>& getAllAthletes() const;
    bool removeAthlete(int athleteId); // 需要考虑移除运动员时，其报名项目和单位内记录的处理

    // --- 项目管理 ---
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq);
    std::optional<std::reference_wrapper<CompetitionEvent>> getCompetitionEvent(int eventId);
    [[nodiscard]] const std::map<int, CompetitionEvent>& getAllCompetitionEvents() const;
    bool removeCompetitionEvent(int eventId); // 需要考虑移除项目时，已报名运动员的处理

    // --- 计分规则管理 ---
    bool addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    std::optional<std::reference_wrapper<ScoreRule>> getScoreRule(int ruleId);
    [[nodiscard]] std::optional<std::reference_wrapper<const ScoreRule>> getScoreRuleConst(int ruleId) const;
    [[nodiscard]] const std::map<int, ScoreRule>& getAllScoreRules() const;
    // 根据参赛人数获取适用的计分规则 (实际应用中可能需要更复杂的逻辑)
    std::optional<std::reference_wrapper<ScoreRule>> findAppropriateScoreRule(int participantCount);

    // --- 系统参数设置 ---
    void setAthleteMaxEventsAllowed(int maxEvents);
    [[nodiscard]] int getAthleteMaxEventsAllowed() const;

    void setMinParticipantsToHoldEvent(int minParticipants);
    [[nodiscard]] int getMinParticipantsToHoldEvent() const;

    // 初始化默认设置 (示例)
    void initializeDefaultSettings();
};

#endif //SYSTEMSETTINGS_H
