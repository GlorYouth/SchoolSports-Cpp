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
#include "Workflow.h" // 新增：引入工作流程阶段定义

// 系统设置类，负责管理全局配置信息
/**
 * @brief 系统核心数据和全局配置管理器。
 *
 * SystemSettings 类是整个运动会管理系统的中枢，负责存储和管理所有关键数据实体，
 * 包括参赛单位 (Unit)、运动员 (Athlete)、比赛项目 (CompetitionEvent)、
 * 计分规则 (ScoreRule) 和比赛结果 (EventResults)。
 * 它还管理全局性的配置参数，如运动员允许报名的最大项目数、项目成立的最小人数、
 * 赛程锁定状态、可用场地列表以及上下午的比赛时间段定义。
 *
 * 此类提供了对这些数据的增、删、改、查接口，并作为其他模块（如报名、赛程、计分）的数据源和配置源。
 */
class SystemSettings {
private:
    std::map<int, Unit> units;                      // 参赛单位列表 <UnitId, Unit>
    std::map<int, CompetitionEvent> competitionEvents; // 比赛项目列表 <EventId, CompetitionEvent>
    std::map<int, Athlete> athletes;                // 运动员列表 <AthleteId, Athlete>
    std::map<int, ScoreRule> scoreRules;            // 计分规则列表 <RuleId, ScoreRule>
    std::map<int, EventResults> eventResultsMap;    // 比赛结果列表 <EventId, EventResults>

    int athleteMaxEventsAllowed;                    // 运动员允许参加的最多项目数
    bool scheduleLocked = false;                    ///< 赛程是否已锁定。如果为true，项目信息不可编辑，报名流程会进行时间冲突检查。
    std::set<std::string> venues;                   ///< 系统中所有可用的比赛场地名称集合。
    std::string morningSessionStart = "08:00";      ///< 上午比赛时间段的默认开始时间。
    std::string morningSessionEnd = "12:00";        ///< 上午比赛时间段的默认结束时间。
    std::string afternoonSessionStart = "14:00";    ///< 下午比赛时间段的默认开始时间。
    std::string afternoonSessionEnd = "18:00";      ///< 下午比赛时间段的默认结束时间。
    WorkflowStage currentWorkflowStage_ = WorkflowStage::SETUP_EVENTS;

public:
    /**
     * @brief SystemSettings 类的构造函数。
     * 构造时会进行一些默认初始化，例如设置运动员最大报名项目数等。
     * 默认计分规则等更复杂的初始化通过 `initializeDefaultSettings()` 完成。
     */
    SystemSettings();

    // --- 单位管理 ---
    /**
     * @brief 添加一个新的参赛单位。
     * @param unitName 单位的名称。
     * @return 如果单位名称未重复且成功添加，返回 true；否则返回 false。
     */
    bool addUnit(const std::string& unitName);
    /**
     * @brief 根据单位ID获取单位对象的可修改引用。
     * @param unitId 要查找的单位ID。
     * @return 如果找到，返回一个包含 Unit 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<Unit>> getUnit(int unitId);
    /**
     * @brief 根据单位ID获取单位对象的常量引用。
     * @param unitId 要查找的单位ID。
     * @return 如果找到，返回一个包含 Unit 常量引用的 std::optional；否则返回 std::nullopt。
     */
    [[nodiscard]] std::optional<utils::RefConst<Unit>> getUnitConst(int unitId) const;
    /**
     * @brief 获取所有参赛单位的列表 (常量引用)。
     * @return 返回一个包含所有单位的 map 的常量引用，键为单位ID，值为单位对象。
     */
    [[nodiscard]] const std::map<int, Unit>& getAllUnits() const;
    /**
     * @brief 根据单位ID移除一个参赛单位。
     * @param unitId 要移除的单位ID。
     * @return 如果单位存在并成功移除，返回 true；否则返回 false。
     * @note 同时会处理与该单位相关的运动员的清理逻辑（例如，将其unitId置为无效）。
     */
    bool removeUnit(int unitId);
    /** @brief 清除系统中所有的单位数据。 */
    void clearUnits();

    // --- 运动员管理 ---
    /**
     * @brief 添加一名新的运动员。
     * @param name 运动员姓名。
     * @param gender 运动员性别。
     * @param unitId 运动员所属单位的ID。
     * @return 如果所属单位存在且成功添加运动员，返回 true；否则返回 false。
     */
    bool addAthlete(const std::string& name, Gender gender, int unitId);
    /**
     * @brief 根据运动员ID获取运动员对象的可修改引用。
     * @param athleteId 要查找的运动员ID。
     * @return 如果找到，返回一个包含 Athlete 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<Athlete>> getAthlete(int athleteId);
    /**
     * @brief 根据运动员ID获取运动员对象的常量引用。
     * @param athleteId 要查找的运动员ID。
     * @return 如果找到，返回一个包含 Athlete 常量引用的 std::optional；否则返回 std::nullopt。
     */
    [[nodiscard]] std::optional<utils::RefConst<Athlete>> getAthleteConst(int athleteId) const;
    /**
     * @brief 获取所有运动员的列表 (常量引用)。
     * @return 返回一个包含所有运动员的 map 的常量引用，键为运动员ID，值为运动员对象。
     */
    [[nodiscard]] const std::map<int, Athlete>& getAllAthletes() const;
    /**
     * @brief 根据运动员ID移除一名运动员。
     * @param athleteId 要移除的运动员ID。
     * @return 如果运动员存在并成功移除，返回 true；否则返回 false。
     * @note 同时会处理该运动员的报名信息和其所属单位的运动员列表。
     */
    bool removeAthlete(int athleteId);
    /** @brief 清除系统中所有的运动员数据。 */
    void clearAthletes();

