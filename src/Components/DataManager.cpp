//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/DataManager.h"
#include <fstream>
#include <iostream> // 用于输出信息
#include <ranges>

// 注意：这是一个非常基础的实现，没有错误处理、版本控制或复杂数据结构的序列化。
// 实际应用中需要更健壮的序列化/反序列化机制 (例如使用JSON, XML, 或自定义二进制格式)。

DataManager::DataManager(SystemSettings& sysSettings) : settings(sysSettings) {}

bool DataManager::backupData(const std::string& filePath) const {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行备份: " << filePath << std::endl;
        return false;
    }

    std::cout << "开始备份数据到 " << filePath << " ..." << std::endl;

    // 备份系统设置参数
    outFile << "AthleteMaxEventsAllowed:" << settings.getAthleteMaxEventsAllowed() << std::endl;
    outFile << "MinParticipantsToHoldEvent:" << settings.getMinParticipantsToHoldEvent() << std::endl;

    // 备份单位
    outFile << "UnitsCount:" << settings.getAllUnits().size() << std::endl;
    for (const auto &val: settings.getAllUnits() | std::views::values) {
        const Unit& unit = val;
        outFile << "Unit:" << unit.getId() << "," << unit.getName() << std::endl;
        // 实际应用中，单位下的运动员ID列表等也需要备份，但这里简化
    }

    // 备份运动员
    outFile << "AthletesCount:" << settings.getAllAthletes().size() << std::endl;
    for (const auto &val: settings.getAllAthletes() | std::views::values) {
        const Athlete& athlete = val;
        outFile << "Athlete:" << athlete.getId() << "," << athlete.getName() << ","
                << static_cast<int>(athlete.getGender()) << "," << athlete.getUnitId() << std::endl;
        // 备份运动员报名的项目ID列表
        outFile << "RegisteredEventsCount:" << athlete.getRegisteredEventIds().size() << std::endl;
        for (const int eventId : athlete.getRegisteredEventIds()) {
            outFile << eventId << " ";
        }
        outFile << std::endl;
    }

    // 备份比赛项目
    outFile << "CompetitionEventsCount:" << settings.getAllCompetitionEvents().size() << std::endl;
    for (const auto &val: settings.getAllCompetitionEvents() | std::views::values) {
        const CompetitionEvent& event = val;
        outFile << "Event:" << event.getId() << "," << event.getName() << ","
                << static_cast<int>(event.getEventType()) << ","
                << static_cast<int>(event.getGenderRequirement()) << ","
                << event.getScoreRuleId() << "," << event.getIsCancelled() << std::endl;
        // 备份项目中的参赛者ID列表
        outFile << "ParticipantAthletesCount:" << event.getParticipantAthleteIds().size() << std::endl;
        for (int athleteId : event.getParticipantAthleteIds()) {
            outFile << athleteId << " ";
        }
        outFile << std::endl;
    }

    // 备份计分规则
    outFile << "ScoreRulesCount:" << settings.getAllScoreRules().size() << std::endl;
    for (const auto &val: settings.getAllScoreRules() | std::views::values) {
        const ScoreRule& rule = val;
        outFile << "ScoreRule:" << rule.getId() << "," << rule.getDescription() << ","
                << rule.appliesTo(0) /* 这是一个简化的示例，实际应存min/max participants */
                << "," << rule.getRanksToAward() << std::endl;
        // 备份名次和分数
        outFile << "ScoresForRanksCount:" << rule.getAllScoresForRanks().size() << std::endl;
        for(const auto& scorePair : rule.getAllScoresForRanks()){
            outFile << scorePair.first << ":" << scorePair.second << " ";
        }
        outFile << std::endl;
    }


    outFile.close();
    std::cout << "数据备份完成。" << std::endl;
    return true;
}

bool DataManager::restoreData(const std::string& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行恢复: " << filePath << std::endl;
        return false;
    }

    // 清空当前settings中的数据 (非常重要，否则会混合数据)
    // settings = SystemSettings(); // 重新构造或者提供一个 clear() 方法

    std::cout << "开始从 " << filePath << " 恢复数据..." << std::endl;
    std::string line;
    // 此处省略了复杂的解析逻辑，实际应用中需要逐行读取并根据标记解析数据，
    // 然后调用 settings.addUnit(), settings.addAthlete() 等方法重建数据。
    // 这是一个非常复杂的部分，需要仔细设计文件格式和解析器。

    // 示例性读取 (非常不完整和不健壮)
    // while (std::getline(inFile, line)) {
    //     if (line.rfind("AthleteMaxEventsAllowed:", 0) == 0) {
    //         settings.setAthleteMaxEventsAllowed(std::stoi(line.substr(line.find(":") + 1)));
    //     }
    //     // ... 更多解析逻辑
    // }


    inFile.close();
    std::cout << "数据恢复过程占位符执行完毕 (未实际恢复)。" << std::endl;
    // 返回false因为没有实际恢复
    std::cerr << "警告: 数据恢复功能尚未完全实现。" << std::endl;
    return false;
}
