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
#include "../../utils.h"
#include "Workflow.h"
#include "../Manager/UnitManager.h"
#include "../Manager/AthleteManager.h"
#include "../Manager/CompetitionEventManager.h"
#include "../Manager/ScoreRuleManager.h"
#include "../Manager/EventResultsManager.h"
#include "../Manager/WorkflowManager.h"
#include "../Manager/SessionManager.h"
#include "../Manager/VenueManager.h"
#include "../Manager/ScheduleLockManager.h"
#include "../Manager/SystemArgsManager.h"

// 系统设置类，负责管理全局配置信息
class SystemSettings {
    friend class UnitManager;  // 允许UnitManager访问私有成员
    friend class AthleteManager; // 允许AthleteManager访问私有成员
    friend class CompetitionEventManager; // 允许CompetitionEventManager访问私有成员
    friend class ScoreRuleManager; // 允许ScoreRuleManager访问私有成员
    friend class EventResultsManager; // 允许EventResultsManager访问私有成员
    friend class WorkflowManager; // 允许WorkflowManager访问私有成员
    friend class SessionManager; // 允许SessionManager访问私有成员
    friend class VenueManager; // 允许VenueManager访问私有成员
    friend class ScheduleLockManager; // 允许ScheduleLockManager访问私有成员
    friend class SystemArgsManager; // 允许SystemArgsManager访问私有成员

private:
    std::map<int, Unit> _unitsMap;                   // 参赛单位列表 <UnitId, Unit>
    std::map<int, CompetitionEvent> _competitionEvents; // 比赛项目列表 <EventId, CompetitionEvent>
    std::map<int, Athlete> _athletesMap;                // 运动员列表 <AthleteId, Athlete>
    std::map<int, ScoreRule> _scoreRules;            // 计分规则列表 <RuleId, ScoreRule>
    std::map<int, EventResults> _eventResultsMap;    // 比赛结果列表 <EventId, EventResults>

    int _athleteMaxEventsAllowed;                    // 运动员允许参加的最多项目数
    bool _scheduleLocked = false;                    ///< 赛程是否已锁定
    std::set<std::string> _venues;                   ///< 系统中所有可用的比赛场地名称集合
    std::string _morningSessionStart = "08:00";      ///< 上午比赛时间段的默认开始时间
    std::string _morningSessionEnd = "12:00";        ///< 上午比赛时间段的默认结束时间
    std::string _afternoonSessionStart = "14:00";    ///< 下午比赛时间段的默认开始时间
    std::string _afternoonSessionEnd = "18:00";      ///< 下午比赛时间段的默认结束时间
    WorkflowStage _currentWorkflowStage = WorkflowStage::SETUP_EVENTS;

public:
    // 代理类实例
    UnitManager units;
    AthleteManager athletes;
    CompetitionEventManager events;
    ScoreRuleManager rules;
    EventResultsManager results;
    WorkflowManager workflow;
    SessionManager sessions;
    VenueManager venues;
    ScheduleLockManager schedule;
    SystemArgsManager args;

    SystemSettings();
    void resetAllIdCounter() const;
    void clearAllData();

    // --- 系统参数设置 ---
    void setAthleteMaxEventsAllowed(int maxEvents);
    [[nodiscard]] int getAthleteMaxEventsAllowed() const;


    // --- 交互操作 ---
    void initializeDefaultSettings();

};

#endif // SYSTEMSETTINGS_H