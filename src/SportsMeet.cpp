#include "SportsMeet.h"
#include "Unit.h"
#include "Athlete.h"
#include "Event.h"
#include "Result.h"
#include "ScoringRule.h"
#include "TimeUtils.h" // <--- 关键的修复
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <map>
#include <sstream>

SportsMeet::SportsMeet() : schedule(*this), maxEventsPerAthlete(2) {
    // 初始化排程参数
    venues = {"主田径场", "跳远沙坑", "铅球区"};
    competitionDays = 2;
    dailyStartTime = 9 * 60; // 09:00
    dailyEndTime = 17 * 60;   // 17:00

    // Default scoring rules can be initialized here if needed
    // Example:
    // scoringRules.push_back(ScoringRule({5, 3, 2, 1})); // For 4-7 participants
    // scoringRules.push_back(ScoringRule({7, 5, 3, 2, 1})); // For 8+ participants
}

SportsMeet::~SportsMeet() = default;

void SportsMeet::setMaxEventsPerAthlete(int max) {
    if (max > 0) {
        maxEventsPerAthlete = max;
    }
}

void SportsMeet::addUnit(const std::string& unitName) {
    if (!findUnit(unitName)) {
        units.emplace_back(std::make_unique<Unit>(unitName));
        std::cout << "单位 '" << unitName << "' 添加成功！\n";
    } else {
        std::cout << "错误: 单位 '" << unitName << "' 已存在。\n";
    }
}

void SportsMeet::addAthleteToUnit(const std::string& unitName, const std::string& athleteId, const std::string& athleteName, const std::string& gender) {
    Unit* unit = findUnit(unitName);
    if (unit) {
        unit->addAthlete(athleteId, athleteName, gender);
        std::cout << "运动员 '" << athleteName << "' 已添加到单位 '" << unitName << "'。\n";
    } else {
        std::cout << "错误: 未找到单位 '" << unitName << "'。\n";
    }
}

void SportsMeet::addEvent(const std::string& name, const std::string& gender, bool isTimeBased) {
    if (findEvent(name, gender)) {
        std::cout << "错误: 项目 '" << name << "' (" << gender << ") 已存在。\n";
        return;
    }
    events.emplace_back(std::make_unique<Event>(name, gender, isTimeBased));
    std::cout << "项目 '" << name << "' 添加成功！\n";
}

void SportsMeet::deleteEvent(const std::string& eventName, const std::string& gender) {
    auto it = std::remove_if(events.begin(), events.end(), [&](const std::unique_ptr<Event>& event) {
        return event->name == eventName && event->gender == gender;
    });

    if (it != events.end()) {
        for (auto& unit : units) {
            for (auto& athlete : unit->athletes) {
                athlete->unregisterFromEvent(eventName);
            }
        }
        events.erase(it, events.end());
        std::cout << "项目 '" << eventName << "' 已被删除。\n";
    } else {
        std::cout << "未找到要删除的项目。\n";
    }
}

void SportsMeet::registerAthleteForEvent(const std::string& athleteId, const std::string& eventName, const std::string& eventGender) {
    Athlete* athlete = findAthlete(athleteId);
    if (!athlete) {
        std::cout << "错误: 未找到学号为 " << athleteId << " 的运动员。\n";
        return;
    }

    Event* event = findEvent(eventName, eventGender);
    if (!event) {
        std::cout << "错误: 未找到项目 '" << eventName << "' (" << eventGender << ")。\n";
        return;
    }

    if (athlete->gender != event->gender) {
        std::cout << "错误: 运动员性别 (" << athlete->gender << ") 与项目要求性别 (" << event->gender << ") 不符。\n";
        return;
    }

    if (!athlete->canRegister(maxEventsPerAthlete)) {
        std::cout << "错误: 运动员 " << athlete->name << " 报名项目已达上限 (" << maxEventsPerAthlete << ")。\n";
        return;
    }

    if (athlete->isRegisteredFor(event->name)) {
        std::cout << "错误: 运动员 " << athlete->name << " 已报名参加项目 '" << event->name << "'。\n";
        return;
    }

    athlete->registerForEvent(event->name);
    event->addAthlete(athlete->id);
    std::cout << "运动员 " << athlete->name << " 成功报名参加项目 '" << event->name << "'。\n";
}

