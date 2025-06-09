#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <sstream> // For std::stringstream

// 前向声明 Athlete 类以避免循环依赖
class Athlete;

/**
 * @brief 定义单项比赛的个人成绩
 */
struct Result {
    std::string athleteId;
    double performance = 0.0;
    int rank = 0;
    int pointsAwarded = 0;

    // Default constructor
    Result() = default;

    // Constructor for recording initial performance
    Result(const std::string& id, double perf) 
        : athleteId(id), performance(perf), rank(0), pointsAwarded(0) {}
};

#endif // RESULT_H 