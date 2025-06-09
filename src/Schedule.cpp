#include "Schedule.h"
#include "Event.h"
#include "Athlete.h"
#include "SportsMeet.h"
#include <algorithm>

// 检查两个时间段 [start1, end1) 和 [start2, end2) 是否重叠
bool timeRangesOverlap(int start1, int end1, int start2, int end2) {
    return start1 < end2 && start2 < end1;
}

// 修改构造函数，接收 SportsMeet 引用
Schedule::Schedule(SportsMeet& sportsMeet) : sm(sportsMeet) {}

void Schedule::addSession(int day, int startTime, int endTime, const std::string& venue, Event* event) {
    scheduled_sessions.emplace_back(day, startTime, endTime, venue, event);

    // 记录场地和运动员的繁忙时段
    venue_busy_slots[venue][day].push_back({startTime, endTime});
    // 修复：使用 event->registeredAthletes 并通过 sm.findAthlete 查找运动员
    for (const auto& athleteId : event->registeredAthletes) {
        Athlete* athlete = sm.findAthlete(athleteId);
        if (athlete) {
            // 修复：使用 athlete->id
            athlete_busy_slots[athlete->id][day].push_back({startTime, endTime});
        }
    }
}

bool Schedule::isVenueAvailable(const std::string& venue, int day, int startTime, int endTime) const {
    if (venue_busy_slots.count(venue) && venue_busy_slots.at(venue).count(day)) {
        for (const auto& busy_slot : venue_busy_slots.at(venue).at(day)) {
            if (timeRangesOverlap(startTime, endTime, busy_slot.first, busy_slot.second)) {
                return false;
            }
        }
    }
    return true;
}

bool Schedule::isAthleteAvailable(const Athlete* athlete, int day, int startTime, int endTime) const {
    const int rest_time = 30; // 强制休息30分钟
    // 我们要检查的带休息时间的总时间段是 [startTime - 30, endTime + 30)
    int check_start = startTime - rest_time;
    int check_end = endTime + rest_time;
    
    // 修复：使用 athlete->id
    if (athlete_busy_slots.count(athlete->id) && athlete_busy_slots.at(athlete->id).count(day)) {
        for (const auto& busy_slot : athlete_busy_slots.at(athlete->id).at(day)) {
            // 检查运动员自己的比赛时间，如果和别的比赛有任何重叠（哪怕是1分钟的休息时间）
            if (timeRangesOverlap(check_start, check_end, busy_slot.first, busy_slot.second)) {
                return false;
            }
        }
    }
    return true;
}

const std::vector<ScheduledSession>& Schedule::getFullSchedule() const {
    return scheduled_sessions;
}

void Schedule::clear() {
    scheduled_sessions.clear();
    venue_busy_slots.clear();
    athlete_busy_slots.clear();
} 