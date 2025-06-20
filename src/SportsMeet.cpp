#include "SportsMeet.h"
#include "Unit.h"
#include "Athlete.h"
#include "Event.h"
#include "Result.h"
#include "ScoringRule.h"
#include "BackupData.h"
#include "Gender.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iomanip>
#include <map>
#include <sstream>
#include <limits> // 添加此行以支持 std::numeric_limits
#include <chrono>

SportsMeet::SportsMeet() : 
    schedule(*this),  // 正确初始化Schedule
    maxEventsPerAthlete(2),
    minParticipantsForCancel(3),
    competitionDays(3),  // 默认三天比赛
    morningStartTime(8 * 60),  // 8:00 AM
    morningEndTime(12 * 60),   // 12:00 PM
    afternoonStartTime(14 * 60), // 2:00 PM
    afternoonEndTime(18 * 60)   // 6:00 PM
{
    venues.push_back("主场");
    venues.push_back("副场");
    
    // 添加默认计分规则 - 使用正确的构造函数
    std::vector<SubRule> standardRules;
    standardRules.emplace_back(7, std::vector<int>{7, 5, 3, 2, 1});
    standardRules.emplace_back(4, std::vector<int>{5, 3, 2});
    scoringRules.emplace_back("标准计分规则", 3, standardRules);
}

SportsMeet::~SportsMeet() = default;

void SportsMeet::setMaxEventsPerAthlete(int max) {
    if (max > 0) {
        maxEventsPerAthlete = max;
        std::cout << "运动员参赛项目数量限制已设置为 " << max << " 个项目。\n";
    } else {
        std::cout << "错误：参赛项目数量限制必须大于0。\n";
    }
}

void SportsMeet::setCompetitionDays(int days) {
    competitionDays = days;
}

void SportsMeet::setMorningTimeWindow(int startHour, int startMinute, int endHour, int endMinute) {
    // 检查时间格式有效性
    if (startHour < 0 || startHour > 23 || startMinute < 0 || startMinute > 59 ||
        endHour < 0 || endHour > 23 || endMinute < 0 || endMinute > 59) {
        std::cout << "错误：时间格式无效。小时必须在0-23之间，分钟必须在0-59之间。\n";
        return;
    }
    
    // 检查开始时间是否早于结束时间
    int newStartTime = startHour * 60 + startMinute;
    int newEndTime = endHour * 60 + endMinute;
    
    if (newStartTime >= newEndTime) {
        std::cout << "错误：上午开始时间必须早于结束时间。\n";
        return;
    }
    
    // 检查上午时间是否与下午时间重叠
    if (newEndTime > afternoonStartTime) {
        std::cout << "错误：上午结束时间不能晚于下午开始时间 (" 
                  << afternoonStartTime / 60 << ":" 
                  << std::setw(2) << std::setfill('0') << afternoonStartTime % 60 << ")。\n";
        return;
    }
    
    morningStartTime = newStartTime;
    morningEndTime = newEndTime;
    
    std::cout << "上午比赛时间窗口已设置为 " 
              << startHour << ":" << std::setw(2) << std::setfill('0') << startMinute 
              << " - " 
              << endHour << ":" << std::setw(2) << std::setfill('0') << endMinute << "\n";
}

void SportsMeet::setAfternoonTimeWindow(int startHour, int startMinute, int endHour, int endMinute) {
    // 检查时间格式有效性
    if (startHour < 0 || startHour > 23 || startMinute < 0 || startMinute > 59 ||
        endHour < 0 || endHour > 23 || endMinute < 0 || endMinute > 59) {
        std::cout << "错误：时间格式无效。小时必须在0-23之间，分钟必须在0-59之间。\n";
        return;
    }
    
    // 检查开始时间是否早于结束时间
    int newStartTime = startHour * 60 + startMinute;
    int newEndTime = endHour * 60 + endMinute;
    
    if (newStartTime >= newEndTime) {
        std::cout << "错误：下午开始时间必须早于结束时间。\n";
        return;
    }
    
    // 检查下午时间是否与上午时间重叠
    if (newStartTime < morningEndTime) {
        std::cout << "错误：下午开始时间不能早于上午结束时间 (" 
                  << morningEndTime / 60 << ":" 
                  << std::setw(2) << std::setfill('0') << morningEndTime % 60 << ")。\n";
        return;
    }
    
    afternoonStartTime = newStartTime;
    afternoonEndTime = newEndTime;
    
    std::cout << "下午比赛时间窗口已设置为 " 
              << startHour << ":" << std::setw(2) << std::setfill('0') << startMinute 
              << " - " 
              << endHour << ":" << std::setw(2) << std::setfill('0') << endMinute << "\n";
}

