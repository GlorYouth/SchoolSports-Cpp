#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include "ScoringRule.h"
#include "Gender.h"

class Event {
public:
    std::string name;
    Gender gender;  // 使用Gender枚举代替字符串
    bool isTimeBased; // true for track, false for field
    std::vector<std::string> registeredAthletes;
    bool isCancelled = false;
    ScoringRule scoringRule;
    int durationMinutes; // 添加表示持续时间的成员变量（分钟）

    Event(const std::string& name, Gender gender, bool isTimeBased, const ScoringRule& rule, int durationMinutes);

    void addAthlete(const std::string& athleteId);
    void removeAthlete(const std::string& athleteId);
    bool allowsGender(Gender athleteGender) const; // 检查项目是否允许特定性别参赛
};

#endif // EVENT_H