    // --- 项目管理 ---
    /**
     * @brief 添加一个新的比赛项目，并自动关联适用的计分规则。
     * @param eventName 项目名称。
     * @param type 项目类型 (径赛/田赛)。
     * @param genderReq 项目性别要求。
     * @return 如果项目名称未重复且成功添加，返回 true；否则返回 false。
     */
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq);
    /**
     * @brief 添加一个新的比赛项目，并手动指定计分规则ID。
     * @param eventName 项目名称。
     * @param type 项目类型。
     * @param genderReq 项目性别要求。
     * @param scoreRuleId 要关联的计分规则ID。
     * @return 如果项目名称未重复且指定的计分规则存在，则成功添加并返回 true；否则返回 false。
     */
    bool addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq, int scoreRuleId);
    /**
     * @brief 根据项目ID获取比赛项目对象的可修改引用。
     * @param eventId 要查找的项目ID。
     * @return 如果找到，返回一个包含 CompetitionEvent 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<CompetitionEvent>> getCompetitionEvent(int eventId);
    /**
     * @brief 根据项目ID获取比赛项目对象的常量引用。
     * @param eventId 要查找的项目ID。
     * @return 如果找到，返回一个包含 CompetitionEvent 常量引用的 std::optional；否则返回 std::nullopt。
     */
    [[nodiscard]] std::optional<utils::RefConst<CompetitionEvent>> getCompetitionEventConst(int eventId) const;
    /**
     * @brief 获取所有比赛项目的列表 (常量引用)。
     * @return 返回一个包含所有项目常量引用的 map，键为项目ID。
     */
    [[nodiscard]] std::map<int, utils::RefConst<CompetitionEvent>> getAllCompetitionEventsConst() const;
    /**
     * @brief 根据项目ID移除一个比赛项目。
     * @param eventId 要移除的项目ID。
     * @return 如果项目存在并成功移除，返回 true；否则返回 false。
     * @note 同时会处理与该项目相关的报名信息和成绩信息。
     */
    bool removeCompetitionEvent(int eventId);
    /** @brief 清除系统中所有的比赛项目数据。 */
    void clearCompetitionEvents();

    // --- 计分规则管理 ---
    /**
     * @brief 添加一条新的计分规则。
     * @param desc 规则描述。
     * @param minP 适用此规则的最小参赛人数。
     * @param maxP 适用此规则的最大参赛人数 (-1表示无上限)。
     * @param ranks 录取名次数。
     * @param scores 名次与分数的映射。
     * @return 如果规则描述不重复且成功添加，返回 true；否则返回 false。
     */
    bool addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores);
    /**
     * @brief 添加一个自定义的计分规则对象
     * @param rule 计分规则对象指针，由SystemSettings接管内存管理
     * @return 添加成功返回true，否则返回false
     */
    bool addCustomScoreRule(ScoreRule* rule);
    /**
     * @brief 获取指定ID的计分规则对象引用
     * @param ruleId 规则ID
     * @return 返回optional，包含规则对象的引用包装
     */
    [[nodiscard]] std::optional<utils::Ref<ScoreRule>> getScoreRule(int ruleId);
    /**
     * @brief 根据规则ID获取计分规则对象的常量引用。
     * @param ruleId 要查找的规则ID。
     * @return 如果找到，返回一个包含 ScoreRule 常量引用的 std::optional；否则返回 std::nullopt。
     */
    [[nodiscard]] std::optional<utils::RefConst<ScoreRule>> getScoreRuleConst(int ruleId) const;
    /**
     * @brief 根据参赛人数查找合适的计分规则。
     * @param participantCount 实际参赛人数。
     * @return 如果找到适用的规则，返回包含 ScoreRule 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<ScoreRule>> findAppropriateScoreRule(int participantCount);
    /**
     * @brief 获取所有计分规则的列表 (常量引用)。
     * @return 返回一个包含所有计分规则的 map 的常量引用，键为规则ID，值为规则对象。
     */
    [[nodiscard]] const std::map<int, ScoreRule>& getAllScoreRules() const;
    /** @brief 清除系统中所有的自定义计分规则（默认规则通常保留）。 */
    void clearScoreRules();


    // --- 系统参数设置 ---
    /**
     * @brief 设置运动员允许报名参加的最大项目数。
     * @param maxEvents 最大项目数。
     */
    void setAthleteMaxEventsAllowed(int maxEvents);
    /**
     * @brief 获取运动员允许报名参加的最大项目数。
     * @return 返回最大项目数。
     */
    [[nodiscard]] int getAthleteMaxEventsAllowed() const;

    // --- 比赛结果管理 ---
    /**
     * @brief 添加或更新一个比赛项目的全部成绩记录。
     * @param er 包含该项目所有成绩的 EventResults 对象。
     * @return 如果成功添加或更新，返回 true。
     */
    bool addOrUpdateEventResults(const EventResults& er);
    /**
     * @brief 根据项目ID获取该项目的成绩记录对象的可修改引用。
     * @param eventId 项目ID。
     * @return 如果找到，返回包含 EventResults 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<EventResults>> getEventResults(int eventId);
    /**
     * @brief 根据项目ID获取该项目的成绩记录对象的常量引用。
     * @param eventId 项目ID。
     * @return 如果找到，返回包含 EventResults 常量引用的 std::optional；否则返回 std::nullopt。
     */
    [[nodiscard]] std::optional<utils::RefConst<EventResults>> getEventResultsConst(int eventId) const;
    /**
     * @brief 清除指定比赛项目的所有已录入成绩。
     * @param eventId 要清除成绩的项目ID。
     * @note 用于成绩需要重新录入或重新计算的场景。
     */
    void clearResultsForEvent(int eventId);
    /** @brief 重置所有参赛单位的累计总分数为0。 */
    void resetAllUnitScores();
    /**
     * @brief 为指定的参赛单位增加分数。
     * @param unitId 单位ID。
     * @param score 要增加的分数值。
     */
    void addScoreToUnit(int unitId, double score);
    bool registerAthleteForEvent(int athleteId, int eventId);
    /**
     * @brief 获取所有已录入成绩的项目的成绩记录 (常量引用)。
     * @return 返回一个 map 的常量引用，键为项目ID，值为 EventResults 对象。
     */
    [[nodiscard]] const std::map<int, EventResults>& getAllEventResults() const;
    /** @brief 清除系统中所有项目的成绩记录。 */
    void clearAllEventResults();

    // --- 交互操作 ---
    // 此方法似乎是Registration类的职责，在SystemSettings中保留可能为了简化调用或历史原因
    // bool registerAthleteForEvent(int athleteId, int eventId);

    /**
     * @brief 初始化系统默认设置，特别是默认的计分规则。
     * 此方法应在系统启动或数据重置时调用，以确保有一套可用的基础计分规则。
     */
    void initializeDefaultSettings();
    /**
     * @brief 获取所有运动员列表的常量引用（旧版，建议使用 getAllAthletes）。
     * @return 返回包含运动员常量引用的 vector。
     */
    [[nodiscard]] std::vector<utils::RefConst<Athlete>> getAllAthlesConst() const;

    // --- 赛程锁定相关 ---
    /** @brief 锁定赛程。锁定后，项目信息（如时间、场地）不可编辑，报名时会进行时间冲突检查。 */
    void lockSchedule();
    /** @brief 解锁赛程。解锁后，项目信息可再次编辑。 */
    void unlockSchedule();
    /**
     * @brief 检查当前赛程是否已锁定。
     * @return 如果赛程已锁定，返回 true；否则返回 false。
     */
    [[nodiscard]] bool isScheduleLocked() const;

    // --- 场地管理 ---
    /**
     * @brief 添加一个新的可用比赛场地。
     * @param venueName 场地名称。
     * @return 如果场地名称未重复且成功添加，返回 true；否则返回 false。
     */
    bool addVenue(const std::string& venueName);
    /**
     * @brief 移除一个比赛场地。
     * @param venueName 要移除的场地名称。
     * @return 如果场地存在并成功移除，返回 true；否则返回 false。
     */
    bool removeVenue(const std::string& venueName);
    /**
     * @brief 获取所有可用比赛场地的列表 (常量引用)。
     * @return 返回一个包含所有场地名称的 std::set 的常量引用。
     */
    [[nodiscard]] const std::set<std::string>& getAllVenues() const;

    // --- 上午/下午时间段管理 ---
    /**
     * @brief 设置上午比赛的开始和结束时间。
     * @param start 开始时间，格式如 "08:00"。
     * @param end 结束时间，格式如 "12:00"。
     */
    void setMorningSession(const std::string& start, const std::string& end);
    /**
     * @brief 设置下午比赛的开始和结束时间。
     * @param start 开始时间，格式如 "14:00"。
     * @param end 结束时间，格式如 "18:00"。
     */
    void setAfternoonSession(const std::string& start, const std::string& end);
    /**
     * @brief 获取上午比赛的开始和结束时间。
     * @return 返回一个 std::pair，包含开始时间和结束时间字符串。
     */
    [[nodiscard]] std::pair<std::string, std::string> getMorningSession() const;
    /**
     * @brief 获取下午比赛的开始和结束时间。
     * @return 返回一个 std::pair，包含开始时间和结束时间字符串。
     */
    [[nodiscard]] std::pair<std::string, std::string> getAfternoonSession() const;

    // --- 工作流程阶段管理 ---
    [[nodiscard]] WorkflowStage getCurrentWorkflowStage() const;
    bool setWorkflowStage(WorkflowStage newStage);
};

#endif //SYSTEMSETTINGS_H