void SportsMeet::addUnit(const std::string& unitName) {
    if (!findUnit(unitName)) {
        units.emplace_back(std::make_unique<Unit>(unitName));
        std::cout << "单位 '" << unitName << "' 添加成功！\n";
    } else {
        std::cout << "错误: 单位 '" << unitName << "' 已存在。\n";
    }
}

void SportsMeet::addAthleteToUnit(const std::string& unitName, const std::string& athleteId, const std::string& athleteName, Gender gender) {
    Unit* unit = findUnit(unitName);
    if (unit) {
        unit->addAthlete(athleteId, athleteName, gender);
        std::cout << "运动员 '" << athleteName << "' 已添加到单位 '" << unitName << "'。\n";
        
        // 标记查询数据结构需要重新初始化
        isHashMapInitialized = false;
        isSortedVectorInitialized = false;
    } else {
        std::cout << "错误: 未找到单位 '" << unitName << "'。\n";
    }
}

void SportsMeet::addEvent(const std::string& name, Gender gender, bool isTimeBased, const ScoringRule& rule, int durationMinutes) {
    if (findEvent(name, gender)) {
        std::cout << "错误: 项目 '" << name << "' (" << genderToString(gender) << ") 已存在。\n";
        return;
    }
    events.emplace_back(std::make_unique<Event>(name, gender, isTimeBased, rule, durationMinutes));
    std::cout << "项目 '" << name << "' 添加成功！\n";
}

