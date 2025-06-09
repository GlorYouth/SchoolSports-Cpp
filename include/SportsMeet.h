#ifndef SPORTSMEET_H
#define SPORTSMEET_H

#include "Unit.h"
#include "Event.h"
#include "Athlete.h"
#include "ScoringRule.h"
#include "Schedule.h" // 包含 Schedule 头文件
#include <vector>
#include <memory>
#include <string>
#include <fstream> // 用于文件操作
#include <map>
#include "Result.h"

class SportsMeet {
private:
    std::vector<std::unique_ptr<Unit>> units;
    std::vector<std::unique_ptr<Event>> events; // 统一的事件列表
    std::map<std::string, std::vector<Result>> eventResults;
    
    // 排程参数
    std::vector<std::string> venues; // 可用场地列表
    int competitionDays;             // 比赛总天数
    int dailyStartTime;              // 每日开始时间 (分钟)
    int dailyEndTime;                // 每日结束时间 (分钟)

    // 系统设置
    int maxEventsPerAthlete;
    int minParticipantsForCancel;       // 项目取消的最低人数要求
    std::vector<ScoringRule> scoringRules; // 计分规则列表

public:
    Schedule schedule; // 秩序册对象

    SportsMeet();
    ~SportsMeet();

    // --- 系统设置功能 ---
    void setMaxEventsPerAthlete(int max);
    void addUnit(const std::string& unitName);
    void addAthleteToUnit(const std::string& unitName, const std::string& athleteId, const std::string& athleteName, const std::string& gender);
    
    // --- 赛事管理 ---
    void addEvent(const std::string& name, const std::string& gender, bool isTimeBased);
    void deleteEvent(const std::string& eventName, const std::string& gender);
    void cancelEvent(const std::string& eventName);
    void manageScoringRules();
    
    // --- 核心功能 ---
    void registerAthlete(); // 运动员报名登记
    void recordAndScoreEvent(Event* event); // Changed to take an Event pointer
    void showAllUnits() const; // For debugging/UI
    void showAllEvents() const; // For debugging/UI
    void showAllAthletes() const; // 新增：显示所有运动员
    const std::vector<std::unique_ptr<Event>>& getAllEvents() const;
    const std::vector<std::unique_ptr<Unit>>& getAllUnits() const;
    
    // --- 查询功能 ---
    Unit* findUnit(const std::string& unitName);
    const Unit* findUnit(const std::string& unitName) const;
    Event* findEvent(const std::string& eventName, const std::string& gender);
    const Event* findEvent(const std::string& eventName, const std::string& gender) const;
    void showUnitDetails(const std::string& unitName) const;
    void showEventDetails(const std::string& eventName, const std::string& gender) const;
    void showUnitResults(const std::string& unitName) const;
    void showAthleteResults(const std::string& athleteId) const;
    Athlete* findAthlete(const std::string& athleteId) const;
    
    // --- 秩序册 ---
    void generateSchedule();
    void showSchedule() const;

    // --- 数据持久化 ---
    void backupData(const std::string& filename) const;
    void restoreData(const std::string& filename);
    
    // --- 报名管理 ---
    void registerAthleteForEvent(const std::string& athleteId, const std::string& eventName, const std::string& eventGender);

    // --- 成绩与排名 ---
    void recordResult(const std::string& eventName, const std::string& eventGender, const std::string& athleteId, double performance);
    void processScoresForEvent(const std::string& eventName, const std::string& gender);

    // ... 其他管理功能待实现 ...
    // void generateSchedule();
    // void recordResults();
    // void queryResults();
};

#endif // SPORTSMEET_H 