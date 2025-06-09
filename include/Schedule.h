#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <string>
#include <vector>
#include <map>
#include <utility> // for std::pair

// 前向声明，避免循环依赖
class Event;
class Athlete;

/**
 * @brief 代表一个具体的、已安排的比赛日程
 *  包含时间、地点和比赛项目
 */
struct ScheduledSession {
    int day;                // 第几天
    int startTime;          // 开始时间 (从午夜开始的分钟数)
    int endTime;            // 结束时间 (从午夜开始的分钟数)
    std::string venue;      // 场地名称 (例如, "主田径场", "跳远沙坑")
    Event* event;           // 在此场次进行的项目

    ScheduledSession(int d, int start, int end, const std::string& v, Event* e)
        : day(d), startTime(start), endTime(end), venue(v), event(e) {}
};

/**
 * @brief 整个运动会的秩序册类
 */
class Schedule {
private:
    std::vector<ScheduledSession> scheduled_sessions;
    // 使用 <day, vector<pair<start, end>>> 结构记录繁忙时段
    std::map<std::string, std::map<int, std::vector<std::pair<int, int>>>> venue_busy_slots;
    std::map<std::string, std::map<int, std::vector<std::pair<int, int>>>> athlete_busy_slots;

public:
    Schedule() = default;

    // 添加一个已安排的场次
    void addSession(int day, int startTime, int endTime, const std::string& venue, Event* event);

    // 检查约束
    bool isVenueAvailable(const std::string& venue, int day, int startTime, int endTime) const;
    bool isAthleteAvailable(const Athlete* athlete, int day, int startTime, int endTime) const;

    // 获取完整秩序册
    const std::vector<ScheduledSession>& getFullSchedule() const;

    void clear();
};

#endif // SCHEDULE_H 