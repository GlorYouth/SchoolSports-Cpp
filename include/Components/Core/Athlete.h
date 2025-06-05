

#ifndef ATHLETE_H
#define ATHLETE_H

#include <string>
#include <vector>
#include <atomic> // 用于生成唯一ID
#include "Constants.h"

// 前向声明
class CompetitionEvent;

/**
 * @brief 代表一名运动员。
 *
 * Athlete 类存储运动员的个人信息，如ID、姓名、性别、所属单位ID，
 * 以及该运动员已报名参加的比赛项目ID列表。
 * ID 由静态原子计数器自动生成，保证唯一性。
 */
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
    /**
     * @brief Athlete 类的构造函数。
     * @param name 运动员姓名。
     * @param gender 运动员性别 (Gender::MALE 或 Gender::FEMALE)。
     * @param unitId 运动员所属单位的ID。
     */
    Athlete(std::string  name, Gender gender, int unitId);

    /**
     * @brief 获取运动员的唯一ID。
     * @return 返回运动员ID。
     */
    [[nodiscard]] int getId() const;

    /**
     * @brief 设置运动员的ID。
     * @param id 要设置的ID。
     * @note 此方法主要用于数据恢复，正常情况下不应直接调用。
     */
    void setId(int id);

    /**
     * @brief 获取运动员的姓名。
     * @return 返回运动员姓名。
     */
    [[nodiscard]] std::string getName() const;

    /**
     * @brief 设置运动员的姓名。
     * @param name 新的运动员姓名。
     */
    void setName(const std::string& name);

    /**
     * @brief 获取运动员的性别。
     * @return 返回运动员性别 (Gender枚举类型)。
     */
    [[nodiscard]] Gender getGender() const;

    /**
     * @brief 设置运动员的性别。
     * @param gender 新的运动员性别 (Gender枚举类型)。
     */
    void setGender(Gender gender);

    /**
     * @brief 获取运动员所属单位的ID。
     * @return 返回所属单位ID。
     */
    [[nodiscard]] int getUnitId() const;

    /**
     * @brief 设置运动员所属单位的ID。
     * @param unitId 新的所属单位ID。
     * @note 通常在创建运动员对象时指定单位ID，一般不轻易修改。
     */
    void setUnitId(int unitId);

    /**
     * @brief 为运动员报名参加一个比赛项目。
     * @param eventId 要报名的比赛项目ID。
     * @param maxEventsAllowed 允许运动员报名的最大项目数。
     * @return 如果报名成功（未超过最大项目数限制且未重复报名），返回 true；否则返回 false。
     */
    bool registerForEvent(int eventId, int maxEventsAllowed);

    /**
     * @brief 为运动员取消报名一个比赛项目。
     * @param eventId 要取消报名的比赛项目ID。
     * @return 如果找到并成功取消报名，返回 true；否则返回 false。
     */
    bool unregisterFromEvent(int eventId);

    /**
     * @brief 获取运动员已报名参加的所有比赛项目的ID列表。
     * @return 返回一个包含比赛项目ID的常量vector引用。
     */
    [[nodiscard]] const std::vector<int>& getRegisteredEventIds() const;

    /**
     * @brief 检查运动员是否已报名参加指定的比赛项目。
     * @param eventId 要检查的比赛项目ID。
     * @return 如果已报名，返回 true；否则返回 false。
     */
    [[nodiscard]] bool isRegisteredForEvent(int eventId) const;

    /**
     * @brief 获取运动员已报名参加的比赛项目数量。
     * @return 返回已报名项目的数量。
     */
    [[nodiscard]] int getRegisteredEventsCount() const;

    /**
     * @brief 重置用于生成下一个运动员ID的静态计数器。
     * @param startId 可选参数，指定ID重新开始的初始值，默认为1。
     * @note 主要用于测试目的，以确保每次测试时ID的生成是可预测的。
     */
    static void resetNextId(int startId = 1);
};


#endif //ATHLETE_H
