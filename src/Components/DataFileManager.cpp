//
// Created by User on 2025/6/15.
//

#include "../../include/Components/DataFileManager.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "../../include/Components/Unit.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"
#include "../../include/Components/Result.h"
#include "../../include/Components/Constants.h"
#include "../../include/utils.h"

DataFileManager::DataFileManager() {
    // 构造函数暂无需初始化内容
}

std::string DataFileManager::generateDefaultBackupFilename() const {
    // 获取当前时间
    const time_t now = time(nullptr);
    const tm* localTime = localtime(&now);
    
    // 格式化时间为字符串
    std::stringstream ss;
    ss << "SchoolSports_" << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S") << ".dat";
    
    return ss.str();
}

bool DataFileManager::saveDataToFile(const SystemSettings& settings, const std::string& filePath) const {
    std::string actualFilePath = filePath;
    
    // 如果没有提供文件路径，创建默认文件名与时间戳
    if (actualFilePath.empty()) {
        actualFilePath = generateDefaultBackupFilename();
    }
    
    // 打开文件
    std::ofstream outFile(actualFilePath);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行备份: " << actualFilePath << std::endl;
        return false;
    }

    std::cout << "开始保存数据到 " << actualFilePath << " ..." << std::endl;

    // 写入各个数据段
    saveMetadata(outFile, settings);
    saveUnits(outFile, settings);
    saveScoreRules(outFile, settings);
    saveEvents(outFile, settings);
    saveAthletes(outFile, settings);
    saveResults(outFile, settings);
    saveVenues(outFile, settings);

    outFile.close();
    std::cout << "数据备份成功完成，文件保存于：" << actualFilePath << std::endl;
    return true;
}

// 保存元数据段
void DataFileManager::saveMetadata(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << "[METADATA]" << std::endl;
    outFile << "VERSION=1.0" << std::endl;
    outFile << "DATE=" << time(nullptr) << std::endl; // 使用UNIX时间戳
    outFile << "WORKFLOW_STAGE=" << static_cast<int>(settings.getCurrentWorkflowStage()) << std::endl;
    outFile << "ATHLETE_MAX_EVENTS=" << settings.getAthleteMaxEventsAllowed() << std::endl;
    outFile << "SCHEDULE_LOCKED=" << (settings.isScheduleLocked() ? "1" : "0") << std::endl;
}

// 保存单位段
void DataFileManager::saveUnits(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[UNITS]" << std::endl;
    outFile << "COUNT=" << settings.getAllUnits().size() << std::endl;
    for (const auto& pair : settings.getAllUnits()) {
        const Unit& unit = pair.second;
        outFile << unit.getId() << "|" << unit.getName() << "|" << unit.getTotalScore() << std::endl;
    }
}

// 保存计分规则段
void DataFileManager::saveScoreRules(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[SCORE_RULES]" << std::endl;
    outFile << "COUNT=" << settings.getAllScoreRules().size() << std::endl;
    for (const auto& pair : settings.getAllScoreRules()) {
        const ScoreRule& rule = pair.second;
        // 写入规则信息
        outFile << rule.getId() << "|" 
                << rule.getDescription() << "|" 
                << rule.getMinParticipants() << "|"
                << rule.getMaxParticipants() << "|"
                << rule.getRanksToAward() << "|";
        
        // 写入名次与对应分数关系
        bool firstScore = true;
        for (const auto& scorePair : rule.getAllScoresForRanks()) {
            if (!firstScore) {
                outFile << ",";
            }
            outFile << scorePair.first << ":" << scorePair.second;
            firstScore = false;
        }
        outFile << std::endl;
    }
}

// 保存比赛项目段
void DataFileManager::saveEvents(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[EVENTS]" << std::endl;
    outFile << "COUNT=" << settings.getAllCompetitionEventsConst().size() << std::endl;
    for (const auto& pair : settings.getAllCompetitionEventsConst()) {
        const CompetitionEvent& event = pair.second;
        outFile << event.getId() << "|"
                << event.getName() << "|"
                << static_cast<int>(event.getEventType()) << "|"
                << static_cast<int>(event.getGenderRequirement()) << "|"
                << event.getScoreRuleId() << "|"
                << (event.getIsCancelled() ? "1" : "0") << "|"
                << event.getVenue() << "|"
                << event.getDurationMinutes() << "|"
                << event.getStartTime() << "|"
                << event.getEndTime() << "|";
                
        // 写入参赛运动员列表
        bool firstAthlete = true;
        for (int athleteId : event.getParticipantAthleteIds()) {
            if (!firstAthlete) {
                outFile << ",";
            }
            outFile << athleteId;
            firstAthlete = false;
        }
        outFile << std::endl;
    }
}

