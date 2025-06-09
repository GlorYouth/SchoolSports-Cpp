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
    Athlete* athlete;           // 取得成绩的运动员指针
    std::string performance;    // 用于显示的成绩，如 "11.2s"
    double sortablePerformance; // 用于排序的成绩
    int rank;                   // 名次
    int pointsAwarded;          // 根据名次获得的积分

    /**
     * @brief 构造函数
     * @param ath 运动员指针
     * @param perf_val 成绩的数值格式
     */
    Result(Athlete* ath, double perf_val)
        : athlete(ath),
          sortablePerformance(perf_val),
          rank(0),
          pointsAwarded(0)
    {
        std::stringstream ss;
        ss << perf_val;
        this->performance = ss.str();
    }
};

#endif // RESULT_H 