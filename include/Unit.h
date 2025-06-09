#ifndef UNIT_H
#define UNIT_H

#include <string>
#include <vector>
#include <memory>

// 前向声明以避免循环依赖
class Athlete;

class Unit {
private:
    std::string name;
    // 使用智能指针管理运动员对象的生命周期
    std::vector<std::unique_ptr<Athlete>> athletes;
    int totalScore;

public:
    Unit(const std::string& name);

    // Getters
    std::string getName() const;
    int getTotalScore() const;
    const std::vector<std::unique_ptr<Athlete>>& getAthletes() const;

    // Setters
    void setTotalScore(int score);

    // Functions
    // 添加运动员，返回指向新创建运动员的原始指针以便于操作
    Athlete* addAthlete(const std::string& name, const std::string& id, bool isMale);
    void addScore(int score);

    // 根据ID查找运动员
    Athlete* findAthlete(const std::string& athleteId);
};

#endif // UNIT_H 