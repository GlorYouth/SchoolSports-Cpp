#ifndef ATHLETE_H
#define ATHLETE_H

#include <string>
#include <vector>

// 前向声明以避免循环依赖
class Event;
class Unit;

class Athlete {
private:
    std::string name;       // 姓名
    std::string id;         // 学号/ID
    bool isMale;            // 性别 true为男, false为女
    Unit* unit;             // 所属单位
    std::vector<Event*> events; // 参加的项目
    int individualScore;    // 个人总分

public:
    Athlete(const std::string& name, const std::string& id, bool isMale, Unit* unit);

    // Getters
    std::string getName() const;
    std::string getId() const;
    bool getIsMale() const;
    Unit* getUnit() const;
    int getIndividualScore() const;

    // Functions
    bool canRegister(int maxEvents);
    void registerForEvent(Event* event);
    const std::vector<Event*>& getRegisteredEvents() const;
    void addScore(int points);
    bool isRegisteredFor(const Event* event) const;
    void deregisterFromEvent(const Event* event);
};

#endif // ATHLETE_H 