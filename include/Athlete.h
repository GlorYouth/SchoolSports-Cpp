#ifndef ATHLETE_H
#define ATHLETE_H

#include <string>
#include <vector>

// 前向声明以避免循环依赖
class Event;
class Unit;

class Athlete {
public:
    std::string name;       // 姓名
    std::string id;         // 学号/ID
    std::string gender;     // "男" or "女"
    Unit* unit;             // 所属单位
    std::vector<std::string> registeredEvents;
    int score = 0;

    Athlete(const std::string& name, const std::string& id, const std::string& gender, Unit* unit);

    bool canRegister(int maxEvents) const;
    void registerForEvent(const std::string& eventName);
    void unregisterFromEvent(const std::string& eventName);
    bool isRegisteredFor(const std::string& eventName) const;
};

#endif // ATHLETE_H 