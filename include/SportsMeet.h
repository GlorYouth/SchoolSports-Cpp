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

class SportsMeet {
private:
    std::vector<std::unique_ptr<Unit>> units;
    std::vector<std::unique_ptr<Event>> events; // 统一的事件列表
    Schedule schedule; // 秩序册对象
    
    // 系统设置
    int maxEventsPerAthlete;
    int minParticipantsForCancel;       // 项目取消的最低人数要求
    std::vector<ScoringRule> scoringRules; // 计分规则列表

public:
    SportsMeet();

    // --- 系统设置功能 ---
    void setMaxEventsPerAthlete(int max);
    void addUnit(const std::string& unitName);
    void addEvent(const std::string& name, EventType type, GenderCategory gender, int duration, int min_participants, int max_participants);
    void deleteEvent(const std::string& name, GenderCategory gender);
    void manageScoringRules();
    
    // --- 核心功能 ---
    void registerAthlete(); // 运动员报名登记
    void recordAndScoreEvent(); // 记录比赛成绩并计分
    void showAllUnits() const; // For debugging/UI
    void showAllEvents() const; // For debugging/UI
    
    // --- 查询功能 ---
    Unit* findUnit(const std::string& unitName);
    const Unit* findUnit(const std::string& unitName) const;
    Event* findEvent(const std::string& eventName, GenderCategory gender);
    const Event* findEvent(const std::string& eventName, GenderCategory gender) const;
    void showUnitDetails(const std::string& unitName) const;
    void showEventDetails(const std::string& eventName, GenderCategory gender) const;
    
    // --- 秩序册 ---
    void generateSchedule();
    void showSchedule() const;

    // --- 数据持久化 ---
    void backupData(const std::string& filename) const;
    void restoreData(const std::string& filename);
    
    // ... 其他管理功能待实现 ...
    // void generateSchedule();
    // void recordResults();
    // void queryResults();
};

#endif // SPORTSMEET_H 