// 保存运动员段
void DataFileManager::saveAthletes(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[ATHLETES]" << std::endl;
    outFile << "COUNT=" << settings.getAllAthletes().size() << std::endl;
    for (const auto& pair : settings.getAllAthletes()) {
        const Athlete& athlete = pair.second;
        outFile << athlete.getId() << "|"
                << athlete.getName() << "|"
                << static_cast<int>(athlete.getGender()) << "|"
                << athlete.getUnitId() << "|";
                
        // 保存报名项目列表
        bool firstEvent = true;
        for (int eventId : athlete.getRegisteredEventIds()) {
            if (!firstEvent) {
                outFile << ",";
            }
            outFile << eventId;
            firstEvent = false;
        }
        outFile << std::endl;
    }
}

// 保存成绩段
void DataFileManager::saveResults(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[RESULTS]" << std::endl;
    
    // 计算所有成绩条目总数
    int resultCount = 0;
    for (const auto& pair : settings.getAllCompetitionEventsConst()) {
        int eventId = pair.first;
        auto eventResultsOpt = settings.getEventResultsConst(eventId);
        if (eventResultsOpt) {
            const EventResults& results = eventResultsOpt.value().get();
            resultCount += results.getResultsList().size();
        }
    }
    outFile << "COUNT=" << resultCount << std::endl;
    
    // 写入成绩详细
    for (const auto& pair : settings.getAllCompetitionEventsConst()) {
        int eventId = pair.first;
        auto eventResultsOpt = settings.getEventResultsConst(eventId);
        if (eventResultsOpt) {
            const EventResults& results = eventResultsOpt.value().get();
            // 写出该项目所有成绩
            for (const auto& result : results.getResultsList()) {
                outFile << eventId << "|"
                        << result.getAthleteId() << "|"
                        << result.getRank() << "|"
                        << result.getScoreRecord() << "|"
                        << result.getPointsAwarded() << std::endl;
            }
        }
    }
}

// 保存场馆信息段
void DataFileManager::saveVenues(std::ofstream& outFile, const SystemSettings& settings) const {
    outFile << std::endl << "[VENUES]" << std::endl;
    outFile << "COUNT=" << settings.getAllVenues().size() << std::endl;
    for (const auto& venue : settings.getAllVenues()) {
        outFile << venue << std::endl;
    }
}

std::string DataFileManager::createRestoreBackup(const SystemSettings& settings) const {
    // 备份操作前创建一份临时备份
    std::string backupFilePath = "auto_backup_before_restore_" + std::to_string(time(0)) + ".dat";
    if (saveDataToFile(settings, backupFilePath)) {
        std::cout << "已创建自动备份: " << backupFilePath << std::endl;
        return backupFilePath;
    } else {
        std::cout << "警告: 创建自动备份失败，继续进行数据恢复操作。" << std::endl;
        return "";
    }
}

bool DataFileManager::loadDataFromFile(SystemSettings& settings, const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行恢复: " << filePath << std::endl;
        return false;
    }

    // 创建临时备份
    createRestoreBackup(settings);

    // 清空当前系统数据
    settings.clearUnits();
    settings.clearAthletes();
    settings.clearCompetitionEvents();
    settings.clearScoreRules();
    
    // 清空所有场馆数据
    std::set<std::string> oldVenues = settings.getAllVenues();
    for (const auto& venue : oldVenues) {
        settings.removeVenue(venue);
    }

    std::cout << "开始从 " << filePath << " 恢复数据..." << std::endl;
    
    std::string line;
    std::string currentSection;
    int expectedCount = 0;
    int currentCount = 0;
    
    // 初始化默认计分规则（因为系统初始化时就会创建）
    bool hasScoreRule1 = false;
    
    // 用于存储运动员和项目的关系，因为加载顺序问题需要临时存储
    std::map<int, std::vector<int>> athleteEventMap;

    while (std::getline(inFile, line)) {
        // 跳过空行
        if (line.empty()) continue;
        
        // 解析新节标记
        if (line.front() == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            expectedCount = 0;
            currentCount = 0;
            std::cout << "正在读取段: " << currentSection << std::endl;
            continue;
        }
        
        // 解析计数行
        if (line.find("COUNT=") == 0) {
            expectedCount = std::stoi(line.substr(6));
            std::cout << "预计" << currentSection << "数量: " << expectedCount << std::endl;
            continue;
        }
        
        // 解析实际数据行
        if (currentSection == "METADATA") {
            processMetadata(line, settings);
        } else if (currentSection == "UNITS") {
            processUnit(line, settings);
            currentCount++;
        } else if (currentSection == "SCORE_RULES") {
            hasScoreRule1 = processScoreRule(line, settings) || hasScoreRule1;
            currentCount++;
        } else if (currentSection == "EVENTS") {
            processEvent(line, settings, athleteEventMap);
            currentCount++;
        } else if (currentSection == "VENUES") {
            // 解析场馆数据
            settings.addVenue(line);
            currentCount++;
        } else if (currentSection == "ATHLETES") {
            processAthlete(line, settings);
            currentCount++;
        } else if (currentSection == "RESULTS") {
            processResult(line, settings);
            currentCount++;
        }
    }

    inFile.close();
    
    std::cout << "数据恢复成功完成！" << std::endl;
    std::cout << "已恢复单位数: " << settings.getAllUnits().size() << std::endl;
    std::cout << "已恢复运动员数: " << settings.getAllAthletes().size() << std::endl;
    std::cout << "已恢复比赛项目数: " << settings.getAllCompetitionEventsConst().size() << std::endl;
    std::cout << "已恢复计分规则数: " << settings.getAllScoreRules().size() << std::endl;
    
    return true;
}

