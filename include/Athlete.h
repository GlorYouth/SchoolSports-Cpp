//
// Created by GlorYouth on 2025/6/2.
//

#ifndef ATHLETE_H
#define ATHLETE_H

#include <string>
#include <vector>
#include <atomic> // 用于生成唯一ID
#include "Constants.h" // 包含性别等定义

// 前向声明
class CompetitionEvent;

class Athlete {
private:
    static std::atomic<int> nextId; // 用于生成唯一的运动员ID
    int id;                         // 运动员ID
    std::string name;               // 姓名
    Gender gender;                  // 性别
    int unitId;                     // 所属单位ID
    std::vector<int> registeredEventIds; // 已报名参赛的项目ID列表
    // 运动员个人总得分可以在需要时计算，或在此处添加字段

public:
    Athlete(std::string  name, Gender gender, int unitId);

    // 获取运动员ID
    [[nodiscard]] int getId() const;

    // 获取姓名
    [[nodiscard]] std::string getName() const;
    // 设置姓名
    void setName(const std::string& name);

    // 获取性别
    [[nodiscard]] Gender getGender() const;
    // 设置性别
    void setGender(Gender gender);

    // 获取所属单位ID
    [[nodiscard]] int getUnitId() const;
    // 设置所属单位ID (通常在创建时指定，不轻易修改)
    void setUnitId(int unitId);

    // 报名参加项目 (只记录项目ID)
    bool registerForEvent(int eventId, int maxEventsAllowed);
    // 取消报名项目
    bool unregisterFromEvent(int eventId);
    // 获取已报名项目ID列表
    [[nodiscard]] const std::vector<int>& getRegisteredEventIds() const;
    // 检查是否已报名某项目
    [[nodiscard]] bool isRegisteredForEvent(int eventId) const;
    // 获取已报名项目数量
    [[nodiscard]] int getRegisteredEventsCount() const;
};


#endif //ATHLETE_H
