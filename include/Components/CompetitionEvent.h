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
    CompetitionEvent(std::string name, EventType type, Gender genderReq);

    // 获取项目ID
    [[nodiscard]] int getId() const;

    // 获取项目名称
    [[nodiscard]] std::string getName() const;
    // 设置项目名称
    void setName(const std::string& name);

    // 获取项目类型
    [[nodiscard]] EventType getEventType() const;
    // 设置项目类型
    void setEventType(EventType type);

    // 获取性别要求
    [[nodiscard]] Gender getGenderRequirement() const;
    // 设置性别要求
    void setGenderRequirement(Gender genderReq);

    // 添加参赛者ID
    bool addParticipant(int athleteId);
    // 移除参赛者ID
    bool removeParticipant(int athleteId);
    // 获取参赛者ID列表
    [[nodiscard]] const std::vector<int>& getParticipantAthleteIds() const;
    // 获取参赛人数
    [[nodiscard]] int getParticipantCount() const;

    // 设置计分规则ID
    void setScoreRuleId(int ruleId);
    // 获取计分规则ID
    [[nodiscard]] int getScoreRuleId() const;

    // 设置项目是否取消
    void setCancelled(bool cancelled);
    // 检查项目是否取消
    [[nodiscard]] bool getIsCancelled() const;

    // 检查运动员是否可以报名此项目 (性别符合)
    [[nodiscard]] bool canAthleteRegister(Gender athleteGender) const;

    // 新增：重置静态ID计数器的方法，用于测试
    static void resetNextId(int startId = 1);

    // 新增setter/getter
    void setDurationMinutes(int minutes);
    int getDurationMinutes() const;
    void setVenue(const std::string& v);
    std::string getVenue() const;
    void setStartTime(const std::string& t);
    void setEndTime(const std::string& t);
    std::string getStartTime() const;
    std::string getEndTime() const;
};

#endif //COMPETITIONEVENT_H
