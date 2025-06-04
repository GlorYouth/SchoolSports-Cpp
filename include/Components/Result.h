//
// Created by GlorYouth on 2025/6/2.
//

#ifndef RESULT_H
#define RESULT_H


#include <string>
#include <vector>
#include <map> // 用于存储成绩，可能是<名次, 运动员ID> 或更复杂的结构
#include <optional>
#include "../../include/utils.h"

// 前向声明
class Athlete;
class CompetitionEvent;

// 比赛成绩记录类
/**
 * @brief 代表一个运动员在特定比赛项目中的单条成绩记录。
 *
 * Result 类存储了运动员在一个项目中所取得的名次、原始成绩记录（如时间或距离）
 * 以及根据该名次和计分规则所获得的分数。
 */
class Result {
private:
    int eventId;        // 对应的比赛项目ID
    int athleteId;      // 获得名次的运动员ID
    int rank;           // 名次
    std::string scoreRecord; // 成绩记录 (例如 "11''2", "2米02")
    // 对于田赛和径赛，成绩的表示和比较方式不同，需要特别处理
    double pointsAwarded; // 根据名次和计分规则获得的分数

public:
    /**
     * @brief Result 类的构造函数。
     * @param eventId 成绩所属的比赛项目ID。
     * @param athleteId 取得该成绩的运动员ID。
     * @param rank 运动员在该项目中获得的名次。
     * @param scoreRecord 运动员的原始成绩记录 (字符串形式)。
     * @param points 根据名次和规则计算出的得分。
     */
    Result(int eventId, int athleteId, int rank, const std::string& scoreRecord, double points);

    /** @brief 获取成绩所属的比赛项目ID。 @return 项目ID。 */
    [[nodiscard]] int getEventId() const;
    /** @brief 获取取得该成绩的运动员ID。 @return 运动员ID。 */
    [[nodiscard]] int getAthleteId() const;
    /** @brief 获取运动员在此项目中的名次。 @return 名次。 */
    [[nodiscard]] int getRank() const;
    /** @brief 获取运动员的原始成绩记录。 @return 成绩记录字符串。 */
    [[nodiscard]] std::string getScoreRecord() const;
    /** @brief 获取运动员根据此成绩获得的积分。 @return 积分。 */
    [[nodiscard]] double getPointsAwarded() const;

    /**
     * @brief 设置运动员的原始成绩记录。
     * @param score 新的成绩记录字符串。
     */
    void setScoreRecord(const std::string& score);
    /**
     * @brief 设置运动员在此项目中的名次。
     * @param r 新的名次。
     */
    void setRank(int r);
    /**
     * @brief 设置运动员根据此成绩获得的积分。
     * @param p 新的积分。
     */
    void setPointsAwarded(double p);

    // 比较函数，用于排序成绩 (需要根据项目类型区分)
    // static bool compareTrackResults(const Result& r1, const Result& r2); // 径赛：时间越小越好
    // static bool compareFieldResults(const Result& r1, const Result& r2); // 田赛：数值越大越好 (大部分情况)
};

// 管理一个项目所有成绩的容器
/**
 * @brief 管理特定比赛项目的所有成绩记录。
 *
 * EventResults 类负责存储和管理一个比赛项目下所有运动员的成绩。
 * 它包含一个Result对象的列表，并提供添加、获取、最终确定成绩等功能。
 */
class EventResults {
private:
    int eventId;
    std::vector<Result> resultsList; // 按名次排序的成绩列表
    bool finalized; // 成绩是否已最终确定

public:
    /**
     * @brief EventResults 类的构造函数。
     * @param eventId 要管理成绩的比赛项目ID。
     */
    explicit EventResults(int eventId);

    /** @brief 获取此成绩集合对应的比赛项目ID。 @return 项目ID。 */
    [[nodiscard]] int getEventId() const; // 添加了 getter 方法

    /**
     * @brief 向此项目的成绩列表中添加一条成绩记录。
     * @param result 要添加的 Result 对象。
     * @note 通常在添加后需要进行排序。
     */
    void addResult(const Result& result);
    /**
     * @brief 获取此项目所有成绩记录的列表（通常按名次排序）。
     * @return 返回一个包含 Result 对象的常量vector引用。
     */
    [[nodiscard]] const std::vector<Result>& getResultsList() const;

    // 录入成绩并排序 (核心逻辑)
    // void recordAndSortResults(const std::vector<std::pair<int, std::string>>& rawScores); // <athleteId, scoreString>

    /**
     * @brief 最终确定该项目的所有成绩。
     * 确定后，通常会触发积分计算和状态锁定，不允许再修改成绩。
     */
    void finalizeResults(); // 最终确定成绩，计算积分等
    /**
     * @brief 检查该项目的成绩是否已最终确定。
     * @return 如果已最终确定，返回 true；否则返回 false。
     */
    [[nodiscard]] bool isFinalized() const;

    /**
     * @brief 根据运动员ID查询其在该项目中的成绩。
     * @param athleteId 要查询的运动员ID。
     * @return 如果找到该运动员的成绩，返回一个包含 Result 引用的 std::optional；否则返回 std::nullopt。
     */
    std::optional<utils::Ref<Result>> getResultForAthlete(int athleteId);
};


#endif //RESULT_H
