#ifndef ATHLETE_H
#define ATHLETE_H

#include <string>
#include <vector>
#include "Gender.h"

// 前向声明以避免循环依赖
class Event;
class Unit;

class Athlete {
public:
    std::string name;       // 姓名
    std::string id;         // 学号/ID
    Gender gender;          // 使用Gender枚举
    Unit* unit;             // 所属单位
    std::vector<std::string> registeredEvents;
    int score = 0;

    Athlete(const std::string& name, const std::string& id, Gender gender, Unit* unit);

    bool canRegister(int maxEvents) const;
    void registerForEvent(const std::string& eventName);
    void unregisterFromEvent(const std::string& eventName);
    bool isRegisteredFor(const std::string& eventName) const;
};

#endif // ATHLETE_H 