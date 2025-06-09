#ifndef RESULT_H
#define RESULT_H

#include <string>

// 前向声明 Athlete 类以避免循环依赖
class Athlete;

/**
 * @brief 定义单项比赛的个人成绩
 */
struct Result {
    Athlete* athlete;           // 取得成绩的运动员指针
    int rank;                   // 名次
    std::string performance;    // 成绩的字符串形式 (例如: "11''2", "2.02m")
    double sortablePerformance; // 用于内部排序的数值格式成绩
    int pointsAwarded;          // 根据名次获得的积分

    /**
     * @brief 构造函数
     * @param ath 运动员指针
     * @param perf 成绩的字符串
     * @param sortable_perf 用于排序的成绩
     */
    Result(Athlete* ath, const std::string& perf, double sortable_perf)
        : athlete(ath),
          rank(0),
          performance(perf),
          sortablePerformance(sortable_perf),
          pointsAwarded(0) {}
};

#endif // RESULT_H 