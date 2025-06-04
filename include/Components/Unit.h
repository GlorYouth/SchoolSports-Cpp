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

/**
 * @brief 代表一个参赛单位（例如：学院、班级）。
 *
 * Unit 类负责存储单位的基本信息，如ID、名称、该单位下的运动员ID列表以及单位总分。
 * ID 由静态原子计数器自动生成，保证唯一性。
 */
class Unit {
private:
    static std::atomic<int> nextId; // 用于生成唯一的单位ID
    int id;                         // 单位ID
    std::string name;               // 单位名称
    std::vector<int> athleteIds;    // 该单位的运动员ID列表
    double totalScore;              // 单位总分

public:
    /**
     * @brief Unit 类的构造函数。
     * @param name 单位的名称。
     */
    explicit Unit(std::string  name);

    /**
     * @brief 获取单位的唯一ID。
     * @return 返回单位ID。
     */
    [[nodiscard]] int getId() const;

    /**
     * @brief 获取单位的名称。
     * @return 返回单位名称。
     */
    [[nodiscard]] std::string getName() const;

    /**
     * @brief 设置单位的名称。
     * @param name 新的单位名称。
     */
    void setName(const std::string& name);

    /**
     * @brief 向单位添加一个运动员的ID。
     * @param athleteId 要添加的运动员ID。
     * @note 实际的 Athlete 对象由专门的管理器（如 AthleteManager）管理。
     */
    void addAthleteId(int athleteId);

    /**
     * @brief 从单位移除一个运动员的ID。
     * @param athleteId 要移除的运动员ID。
     */
    void removeAthleteId(int athleteId);

    /**
     * @brief 获取该单位所有运动员的ID列表。
     * @return 返回一个包含运动员ID的常量vector引用。
     */
    [[nodiscard]] const std::vector<int>& getAthleteIds() const;

    /**
     * @brief 为单位增加分数。
     * @param score 要增加的分数值。
     */
    void addScore(double score);

    /**
     * @brief 获取单位当前的总得分。
     * @return 返回单位总分。
     */
    [[nodiscard]] double getTotalScore() const;

    /**
     * @brief 重置单位的总分数为0。
     */
    void resetScore();

    /**
     * @brief 重置用于生成下一个单位ID的静态计数器。
     * @param startId 可选参数，指定ID重新开始的初始值，默认为1。
     * @note 主要用于测试目的，以确保每次测试时ID的生成是可预测的。
     */
    static void resetNextId(int startId = 1);

    // 为了简化，这里不直接存储 Athlete 和 CompetitionEvent 对象指针
    // 而是通过ID关联，具体对象由中心管理器（如 SystemSettings 或专门的管理器）管理
};


#endif //UNIT_H