void SportsMeet::recordResult(const std::string& eventName, const std::string& eventGender, const std::string& athleteId, double performance) {
    Event* event = findEvent(eventName, eventGender);
    if (!event) {
        std::cout << "错误: 未找到要记录成绩的项目。\n";
        return;
    }
    Athlete* athlete = findAthlete(athleteId);
    if (!athlete) {
        std::cout << "错误: 未找到运动员。\n";
        return;
    }

    // This is a simplified recording. A full implementation would involve sorting and scoring.
    Result result(athleteId, performance);
    eventResults[event->name].push_back(result);
    std::cout << "已记录 " << athlete->name << " 在项目 '" << event->name << "' 的成绩: " << performance << "\n";
}


void SportsMeet::showAllUnits() const {
    std::cout << "\n--- 所有单位 ---\n";
    if (units.empty()) {
        std::cout << "没有单位信息。\n";
        return;
    }
    for (size_t i = 0; i < units.size(); ++i) {
        std::cout << i + 1 << ". " << units[i]->name << " (总分: " << units[i]->score << ")\n";
    }
}

void SportsMeet::showAllEvents() const {
    std::cout << "\n--- 所有项目 ---\n";
    if (events.empty()) {
        std::cout << "没有项目信息。\n";
        return;
    }
    for (size_t i = 0; i < events.size(); ++i) {
        std::cout << i + 1 << ". "
                  << events[i]->name
                  << " (" << events[i]->gender << ")"
                  << (events[i]->isTimeBased ? " [计时赛]" : " [计分赛]")
                  << (events[i]->isCancelled ? " [已取消]" : "")
                  << "\n";
    }
}

const std::vector<std::unique_ptr<Event>>& SportsMeet::getAllEvents() const {
    return events;
}

const std::vector<std::unique_ptr<Unit>>& SportsMeet::getAllUnits() const {
    return units;
}

Unit* SportsMeet::findUnit(const std::string& unitName) {
     auto it = std::find_if(units.begin(), units.end(), [&](const auto& unit_ptr){
        return unit_ptr->name == unitName;
    });
    return (it != units.end()) ? it->get() : nullptr;
}

const Unit* SportsMeet::findUnit(const std::string& unitName) const {
     auto it = std::find_if(units.begin(), units.end(), [&](const auto& unit_ptr){
        return unit_ptr->name == unitName;
    });
    return (it != units.end()) ? it->get() : nullptr;
}

Event* SportsMeet::findEvent(const std::string& eventName, const std::string& gender) {
    auto it = std::find_if(events.begin(), events.end(), [&](const std::unique_ptr<Event>& event) {
        return event->name == eventName && event->gender == gender;
    });
    return it != events.end() ? it->get() : nullptr;
}

const Event* SportsMeet::findEvent(const std::string& eventName, const std::string& gender) const {
    auto it = std::find_if(events.begin(), events.end(), [&](const std::unique_ptr<Event>& event) {
        return event->name == eventName && event->gender == gender;
    });
    return it != events.end() ? it->get() : nullptr;
}

Athlete* SportsMeet::findAthlete(const std::string& athleteId) const {
    for (const auto& unit : units) {
        if (auto* athlete = unit->findAthlete(athleteId)) {
            return athlete;
        }
    }
    return nullptr;
}