void DataFileManager::processMetadata(const std::string& line, SystemSettings& settings) {
    // 解析元数据
    size_t equalsPos = line.find('=');
    if (equalsPos != std::string::npos) {
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        if (key == "VERSION") {
            // 检查版本兼容性
            if (value != "1.0") {
                std::cerr << "警告: 文件版本(" << value << ")可能与当前系统不兼容。" << std::endl;
            }
        } else if (key == "WORKFLOW_STAGE") {
            settings.setWorkflowStage(static_cast<WorkflowStage>(std::stoi(value)));
        } else if (key == "ATHLETE_MAX_EVENTS") {
            settings.setAthleteMaxEventsAllowed(std::stoi(value));
        } else if (key == "SCHEDULE_LOCKED") {
            if (value == "1") {
                settings.lockSchedule();
            }
        }
    }
}

void DataFileManager::processUnit(const std::string& line, SystemSettings& settings) {
    // 解析单位数据: ID|名称|总分
    std::stringstream ss(line);
    std::string token;
    
    // ID
    std::getline(ss, token, '|');
    int id = std::stoi(token);
    
    // 名称
    std::getline(ss, token, '|');
    std::string name = token;
    
    // 添加单位
    int newId = settings.addUnit(name);
    if (newId != id) {
        std::cerr << "警告: 单位ID不匹配。期望: " << id << ", 实际: " << newId << std::endl;
    }
    
    // 总分
    std::getline(ss, token, '|');
    double totalScore = std::stod(token);
    if (totalScore > 0) {
        settings.addScoreToUnit(newId, totalScore); // 添加总分（如果有）
    }
}

bool DataFileManager::processScoreRule(const std::string& line, SystemSettings& settings) {
    // 解析计分规则数据: ID|描述|最小人数|最大人数|录取名次|分数...
    std::stringstream ss(line);
    std::string token;
    
    // ID
    std::getline(ss, token, '|');
    int id = std::stoi(token);
    
    // 描述
    std::getline(ss, token, '|');
    std::string description = token;
    
    // 最小人数
    std::getline(ss, token, '|');
    int minParticipants = std::stoi(token);
    
    // 最大人数
    std::getline(ss, token, '|');
    int maxParticipants = std::stoi(token);
    
    // 录取名次数
    std::getline(ss, token, '|');
    int ranksToAward = std::stoi(token);
    
    // 获取名次与分数关系
    std::getline(ss, token, '|');
    std::stringstream scoresStream(token);
    std::string scoreToken;
    std::map<int, double> scoresMap;
    
    while (std::getline(scoresStream, scoreToken, ',')) {
        if (size_t colonPos = scoreToken.find(':'); colonPos != std::string::npos) {
            int rank = std::stoi(scoreToken.substr(0, colonPos));
            double points = std::stod(scoreToken.substr(colonPos + 1));
            scoresMap[rank] = points;
        }
    }
    
    // 检查是否是ID为1的默认规则
    if (id == 1) {
        std::cout << "发现ID为1的默认计分规则" << std::endl;
        return true;
    } else {
        // 使用正确的参数创建规则
        bool success = settings.addScoreRule(description, minParticipants, maxParticipants, ranksToAward, scoresMap);
        if (success) {
            std::cout << "成功添加计分规则" << description << std::endl;
        } else {
            std::cerr << "添加计分规则失败：" << description << std::endl;
        }
        return false;
    }
}

