//
// Created by GlorYouth on 2025/6/2.
//

#ifndef RESULT_H
#define RESULT_H


#include <string>
#include <vector>
#include <map> // 用于存储成绩，可能是<名次, 运动员ID> 或更复杂的结构
#include <optional>

// 前向声明
class Athlete;
class CompetitionEvent;

// 比赛成绩记录类
class Result {
private:
    int eventId;        // 对应的比赛项目ID
    int athleteId;      // 获得名次的运动员ID
    int rank;           // 名次
    std::string scoreRecord; // 成绩记录 (例如 "11''2", "2米02")
    // 对于田赛和径赛，成绩的表示和比较方式不同，需要特别处理
    double pointsAwarded; // 根据名次和计分规则获得的分数

public:
    Result(int eventId, int athleteId, int rank, const std::string& scoreRecord, double points);

    [[nodiscard]] int getEventId() const;
    [[nodiscard]] int getAthleteId() const;
    [[nodiscard]] int getRank() const;
    [[nodiscard]] std::string getScoreRecord() const;
    [[nodiscard]] double getPointsAwarded() const;

    // 设置成绩 (可能在成绩录入后)
    void setScoreRecord(const std::string& score);
    void setRank(int r);
    void setPointsAwarded(double p);

    // 比较函数，用于排序成绩 (需要根据项目类型区分)
    // static bool compareTrackResults(const Result& r1, const Result& r2); // 径赛：时间越小越好
    // static bool compareFieldResults(const Result& r1, const Result& r2); // 田赛：数值越大越好 (大部分情况)
};

// 管理一个项目所有成绩的容器
class EventResults {
private:
    int eventId;
    std::vector<Result> resultsList; // 按名次排序的成绩列表
    bool finalized; // 成绩是否已最终确定

public:
    explicit EventResults(int eventId);

    void addResult(const Result& result);
    [[nodiscard]] const std::vector<Result>& getResultsList() const;

    // 录入成绩并排序 (核心逻辑)
    // void recordAndSortResults(const std::vector<std::pair<int, std::string>>& rawScores); // <athleteId, scoreString>

    void finalizeResults(); // 最终确定成绩，计算积分等
    [[nodiscard]] bool isFinalized() const;

    // 根据运动员ID查询成绩
    std::optional<std::reference_wrapper<Result>> getResultForAthlete(int athleteId);
};


#endif //RESULT_H
