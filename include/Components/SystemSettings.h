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
#include "Result.h"
#include "../utils.h"

// 系统设置类，负责管理全局配置信息
class SystemSettings {
private:
    std::map<int, Unit> units;                      // 参赛单位列表 <UnitId, Unit>
    std::map<int, CompetitionEvent> competitionEvents; // 比赛项目列表 <EventId, CompetitionEvent>
    std::map<int, Athlete> athletes;                // 运动员列表 <AthleteId, Athlete>
    std::map<int, ScoreRule> scoreRules;            // 计分规则列表 <RuleId, ScoreRule>
    std::map<int, EventResults> eventResultsMap;    // 比赛结果列表 <EventId, EventResults>

    int athleteMaxEventsAllowed;                    // 运动员允许参加的最多项目数
    int minParticipantsToHoldEvent;                 // 项目成立的最小参赛人数

public:
    SystemSettings();

    // --- 单位管理 ---
    bool addUnit(const std::string& unitName);
    std::optional<utils::Ref<Unit>> getUnit(int unitId);
    [[nodiscard]] std::optional<utils::RefConst<Unit>> getUnitConst(int unitId) const;
    [[nodiscard]] const std::map<int, Unit>& getAllUnits() const;
    bool removeUnit(int unitId);

    // --- 运动员管理 ---
    bool addAthlete(const std::string& name, Gender gender, int unitId);
    std::optional<utils::Ref<Athlete>> getAthlete(int athleteId);
    [[nodiscard]] std::optional<utils::RefConst<Athlete>> getAthleteConst(int athleteId) const; // 添加了 const 版本
    [[nodiscard]] const std::map<int, Athlete>& getAllAthletes() const;
    bool removeAthlete(int athleteId);

    // --- 项目管理 ---
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq);
    std::optional<utils::Ref<CompetitionEvent>> getCompetitionEvent(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<CompetitionEvent>> getCompetitionEventConst(int eventId) const; // 添加了 const 版本
    [[nodiscard]] const std::map<int, CompetitionEvent>& getAllCompetitionEvents() const;
    bool removeCompetitionEvent(int eventId);

    // --- 计分规则管理 ---
    bool addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    std::optional<utils::Ref<ScoreRule>> getScoreRule(int ruleId);
    [[nodiscard]] std::optional<utils::RefConst<ScoreRule>> getScoreRuleConst(int ruleId) const;
    std::optional<utils::Ref<ScoreRule>> findAppropriateScoreRule(int participantCount);
    [[nodiscard]] const std::map<int, ScoreRule>& getAllScoreRules() const;


    // --- 系统参数设置 ---
    void setAthleteMaxEventsAllowed(int maxEvents);
    [[nodiscard]] int getAthleteMaxEventsAllowed() const;

    void setMinParticipantsToHoldEvent(int minParticipants);
    [[nodiscard]] int getMinParticipantsToHoldEvent() const;

    // --- 比赛结果管理 ---
    bool addOrUpdateEventResults(const EventResults& er);
    std::optional<utils::Ref<EventResults>> getEventResults(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<EventResults>> getEventResultsConst(int eventId) const;
    void clearResultsForEvent(int eventId); // 用于重新计算时清除旧成绩
    void resetAllUnitScores(); // 工具函数，重置所有单位分数
    void addScoreToUnit(int unitId, double score); // 新增：为指定单位增加分数
    [[nodiscard]] const std::map<int, EventResults>& getAllEventResults() const; // 获取所有成绩的 getter 方法


    void initializeDefaultSettings(); // 初始化默认设置
    [[nodiscard]] std::vector<utils::RefConst<Athlete>> getAllAthlesConst() const;
};

#endif //SYSTEMSETTINGS_H