void SportsMeet::showUnitDetails(const std::string& unitName) const {
    const Unit* unit = findUnit(unitName);
    if (unit) {
        std::cout << "\n--- 单位详情: " << unit->name << " ---\n";
        std::cout << "总分: " << unit->score << "\n";
        std::cout << "运动员列表 (" << unit->athletes.size() << "人):\n";
        for (const auto& athlete : unit->athletes) {
            std::cout << "  - " << athlete->name << " (ID: " << athlete->id << ", 性别: " << athlete->gender << ")\n";
            std::cout << "    个人得分: " << athlete->score << "\n";
            std::cout << "    已报名项目 (" << athlete->registeredEvents.size() << "):\n";
            if (athlete->registeredEvents.empty()) {
                std::cout << "      - 无\n";
            } else {
                for (const auto& eventName : athlete->registeredEvents) {
                    std::cout << "      - " << eventName << "\n";
                }
            }
        }
    } else {
        std::cout << "未找到单位: " << unitName << "\n";
    }
}

void SportsMeet::showEventDetails(const std::string& eventName, const std::string& gender) const {
    const Event* event = findEvent(eventName, gender);
    if (!event) {
        std::cout << "未找到项目 " << eventName << " (" << gender << ")\n";
        return;
    }

    std::cout << "\n--- 项目详情: " << event->name << " ---\n";
    std::cout << "  性别要求: " << event->gender << "\n";
    std::cout << "  类型: " << (event->isTimeBased ? "径赛 (时间越短越好)" : "田赛 (数值越高越好)") << "\n";
    std::cout << "  状态: " << (event->isCancelled ? "已取消" : "正常") << "\n";
    std::cout << "  已报名运动员 (" << event->registeredAthletes.size() << "人):\n";
    if (event->registeredAthletes.empty()) {
        std::cout << "    - 无\n";
    } else {
        for (const auto& athleteId : event->registeredAthletes) {
            const Athlete* athlete = findAthlete(athleteId);
            if (athlete) {
                std::cout << "    - " << athlete->name << " (ID: " << athlete->id << ", 单位: " << athlete->unit->name << ")\n";
            }
        }
    }
}

void SportsMeet::showUnitResults(const std::string& unitName) const {
    const Unit* unit = findUnit(unitName);
    if (!unit) {
        std::cout << "未找到单位: " << unitName << std::endl;
        return;
    }
    std::cout << "\n--- " << unitName << " 成绩汇总 ---\n";
    // Implementation requires iterating through events and results, which is complex.
    // This is a simplified placeholder.
}

void SportsMeet::showAthleteResults(const std::string& athleteId) const {
    const Athlete* athlete = findAthlete(athleteId);
    if (!athlete) {
        std::cout << "未找到运动员: " << athleteId << std::endl;
        return;
    }
    std::cout << "\n--- " << athlete->name << " (ID: " << athleteId << ") 成绩汇总 ---\n";
    // Implementation requires iterating through events and results.
    // This is a simplified placeholder.
}

void SportsMeet::cancelEvent(const std::string& eventName) {
    // This functionality needs to be fully implemented.
    // It should find an event and set its `isCancelled` flag to true.
}

void SportsMeet::manageScoringRules() {
    // This functionality needs to be fully implemented.
}

void SportsMeet::recordAndScoreEvent(Event* event) {
    if (!event) return;

    std::cout << "\n--- 记录项目成绩: " << event->name << " ---\n";
    if (event->isCancelled) {
        std::cout << "该项目已取消。\n";
        return;
    }

    if (event->registeredAthletes.empty()) {
        std::cout << "该项目没有运动员报名。\n";
        return;
    }

    for (const auto& athleteId : event->registeredAthletes) {
        const Athlete* athlete = findAthlete(athleteId);
        if (!athlete) continue;

        double performance;
        std::cout << "请输入运动员 " << athlete->name << " (ID: " << athleteId << ") 的成绩: ";
        std::cin >> performance;
        while (std::cin.fail()) {
            std::cout << "输入无效，请输入一个数字: ";
            std::cin.clear();
            std::cin.ignore(256,'\n');
            std::cin >> performance;
        }
        recordResult(event->name, event->gender, athleteId, performance);
    }
}

void SportsMeet::processScoresForEvent(const std::string& eventName, const std::string& gender) {
    // This is a placeholder for a more complex scoring logic
    // ...
}


// --- 秩序册实现 ---

