//
// Created by GlorYouth on 2025/6/2.
//

#ifndef COMPETITIONEVENT_H
#define COMPETITIONEVENT_H


#include <string>
#include <vector>
#include <atomic> // 用于生成唯一ID
#include "Constants.h"

// 前向声明
class Athlete;
class ScoreRule;
class Result; // 稍后定义 Result 类

/**
 * @brief 代表一个比赛项目。
 *
 * CompetitionEvent 类存储比赛项目的详细信息，如ID、名称、类型（径赛/田赛）、
 * 性别要求、参赛运动员ID列表、应用的计分规则ID、项目是否取消，
 *以及赛程相关的持续时间、场地、开始和结束时间。
 * ID 由静态原子计数器自动生成，保证唯一性。
 */
class CompetitionEvent {
private:
    static std::atomic<int> nextId; // 用于生成唯一的项目ID
    int id;                         // 项目ID
    std::string name;               // 项目名称 (例如: 男子100米)
    EventType type;                 // 项目类型 (径赛/田赛)
    Gender genderRequirement;       // 项目性别要求 (男子/女子)
    std::vector<int> participantAthleteIds; // 参赛运动员ID列表
    int scoreRuleId;                // 应用的计分规则ID (规则由 SystemSettings 管理)
    bool isCancelled;               // 项目是否因人数不足而取消
    // 新增字段
    int durationMinutes;            // 持续时间（分钟），必须手动设置
    std::string venue;              // 场地
    std::string startTime;          // 比赛开始时间（如 "08:00"）
    std::string endTime;            // 比赛结束时间（如 "08:30"）

    // 比赛结果相关，可以考虑用 Result 类来管理
    // std::vector<Result> results; // 存储比赛名次和成绩

    // 秩序册相关信息 (后续添加)
    // std::string scheduledTime;

public:
    /**
     * @brief CompetitionEvent 类的构造函数。
     * @param name 项目名称。
     * @param type 项目类型 (EventType::TRACK 或 EventType::FIELD)。
     * @param genderReq 项目性别要求 (Gender::MALE, Gender::FEMALE, 或 Gender::MIXED)。
     * @param scoreRuleId 应用的计分规则ID，默认为-1（未指定）。
     * @param durationMinutes 项目持续时间（分钟），默认为0（未设置）。
     */
    CompetitionEvent(std::string name, EventType type, Gender genderReq, int scoreRuleId = -1, int durationMinutes = 0);

    /**
     * @brief 获取项目的唯一ID。
     * @return 返回项目ID。
     */
    [[nodiscard]] int getId() const;

    /**
     * @brief 设置项目的ID。
     * @param id 要设置的ID。
     * @note 此方法主要用于数据恢复，正常情况下不应直接调用。
     */
    void setId(int id);

    /**
     * @brief 获取项目名称。
     * @return 返回项目名称。
     */
    [[nodiscard]] std::string getName() const;
    /**
     * @brief 设置项目名称。
     * @param name 新的项目名称。
     */
    void setName(const std::string& name);

    /**
     * @brief 获取项目类型。
     * @return 返回项目类型 (EventType 枚举)。
     */
    [[nodiscard]] EventType getEventType() const;
    /**
     * @brief 设置项目类型。
     * @param type 新的项目类型 (EventType 枚举)。
     */
    void setEventType(EventType type);

    /**
     * @brief 获取项目的性别要求。
     * @return 返回项目性别要求 (Gender 枚举)。
     */
    [[nodiscard]] Gender getGenderRequirement() const;
    /**
     * @brief 设置项目的性别要求。
     * @param genderReq 新的项目性别要求 (Gender 枚举)。
     */
    void setGenderRequirement(Gender genderReq);

    /**
     * @brief 向项目中添加一名参赛运动员的ID。
     * @param athleteId 要添加的运动员ID。
     * @return 如果运动员未重复添加，则返回 true；否则返回 false。
     */
    bool addParticipant(int athleteId);
    /**
     * @brief 从项目中移除一名参赛运动员的ID。
     * @param athleteId 要移除的运动员ID。
     * @return 如果找到并成功移除，返回 true；否则返回 false。
     */
    bool removeParticipant(int athleteId);
    /**
     * @brief 获取项目中所有参赛运动员的ID列表。
     * @return 返回一个包含运动员ID的常量vector引用。
     */
    [[nodiscard]] const std::vector<int>& getParticipantAthleteIds() const;
    /**
     * @brief 获取项目当前的参赛人数。
     * @return 返回参赛人数。
     */
    [[nodiscard]] int getParticipantCount() const;

    /**
     * @brief 设置该项目应用的计分规则ID。
     * @param ruleId 计分规则的ID。
     */
    void setScoreRuleId(int ruleId);
    /**
     * @brief 获取该项目应用的计分规则ID。
     * @return 返回计分规则ID。
     */
    [[nodiscard]] int getScoreRuleId() const;

    /**
     * @brief 设置项目是否因故取消。
     * @param cancelled 如果项目取消，则为 true；否则为 false。
     */
    void setCancelled(bool cancelled);
    /**
     * @brief 检查项目是否已被取消。
     * @return 如果项目已取消，返回 true；否则返回 false。
     */
    [[nodiscard]] bool getIsCancelled() const;

    /**
     * @brief 检查指定性别的运动员是否可以报名参加此项目。
     * @param athleteGender 运动员的性别。
     * @return 如果性别符合项目要求 (或项目为混合性别)，返回 true；否则返回 false。
     */
    [[nodiscard]] bool canAthleteRegister(Gender athleteGender) const;

    /**
     * @brief 重置用于生成下一个项目ID的静态计数器。
     * @param startId 可选参数，指定ID重新开始的初始值，默认为1。
     * @note 主要用于测试目的，以确保每次测试时ID的生成是可预测的。
     */
    static void resetNextId(int startId = 1);

    /**
     * @brief 设置项目的预计持续时间（分钟）。
     * @param minutes 持续时间，单位为分钟。
     */
    void setDurationMinutes(int minutes);
    /**
     * @brief 获取项目的预计持续时间（分钟）。
     * @return 返回持续时间（分钟）。
     */
    [[nodiscard]] int getDurationMinutes() const;
    /**
     * @brief 设置项目的比赛场地。
     * @param v 场地名称或描述。
     */
    void setVenue(const std::string& v);
    /**
     * @brief 获取项目的比赛场地。
     * @return 返回场地名称或描述。
     */
    [[nodiscard]] std::string getVenue() const;
    /**
     * @brief 设置项目的计划开始时间。
     * @param t 开始时间字符串，例如 "08:00"。
     */
    void setStartTime(const std::string& t);
    /**
     * @brief 设置项目的计划结束时间。
     * @param t 结束时间字符串，例如 "08:30"。
     */
    void setEndTime(const std::string& t);
    /**
     * @brief 获取项目的计划开始时间。
     * @return 返回开始时间字符串。
     */
    [[nodiscard]] std::string getStartTime() const;
    /**
     * @brief 获取项目的计划结束时间。
     * @return 返回结束时间字符串。
     */
    [[nodiscard]] std::string getEndTime() const;
};

#endif //COMPETITIONEVENT_H
