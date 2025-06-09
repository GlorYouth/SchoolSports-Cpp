#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <memory>

// 前向声明以避免循环依赖
class Athlete;

class Unit {
public:
    std::string name;
    int score = 0;
    std::vector<std::unique_ptr<Athlete>> athletes;

    Unit(const std::string& name);

    void addAthlete(const std::string& athleteId, const std::string& athleteName, const std::string& gender);
    void addScore(int points);
    Athlete* findAthlete(const std::string& athleteId);
};

#endif // UNIT_H 