/**
 * @brief 生成秩序册
 * @details 使用一个简单的贪心算法来安排所有赛事。
 * 它会遍历所有项目，并为每个项目寻找第一个可用的时间槽和场地。
 * 这个实现是基础的，可能无法为非常复杂的约束找到最优解。
 */
void SportsMeet::generateSchedule() {
    schedule.clear(); // 清空现有秩序册
    const int eventDuration = 60; // 假设所有项目持续60分钟
    const int timeStep = 30;      // 以30分钟为步长寻找可用时间

    std::cout << "\n正在生成秩序册...\n";

    for (const auto& event : events) {
        if (event->isCancelled || event->registeredAthletes.empty()) {
            continue; // 跳过已取消或无人报名的项目
        }

        bool scheduled = false;
        for (int day = 1; day <= competitionDays; ++day) {
            for (int time = dailyStartTime; time < dailyEndTime; time += timeStep) {
                int endTime = time + eventDuration;
                if (endTime > dailyEndTime) continue; // 超出当天比赛时间

                for (const auto& venue : venues) {
                    // 检查场地是否可用
                    if (!schedule.isVenueAvailable(venue, day, time, endTime)) {
                        continue;
                    }

                    // 检查所有运动员是否都可用
                    bool allAthletesAvailable = true;
                    for (const auto& athleteId : event->registeredAthletes) {
                        Athlete* athlete = findAthlete(athleteId);
                        if (!athlete || !schedule.isAthleteAvailable(athlete, day, time, endTime)) {
                            allAthletesAvailable = false;
                            break;
                        }
                    }

                    if (allAthletesAvailable) {
                        schedule.addSession(day, time, endTime, venue, event.get());
                        std::cout << "  - 已安排项目: " << event->name << " (" << event->gender << ") 在第 " << day << " 天, "
                                  << std::setw(2) << std::setfill('0') << time / 60 << ":"
                                  << std::setw(2) << std::setfill('0') << time % 60 << ", " << venue << "\n";
                        scheduled = true;
                        goto next_event; // 使用 goto 跳转到下一个项目的安排
                    }
                }
            }
        }

    next_event:
        if (!scheduled) {
            std::cout << "  - 警告: 无法为项目 " << event->name << " (" << event->gender << ") 找到合适的时间安排。\n";
        }
    }
    std::cout << "秩序册生成完毕！\n";
}

/**
 * @brief 显示秩序册
 * @details 从 Schedule 对象获取数据并格式化输出。
 */
void SportsMeet::showSchedule() const {
    const auto& sessions = schedule.getFullSchedule();
    if (sessions.empty()) {
        std::cout << "\n秩序册为空，请先生成秩序册。\n";
        return;
    }

    std::cout << "\n--- 运动会秩序册 ---\n";

    // 按天分组
    std::map<int, std::vector<const ScheduledSession*>> sessionsByDay;
    for (const auto& session : sessions) {
        sessionsByDay[session.day].push_back(&session);
    }
    
    // 按天和时间排序并打印
    for (auto const& [day, daySessions] : sessionsByDay) {
        std::cout << "\n--- 第 " << day << " 天 ---\n";
        
        // C++17 a bit tricky to sort vector of pointers, create a copy to sort
        auto sortedSessions = daySessions;
        std::sort(sortedSessions.begin(), sortedSessions.end(), [](const ScheduledSession* a, const ScheduledSession* b){
            return a->startTime < b->startTime;
        });

        for (const auto* session : sortedSessions) {
            std::cout << std::setw(2) << std::setfill('0') << session->startTime / 60 << ":" << std::setw(2) << std::setfill('0') << session->startTime % 60
                      << " - "
                      << std::setw(2) << std::setfill('0') << session->endTime / 60 << ":" << std::setw(2) << std::setfill('0') << session->endTime % 60
                      << " | " << std::setw(10) << std::left << session->venue
                      << " | " << session->event->name << " (" << session->event->gender << ")\n";
        }
    }
}
// ... 其他管理功能待实现 ...
// void generateSchedule();
// void recordResults();
// void queryResults();