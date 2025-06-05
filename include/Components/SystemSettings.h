#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <vector>
#include <string>
#include <map>
#include <optional>
#include <set>

#include "Unit.h"
#include "CompetitionEvent.h"
#include "Athlete.h"
#include "ScoreRule.h"
#include "Result.h"
#include "../utils.h"
#include "Workflow.h"
#include "UnitManager.h"
#include "AthleteManager.h"

// 系统设置类，负责管理全局配置信息
class SystemSettings {
    friend class UnitManager;  // 允许UnitManager访问私有成员
    friend class AthleteManager; // 允许AthleteManager访问私有成员

private:
    std::map<int, Unit> _unitsMap;                   // 参赛单位列表 <UnitId, Unit>
    std::map<int, CompetitionEvent> competitionEvents; // 比赛项目列表 <EventId, CompetitionEvent>
    std::map<int, Athlete> _athletesMap;                // 运动员列表 <AthleteId, Athlete>
    std::map<int, ScoreRule> scoreRules;            // 计分规则列表 <RuleId, ScoreRule>
    std::map<int, EventResults> eventResultsMap;    // 比赛结果列表 <EventId, EventResults>

    int athleteMaxEventsAllowed;                    // 运动员允许参加的最多项目数
    bool scheduleLocked = false;                    ///< 赛程是否已锁定
    std::set<std::string> venues;                   ///< 系统中所有可用的比赛场地名称集合
    std::string morningSessionStart = "08:00";      ///< 上午比赛时间段的默认开始时间
    std::string morningSessionEnd = "12:00";        ///< 上午比赛时间段的默认结束时间
    std::string afternoonSessionStart = "14:00";    ///< 下午比赛时间段的默认开始时间
    std::string afternoonSessionEnd = "18:00";      ///< 下午比赛时间段的默认结束时间
    WorkflowStage currentWorkflowStage_ = WorkflowStage::SETUP_EVENTS;

public:
    // 代理类实例
    UnitManager units;
    AthleteManager athletes;

    SystemSettings();
    void resetAllIdCounter();

    // --- 项目管理 ---
    int addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId);
    std::optional<utils::Ref<CompetitionEvent>> getCompetitionEvent(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<CompetitionEvent>> getCompetitionEventConst(int eventId) const;
    [[nodiscard]] std::map<int, utils::RefConst<CompetitionEvent>> getAllCompetitionEventsConst() const;
    bool removeCompetitionEvent(int eventId);
    void clearCompetitionEvents();
    static void resetCompetitionEventIdCounter();

    // --- 计分规则管理 ---
    bool addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    bool addCustomScoreRule(ScoreRule* rule);
    [[nodiscard]] std::optional<utils::Ref<ScoreRule>> getScoreRule(int ruleId);
    [[nodiscard]] std::optional<utils::RefConst<ScoreRule>> getScoreRuleConst(int ruleId) const;
    std::optional<utils::Ref<ScoreRule>> findAppropriateScoreRule(int participantCount);
    [[nodiscard]] const std::map<int, ScoreRule>& getAllScoreRules() const;
    void clearScoreRules();
    static void resetScoreRuleIdCounter();

    // --- 系统参数设置 ---
    void setAthleteMaxEventsAllowed(int maxEvents);
    [[nodiscard]] int getAthleteMaxEventsAllowed() const;

    // --- 比赛结果管理 ---
    bool addOrUpdateEventResults(const EventResults& er);
    std::optional<utils::Ref<EventResults>> getEventResults(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<EventResults>> getEventResultsConst(int eventId) const;
    void clearResultsForEvent(int eventId);
    void resetAllUnitScores();
    void addScoreToUnit(int unitId, double score);
    [[nodiscard]] const std::map<int, EventResults>& getAllEventResults() const;
    void clearAllEventResults();

    // --- 交互操作 ---
    void initializeDefaultSettings();

    // --- 赛程锁定相关 ---
    void lockSchedule();
    void unlockSchedule();
    [[nodiscard]] bool isScheduleLocked() const;

    // --- 场地管理 ---
    bool addVenue(const std::string& venueName);
    bool removeVenue(const std::string& venueName);
    [[nodiscard]] const std::set<std::string>& getAllVenues() const;

    // --- 上午/下午时间段管理 ---
    void setMorningSession(const std::string& start, const std::string& end);
    void setAfternoonSession(const std::string& start, const std::string& end);
    [[nodiscard]] std::pair<std::string, std::string> getMorningSession() const;
    [[nodiscard]] std::pair<std::string, std::string> getAfternoonSession() const;

    // --- 工作流程阶段管理 ---
    [[nodiscard]] WorkflowStage getCurrentWorkflowStage() const;
    bool setWorkflowStage(WorkflowStage newStage);
};

#endif // SYSTEMSETTINGS_H