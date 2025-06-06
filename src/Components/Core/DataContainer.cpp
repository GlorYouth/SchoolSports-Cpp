#include "../../../include/Components/Core/DataContainer.h"
#include "../../../include/Components/Core/Schedule.h"
#include <iostream>
#include <sstream>
#include <ctime>

DataContainer::DataContainer() :
    currentWorkflowStage(WorkflowStage::SETUP_EVENTS),
    athleteMaxEventsAllowed(3),
    scheduleLocked(false),
    morningSessionStart("08:00"),
    morningSessionEnd("12:00"),
    afternoonSessionStart("14:00"),
    afternoonSessionEnd("18:00") {
}

void DataContainer::clear() {
    // 清空所有数据结构
    unitsMap.clear();
    athletesMap.clear();
    eventsMap.clear();
    rulesMap.clear();
    resultsMap.clear();
    venues.clear();
    scheduleEntries.clear();
    
    // 恢复默认系统设置
    currentWorkflowStage = WorkflowStage::SETUP_EVENTS;
    athleteMaxEventsAllowed = 3;
    scheduleLocked = false;
    morningSessionStart = "08:00";
    morningSessionEnd = "12:00";
    afternoonSessionStart = "14:00";
    afternoonSessionEnd = "18:00";
}

bool DataContainer::serialize(std::ostream& outStream) const {
    if (!outStream.good()) {
        return false;
    }

    try {
        // 写入元数据段
        outStream << "[METADATA]" << std::endl;
        outStream << "VERSION=1.0" << std::endl;
        outStream << "DATE=" << time(nullptr) << std::endl;
        outStream << "WORKFLOW_STAGE=" << static_cast<int>(currentWorkflowStage) << std::endl;
        outStream << "ATHLETE_MAX_EVENTS=" << athleteMaxEventsAllowed << std::endl;
        outStream << "SCHEDULE_LOCKED=" << (scheduleLocked ? "1" : "0") << std::endl;
        outStream << "MORNING_SESSION_START=" << morningSessionStart << std::endl;
        outStream << "MORNING_SESSION_END=" << morningSessionEnd << std::endl;
        outStream << "AFTERNOON_SESSION_START=" << afternoonSessionStart << std::endl;
        outStream << "AFTERNOON_SESSION_END=" << afternoonSessionEnd << std::endl;

        // 写入单位段
        outStream << std::endl << "[UNITS]" << std::endl;
        outStream << "COUNT=" << unitsMap.size() << std::endl;
        for (const auto& [id, unit] : unitsMap) {
            outStream << unit.getId() << "|" << unit.getName() << "|" << unit.getTotalScore() << std::endl;
        }

        // 写入计分规则段
        outStream << std::endl << "[SCORE_RULES]" << std::endl;
        outStream << "COUNT=" << rulesMap.size() << std::endl;
        for (const auto& [id, rule] : rulesMap) {
            outStream << rule.getId() << "|" 
                    << rule.getDescription() << "|" 
                    << rule.getMinParticipants() << "|"
                    << rule.getMaxParticipants() << "|"
                    << rule.getRanksToAward() << "|";
            
            // 写入名次与对应分数关系
            bool firstScore = true;
            for (const auto& scorePair : rule.getAllScoresForRanks()) {
                if (!firstScore) {
                    outStream << ",";
                }
                outStream << scorePair.first << ":" << scorePair.second;
                firstScore = false;
            }
            outStream << std::endl;
        }

        // 写入比赛项目段
        outStream << std::endl << "[EVENTS]" << std::endl;
        outStream << "COUNT=" << eventsMap.size() << std::endl;
        for (const auto& [id, event] : eventsMap) {
            outStream << event.getId() << "|"
                    << event.getName() << "|"
                    << static_cast<int>(event.getEventType()) << "|"
                    << static_cast<int>(event.getGenderRequirement()) << "|"
                    << event.getScoreRuleId() << "|"
                    << (event.getIsCancelled() ? "1" : "0") << "|"
                    << event.getDurationMinutes() << "|"
                    << event.getVenue() << "|"
                    << event.getStartTime() << "|"
                    << event.getEndTime() << "|";
            
            // 写入参赛运动员列表
            const auto& athletes = event.getParticipantAthleteIds();
            for (size_t i = 0; i < athletes.size(); ++i) {
                if (i > 0) outStream << ",";
                outStream << athletes[i];
            }
            outStream << std::endl;
        }

        // 写入运动员段
        outStream << std::endl << "[ATHLETES]" << std::endl;
        outStream << "COUNT=" << athletesMap.size() << std::endl;
        for (const auto& [id, athlete] : athletesMap) {
            outStream << athlete.getId() << "|"
                    << athlete.getName() << "|"
                    << static_cast<int>(athlete.getGender()) << "|"
                    << athlete.getUnitId() << "|";
            
            // 写入已报名项目列表
            const auto& events = athlete.getRegisteredEventIds();
            for (size_t i = 0; i < events.size(); ++i) {
                if (i > 0) outStream << ",";
                outStream << events[i];
            }
            outStream << std::endl;
        }

        // 写入成绩段
        outStream << std::endl << "[RESULTS]" << std::endl;
        int resultCount = 0;
        for (const auto& [eventId, results] : resultsMap) {
            resultCount += results.getResultsList().size();
        }
        outStream << "COUNT=" << resultCount << std::endl;
        
        for (const auto& [eventId, results] : resultsMap) {
            for (const auto& result : results.getResultsList()) {
                outStream << eventId << "|"
                        << result.getAthleteId() << "|"
                        << result.getRank() << "|"
                        << result.getScoreRecord() << "|"
                        << result.getPointsAwarded() << std::endl;
            }
        }

        // 写入场馆信息段
        outStream << std::endl << "[VENUES]" << std::endl;
        outStream << "COUNT=" << venues.size() << std::endl;
        for (const auto& venue : venues) {
            outStream << venue << std::endl;
        }
        
        // 写入赛程安排段
        outStream << std::endl << "[SCHEDULE]" << std::endl;
        outStream << "COUNT=" << scheduleEntries.size() << std::endl;
        for (const auto& entry : scheduleEntries) {
            outStream << entry.eventId << "|"
                     << entry.startTime << "|"
                     << entry.endTime << "|"
                     << entry.venue << std::endl;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "序列化数据时发生错误: " << e.what() << std::endl;
        return false;
    }
}

bool DataContainer::deserialize(std::istream& inStream) {
    if (!inStream.good()) {
        return false;
    }

    try {
        // 先清空当前数据
        clear();
        
        std::string line;
        std::map<int, std::vector<int>> athleteEventMap; // 用于存储运动员-项目关系
        
        enum class Section {
            NONE,
            METADATA,
            UNITS,
            SCORE_RULES,
            EVENTS,
            ATHLETES,
            RESULTS,
            VENUES,
            SCHEDULE
        };
        
        Section currentSection = Section::NONE;
        
        // 逐行解析文件
        while (std::getline(inStream, line)) {
            if (line.empty()) continue;
            
            // 判断当前段落
            if (line == "[METADATA]") {
                currentSection = Section::METADATA;
                continue;
            } else if (line == "[UNITS]") {
                currentSection = Section::UNITS;
                continue;
            } else if (line == "[SCORE_RULES]") {
                currentSection = Section::SCORE_RULES;
                continue;
            } else if (line == "[EVENTS]") {
                currentSection = Section::EVENTS;
                continue;
            } else if (line == "[ATHLETES]") {
                currentSection = Section::ATHLETES;
                continue;
            } else if (line == "[RESULTS]") {
                currentSection = Section::RESULTS;
                continue;
            } else if (line == "[VENUES]") {
                currentSection = Section::VENUES;
                continue;
            } else if (line == "[SCHEDULE]") {
                currentSection = Section::SCHEDULE;
                continue;
            }
            
            // 跳过计数行
            if (line.starts_with("COUNT=")) continue;
            
            // 根据当前段落处理数据
            switch (currentSection) {
                case Section::METADATA:
                    {
                        size_t equalsPos = line.find('=');
                        if (equalsPos != std::string::npos) {
                            std::string key = line.substr(0, equalsPos);
                            std::string value = line.substr(equalsPos + 1);
                            
                            if (key == "WORKFLOW_STAGE") {
                                currentWorkflowStage = static_cast<WorkflowStage>(std::stoi(value));
                            } else if (key == "ATHLETE_MAX_EVENTS") {
                                athleteMaxEventsAllowed = std::stoi(value);
                            } else if (key == "SCHEDULE_LOCKED") {
                                scheduleLocked = (value == "1");
                            } else if (key == "MORNING_SESSION_START") {
                                morningSessionStart = value;
                            } else if (key == "MORNING_SESSION_END") {
                                morningSessionEnd = value;
                            } else if (key == "AFTERNOON_SESSION_START") {
                                afternoonSessionStart = value;
                            } else if (key == "AFTERNOON_SESSION_END") {
                                afternoonSessionEnd = value;
                            }
                        }
                    }
                    break;
                    
                case Section::UNITS:
                    {
                        // 解析单位数据: ID|名称|总分
                        std::stringstream ss(line);
                        std::string token;
                        
                        // ID
                        std::getline(ss, token, '|');
                        int id = std::stoi(token);
                        
                        // 名称
                        std::getline(ss, token, '|');
                        std::string name = token;
                        
                        // 创建单位
                        Unit unit(name);
                        unit.setId(id);
                        
                        // 总分（如果有）
                        if (std::getline(ss, token, '|')) {
                            double totalScore = std::stod(token);
                            if (totalScore > 0) {
                                unit.addScore(totalScore);
                            }
                        }
                        
                        // 添加到Map
                        unitsMap.insert({id, unit});
                    }
                    break;
                    
                case Section::SCORE_RULES:
                    {
                        // 解析计分规则: ID|描述|最小人数|最大人数|录取名次|分数...
                        std::stringstream ss(line);
                        std::string token;
                        
                        // ID
                        std::getline(ss, token, '|');
                        int id = std::stoi(token);
                        
                        // 描述
                        std::getline(ss, token, '|');
                        std::string desc = token;
                        
                        // 最小人数
                        std::getline(ss, token, '|');
                        int minP = std::stoi(token);
                        
                        // 最大人数
                        std::getline(ss, token, '|');
                        int maxP = std::stoi(token);
                        
                        // 录取名次
                        std::getline(ss, token, '|');
                        int ranks = std::stoi(token);
                        
                        // 分数映射
                        std::getline(ss, token, '|');
                        std::map<int, double> scores;
                        
                        std::stringstream scoresStream(token);
                        std::string scorePair;
                        while (std::getline(scoresStream, scorePair, ',')) {
                            size_t colonPos = scorePair.find(':');
                            if (colonPos != std::string::npos) {
                                int rank = std::stoi(scorePair.substr(0, colonPos));
                                double score = std::stod(scorePair.substr(colonPos + 1));
                                scores[rank] = score;
                            }
                        }
                        
                        // 创建规则
                        ScoreRule rule(desc, minP, maxP, ranks, scores);
                        int ruleId = rule.getId();
                        
                        // 添加到Map
                        rulesMap.insert({ruleId, rule});
                    }
                    break;
                    
                case Section::EVENTS:
                    {
                        // 解析比赛项目: ID|名称|类型|性别|规则ID|是否取消|持续时间|场地|开始时间|结束时间|参赛运动员...
                        std::stringstream ss(line);
                        std::string token;
                        
                        // ID
                        std::getline(ss, token, '|');
                        int id = std::stoi(token);
                        
                        // 名称
                        std::getline(ss, token, '|');
                        std::string name = token;
                        
                        // 类型
                        std::getline(ss, token, '|');
                        auto type = static_cast<EventType>(std::stoi(token));
                        
                        // 性别
                        std::getline(ss, token, '|');
                        auto gender = static_cast<Gender>(std::stoi(token));
                        
                        // 规则ID
                        std::getline(ss, token, '|');
                        int ruleId = std::stoi(token);
                        
                        // 创建项目
                        CompetitionEvent event(name, type, gender, ruleId);
                        event.setId(id);
                        
                        // 是否取消
                        std::getline(ss, token, '|');
                        event.setCancelled(token == "1");
                        
                        // 持续时间
                        std::getline(ss, token, '|');
                        event.setDurationMinutes(std::stoi(token));
                        
                        // 场地
                        std::getline(ss, token, '|');
                        event.setVenue(token);
                        
                        // 开始时间
                        std::getline(ss, token, '|');
                        event.setStartTime(token);
                        
                        // 结束时间
                        std::getline(ss, token, '|');
                        event.setEndTime(token);
                        
                        // 参赛运动员
                        if (std::getline(ss, token, '|') && !token.empty()) {
                            std::stringstream athletesStream(token);
                            std::string athleteIdStr;
                            while (std::getline(athletesStream, athleteIdStr, ',')) {
                                if (!athleteIdStr.empty()) {
                                    int athleteId = std::stoi(athleteIdStr);
                                    event.addParticipant(athleteId);
                                    
                                    // 存储运动员-项目关系，稍后处理
                                    if (athleteEventMap.find(athleteId) == athleteEventMap.end()) {
                                        athleteEventMap.insert({athleteId, std::vector<int>()});
                                    }
                                    athleteEventMap.at(athleteId).push_back(id);
                                }
                            }
                        }
                        
                        // 添加到Map
                        eventsMap.insert({id, event});
                    }
                    break;
                    
                case Section::ATHLETES:
                    {
                        // 解析运动员: ID|姓名|性别|单位ID|已报名项目...
                        std::stringstream ss(line);
                        std::string token;
                        
                        // ID
                        std::getline(ss, token, '|');
                        int id = std::stoi(token);
                        
                        // 姓名
                        std::getline(ss, token, '|');
                        std::string name = token;
                        
                        // 性别
                        std::getline(ss, token, '|');
                        Gender gender = static_cast<Gender>(std::stoi(token));
                        
                        // 单位ID
                        std::getline(ss, token, '|');
                        int unitId = std::stoi(token);
                        
                        // 创建运动员
                        Athlete athlete(name, gender, unitId);
                        athlete.setId(id);
                        
                        // 已报名项目
                        if (std::getline(ss, token, '|') && !token.empty()) {
                            std::stringstream eventsStream(token);
                            std::string eventIdStr;
                            while (std::getline(eventsStream, eventIdStr, ',')) {
                                if (!eventIdStr.empty()) {
                                    int eventId = std::stoi(eventIdStr);
                                    athlete.registerForEvent(eventId, athleteMaxEventsAllowed);
                                }
                            }
                        }
                        
                        // 添加到Map
                        athletesMap.insert({id, athlete});
                        
                        // 更新单位运动员列表
                        if (unitsMap.find(unitId) != unitsMap.end()) {
                            unitsMap.at(unitId).addAthleteId(id);
                        }
                    }
                    break;
                    
                case Section::RESULTS:
                    {
                        // 解析成绩: 项目ID|运动员ID|名次|成绩|得分
                        std::stringstream ss(line);
                        std::string token;
                        
                        // 项目ID
                        std::getline(ss, token, '|');
                        int eventId = std::stoi(token);
                        
                        // 运动员ID
                        std::getline(ss, token, '|');
                        int athleteId = std::stoi(token);
                        
                        // 名次
                        std::getline(ss, token, '|');
                        int rank = std::stoi(token);
                        
                        // 成绩记录
                        std::getline(ss, token, '|');
                        std::string scoreRecord = token;
                        
                        // 得分
                        std::getline(ss, token, '|');
                        double points = std::stod(token);
                        
                        // 创建成绩记录
                        Result result(eventId, athleteId, rank, scoreRecord, points);
                        
                        // 如果这个项目的成绩集合不存在，创建一个新的
                        if (resultsMap.find(eventId) == resultsMap.end()) {
                            EventResults newResults(eventId);
                            resultsMap.insert({eventId, newResults});
                        }
                        
                        // 添加成绩记录
                        resultsMap.at(eventId).addResult(result);
                        
                        // 如果运动员存在，且所属单位存在，为单位加分
                        auto athleteIt = athletesMap.find(athleteId);
                        if (athleteIt != athletesMap.end()) {
                            int unitId = athleteIt->second.getUnitId();
                            auto unitIt = unitsMap.find(unitId);
                            if (unitIt != unitsMap.end()) {
                                unitIt->second.addScore(points);
                            }
                        }
                    }
                    break;
                    
                case Section::VENUES:
                    {
                        // 场地只有一个字段
                        venues.insert(line);
                    }
                    break;
                
                case Section::SCHEDULE:
                    {
                        // 解析赛程安排: 项目ID|开始时间|结束时间|场地
                        std::stringstream ss(line);
                        std::string token;
                        
                        ScheduleEntry entry;
                        
                        // 项目ID
                        std::getline(ss, token, '|');
                        entry.eventId = std::stoi(token);
                        
                        // 开始时间
                        std::getline(ss, token, '|');
                        entry.startTime = token;
                        
                        // 结束时间
                        std::getline(ss, token, '|');
                        entry.endTime = token;
                        
                        // 场地
                        std::getline(ss, token, '|');
                        entry.venue = token;
                        
                        // 添加到赛程列表
                        scheduleEntries.push_back(entry);
                    }
                    break;
                    
                default:
                    break;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "反序列化数据时发生错误: " << e.what() << std::endl;
        return false;
    }
}

DataContainer DataContainer::createSnapshot() const {
    // 简单地创建当前对象的副本
    return *this;
}

void DataContainer::restoreFromSnapshot(const DataContainer& snapshot) {
    // 将当前对象替换为快照内容
    *this = snapshot;
} 