void SportsMeet::deleteEvent(const std::string& eventName, Gender gender) {
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

void SportsMeet::registerAthleteForEvent(const std::string& athleteId, const std::string& eventName, Gender eventGender) {
    Athlete* athlete = findAthlete(athleteId);
    if (!athlete) {
        std::cout << "错误: 未找到学号为 " << athleteId << " 的运动员。\n";
        return;
    }

    Event* event = findEvent(eventName, eventGender);
    if (!event) {
        std::cout << "错误: 未找到项目 '" << eventName << "' (" << genderToString(eventGender) << ")。\n";
        return;
    }

    // 使用Event的allowsGender方法检查性别要求
    if (!event->allowsGender(athlete->gender)) {
        std::cout << "错误: 运动员性别 (" << genderToString(athlete->gender) << ") 与项目要求性别 (" 
                  << genderToString(event->gender) << ") 不符。\n";
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

void SportsMeet::recordResult(const std::string& eventName, Gender eventGender, const std::string& athleteId, double performance) {
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

    // 使用 eventName + "_" + genderToString(eventGender) 作为键
    Result result(athleteId, performance);
    eventResults[eventName + "_" + genderToString(eventGender)].push_back(result);
    std::cout << "已记录 " << athlete->name << " 在项目 '" << eventName << "' 的成绩: " << performance << "\n";
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
                  << " (" << genderToString(events[i]->gender) << ")"
                  << (events[i]->isTimeBased ? " [计时赛]" : " [计分赛]")
                  << " [计分规则: " << events[i]->scoringRule.ruleName << "]"
                  << " [持续时间: " << events[i]->durationMinutes << "分钟]"
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
                          << ", 性别: " << genderToString(athlete->gender) << "\n";
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

Event* SportsMeet::findEvent(const std::string& eventName, Gender gender) {
    for (auto& event : events) {
        if (event->name == eventName && event->gender == gender) {
            return event.get();
        }
    }
    return nullptr;
}

const Event* SportsMeet::findEvent(const std::string& eventName, Gender gender) const {
    for (const auto& event : events) {
        if (event->name == eventName && event->gender == gender) {
            return event.get();
        }
    }
    return nullptr;
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
            std::cout << "  - " << athlete->name << " (ID: " << athlete->id << ", 性别: " << genderToString(athlete->gender) << ")\n";
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

void SportsMeet::showEventDetails(const std::string& eventName, Gender gender) const {
    const Event* event = findEvent(eventName, gender);
    if (!event) {
        std::cout << "未找到项目 '" << eventName << "' (" << genderToString(gender) << ")。\n";
        return;
    }
    
    std::cout << "\n--- 项目详情: " << event->name << " (" << genderToString(event->gender) << ") ---\n";
    std::cout << "类型: " << (event->isTimeBased ? "径赛(计时)" : "田赛(计分)") << "\n";
    std::cout << "状态: " << (event->isCancelled ? "已取消" : "正常") << "\n";
    std::cout << "计分规则: " << event->scoringRule.ruleName << "\n";
    std::cout << "持续时间: " << event->durationMinutes << " 分钟\n";
    
    std::cout << "已报名运动员数量: " << event->registeredAthletes.size() << "\n";
    if (!event->registeredAthletes.empty()) {
        std::cout << "已报名运动员:\n";
        for (size_t i = 0; i < event->registeredAthletes.size(); ++i) {
            Athlete* athlete = findAthlete(event->registeredAthletes[i]);
            if (athlete) {
                std::cout << i + 1 << ". " << athlete->name << " (" << athlete->id 
                          << "), " << genderToString(athlete->gender) << ", " << athlete->unit->name << "\n";
            }
        }
    }
    
    // 显示项目成绩
    std::string eventKey = eventName + "_" + genderToString(gender);
    auto resultsIt = eventResults.find(eventKey);
    if (resultsIt != eventResults.end() && !resultsIt->second.empty()) {
        std::cout << "\n成绩记录:\n";
        for (size_t i = 0; i < resultsIt->second.size(); ++i) {
            const auto& result = resultsIt->second[i];
            Athlete* athlete = findAthlete(result.athleteId);
            if (athlete) {
                std::cout << i + 1 << ". " << athlete->name << " (" << athlete->unit->name << "): " 
                          << result.performance << (event->isTimeBased ? " 秒" : " 米") << "\n";
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
            std::cout << "--- 项目: " << eventName << " (" << genderToString(event->gender) << ") ---\n";
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
                 std::cout << "--- 项目: " << eventName << " (" << genderToString(event->gender) << ") ---\n";
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
    while (true) {
        std::cout << "\n--- 计分规则管理 ---\n";
        std::cout << "  1. 查看所有计分规则\n";
        std::cout << "  2. 添加新计分规则\n";
        std::cout << "  3. 返回上级菜单\n";
        std::cout << "-----------------------\n";
        std::cout << "请输入选项: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::cout << "\n--- 当前计分规则 ---\n";
            for (size_t i = 0; i < scoringRules.size(); ++i) {
                const auto& rule = scoringRules[i];
                std::cout << i + 1 << ". " << rule.ruleName 
                          << " (最少参赛人数: " << rule.minParticipantsRequired << ")\n";
                
                for (size_t j = 0; j < rule.subRules.size(); ++j) {
                    const auto& subRule = rule.subRules[j];
                    std::cout << "   子规则 " << j + 1 << ": 参赛人数 >= " 
                              << subRule.minParticipants << " 时\n";
                    std::cout << "     计分: ";
                    for (int score : subRule.scores) {
                        std::cout << score << " ";
                    }
                    std::cout << "\n";
                }
            }
        } else if (choice == 2) {
            std::string ruleName;
            int minParticipantsRequired;
            
            std::cout << "请输入规则名称: ";
            std::getline(std::cin, ruleName);
            
            std::cout << "请输入举办比赛的最少参赛人数: ";
            std::cin >> minParticipantsRequired;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::vector<SubRule> subRules;
            while (true) {
                std::cout << "\n--- 添加子规则 ---\n";
                std::cout << "  1. 添加子规则\n";
                std::cout << "  2. 完成子规则添加\n";
                std::cout << "-----------------\n";
                std::cout << "请输入选项: ";
                
                int subChoice;
                std::cin >> subChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (subChoice == 1) {
                    int minParticipants;
                    std::string scoresStr;
                    
                    std::cout << "请输入子规则适用的最少参赛人数: ";
                    std::cin >> minParticipants;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    
                    std::cout << "请输入计分列表 (用空格分隔, 例如 7 5 3 2 1): ";
                    std::getline(std::cin, scoresStr);
                    
                    std::vector<int> scores;
                    std::stringstream ss(scoresStr);
                    int score;
                    while(ss >> score) {
                        scores.push_back(score);
                    }
                    
                    subRules.emplace_back(minParticipants, std::move(scores));
                    std::cout << "子规则已添加。\n";
                } else if (subChoice == 2) {
                    break;
                } else {
                    std::cout << "无效选项。\n";
                }
            }
            
            if (!subRules.empty()) {
                // 按照最少参赛人数降序排序子规则
                std::sort(subRules.begin(), subRules.end(), [](const SubRule& a, const SubRule& b) {
                    return a.minParticipants > b.minParticipants;
                });
                
                scoringRules.emplace_back(ruleName, minParticipantsRequired, std::move(subRules));
                std::cout << "新计分规则已添加。\n";
            } else {
                std::cout << "未添加任何子规则，计分规则创建失败。\n";
            }
        } else if (choice == 3) {
            break;
        } else {
            std::cout << "无效选项。\n";
        }
    }
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

void SportsMeet::processScoresForEvent(const std::string& eventName, Gender gender) {
    Event* event = findEvent(eventName, gender);
    if (!event) {
        std::cout << "错误: 未找到项目 '" << eventName << "' (" << genderToString(gender) << ")。\n";
        return;
    }
    
    // 使用 eventName + "_" + gender 作为键
    std::string eventKey = eventName + "_" + genderToString(gender);
    auto resultsIt = eventResults.find(eventKey);
    if (resultsIt == eventResults.end() || resultsIt->second.empty()) {
        std::cout << "没有可用于计分的成绩记录。\n";
        return;
    }
    
    // 复制比赛结果以进行排序
    std::vector<Result> results = resultsIt->second;
    
    // 按照成绩排序 (计时赛和计分赛的排序方向相反)
    if (event->isTimeBased) {
        // 计时赛，小的成绩更好
        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.performance < b.performance;
        });
    } else {
        // 计分赛，大的成绩更好
        std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
            return a.performance > b.performance;
        });
    }
    
    // 确定适用的计分规则
    if (results.size() < (size_t)minParticipantsForCancel) {
        std::cout << "项目 '" << eventName << "' 参赛人数不足，该项目被取消。\n";
        event->isCancelled = true;
        return;
    }
    
    // 获取绑定的计分规则
    const ScoringRule& rule = event->scoringRule;
    
    // 获取基于参赛人数的适用子规则
    const SubRule* subRule = rule.getSubRuleForParticipants(results.size());
    
    if (!subRule) {
        std::cout << "无法为项目 '" << eventName << "' 找到适用的计分规则，参赛人数： " 
                  << results.size() << "\n";
        return;
    }
    
    std::cout << "\n--- 项目 '" << eventName << "' 计分结果 ---\n";
    std::cout << "使用规则：" << rule.ruleName << "\n";
    std::cout << "参赛人数：" << results.size() << "\n";
    
    // 计分人数不能超过参赛人数或计分表的长度
    int placesToAward = std::min(static_cast<size_t>(subRule->scores.size()), results.size());
    
    for (int i = 0; i < placesToAward; ++i) {
        results[i].rank = i + 1;
        results[i].points = subRule->scores[i];
        
        Athlete* athlete = findAthlete(results[i].athleteId);
        if (athlete) {
            athlete->score += results[i].points;
            
            // 找到运动员所属单位并计分
            for (auto& unit : units) {
                auto athlete_it = std::find_if(unit->athletes.begin(), unit->athletes.end(),
                    [&](const std::unique_ptr<Athlete>& a) { 
                        return a->id == results[i].athleteId; 
                    });
                
                if (athlete_it != unit->athletes.end()) {
                    unit->score += results[i].points;
                    break;
                }
            }
            
            std::cout << results[i].rank << ". " 
                      << athlete->name << ": " 
                      << results[i].performance << ", 得分: " 
                      << results[i].points << "\n";
        }
    }
    
    // 更新事件结果
    resultsIt->second = results;
}


// --- 秩序册实现 ---

/**
 * @brief 生成秩序册
 * @details 使用一个更复杂的算法来安排所有赛事。
 * 它会优先安排参与者多的项目，并尝试在每日的上下午时间窗口中均衡地安排赛事。
 */
void SportsMeet::generateSchedule() {
    // 首先清空现有秩序册
    schedule.clear();
    
    // 检查所有项目的报名人数，标记参赛人数不足的项目为已取消
    for (auto& event : events) {
        if (!event->isCancelled && event->registeredAthletes.size() < (size_t)minParticipantsForCancel) {
            std::cout << "项目 '" << event->name << " (" << genderToString(event->gender) << ")' 参赛人数不足，该项目被取消。\n";
            event->isCancelled = true;
        }
    }
    
    std::vector<Event*> eventsToSchedule;
    for (auto& event : events) {
        // 只安排有选手报名的、未被取消的项目
        if (!event->isCancelled && !event->registeredAthletes.empty()) {
            eventsToSchedule.push_back(event.get());
        }
    }
    
    if (eventsToSchedule.empty()) {
        std::cout << "没有需要安排的项目。" << std::endl;
        return;
    }
    
    // 按项目的参赛人数降序排序，优先安排复杂项目
    std::sort(eventsToSchedule.begin(), eventsToSchedule.end(), [](const Event* a, const Event* b) {
        return a->registeredAthletes.size() > b->registeredAthletes.size();
    });
    
    std::cout << "\n正在生成秩序册...\n";
    
    const int timeStep = 15;     // 以15分钟为步长寻找可用时间
    
    // 使用实例中的时间窗口设置
    const std::vector<std::pair<int, int>> timeWindows = {
        {morningStartTime, morningEndTime},      // 上午时间窗口
        {afternoonStartTime, afternoonEndTime}  // 下午时间窗口
    };
    
    for (Event* event : eventsToSchedule) {
        long bestScore = -1;
        int bestDay = -1;
        int bestStartTime = -1;
        // 根据项目类型（计时/计分）分配场地
        std::string venue = event->isTimeBased ? "主场" : "副场";
        
        for (int day = 1; day <= competitionDays; ++day) {
            for (const auto& window : timeWindows) {
                for (int startTime = window.first; startTime <= window.second - event->durationMinutes; startTime += timeStep) {
                    int endTime = startTime + event->durationMinutes;
                    
                    // 检查场地和所有运动员是否都可用
                    bool isAvailable = schedule.isVenueAvailable(venue, day, startTime, endTime);
                    if (isAvailable) {
                        for (const auto& athleteId : event->registeredAthletes) {
                            Athlete* athlete = findAthlete(athleteId);
                            if (!athlete || !schedule.isAthleteAvailable(athlete, day, startTime, endTime)) {
                                isAvailable = false;
                                break;
                            }
                        }
                    }
                    
                    // 如果找到可用时间，则计算一个分数来评估这个时间点的好坏（倾向于靠前和均衡）
                    if (isAvailable) {
                        int morningEvents = 0;
                        int afternoonEvents = 0;
                        for (const auto& session : schedule.getFullSchedule()) {
                            if (session.day == day) {
                                if (session.startTime < 13 * 60) morningEvents++;
                                else afternoonEvents++;
                            }
                        }
                        if (startTime < 13 * 60) morningEvents++;
                        else afternoonEvents++;
                        
                        long balancePenalty = std::abs(morningEvents - afternoonEvents);
                        long currentScore = (long)day * 100000 + balancePenalty * 1000 + startTime;
                        
                        if (bestScore == -1 || currentScore < bestScore) {
                            bestScore = currentScore;
                            bestDay = day;
                            bestStartTime = startTime;
                        }
                    }
                }
            }
        }
        
        // 将项目安排在找到的最佳时间
        if (bestDay != -1) {
            schedule.addSession(bestDay, bestStartTime, bestStartTime + event->durationMinutes, venue, event);
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
                  << std::setw(30) << session.event->name + " (" + genderToString(session.event->gender) + ")"
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
    write_binary(ofs, rule.ruleName);
    write_binary(ofs, rule.minParticipantsRequired);
    
    // 写入子规则数量
    size_t subRulesCount = rule.subRules.size();
    write_binary(ofs, subRulesCount);
    
    // 写入每个子规则
    for (const auto& subRule : rule.subRules) {
        write_binary(ofs, subRule.minParticipants);
        write_vector(ofs, subRule.scores, [](std::ofstream& out, int score){ write_binary(out, score); });
    }
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
    write_binary(ofs, data.scoringRuleName);
    write_binary(ofs, data.durationMinutes);
    write_vector(ofs, data.registeredAthletes, [](std::ofstream& out, const std::string& str){ write_binary(out, str); });
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
    read_binary(ifs, rule.ruleName);
    read_binary(ifs, rule.minParticipantsRequired);
    
    // 读取子规则数量
    size_t subRulesCount;
    read_binary(ifs, subRulesCount);
    
    rule.subRules.clear();
    rule.subRules.reserve(subRulesCount);
    
    // 读取每个子规则
    for (size_t i = 0; i < subRulesCount; ++i) {
        SubRule subRule;
        read_binary(ifs, subRule.minParticipants);
        read_vector(ifs, subRule.scores, [](std::ifstream& in, int& score){ read_binary(in, score); });
        rule.subRules.push_back(std::move(subRule));
    }
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
    read_binary(ifs, data.scoringRuleName);
    read_binary(ifs, data.durationMinutes);
    read_vector(ifs, data.registeredAthletes, [](std::ifstream& in, std::string& str){ read_binary(in, str); });
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
    dataPackage.competitionDays = this->competitionDays;  // 添加比赛天数
    dataPackage.morningStartTime = this->morningStartTime;
    dataPackage.morningEndTime = this->morningEndTime;
    dataPackage.afternoonStartTime = this->afternoonStartTime;
    dataPackage.afternoonEndTime = this->afternoonEndTime;
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
        eventData.scoringRuleName = event_ptr->scoringRule.ruleName;
        eventData.durationMinutes = event_ptr->durationMinutes;
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
    write_binary(ofs, dataPackage.competitionDays);  // 写入比赛天数
    write_binary(ofs, dataPackage.morningStartTime);
    write_binary(ofs, dataPackage.morningEndTime);
    write_binary(ofs, dataPackage.afternoonStartTime);
    write_binary(ofs, dataPackage.afternoonEndTime);
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
    read_binary(ifs, dataPackage.competitionDays);  // 读取比赛天数
    read_binary(ifs, dataPackage.morningStartTime);
    read_binary(ifs, dataPackage.morningEndTime);
    read_binary(ifs, dataPackage.afternoonStartTime);
    read_binary(ifs, dataPackage.afternoonEndTime);
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
    this->competitionDays = dataPackage.competitionDays;  // 恢复比赛天数
    this->morningStartTime = dataPackage.morningStartTime;
    this->morningEndTime = dataPackage.morningEndTime;
    this->afternoonStartTime = dataPackage.afternoonStartTime;
    this->afternoonEndTime = dataPackage.afternoonEndTime;
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
    for (const auto& event_data : dataPackage.allEvents) {
        // 在恢复时，需要根据名称找到计分规则
        auto it = std::find_if(scoringRules.begin(), scoringRules.end(),
                               [&](const ScoringRule& rule){ return rule.ruleName == event_data.scoringRuleName; });
        
        if (it != scoringRules.end()) {
            auto event = std::make_unique<Event>(event_data.name, event_data.gender, event_data.isTimeBased, *it, event_data.durationMinutes);
            event->isCancelled = event_data.isCancelled;
            event->registeredAthletes = event_data.registeredAthletes;
            this->events.push_back(std::move(event));
        }
    }
    
    // 6. 恢复成绩
    this->eventResults = dataPackage.allEventResults;
    
    // 7. 重置查询数据结构状态
    this->isHashMapInitialized = false;
    this->isSortedVectorInitialized = false;

    std::cout << "数据恢复成功。" << std::endl;
}

const std::vector<ScoringRule>& SportsMeet::getScoringRules() const {
    return scoringRules;
}

// 初始化运动员查询数据结构
void SportsMeet::initializeAthleteSearchStructures() const {
    // 清空之前的数据
    athleteHashMap.clear();
    sortedAthletes.clear();
    
    // 收集所有运动员
    for (const auto& unit : units) {
        for (const auto& athlete : unit->athletes) {
            athleteHashMap[athlete->id] = athlete.get();
            sortedAthletes.emplace_back(athlete->id, athlete.get());
        }
    }
    
    // 排序以便进行二分查找
    std::sort(sortedAthletes.begin(), sortedAthletes.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    isHashMapInitialized = true;
    isSortedVectorInitialized = true;
}

// 使用哈希表查找运动员
Athlete* SportsMeet::findAthleteByHash(const std::string& athleteId) const {
    if (!isHashMapInitialized) {
        initializeAthleteSearchStructures();
    }
    
    auto it = athleteHashMap.find(athleteId);
    if (it != athleteHashMap.end()) {
        return it->second;
    }
    return nullptr;
}

// 使用二分查找运动员
Athlete* SportsMeet::findAthleteByBinarySearch(const std::string& athleteId) const {
    if (!isSortedVectorInitialized) {
        initializeAthleteSearchStructures();
    }
    
    auto it = std::lower_bound(sortedAthletes.begin(), sortedAthletes.end(), athleteId,
                               [](const auto& pair, const std::string& id) {
                                   return pair.first < id;
                               });
    
    if (it != sortedAthletes.end() && it->first == athleteId) {
        return it->second;
    }
    return nullptr;
}

// 运动员查询算法性能比较
void SportsMeet::compareAthleteSearchAlgorithms() const {
    if (units.empty()) {
        std::cout << "没有单位信息，无法进行查询性能比较。\n";
        return;
    }
    
    // 确保数据结构已初始化
    initializeAthleteSearchStructures();
    
    // 获取所有运动员ID用于测试
    std::vector<std::string> athleteIds;
    for (const auto& unit : units) {
        for (const auto& athlete : unit->athletes) {
            athleteIds.push_back(athlete->id);
        }
    }
    
    if (athleteIds.empty()) {
        std::cout << "没有运动员信息，无法进行查询性能比较。\n";
        return;
    }
    
    std::cout << "\n=== 运动员查询算法性能比较 ===\n";
    std::cout << "测试数据规模: " << athleteIds.size() << " 个运动员\n\n";
    
    // 线性查找性能测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& id : athleteIds) {
            Athlete* athlete = findAthlete(id);
            if (!athlete) {
                std::cout << "错误: 线性查找未找到运动员 " << id << "\n";
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "1. 线性查找 (O(n*m)):\n";
        std::cout << "   总耗时: " << duration.count() << " 毫秒\n";
        std::cout << "   平均每次查询: " << duration.count() / athleteIds.size() << " 毫秒\n\n";
    }
    
    // 哈希表查找性能测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& id : athleteIds) {
            Athlete* athlete = findAthleteByHash(id);
            if (!athlete) {
                std::cout << "错误: 哈希表查找未找到运动员 " << id << "\n";
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "2. 哈希表查找 (O(1)):\n";
        std::cout << "   总耗时: " << duration.count() << " 毫秒\n";
        std::cout << "   平均每次查询: " << duration.count() / athleteIds.size() << " 毫秒\n\n";
    }
    
    // 二分查找性能测试
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& id : athleteIds) {
            Athlete* athlete = findAthleteByBinarySearch(id);
            if (!athlete) {
                std::cout << "错误: 二分查找未找到运动员 " << id << "\n";
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << "3. 二分查找 (O(log n)):\n";
        std::cout << "   总耗时: " << duration.count() << " 毫秒\n";
        std::cout << "   平均每次查询: " << duration.count() / athleteIds.size() << " 毫秒\n\n";
    }
    
    std::cout << "=== 性能比较结论 ===\n";
    std::cout << "从理论上分析:\n";
    std::cout << "- 线性查找: O(n*m) 复杂度，其中n是单位数，m是每个单位的平均运动员数\n";
    std::cout << "- 哈希表查找: O(1) 期望复杂度，但需要额外空间存储哈希表\n";
    std::cout << "- 二分查找: O(log n) 复杂度，n是总运动员数，但需要预先排序\n";
    std::cout << "实际表现见上述测试数据。\n";
    
    // 提示数据结构维护的开销
    std::cout << "\n注意: 哈希表和排序数组需要在数据变更时更新，这会带来额外开销。\n";
    std::cout << "在大型比赛中，查询次数远多于变更次数时，优化的查找方法优势更为明显。\n";
}