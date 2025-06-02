//
// Created by GlorYouth on 2025/6/2.
//

#ifndef UNIT_H
#define UNIT_H


#include <string>
#include <vector>
#include <atomic> // 用于生成唯一ID

// 前向声明
class Athlete;
class CompetitionEvent;

class Unit {
private:
    static std::atomic<int> nextId; // 用于生成唯一的单位ID
    int id;                         // 单位ID
    std::string name;               // 单位名称
    std::vector<int> athleteIds;    // 该单位的运动员ID列表
    double totalScore;              // 单位总分

public:
    explicit Unit(const std::string& name);

    // 获取单位ID
    [[nodiscard]] int getId() const;

    // 获取单位名称
    [[nodiscard]] std::string getName() const;
    // 设置单位名称
    void setName(const std::string& name);

    // 添加运动员ID (实际运动员对象由 AthleteManager 管理)
    void addAthleteId(int athleteId);
    // 移除运动员ID
    void removeAthleteId(int athleteId);
    // 获取运动员ID列表
    [[nodiscard]] const std::vector<int>& getAthleteIds() const;

    // 更新总分
    void addScore(double score);
    // 获取总分
    [[nodiscard]] double getTotalScore() const;
    // 重置分数
    void resetScore();

    // 为了简化，这里不直接存储 Athlete 和 CompetitionEvent 对象指针
    // 而是通过ID关联，具体对象由中心管理器（如 SystemSettings 或专门的管理器）管理
};


#endif //UNIT_H
