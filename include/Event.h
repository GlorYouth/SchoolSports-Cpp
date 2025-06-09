#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include <memory>
#include "Athlete.h"
#include "Result.h"

// 前向声明 Athlete 类以避免循环依赖
class Athlete;

// 定义赛事类型（径赛/田赛）
enum class EventType {
    TRACK, // 径赛
    FIELD  // 田赛
};

// 定义性别分类
enum class GenderCategory {
    MALE,
    FEMALE
};

class Event {
private:
    std::string name;       // 项目名称
    EventType type;         // 项目类型
    GenderCategory gender;  // 性别组别
    int duration;           // 项目持续时间（单位：分钟）
    int minParticipants;
    int maxParticipants;
    std::vector<Athlete*> participants; // 参赛运动员列表
    std::vector<Result> results;   // 比赛成绩列表

public:
    // 构造函数
    Event(const std::string& name, EventType type, GenderCategory gender, int duration, int min, int max);

    // Getters
    std::string getName() const;
    EventType getType() const;
    GenderCategory getGender() const;
    int getDuration() const;
    int getMinParticipants() const;
    int getMaxParticipants() const;
    int getParticipantCount() const;
    const std::vector<Athlete*>& getParticipants() const;
    
    // Functions
    void addParticipant(Athlete* athlete);
    void setDuration(int new_duration);
    void setMinParticipants(int min);
    void setMaxParticipants(int max);
    
    // 新增成绩相关方法
    void setResults(const std::vector<Result>& results);
    const std::vector<Result>& getResults() const;
    bool hasResults() const;
};

#endif // EVENT_H 