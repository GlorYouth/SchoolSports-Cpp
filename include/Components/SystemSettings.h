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
    // 新增：赛程锁定标志
    bool scheduleLocked = false;
    // 新增：场地表
    std::set<std::string> venues;                   // 所有可用场地
    // 新增：上午/下午时间段，格式08:00
    std::string morningSessionStart = "08:00";
    std::string morningSessionEnd = "12:00";
    std::string afternoonSessionStart = "14:00";
    std::string afternoonSessionEnd = "18:00";

public:
    SystemSettings();

    // --- 单位管理 ---
    bool addUnit(const std::string& unitName);
    std::optional<utils::Ref<Unit>> getUnit(int unitId);
    [[nodiscard]] std::optional<utils::RefConst<Unit>> getUnitConst(int unitId) const;
    [[nodiscard]] const std::map<int, Unit>& getAllUnits() const;
    bool removeUnit(int unitId);
    void clearUnits(); // 新增：清除所有单位数据

    // --- 运动员管理 ---
    bool addAthlete(const std::string& name, Gender gender, int unitId);
    std::optional<utils::Ref<Athlete>> getAthlete(int athleteId);
    [[nodiscard]] std::optional<utils::RefConst<Athlete>> getAthleteConst(int athleteId) const; // 添加了 const 版本
    [[nodiscard]] const std::map<int, Athlete>& getAllAthletes() const;
    bool removeAthlete(int athleteId);
    void clearAthletes(); // 新增：清除所有运动员数据

    // --- 项目管理 ---
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq);
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId);
    std::optional<utils::Ref<CompetitionEvent>> getCompetitionEvent(int eventId);
    [[nodiscard]] std::optional<utils::RefConst<CompetitionEvent>> getCompetitionEventConst(int eventId) const; // 添加了 const 版本
    [[nodiscard]] std::map<int, utils::RefConst<CompetitionEvent>> getAllCompetitionEventsConst() const;
    bool removeCompetitionEvent(int eventId);
    void clearCompetitionEvents(); // 新增：清除所有项目数据

    // --- 计分规则管理 ---
    bool addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    std::optional<utils::Ref<ScoreRule>> getScoreRule(int ruleId);
    [[nodiscard]] std::optional<utils::RefConst<ScoreRule>> getScoreRuleConst(int ruleId) const;
    std::optional<utils::Ref<ScoreRule>> findAppropriateScoreRule(int participantCount);
    [[nodiscard]] const std::map<int, ScoreRule>& getAllScoreRules() const;
    void clearScoreRules(); // 新增：清除所有计分规则


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
    void clearAllEventResults(); // 新增：清除所有项目的成绩记录

    // --- 交互操作 ---
    bool registerAthleteForEvent(int athleteId, int eventId);

    void initializeDefaultSettings(); // 初始化默认设置
    [[nodiscard]] std::vector<utils::RefConst<Athlete>> getAllAthlesConst() const;

    // 赛程锁定相关
    void lockSchedule();
    void unlockSchedule();
    bool isScheduleLocked() const;

    // --- 场地管理 ---
    // 添加场地
    bool addVenue(const std::string& venueName);
    // 移除场地
    bool removeVenue(const std::string& venueName);
    // 获取所有场地
    const std::set<std::string>& getAllVenues() const;

    // --- 上午/下午时间段管理 ---
    // 设置上午时间段
    void setMorningSession(const std::string& start, const std::string& end);
    // 设置下午时间段
    void setAfternoonSession(const std::string& start, const std::string& end);
    // 获取上午时间段
    std::pair<std::string, std::string> getMorningSession() const;
    // 获取下午时间段
    std::pair<std::string, std::string> getAfternoonSession() const;
};

#endif //SYSTEMSETTINGS_H