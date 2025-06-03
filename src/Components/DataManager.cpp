//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/DataManager.h"
#include <fstream>
#include <iostream> // 用于输出信息
#include <ranges>
#include <vector>   // 为 std::vector

// 包含业务实体头文件以及SystemSettings中方法可能涉及的类型
#include "../../include/Components/SystemSettings.h" // 包含完整的SystemSettings定义以访问其所有方法
#include "../../include/Components/Unit.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"      // utils::RefConst<ScoreRule> in getScoreRuleConst
#include "../../include/Components/Constants.h"     // 为了 EventType, Gender 等
#include "../../include/utils.h" // For utils::RefConst

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
    for (const auto &pair : settings.getAllUnits()) {
        const Unit& unit = pair.second;
        outFile << "Unit:" << unit.getId() << "," << unit.getName() << std::endl;
    }

    // 备份运动员
    outFile << "AthletesCount:" << settings.getAllAthletes().size() << std::endl;
    for (const auto &pair : settings.getAllAthletes()) {
        const Athlete& athlete = pair.second;
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
    outFile << "CompetitionEventsCount:" << settings.getAllCompetitionEventsConst().size() << std::endl;
    for (const auto &pair : settings.getAllCompetitionEventsConst()) {
        const CompetitionEvent& event = pair.second;
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
    for (const auto &pair : settings.getAllScoreRules()) {
        const ScoreRule& rule = pair.second;
        outFile << "ScoreRule:" << rule.getId() << "," << rule.getDescription() << ","
                << rule.getMinParticipants() << "," << rule.getMaxParticipants() << "," // 使用实际的 min/max
                << rule.getRanksToAward() << std::endl;
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

// 实现导入示例数据的方法
bool DataManager::loadSampleData() {
    std::cout << "开始导入示例数据..." << std::endl;

    // 检查默认计分规则是否存在 (ID为1)
    std::optional<utils::RefConst<ScoreRule>> defaultRuleOpt = settings.getScoreRuleConst(1);
    if (!defaultRuleOpt) {
        std::cerr << "错误：未找到ID为1的默认计分规则。请确保已初始化。" << std::endl;
        return false;
    }
    int defaultScoreRuleId = 1;

    // 清理现有数据 (计分规则除外)
    settings.clearCompetitionEvents();
    settings.clearAthletes();
    settings.clearUnits();
    // 新增：清理场地表
    std::set<std::string> oldVenues = settings.getAllVenues();
    for (const auto& v : oldVenues) settings.removeVenue(v);
    std::cout << "已清除旧的单位、运动员、比赛项目和场地数据（计分规则已保留）。" << std::endl;

    // 添加示例场地
    settings.addVenue("田径场A");
    settings.addVenue("体育馆B");
    settings.addVenue("投掷场C");

    // 添加示例单位
    settings.addUnit("计算机学院");     // 预期ID 1
    settings.addUnit("外国语学院");   // 预期ID 2
    settings.addUnit("体育学院");       // 预期ID 3
    int unitIdCompSci = 1; 
    int unitIdForeignLang = 2;
    int unitIdSports = 3;

    // 添加示例比赛项目（带持续时间和场地）
    settings.addCompetitionEvent("男子100米", EventType::TRACK, Gender::MALE, defaultScoreRuleId);    // 1
    settings.addCompetitionEvent("女子跳远", EventType::FIELD, Gender::FEMALE, defaultScoreRuleId);  // 2
    settings.addCompetitionEvent("男子铅球", EventType::FIELD, Gender::MALE, defaultScoreRuleId);    // 3
    settings.addCompetitionEvent("女子200米", EventType::TRACK, Gender::FEMALE, defaultScoreRuleId); // 4
    int eventIdM100m = 1;
    int eventIdWLongJump = 2;
    int eventIdMShotPut = 3;
    int eventIdW200m = 4;
    // 设置项目持续时间和场地
    if (auto e = settings.getCompetitionEvent(eventIdM100m)) {
        e.value().get().setDurationMinutes(10);
        e.value().get().setVenue("田径场A");
    }
    if (auto e = settings.getCompetitionEvent(eventIdWLongJump)) {
        e.value().get().setDurationMinutes(20);
        e.value().get().setVenue("体育馆B");
    }
    if (auto e = settings.getCompetitionEvent(eventIdMShotPut)) {
        e.value().get().setDurationMinutes(15);
        e.value().get().setVenue("投掷场C");
    }
    if (auto e = settings.getCompetitionEvent(eventIdW200m)) {
        e.value().get().setDurationMinutes(12);
        e.value().get().setVenue("田径场A");
    }

    // 添加示例运动员
    settings.addAthlete("张三", Gender::MALE, unitIdCompSci);       // 1
    settings.addAthlete("李四", Gender::FEMALE, unitIdForeignLang);  // 2
    settings.addAthlete("王五", Gender::MALE, unitIdSports);          // 3
    settings.addAthlete("赵六", Gender::FEMALE, unitIdCompSci);     // 4
    settings.addAthlete("孙七", Gender::MALE, unitIdForeignLang);    // 5
    settings.addAthlete("周八", Gender::FEMALE, unitIdSports);        // 6
    int athleteIdZhang = 1;
    int athleteIdLi = 2;
    int athleteIdWang = 3;
    int athleteIdZhao = 4;
    int athleteIdSun = 5;
    int athleteIdZhou = 6;

    // 为运动员报名项目
    settings.registerAthleteForEvent(athleteIdZhang, eventIdM100m);
    settings.registerAthleteForEvent(athleteIdLi, eventIdWLongJump);
    settings.registerAthleteForEvent(athleteIdWang, eventIdM100m);
    settings.registerAthleteForEvent(athleteIdWang, eventIdMShotPut);
    settings.registerAthleteForEvent(athleteIdZhao, eventIdW200m);
    settings.registerAthleteForEvent(athleteIdZhou, eventIdWLongJump);
    settings.registerAthleteForEvent(athleteIdZhou, eventIdW200m);

    std::cout << "示例数据导入成功。" << std::endl;
    std::cout << "当前单位数量: " << settings.getAllUnits().size() << std::endl;
    std::cout << "当前比赛项目数量: " << settings.getAllCompetitionEventsConst().size() << std::endl;
    std::cout << "当前运动员数量: " << settings.getAllAthletes().size() << std::endl;
    std::cout << "当前场地数量: " << settings.getAllVenues().size() << std::endl;

    return true;
}
