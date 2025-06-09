#include "SportsMeet.h"
#include "Unit.h"
#include "Athlete.h"
#include "Event.h"
#include "Result.h"
#include "ScoringRule.h"
#include "BackupData.h"
#include "TimeUtils.h" // <--- 关键的修复
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <map>
#include <sstream>

SportsMeet::SportsMeet() : schedule(*this), maxEventsPerAthlete(3), minParticipantsForCancel(4) {
    // 初始化排程参数
    venues = {"主田径场", "跳远沙坑", "铅球区"};
    competitionDays = 2;
    dailyStartTime = 9 * 60; // 09:00
    dailyEndTime = 17 * 60;   // 17:00

    // 初始化默认计分规则
    // 规则1: 参赛人数 >= 7, 取前5名
    scoringRules.emplace_back(7, std::vector<int>{7, 5, 3, 2, 1});
    // 规则2: 参赛人数 4-6人, 取前3名
    scoringRules.emplace_back(4, std::vector<int>{5, 3, 2});
    
    // 将规则按最少参与人数降序排序，便于查找
    std::sort(scoringRules.begin(), scoringRules.end(), [](const ScoringRule& a, const ScoringRule& b) {
        return a.minParticipants > b.minParticipants;
    });
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

void SportsMeet::showAllAthletes() const {
    std::cout << "\n--- 所有运动员列表 ---\n";
    bool foundAny = false;
    for (const auto& unit : units) {
        if (!unit->athletes.empty()) {
            foundAny = true;
            std::cout << "--- 单位: " << unit->name << " ---\n";
            for (const auto& athlete : unit->athletes) {
                std::cout << "  - ID: " << athlete->id
                          << ", 姓名: " << athlete->name
                          << ", 性别: " << athlete->gender << "\n";
            }
        }
    }

    if (!foundAny) {
        std::cout << "系统中没有运动员信息。\n";
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

    // 新增：显示比赛结果
    auto it = eventResults.find(event->name);
    if (it != eventResults.end() && !it->second.empty()) {
        std::cout << "\n  比赛结果:\n";
        std::cout << "    " << std::left << std::setw(8) << "名次"
                  << std::setw(15) << "姓名"
                  << std::setw(15) << "单位"
                  << std::setw(12) << "成绩"
                  << std::setw(10) << "得分" << std::endl;
        std::cout << "    ------------------------------------------------------\n";
        for (const auto& result : it->second) {
            if (result.rank > 0) { // 只显示获得名次的
                const Athlete* athlete = findAthlete(result.athleteId);
                if(athlete) {
                    std::cout << "    " << std::left << std::setw(8) << result.rank
                              << std::setw(15) << athlete->name
                              << std::setw(15) << athlete->unit->name
                              << std::setw(12) << result.performance
                              << std::setw(10) << result.points << std::endl;
                }
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
    std::cout << "单位总分: " << unit->score << "\n\n";

    bool foundResults = false;
    for (const auto& pair : eventResults) {
        const std::string& eventName = pair.first;
        const std::vector<Result>& results = pair.second;
        
        std::vector<const Result*> unitEventResults;
        for (const auto& result : results) {
            const Athlete* athlete = findAthlete(result.athleteId);
            if (athlete && athlete->unit->name == unitName && result.rank > 0) {
                unitEventResults.push_back(&result);
            }
        }

        if (!unitEventResults.empty()) {
            foundResults = true;
            const Event* event = findEvent(eventName, findAthlete(unitEventResults[0]->athleteId)->gender);
            std::cout << "--- 项目: " << eventName << " (" << (event ? event->gender : "") << ") ---\n";
             for (const auto* result : unitEventResults) {
                const Athlete* athlete = findAthlete(result->athleteId);
                std::cout << "  - " << std::left << std::setw(15) << athlete->name
                          << "名次: " << std::setw(5) << result->rank
                          << "成绩: " << std::setw(10) << result->performance
                          << "得分: " << result->points << "\n";
            }
            std::cout << "\n";
        }
    }

    if (!foundResults) {
        std::cout << "该单位目前没有获得任何名次。\n";
    }
}

void SportsMeet::showAthleteResults(const std::string& athleteId) const {
    const Athlete* athlete = findAthlete(athleteId);
    if (!athlete) {
        std::cout << "未找到运动员: " << athleteId << std::endl;
        return;
    }

    std::cout << "\n--- " << athlete->name << " (ID: " << athleteId << ") 成绩汇总 ---\n";
    std::cout << "个人总分: " << athlete->score << "\n\n";

    bool foundResults = false;
    for (const auto& pair : eventResults) {
        const std::string& eventName = pair.first;
        const std::vector<Result>& results = pair.second;

        for (const auto& result : results) {
            if (result.athleteId == athleteId && result.rank > 0) {
                foundResults = true;
                const Event* event = findEvent(eventName, athlete->gender);
                 std::cout << "--- 项目: " << eventName << " (" << (event ? event->gender : "") << ") ---\n";
                 std::cout << "  - 名次: " << result.rank
                           << ", 成绩: " << result.performance
                           << ", 得分: " << result.points << "\n\n";
            }
        }
    }
    
    if (!foundResults) {
        std::cout << "该运动员目前没有获得任何名次。\n";
    }
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
    Event* event = findEvent(eventName, gender);
    if (!event) {
        std::cout << "错误：找不到项目 " << eventName << " (" << gender << ") 以进行计分。\n";
        return;
    }

    auto it = eventResults.find(event->name);
    if (it == eventResults.end() || it->second.empty()) {
        std::cout << "项目 " << eventName << " 没有成绩记录可供处理。\n";
        return;
    }

    std::vector<Result>& results = it->second;

    // 排序
    bool sortAscending = event->isTimeBased; // 计时项目，成绩越小越好
    std::sort(results.begin(), results.end(), [sortAscending](const Result& a, const Result& b) {
        if (sortAscending) {
            return a.performance < b.performance;
        } else {
            return a.performance > b.performance;
        }
    });

    // 查找合适的计分规则
    const ScoringRule* ruleToUse = nullptr;
    for (const auto& rule : scoringRules) {
        if (results.size() >= (size_t)rule.minParticipants) {
            ruleToUse = &rule;
            break; 
        }
    }

    // 分配名次和积分
    if (ruleToUse) {
        std::cout << "\n--- 项目 '" << eventName << "' 计分结果 ---\n";
        int placesToAward = ruleToUse->scores.size();
        for (int i = 0; i < (int)results.size() && i < placesToAward; ++i) {
            results[i].rank = i + 1;
            results[i].points = ruleToUse->scores[i];
            
            Athlete* athlete = findAthlete(results[i].athleteId);
            if (athlete) {
                athlete->score += results[i].points;
                Unit* unit = findUnit(athlete->unit->name); // 通过名字找到运动员所在的单位
                if(unit) {
                    unit->score += results[i].points;
                }

                 std::cout << "第 " << results[i].rank << " 名: " 
                      << athlete->name << " (" << unit->name << ")"
                      << ", 成绩: " << results[i].performance 
                      << ", 获得 " << results[i].points << " 分。\n";
            }
        }
    } else {
        std::cout << "没有适用于 " << results.size() << " 人参赛的计分规则。\n";
    }
}


// --- 秩序册实现 ---

/**
 * @brief 生成秩序册
 * @details 使用一个更复杂的算法来安排所有赛事。
 * 它会优先安排参与者多的项目，并尝试在每日的上下午时间窗口中均衡地安排赛事。
 */
void SportsMeet::generateSchedule() {
    schedule.clear(); // 清空现有秩序册

    std::vector<Event*> events_to_schedule;
    for (auto& event_ptr : this->events) {
        // 只安排有选手报名的、未被取消的项目
        if (!event_ptr->isCancelled && !event_ptr->registeredAthletes.empty()) {
            events_to_schedule.push_back(event_ptr.get());
        }
    }

    if (events_to_schedule.empty()) {
        std::cout << "没有需要安排的项目。" << std::endl;
        return;
    }

    // 1. 按项目的参赛人数降序排序，优先安排复杂项目
    std::sort(events_to_schedule.begin(), events_to_schedule.end(), [](const Event* a, const Event* b) {
        return a->registeredAthletes.size() > b->registeredAthletes.size();
    });

    std::cout << "\n正在生成秩序册...\n";

    const int eventDuration = 60; // 假设所有项目持续60分钟
    const int time_step = 15;     // 以15分钟为步长寻找可用时间

    // 定义比赛时间窗口
    const std::vector<std::pair<int, int>> time_windows = {
        {9 * 60, 12 * 60},  // 上午: 09:00 - 12:00
        {14 * 60, 17 * 60} // 下午: 14:00 - 17:00
    };

    for (Event* event : events_to_schedule) {
        long best_score = -1;
        int best_day = -1;
        int best_start_time = -1;
        // 根据项目类型（计时/计分）分配场地
        std::string venue = event->isTimeBased ? "主田径场" : "副场地";

        for (int day = 1; day <= competitionDays; ++day) {
            for (const auto& window : time_windows) {
                for (int start_time = window.first; start_time <= window.second - eventDuration; start_time += time_step) {
                    int end_time = start_time + eventDuration;
                    
                    // 检查场地和所有运动员是否都可用
                    bool is_available = schedule.isVenueAvailable(venue, day, start_time, end_time);
                    if (is_available) {
                        for (const auto& athleteId : event->registeredAthletes) {
                            Athlete* athlete = findAthlete(athleteId);
                            if (!athlete || !schedule.isAthleteAvailable(athlete, day, start_time, end_time)) {
                                is_available = false;
                                break;
                            }
                        }
                    }

                    // 如果找到可用时间，则计算一个分数来评估这个时间点的好坏（倾向于靠前和均衡）
                    if (is_available) {
                        int morning_events = 0;
                        int afternoon_events = 0;
                        for (const auto& session : schedule.getFullSchedule()) {
                            if (session.day == day) {
                                if (session.startTime < 13 * 60) morning_events++;
                                else afternoon_events++;
                            }
                        }
                        if (start_time < 13 * 60) morning_events++;
                        else afternoon_events++;
                        
                        long balance_penalty = std::abs(morning_events - afternoon_events);
                        long current_score = (long)day * 100000 + balance_penalty * 1000 + start_time;

                        if (best_score == -1 || current_score < best_score) {
                            best_score = current_score;
                            best_day = day;
                            best_start_time = start_time;
                        }
                    }
                }
            }
        }
        
        // 将项目安排在找到的最佳时间
        if (best_day != -1) {
            schedule.addSession(best_day, best_start_time, best_start_time + eventDuration, venue, event);
        } else {
            std::cout << "警告：项目 " << event->name << " 无法在 " << competitionDays << " 天内找到合适的时间安排！" << std::endl;
        }
    }
    std::cout << "秩序册生成完毕！" << std::endl;
}

/**
 * @brief 显示秩序册
 * @details 从 Schedule 对象获取数据并格式化输出，包含每日详情和参赛者列表。
 */
void SportsMeet::showSchedule() const {
    const auto& full_schedule = schedule.getFullSchedule();
    if (full_schedule.empty()) {
        std::cout << "\n秩序册为空，请先生成秩序册。\n";
        return;
    }

    // 按天和时间排序
    auto sorted_schedule = full_schedule;
    std::sort(sorted_schedule.begin(), sorted_schedule.end(), [](const ScheduledSession& a, const ScheduledSession& b) {
        if (a.day != b.day) return a.day < b.day;
        return a.startTime < b.startTime;
    });

    std::cout << "\n======================== 运动会秩序册 ========================\n" << std::endl;

    int current_day = -1;
    for (const auto& session : sorted_schedule) {
        if (session.day != current_day) {
            current_day = session.day;
            if (current_day > 1) {
                 std::cout << "-------------------------------------------------------------------\n" << std::endl;
            }
            std::cout << "-------------------- 第 " << current_day << " 天 --------------------" << std::endl;
            std::cout << std::left << std::setw(20) << "时间"
                      << std::setw(30) << "项目名称"
                      << std::setw(20) << "场地" << std::endl;
            std::cout << "-------------------------------------------------------------------\n";
        }
        
        std::stringstream time_ss;
        time_ss << std::setw(2) << std::setfill('0') << session.startTime / 60 << ":" << std::setw(2) << std::setfill('0') << session.startTime % 60
                << " - "
                << std::setw(2) << std::setfill('0') << session.endTime / 60 << ":" << std::setw(2) << std::setfill('0') << session.endTime % 60;

        std::cout << std::left << std::setw(20) << time_ss.str()
                  << std::setw(30) << session.event->name + " (" + session.event->gender + ")"
                  << std::setw(20) << session.venue << std::endl;

        // 新增：显示该项目的所有参赛者
        const auto& participants = session.event->registeredAthletes;
        if (!participants.empty()) {
            std::string time_col_indent(20, ' ');
            std::string list_indent(24, ' '); 

            std::cout << time_col_indent << "  └ 参赛者:" << std::endl;
            
            for (const auto& athleteId : participants) {
                const Athlete* athlete = findAthlete(athleteId);
                if (athlete) {
                     std::cout << list_indent << "- " << athlete->name 
                               << " (" << athlete->unit->name << ")" << std::endl;
                }
            }
        }
    }
    std::cout << "\n==================================================================\n" << std::endl;
}
// ... 其他管理功能待实现 ...
// void generateSchedule();
// void recordResults();
// void queryResults();

// --- 辅助函数，用于将数据写入二进制文件流 ---
template<typename T>
void write_binary(std::ofstream& ofs, const T& value) {
    ofs.write(reinterpret_cast<const char*>(&value), sizeof(T));
}
void write_binary(std::ofstream& ofs, const std::string& str) {
    size_t len = str.length();
    write_binary(ofs, len);
    ofs.write(str.c_str(), len);
}
template<typename T, typename Func>
void write_vector(std::ofstream& ofs, const std::vector<T>& vec, Func write_element) {
    size_t size = vec.size();
    write_binary(ofs, size);
    for (const auto& elem : vec) {
        write_element(ofs, elem);
    }
}
template<typename K, typename V, typename FuncV>
void write_map(std::ofstream& ofs, const std::map<K, V>& m, FuncV write_value) {
    size_t size = m.size();
    write_binary(ofs, size);
    for (const auto& pair : m) {
        write_binary(ofs, pair.first);
        write_value(ofs, pair.second);
    }
}

// --- 各种数据结构的写入函数 ---
void write_scoring_rule(std::ofstream& ofs, const ScoringRule& rule) {
    write_binary(ofs, rule.minParticipants);
    write_vector(ofs, rule.scores, [](std::ofstream& out, int score){ write_binary(out, score); });
}
void write_athlete_data(std::ofstream& ofs, const AthleteData& data) {
    write_binary(ofs, data.id);
    write_binary(ofs, data.name);
    write_binary(ofs, data.gender);
    write_binary(ofs, data.score);
    write_vector(ofs, data.registeredEvents, [](std::ofstream& out, const std::string& s){ write_binary(out, s); });
}
void write_unit_data(std::ofstream& ofs, const UnitData& data) {
    write_binary(ofs, data.name);
    write_binary(ofs, data.score);
    write_vector(ofs, data.athletes, write_athlete_data);
}
void write_event_data(std::ofstream& ofs, const EventData& data) {
    write_binary(ofs, data.name);
    write_binary(ofs, data.gender);
    write_binary(ofs, data.isTimeBased);
    write_binary(ofs, data.isCancelled);
    write_vector(ofs, data.registeredAthletes, [](std::ofstream& out, const std::string& s){ write_binary(out, s); });
}
void write_result(std::ofstream& ofs, const Result& result) {
    write_binary(ofs, result.athleteId);
    write_binary(ofs, result.performance);
    write_binary(ofs, result.rank);
    write_binary(ofs, result.points);
}


// --- 辅助函数，用于从二进制文件流读取数据 ---
template<typename T>
void read_binary(std::ifstream& ifs, T& value) {
    ifs.read(reinterpret_cast<char*>(&value), sizeof(T));
}
void read_binary(std::ifstream& ifs, std::string& str) {
    size_t len;
    read_binary(ifs, len);
    str.resize(len);
    if (len > 0) {
        ifs.read(&str[0], len);
    }
}
template<typename T, typename Func>
void read_vector(std::ifstream& ifs, std::vector<T>& vec, Func read_element) {
    size_t size;
    read_binary(ifs, size);
    vec.resize(size);
    for (size_t i = 0; i < size; ++i) {
        read_element(ifs, vec[i]);
    }
}
template<typename K, typename V, typename FuncV>
void read_map(std::ifstream& ifs, std::map<K, V>& m, FuncV read_value) {
    size_t size;
    read_binary(ifs, size);
    for (size_t i = 0; i < size; ++i) {
        K key;
        V value;
        read_binary(ifs, key);
        read_value(ifs, value);
        m[key] = value;
    }
}

// --- 各种数据结构的读取函数 ---
void read_scoring_rule(std::ifstream& ifs, ScoringRule& rule) {
    read_binary(ifs, rule.minParticipants);
    read_vector(ifs, rule.scores, [](std::ifstream& in, int& score){ read_binary(in, score); });
}
void read_athlete_data(std::ifstream& ifs, AthleteData& data) {
    read_binary(ifs, data.id);
    read_binary(ifs, data.name);
    read_binary(ifs, data.gender);
    read_binary(ifs, data.score);
    read_vector(ifs, data.registeredEvents, [](std::ifstream& in, std::string& s){ read_binary(in, s); });
}
void read_unit_data(std::ifstream& ifs, UnitData& data) {
    read_binary(ifs, data.name);
    read_binary(ifs, data.score);
    read_vector(ifs, data.athletes, read_athlete_data);
}
void read_event_data(std::ifstream& ifs, EventData& data) {
    read_binary(ifs, data.name);
    read_binary(ifs, data.gender);
    read_binary(ifs, data.isTimeBased);
    read_binary(ifs, data.isCancelled);
    read_vector(ifs, data.registeredAthletes, [](std::ifstream& in, std::string& s){ read_binary(in, s); });
}
void read_result(std::ifstream& ifs, Result& result) {
    read_binary(ifs, result.athleteId);
    read_binary(ifs, result.performance);
    read_binary(ifs, result.rank);
    read_binary(ifs, result.points);
}


void SportsMeet::backupData(const std::string& filename) const {
    std::cout << "正在备份数据到 " << filename << " ..." << std::endl;
    BackupData dataPackage;

    // 1. 填充系统设置
    dataPackage.maxEventsPerAthlete = this->maxEventsPerAthlete;
    dataPackage.minParticipantsForCancel = this->minParticipantsForCancel;
    dataPackage.allScoringRules = this->scoringRules;

    // 2. 填充单位和运动员数据
    for (const auto& unit_ptr : this->units) {
        UnitData unitData;
        unitData.name = unit_ptr->name;
        unitData.score = unit_ptr->score;
        for (const auto& athlete_ptr : unit_ptr->athletes) {
            AthleteData athleteData;
            athleteData.id = athlete_ptr->id;
            athleteData.name = athlete_ptr->name;
            athleteData.gender = athlete_ptr->gender;
            athleteData.score = athlete_ptr->score;
            athleteData.registeredEvents = athlete_ptr->registeredEvents;
            unitData.athletes.push_back(athleteData);
        }
        dataPackage.allUnits.push_back(unitData);
    }
    
    // 3. 填充项目数据
    for (const auto& event_ptr : this->events) {
        EventData eventData;
        eventData.name = event_ptr->name;
        eventData.gender = event_ptr->gender;
        eventData.isTimeBased = event_ptr->isTimeBased;
        eventData.isCancelled = event_ptr->isCancelled;
        eventData.registeredAthletes = event_ptr->registeredAthletes;
        dataPackage.allEvents.push_back(eventData);
    }

    // 4. 填充成绩数据
    dataPackage.allEventResults = this->eventResults;

    // 5. 写入文件
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "错误: 无法打开文件进行写入: " << filename << std::endl;
        return;
    }

    write_binary(ofs, dataPackage.maxEventsPerAthlete);
    write_binary(ofs, dataPackage.minParticipantsForCancel);
    write_vector(ofs, dataPackage.allScoringRules, write_scoring_rule);
    write_vector(ofs, dataPackage.allUnits, write_unit_data);
    write_vector(ofs, dataPackage.allEvents, write_event_data);
    write_map(ofs, dataPackage.allEventResults, [](std::ofstream& out, const std::vector<Result>& results){
        write_vector(out, results, write_result);
    });
    
    ofs.close();
    std::cout << "数据备份成功。" << std::endl;
}


void SportsMeet::restoreData(const std::string& filename) {
    std::cout << "正在从 " << filename << " 恢复数据..." << std::endl;

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "错误: 无法打开文件进行读取: " << filename << std::endl;
        return;
    }

    BackupData dataPackage;

    // 1. 从文件读取数据
    read_binary(ifs, dataPackage.maxEventsPerAthlete);
    read_binary(ifs, dataPackage.minParticipantsForCancel);
    read_vector(ifs, dataPackage.allScoringRules, read_scoring_rule);
    read_vector(ifs, dataPackage.allUnits, read_unit_data);
    read_vector(ifs, dataPackage.allEvents, read_event_data);
    read_map(ifs, dataPackage.allEventResults, [](std::ifstream& in, std::vector<Result>& results){
        read_vector(in, results, read_result);
    });
    
    ifs.close();

    // 2. 清空当前状态
    this->units.clear();
    this->events.clear();
    this->scoringRules.clear();
    this->eventResults.clear();
    this->schedule.clear();

    // 3. 恢复设置
    this->maxEventsPerAthlete = dataPackage.maxEventsPerAthlete;
    this->minParticipantsForCancel = dataPackage.minParticipantsForCancel;
    this->scoringRules = dataPackage.allScoringRules;

    // 4. 重建单位和运动员
    for (const auto& unitData : dataPackage.allUnits) {
        auto newUnit = std::make_unique<Unit>(unitData.name);
        newUnit->score = unitData.score;
        for (const auto& athleteData : unitData.athletes) {
            newUnit->addAthlete(athleteData.id, athleteData.name, athleteData.gender);
            Athlete* pAthlete = newUnit->findAthlete(athleteData.id);
            if(pAthlete) {
                pAthlete->score = athleteData.score;
                pAthlete->registeredEvents = athleteData.registeredEvents;
            }
        }
        this->units.push_back(std::move(newUnit));
    }

    // 5. 重建项目
    for (const auto& eventData : dataPackage.allEvents) {
        auto newEvent = std::make_unique<Event>(eventData.name, eventData.gender, eventData.isTimeBased);
        newEvent->isCancelled = eventData.isCancelled;
        newEvent->registeredAthletes = eventData.registeredAthletes;
        this->events.push_back(std::move(newEvent));
    }
    
    // 6. 恢复成绩
    this->eventResults = dataPackage.allEventResults;

    std::cout << "数据恢复成功。" << std::endl;
}