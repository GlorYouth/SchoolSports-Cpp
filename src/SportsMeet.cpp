#include "SportsMeet.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <map>
#include "BackupData.h"
#include <cmath> // For std::abs
#include <iomanip>
#include <windows.h>
#include "TimeUtils.h" // 引入时间转换工具

// --- 辅助函数，用于将数据写入二进制文件流 ---

// 写入一个普通的、固定大小的变量
template<typename T>
void write_binary(std::ofstream& ofs, const T& value) {
    ofs.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

// 特化：写入 std::string
void write_binary(std::ofstream& ofs, const std::string& str) {
    size_t len = str.length();
    write_binary(ofs, len); // 先写入长度
    ofs.write(str.c_str(), len); // 再写入字符串内容
}

// 特化：写入 std::vector (需要一个辅助函数来写入每个元素)
template<typename T, typename Func>
void write_vector(std::ofstream& ofs, const std::vector<T>& vec, Func write_element) {
    size_t size = vec.size();
    write_binary(ofs, size);
    for (const auto& elem : vec) {
        write_element(ofs, elem);
    }
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

// --- 各种数据结构的写入函数 ---
void write_result_data(std::ofstream& ofs, const ResultData& data) {
    write_binary(ofs, data.athleteId);
    write_binary(ofs, data.rank);
    write_binary(ofs, data.performance);
    write_binary(ofs, data.sortablePerformance);
    write_binary(ofs, data.pointsAwarded);
}

void write_event_data(std::ofstream& ofs, const EventData& data) {
    write_binary(ofs, data.name);
    write_binary(ofs, data.type);
    write_binary(ofs, data.gender);
    write_binary(ofs, data.duration);
    write_binary(ofs, data.minParticipants);
    write_binary(ofs, data.maxParticipants);
    write_vector(ofs, data.participantIds, [](std::ofstream& out, const std::string& s){ write_binary(out, s); });
    write_vector(ofs, data.results, write_result_data);
}

void write_athlete_data(std::ofstream& ofs, const AthleteData& data) {
    write_binary(ofs, data.name);
    write_binary(ofs, data.id);
    write_binary(ofs, data.isMale);
    write_binary(ofs, data.unitName);
    write_binary(ofs, data.individualScore);
}

void write_unit_data(std::ofstream& ofs, const UnitData& data) {
    write_binary(ofs, data.name);
    write_binary(ofs, data.totalScore);
    write_vector(ofs, data.athletes, write_athlete_data);
}

void write_scoring_rule(std::ofstream& ofs, const ScoringRule& rule) {
    write_binary(ofs, rule.minParticipants);
    size_t size = rule.scores.size();
    write_binary(ofs, size);
    if(size > 0) {
        ofs.write(reinterpret_cast<const char*>(rule.scores.data()), size * sizeof(int));
    }
}

SportsMeet::SportsMeet() : maxEventsPerAthlete(3), minParticipantsForCancel(4) {
    std::cout << "欢迎使用学校运动会管理系统!" << std::endl;
    
    // 初始化默认计分规则
    // 规则1: 参赛人数>=7 (原>6), 取前5名
    scoringRules.emplace_back(7, std::vector<int>{7, 5, 3, 2, 1});
    // 规则2: 参赛人数 4-6人, 取前3名
    scoringRules.emplace_back(4, std::vector<int>{5, 3, 2});
    
    // 将规则按最少参与人数降序排序，便于查找
    std::sort(scoringRules.begin(), scoringRules.end(), [](const ScoringRule& a, const ScoringRule& b) {
        return a.minParticipants > b.minParticipants;
    });
}

void SportsMeet::setMaxEventsPerAthlete(int max) {
    if (max > 0) {
        maxEventsPerAthlete = max;
        std::cout << "运动员参赛项目限制已更新为: " << max << "项" << std::endl;
    } else {
        std::cout << "设置失败，限制数必须为正整数。" << std::endl;
    }
}

void SportsMeet::addUnit(const std::string& unitName) {
    if (findUnit(unitName) == nullptr) {
        units.emplace_back(std::make_unique<Unit>(unitName));
        std::cout << "参赛单位 '" << unitName << "' 添加成功。" << std::endl;
    } else {
        std::cout << "错误: 单位 '" << unitName << "' 已存在。" << std::endl;
    }
}

void SportsMeet::addEvent(const std::string& name, EventType type, GenderCategory gender, int duration, int min_participants, int max_participants) {
    if (findEvent(name, gender) == nullptr) {
        events.emplace_back(std::make_unique<Event>(name, type, gender, duration, min_participants, max_participants));
        std::cout << "比赛项目 '" << name << "' 添加成功。" << std::endl;
    } else {
        std::cout << "错误: 项目 '" << name << "' (" << (gender == GenderCategory::MALE ? "男" : "女") << ") 已存在。" << std::endl;
    }
}

void SportsMeet::deleteEvent(const std::string& name, GenderCategory gender) {
    auto it = std::find_if(events.begin(), events.end(), [&](const auto& event_ptr){
        return event_ptr->getName() == name && event_ptr->getGender() == gender;
    });

    if (it == events.end()) {
        std::cout << "错误：未找到要删除的项目。" << std::endl;
        return;
    }

    Event* event_to_delete = it->get();

    // 1. 通知所有运动员取消对该项目的注册
    for (auto& unit_ptr : units) {
        for (auto& athlete_ptr : unit_ptr->getAthletes()) {
            athlete_ptr->deregisterFromEvent(event_to_delete);
        }
    }

    // 2. 从主列表中删除项目
    events.erase(it);

    std::cout << "项目 '" << name << "' 已成功删除。" << std::endl;
}

void SportsMeet::manageScoringRules() {
    std::cout << "\n--- 计分规则管理 ---" << std::endl;
    std::cout << "当前规则:" << std::endl;
    for (const auto& rule : scoringRules) {
        std::cout << "  - 当参赛人数 >= " << rule.minParticipants << ", 计 " << rule.scores.size() << " 名, 分值为: ";
        for (size_t i = 0; i < rule.scores.size(); ++i) {
            std::cout << rule.scores[i] << (i == rule.scores.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;
    }
    std::cout << "注意: 参赛人数不足 " << minParticipantsForCancel << " 人的项目将被取消。" << std::endl;
    
    // 交互式添加新规则
    std::cout << "\n是否要添加新的计分规则? (y/n): ";
    char choice;
    std::cin >> choice;
    if (choice != 'y' && choice != 'Y') {
        return;
    }

    int min_participants;
    std::cout << "请输入新规则适用的【最少】参赛人数: ";
    std::cin >> min_participants;

    // 检查规则是否已存在
    for (const auto& rule : scoringRules) {
        if (rule.minParticipants == min_participants) {
            std::cout << "错误: 针对 " << min_participants << " 人的规则已存在。" << std::endl;
            return;
        }
    }

    int num_places;
    std::cout << "请输入要计分的【名次数】: ";
    std::cin >> num_places;
    
    std::vector<int> scores;
    std::cout << "请依次输入每个名次的得分: " << std::endl;
    for (int i = 0; i < num_places; ++i) {
        int score;
        std::cout << "  第 " << i + 1 << " 名得分: ";
        std::cin >> score;
        scores.push_back(score);
    }

    scoringRules.emplace_back(min_participants, scores);
    // 重新排序以确保逻辑正确
    std::sort(scoringRules.begin(), scoringRules.end(), [](const ScoringRule& a, const ScoringRule& b) {
        return a.minParticipants > b.minParticipants;
    });

    std::cout << "新计分规则添加成功！" << std::endl;
}

void SportsMeet::recordAndScoreEvent() {
    std::cout << "\n--- 记录比赛成绩 ---" << std::endl;
    
    // 1. 选择项目
    std::string eventName;
    int genderChoice;
    std::cout << "请输入要记录成绩的项目名称: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, eventName);
    std::cout << "请选择性别组别 (1: 男, 2: 女): ";
    std::cin >> genderChoice;
    
    Event* event = findEvent(eventName, (genderChoice == 1) ? GenderCategory::MALE : GenderCategory::FEMALE);
    if (!event) {
        std::cout << "错误: 未找到该项目。" << std::endl;
        return;
    }
    if (event->hasResults()) {
        std::cout << "错误: 该项目已有成绩记录。" << std::endl;
        return;
    }

    // 2. 检查参赛人数
    auto participants = event->getParticipants();
    if (participants.size() < minParticipantsForCancel) {
        std::cout << "项目 '" << eventName << "' 因参赛人数不足 " << minParticipantsForCancel << " 人，已被自动取消。" << std::endl;
        // 可选: 添加一个状态标记项目为已取消
        return;
    }
    
    std::cout << "为项目 '" << event->getName() << "' 录入成绩，共有 " << participants.size() << " 名选手。" << std::endl;

    // 3. 循环输入成绩
    std::vector<Result> results;
    for (Athlete* athlete : participants) {
        std::string perfStr;
        double perfVal;
        std::cout << "  - 请输入选手 [" << athlete->getUnit()->getName() << "] " 
                  << athlete->getName() << " 的成绩 (数值，如11.2或2.02): ";
        std::cin >> perfVal;
        std::cout << "    请输入成绩的显示文本 (如 11''2 或 2m02): ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, perfStr);

        results.emplace_back(athlete, perfStr, perfVal);
    }
    
    // 4. 排序
    bool sortAscending = (event->getType() == EventType::TRACK); // 径赛成绩越小越好
    std::sort(results.begin(), results.end(), [sortAscending](const Result& a, const Result& b){
        if (sortAscending) {
            return a.sortablePerformance < b.sortablePerformance;
        } else {
            return a.sortablePerformance > b.sortablePerformance;
        }
    });

    // 5. 选择计分规则
    const ScoringRule* ruleToUse = nullptr;
    for (const auto& rule : scoringRules) {
        if (participants.size() >= rule.minParticipants) {
            ruleToUse = &rule;
            break; // 因为已排序，找到的第一个就是最合适的
        }
    }

    // 6. & 7. 分配名次和积分
    if (ruleToUse) {
        std::cout << "\n--- 比赛结果与计分 ---" << std::endl;
        int placesToAward = ruleToUse->scores.size();
        for (int i = 0; i < results.size() && i < placesToAward; ++i) {
            results[i].rank = i + 1;
            results[i].pointsAwarded = ruleToUse->scores[i];
            
            // 更新运动员和单位分数
            results[i].athlete->addScore(results[i].pointsAwarded);
            results[i].athlete->getUnit()->addScore(results[i].pointsAwarded);

            std::cout << "第 " << results[i].rank << " 名: " 
                      << results[i].athlete->getName() << " (" << results[i].athlete->getUnit()->getName() << ")"
                      << ", 成绩: " << results[i].performance 
                      << ", 获得 " << results[i].pointsAwarded << " 分。" << std::endl;
        }
    } else {
        std::cout << "没有适用于 " << participants.size() << " 人参赛的计分规则。" << std::endl;
    }

    // 8. 存储成绩
    event->setResults(results);
    std::cout << "\n项目 '" << event->getName() << "' 的成绩已记录并保存。" << std::endl;
}

// 占位实现
void SportsMeet::registerAthlete() {
    std::cout << "\n--- 运动员报名登记 ---" << std::endl;
    
    // 1. 选择单位
    std::string unitName;
    std::cout << "请输入运动员所属单位名称: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, unitName);

    Unit* unit = findUnit(unitName);
    if (!unit) {
        std::cout << "错误: 未找到单位 '" << unitName << "'。请先在系统设置中添加单位。" << std::endl;
        return;
    }

    // 2. 输入运动员信息
    std::string athleteName, athleteId;
    int genderChoice;
    std::cout << "请输入运动员姓名: ";
    std::getline(std::cin, athleteName);
    std::cout << "请输入运动员ID (学号): ";
    std::getline(std::cin, athleteId);
    std::cout << "请选择性别 (1: 男, 2: 女): ";
    std::cin >> genderChoice;
    bool isMale = (genderChoice == 1);

    Athlete* newAthlete = unit->addAthlete(athleteName, athleteId, isMale);
    std::cout << "运动员 '" << athleteName << "' 信息创建成功。" << std::endl;
    
    // 3. 为运动员选择项目
    for (int i = 0; i < maxEventsPerAthlete; ++i) {
        std::cout << "\n--- 为 " << newAthlete->getName() << " 报名第 " << i + 1 << " 个项目 ---" << std::endl;
        
        std::vector<Event*> available_events;
        for(const auto& event_ptr : this->events) {
            if (event_ptr->getGender() == (isMale ? GenderCategory::MALE : GenderCategory::FEMALE)) {
                available_events.push_back(event_ptr.get());
            }
        }

        if (available_events.empty()) {
            std::cout << "当前没有可报名的" << (isMale ? "男子" : "女子") << "项目。" << std::endl;
            break;
        }

        std::cout << "可报名的" << (isMale ? "男子" : "女子") << "项目列表:" << std::endl;
        for (size_t j = 0; j < available_events.size(); ++j) {
            std::cout << j + 1 << ". " << available_events[j]->getName() << std::endl;
        }

        std::cout << "请输入项目编号来报名 (输入 0 完成报名): ";
        int eventChoice;
        std::cin >> eventChoice;

        if (eventChoice == 0) break;
        
        if (eventChoice > 0 && eventChoice <= static_cast<int>(available_events.size())) {
            Event* selectedEvent = available_events[eventChoice - 1];
            
            if (newAthlete->isRegisteredFor(selectedEvent)) {
                 std::cout << "错误: 该运动员已报名项目 '" << selectedEvent->getName() << "'。" << std::endl;
                i--;
            } else {
                newAthlete->registerForEvent(selectedEvent);
                std::cout << "报名成功: " << selectedEvent->getName() << std::endl;
            }
        } else {
            std::cout << "无效的项目编号。" << std::endl;
            i--;
        }
    }
    std::cout << "\n运动员 '" << newAthlete->getName() << "' 报名流程结束，共报名 " << newAthlete->getRegisteredEvents().size() << " 个项目。" << std::endl;
}

void SportsMeet::showAllUnits() const {
    std::cout << "\n--- 当前所有参赛单位 ---" << std::endl;
    if (units.empty()) {
        std::cout << "暂无参赛单位。" << std::endl;
    } else {
        for (const auto& unit : units) {
            std::cout << "- " << unit->getName() << std::endl;
        }
    }
}

void SportsMeet::showAllEvents() const {
    std::cout << "\n--- 当前所有比赛项目 ---" << std::endl;
    if (events.empty()) {
        std::cout << "暂无比赛项目。" << std::endl;
        return;
    }
    
    std::cout << "男子项目:" << std::endl;
    bool male_found = false;
    for (const auto& event_ptr : events) {
        if (event_ptr->getGender() == GenderCategory::MALE) {
            std::cout << "  - " << event_ptr->getName() << std::endl;
            male_found = true;
        }
    }
    if (!male_found) std::cout << "  暂无" << std::endl;

    std::cout << "女子项目:" << std::endl;
    bool female_found = false;
    for (const auto& event_ptr : events) {
        if (event_ptr->getGender() == GenderCategory::FEMALE) {
            std::cout << "  - " << event_ptr->getName() << std::endl;
            female_found = true;
        }
    }
    if (!female_found) std::cout << "  暂无" << std::endl;
}

Unit* SportsMeet::findUnit(const std::string& unitName) {
    auto it = std::find_if(units.begin(), units.end(), [&](const auto& unit_ptr){
        return unit_ptr->getName() == unitName;
    });
    return (it != units.end()) ? it->get() : nullptr;
}

const Unit* SportsMeet::findUnit(const std::string& unitName) const {
    auto it = std::find_if(units.begin(), units.end(), [&](const auto& unit_ptr){
        return unit_ptr->getName() == unitName;
    });
    return (it != units.end()) ? it->get() : nullptr;
}

Event* SportsMeet::findEvent(const std::string& eventName, GenderCategory gender) {
    auto it = std::find_if(events.begin(), events.end(), [&](const auto& event_ptr){
        return event_ptr->getName() == eventName && event_ptr->getGender() == gender;
    });
    return (it != events.end()) ? it->get() : nullptr;
}

const Event* SportsMeet::findEvent(const std::string& eventName, GenderCategory gender) const {
    auto it = std::find_if(events.begin(), events.end(), [&](const auto& event_ptr){
        return event_ptr->getName() == eventName && event_ptr->getGender() == gender;
    });
    return (it != events.end()) ? it->get() : nullptr;
}

void SportsMeet::showUnitDetails(const std::string& unitName) const {
    const Unit* unit = findUnit(unitName);
    if (!unit) {
        std::cout << "错误: 未找到单位 '" << unitName << "'" << std::endl;
        return;
    }

    std::cout << "\n--- 单位详情: " << unit->getName() << " ---" << std::endl;
    std::cout << "总分: " << unit->getTotalScore() << std::endl;
    std::cout << "运动员列表:" << std::endl;
    const auto& athletes = unit->getAthletes();
    if (athletes.empty()) {
        std::cout << "  该单位暂无运动员。" << std::endl;
    } else {
        for (const auto& athletePtr : athletes) {
            std::cout << "  - 姓名: " << athletePtr->getName() << ", ID: " << athletePtr->getId()
                      << ", 性别: " << (athletePtr->getIsMale() ? "男" : "女") 
                      << ", 个人总分: " << athletePtr->getIndividualScore() << std::endl;
            const auto& registeredEvents = athletePtr->getRegisteredEvents();
            if (!registeredEvents.empty()) {
                std::cout << "    已报项目: ";
                for (const auto& event : registeredEvents) {
                    std::cout << event->getName() << " ";
                }
                std::cout << std::endl;
            }
        }
    }
}

void SportsMeet::showEventDetails(const std::string& eventName, GenderCategory gender) const {
    const Event* event = findEvent(eventName, gender);
     if (!event) {
        std::cout << "错误: 未找到项目 '" << eventName << "'" << std::endl;
        return;
    }
    
    std::cout << "\n--- 项目详情: " << event->getName() << " (" << (gender == GenderCategory::MALE ? "男子" : "女子") << ") ---" << std::endl;
    std::cout << "参赛选手:" << std::endl;
    const auto& participants = event->getParticipants();
    if (participants.empty()) {
        std::cout << "  该项目暂无参赛选手。" << std::endl;
    } else {
        for (const auto& athlete : participants) {
            std::cout << "  - 姓名: " << athlete->getName() << ", 单位: " << athlete->getUnit()->getName() << std::endl;
        }
    }

    // 新增：显示比赛结果
    if (event->hasResults()) {
        std::cout << "\n比赛结果:" << std::endl;
        const auto& results = event->getResults();
        for (const auto& result : results) {
            if (result.rank > 0) { // 只显示获得名次的
                 std::cout << "  第 " << result.rank << " 名: " 
                           << result.athlete->getName() 
                           << "\t单位: " << result.athlete->getUnit()->getName()
                           << "\t成绩: " << result.performance 
                           << "\t得分: " << result.pointsAwarded << std::endl;
            }
        }
    }
}

void SportsMeet::generateSchedule() {
    std::vector<Event*> events_to_schedule;
    for (auto& event_ptr : this->events) {
        if (!event_ptr->getParticipants().empty()) {
            events_to_schedule.push_back(event_ptr.get());
        }
    }

    if (events_to_schedule.empty()) {
        std::cout << "没有需要安排的项目。" << std::endl;
        return;
    }

    // 1. 按项目的"复杂性"（参赛人数）降序排序，优先安排复杂项目
    std::sort(events_to_schedule.begin(), events_to_schedule.end(), [](const Event* a, const Event* b) {
        return a->getParticipantCount() > b->getParticipantCount();
    });

    schedule.clear();
    
    const std::vector<std::pair<int, int>> time_windows = {
        {9 * 60, 12 * 60},  // 上午: 09:00 - 12:00
        {14 * 60, 17 * 60} // 下午: 14:00 - 17:00
    };
    const int time_step = 15;

    for (Event* event : events_to_schedule) {
        long best_score = -1;
        int best_day = -1;
        int best_start_time = -1;
        const std::string& venue = (event->getType() == EventType::TRACK) ? "主田径场" : "副场地";

        for (int day = 1; day <= 7; ++day) {
            for (const auto& window : time_windows) {
                for (int start_time = window.first; start_time <= window.second - event->getDuration(); start_time += time_step) {
                    int end_time = start_time + event->getDuration();

                    bool is_available = schedule.isVenueAvailable(venue, day, start_time, end_time);
                    if (is_available) {
                        for (const auto& athlete : event->getParticipants()) {
                            if (!schedule.isAthleteAvailable(athlete, day, start_time, end_time)) {
                                is_available = false;
                                break;
                            }
                        }
                    }

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

        if (best_day != -1) {
            schedule.addSession(best_day, best_start_time, best_start_time + event->getDuration(), venue, event);
        } else {
            std::cout << "警告：项目 " << event->getName() << " 无法在7天内找到合适的时间安排！" << std::endl;
        }
    }

    std::cout << "秩序册已生成！" << std::endl;
}

void SportsMeet::showSchedule() const {
    const auto& full_schedule = schedule.getFullSchedule();
    if (full_schedule.empty()) {
        std::cout << "秩序册为空，请先生成秩序册。" << std::endl;
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
            std::cout << "\n-------------------- 第 " << current_day << " 天 --------------------" << std::endl;
            std::cout << std::left << std::setw(20) << "时间"
                      << std::setw(30) << "项目名称"
                      << std::setw(20) << "场地" << std::endl;
            std::cout << "------------------------------------------------------------" << std::endl;
        }
        
        std::string time_str = TimeUtils::toHHMM(session.startTime) + " - " + TimeUtils::toHHMM(session.endTime);

        std::cout << std::left << std::setw(20) << time_str
                  << std::setw(30) << session.event->getName()
                  << std::setw(20) << session.venue << std::endl;

        // 新增：显示该项目的所有参赛者
        const auto& participants = session.event->getParticipants();
        if (!participants.empty()) {
            std::string time_col_indent(20, ' ');
            // 基础缩进，用于对齐 "  └" 之后的内容
            std::string list_indent(24, ' '); 

            // 打印列表标题
            std::cout << time_col_indent << "  └ 参赛者:" << std::endl;
            
            // 逐行打印每个参赛者，形成一个垂直对齐的列
            for (const auto& athlete : participants) {
                std::cout << list_indent << "- " << athlete->getName() 
                          << " (" << athlete->getUnit()->getName() << ")" << std::endl;
            }
        }
    }

    std::cout << "\n=================================================================\n" << std::endl;
}

void SportsMeet::backupData(const std::string& filename) const {
    std::cout << "正在备份数据到 " << filename << " ..." << std::endl;
    BackupData dataPackage;

    // 1. System settings
    dataPackage.maxEventsPerAthlete = this->maxEventsPerAthlete;
    dataPackage.minParticipantsForCancel = this->minParticipantsForCancel;
    dataPackage.allScoringRules = this->scoringRules;

    // 2. Units and Athletes
    for (const auto& unit_ptr : this->units) {
        UnitData unitData;
        unitData.name = unit_ptr->getName();
        unitData.totalScore = unit_ptr->getTotalScore();
        for (const auto& athletePtr : unit_ptr->getAthletes()) {
            AthleteData athleteData;
            athleteData.name = athletePtr->getName();
            athleteData.id = athletePtr->getId();
            athleteData.isMale = athletePtr->getIsMale();
            athleteData.unitName = athletePtr->getUnit()->getName();
            athleteData.individualScore = athletePtr->getIndividualScore();
            unitData.athletes.push_back(athleteData);
        }
        dataPackage.allUnits.push_back(unitData);
    }
    
    // 3. Events (now from a single list)
    for (const auto& event_ptr : this->events) {
        EventData eventData;
        eventData.name = event_ptr->getName();
        eventData.type = event_ptr->getType();
        eventData.gender = event_ptr->getGender();
        eventData.duration = event_ptr->getDuration();
        eventData.minParticipants = event_ptr->getMinParticipants();
        eventData.maxParticipants = event_ptr->getMaxParticipants();
        for (const auto& participant : event_ptr->getParticipants()) {
            eventData.participantIds.push_back(participant->getId());
        }
        for (const auto& result : event_ptr->getResults()) {
            ResultData resultData;
            resultData.athleteId = result.athlete->getId();
            resultData.rank = result.rank;
            resultData.performance = result.performance;
            resultData.sortablePerformance = result.sortablePerformance;
            resultData.pointsAwarded = result.pointsAwarded;
            eventData.results.push_back(resultData);
        }
        dataPackage.allEvents.push_back(eventData);
    }

    // 4. Write to file
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
    
    ofs.close();
    std::cout << "数据备份成功。" << std::endl;
}

// --- 各种数据结构的读取函数 ---
void read_result_data(std::ifstream& ifs, ResultData& data) {
    read_binary(ifs, data.athleteId);
    read_binary(ifs, data.rank);
    read_binary(ifs, data.performance);
    read_binary(ifs, data.sortablePerformance);
    read_binary(ifs, data.pointsAwarded);
}

void read_event_data(std::ifstream& ifs, EventData& data) {
    read_binary(ifs, data.name);
    read_binary(ifs, data.type);
    read_binary(ifs, data.gender);
    read_binary(ifs, data.duration);
    read_binary(ifs, data.minParticipants);
    read_binary(ifs, data.maxParticipants);
    read_vector(ifs, data.participantIds, [](std::ifstream& in, std::string& s){ read_binary(in, s); });
    read_vector(ifs, data.results, read_result_data);
}

void read_athlete_data(std::ifstream& ifs, AthleteData& data) {
    read_binary(ifs, data.name);
    read_binary(ifs, data.id);
    read_binary(ifs, data.isMale);
    read_binary(ifs, data.unitName);
    read_binary(ifs, data.individualScore);
}

void read_unit_data(std::ifstream& ifs, UnitData& data) {
    read_binary(ifs, data.name);
    read_binary(ifs, data.totalScore);
    read_vector(ifs, data.athletes, read_athlete_data);
}

void read_scoring_rule(std::ifstream& ifs, ScoringRule& rule) {
    read_binary(ifs, rule.minParticipants);
    size_t size;
    read_binary(ifs, size);
    rule.scores.resize(size);
    if(size > 0) {
        ifs.read(reinterpret_cast<char*>(rule.scores.data()), size * sizeof(int));
    }
}

void SportsMeet::restoreData(const std::string& filename) {
    std::cout << "正在从 " << filename << " 恢复数据..." << std::endl;

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "错误: 无法打开文件进行读取: " << filename << std::endl;
        return;
    }

    BackupData dataPackage;

    // 1. 从文件读取数据到 dataPackage
    read_binary(ifs, dataPackage.maxEventsPerAthlete);
    read_binary(ifs, dataPackage.minParticipantsForCancel);
    read_vector(ifs, dataPackage.allScoringRules, read_scoring_rule);
    read_vector(ifs, dataPackage.allUnits, read_unit_data);
    read_vector(ifs, dataPackage.allEvents, read_event_data);
    
    ifs.close();

    // 2. 清空当前状态
    this->units.clear();
    this->events.clear();
    this->scoringRules.clear();

    // 3. 恢复设置
    this->maxEventsPerAthlete = dataPackage.maxEventsPerAthlete;
    this->minParticipantsForCancel = dataPackage.minParticipantsForCancel;
    this->scoringRules = dataPackage.allScoringRules;

    // 4. 重建单位和运动员
    for (const auto& unitData : dataPackage.allUnits) {
        auto newUnit = std::make_unique<Unit>(unitData.name);
        newUnit->setTotalScore(unitData.totalScore);
        for (const auto& athleteData : unitData.athletes) {
            Athlete* newAthlete = newUnit->addAthlete(athleteData.name, athleteData.id, athleteData.isMale);
            newAthlete->addScore(athleteData.individualScore);
        }
        this->units.push_back(std::move(newUnit));
    }
    
    // Create athlete map for linking
    std::map<std::string, Athlete*> athleteMap;
    for(auto& unit_ptr : this->units) {
        for(auto& athletePtr : unit_ptr->getAthletes()) {
            athleteMap[athletePtr->getId()] = athletePtr.get();
        }
    }

    // 5. 重建项目
    for (const auto& eventData : dataPackage.allEvents) {
        this->addEvent(eventData.name, eventData.type, eventData.gender, eventData.duration, eventData.minParticipants, eventData.maxParticipants);
        Event* currentEvent = findEvent(eventData.name, eventData.gender);
        
        // 关联参赛者
        for (const auto& athleteId : eventData.participantIds) {
            if (athleteMap.count(athleteId)) {
                Athlete* pAthlete = athleteMap[athleteId];
                currentEvent->addParticipant(pAthlete);
                pAthlete->registerForEvent(currentEvent);
            }
        }

        // 重建成绩
        std::vector<Result> newResults;
        for (const auto& resultData : eventData.results) {
             if (athleteMap.count(resultData.athleteId)) {
                Athlete* pAthlete = athleteMap[resultData.athleteId];
                Result newResult(pAthlete, resultData.performance, resultData.sortablePerformance);
                newResult.rank = resultData.rank;
                newResult.pointsAwarded = resultData.pointsAwarded;
                newResults.push_back(newResult);
            }
        }
        currentEvent->setResults(newResults);
    }
    
    std::cout << "数据恢复成功。" << std::endl;
} 