void DataFileManager::processEvent(const std::string& line, SystemSettings& settings, std::map<int, std::vector<int>>& athleteEventMap) {
    // 解析比赛项目数据
    std::stringstream ss(line);
    std::string token;
    
    // ID
    std::getline(ss, token, '|');
    int id = std::stoi(token);
    
    // 名称
    std::getline(ss, token, '|');
    std::string name = token;
    
    // 项目类型
    std::getline(ss, token, '|');
    auto eventType = static_cast<EventType>(std::stoi(token));
    
    // 性别要求
    std::getline(ss, token, '|');
    auto gender = static_cast<Gender>(std::stoi(token));
    
    // 计分规则ID
    std::getline(ss, token, '|');
    int scoreRuleId = std::stoi(token);
    
    // 是否已取消
    std::getline(ss, token, '|');
    bool isCancelled = (token == "1");
    
    // 场馆
    std::getline(ss, token, '|');
    std::string venue = token;
    
    // 持续时间
    std::getline(ss, token, '|');
    int durationMinutes = std::stoi(token);
    
    // 开始时间
    std::getline(ss, token, '|');
    std::string startTime = token;
    
    // 结束时间
    std::getline(ss, token, '|');
    std::string endTime = token;
    
    // 参赛运动员ID
    std::getline(ss, token, '|');
    std::vector<int> athleteIds;
    if (!token.empty()) {
        std::stringstream athletesStream(token);
        std::string athleteToken;
        
        while (std::getline(athletesStream, athleteToken, ',')) {
            athleteIds.push_back(std::stoi(athleteToken));
        }
    }
    
    // 添加或更新比赛项目
    int newId = settings.addCompetitionEvent(name, eventType, gender, scoreRuleId);
    if (newId != id) {
        std::cerr << "警告: 比赛项目ID不匹配。期望: " << id << ", 实际: " << newId << std::endl;
    }
    
    // 获取比赛项目引用进行设置
    auto eventOpt = settings.getCompetitionEvent(newId);
    if (eventOpt) {
        CompetitionEvent& event = eventOpt.value().get();
        if (isCancelled) {
            event.setCancelled(true);
        }
        if (!venue.empty()) {
            // 添加场馆（如果不存在）
            settings.addVenue(venue);
            event.setVenue(venue);
        }
        event.setDurationMinutes(durationMinutes);
        if (!startTime.empty()) {
            event.setStartTime(startTime);
        }
        if (!endTime.empty()) {
            event.setEndTime(endTime);
        }
        
        // 存储运动员与项目的关联关系，等待运动员数据加载完成后再处理
        for (int athleteId : athleteIds) {
            athleteEventMap[athleteId].push_back(newId);
        }
    }
}

void DataFileManager::processAthlete(const std::string& line, SystemSettings& settings) {
    // 解析运动员数据: ID|姓名|性别|单位ID|已报名项目...
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
    auto gender = static_cast<Gender>(std::stoi(token));
    
    // 单位ID
    std::getline(ss, token, '|');
    int unitId = std::stoi(token);
    
    // 添加运动员
    int newId = settings.addAthlete(name, gender, unitId);
    if (newId != id) {
        std::cerr << "警告: 运动员ID不匹配。期望: " << id << ", 实际: " << newId << std::endl;
    }
    
    // 解析已报名项目
    std::getline(ss, token, '|');
    if (!token.empty()) {
        std::stringstream eventsStream(token);
        std::string eventToken;
        
        while (std::getline(eventsStream, eventToken, ',')) {
            int eventId = std::stoi(eventToken);
            // 添加运动员参加项目
            settings.registerAthleteForEvent(newId, eventId);
        }
    }
}

void DataFileManager::processResult(const std::string& line, SystemSettings& settings) {
    // 解析成绩数据: 项目ID|运动员ID|名次|成绩记录|获得分数
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
    
    // 获得分数
    std::getline(ss, token, '|');
    double points = std::stod(token);
    
    // 创建成绩对象
    Result result(eventId, athleteId, rank, scoreRecord, points);
    
    // 获取或创建项目成绩集合
    auto eventResultsOpt = settings.getEventResults(eventId);
    EventResults* eventResults = nullptr;
    
    if (eventResultsOpt) {
        // 成绩集合已存在，直接使用
        eventResults = &(eventResultsOpt.value().get());
    } else {
        // 创建新的成绩集合
        EventResults newResults(eventId);
        settings.addOrUpdateEventResults(newResults);
        eventResultsOpt = settings.getEventResults(eventId);
        if (eventResultsOpt) {
            eventResults = &(eventResultsOpt.value().get());
        }
    }
    
    // 添加成绩记录
    if (eventResults) {
        eventResults->addResult(result);
        
        // 为单位添加分数
        auto athleteOpt = settings.getAthleteConst(athleteId);
        if (athleteOpt) {
            int unitId = athleteOpt.value().get().getUnitId();
            settings.addScoreToUnit(unitId, points);
        }
    }
} 