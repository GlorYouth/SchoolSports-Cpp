//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/DataManager.h"
#include <fstream>
#include <iostream> // 用于输出信息
#include <ranges>
#include <vector>   // 为 std::vector

// 包含业务实体头文件及SystemSettings中方法涉及的所有类
#include "../../include/Components/SystemSettings.h" // 包含引用SystemSettings中所有方法
#include "../../include/Components/Unit.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"      // utils::RefConst<ScoreRule> in getScoreRuleConst
#include "../../include/Components/Constants.h"     // 为了 EventType, Gender 等
#include "../../include/utils.h" // For utils::RefConst
#include "../../include/Components/Result.h" // 为了导入成绩

DataManager::DataManager(SystemSettings& sysSettings) : 
    settings(sysSettings), 
    stage1DataImported(false),
    stage2DataImported(false),
    stage3DataImported(false) {}

bool DataManager::backupData(const std::string& filePath) const {
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行备份: " << filePath << std::endl;
        return false;
    }

    std::cout << "开始备份数据到 " << filePath << " ..." << std::endl;

    // 备份系统配置参数
    outFile << "AthleteMaxEventsAllowed:" << settings.getAthleteMaxEventsAllowed() << std::endl;

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
        // 备份运动员已报名项目ID列表
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

    // 清空当前settings中的数据 (非常重要，否则会有数据混乱)
    // settings = SystemSettings(); // 若类提供一个 clear() 方法

    std::cout << "开始从 " << filePath << " 恢复数据..." << std::endl;
    std::string line;
    // 此处省略了复杂的解析逻辑，实际应该需要解析读取的数据标记节点，
    // 然后用于 settings.addUnit(), settings.addAthlete() 等方法重建数据。
    // 这是一个非常复杂的部分，需要细致处理文件格式和解析错误

    // 示例自动读取 (非常简化且不容错)
    // while (std::getline(inFile, line)) {
    //     if (line.rfind("AthleteMaxEventsAllowed:", 0) == 0) {
    //         settings.setAthleteMaxEventsAllowed(std::stoi(line.substr(line.find(":") + 1)));
    //     }
    //     // ... 更多逻辑
    // }


    inFile.close();
    std::cout << "数据恢复—占位符执行完毕 (未实际恢复)。" << std::endl;
    // 返回false因为没有实际恢复
    std::cerr << "警告: 数据恢复功能尚未完全实现。" << std::endl;
    return false;
}

// 为向后兼容保留原有的loadSampleData方法
bool DataManager::loadSampleData() {
    // 重置导入状态
    resetImportStatus();
    
    // 按顺序导入所有阶段的数据
    bool stage1Success = loadSampleStage1Data();
    if (!stage1Success) {
        return false;
    }
    
    bool stage2Success = loadSampleStage2Data();
    if (!stage2Success) {
        return false;
    }
    
    bool stage3Success = loadSampleStage3Data();
    return stage3Success;
}

// 实现阶段1数据导入：单位、场地、计分规则
bool DataManager::loadSampleStage1Data() {
    std::cout << "开始导入阶段1示例数据（单位、场地）..." << std::endl;

    // 确保默认计分规则（ID为1）存在
    std::optional<utils::RefConst<ScoreRule>> defaultRuleOpt = settings.getScoreRuleConst(1);
    if (!defaultRuleOpt) {
        std::cerr << "错误：未找到ID为1的默认计分规则，请确保已初始化。" << std::endl;
        return false;
    }
    
    // 清除现有单位和场地数据
    settings.clearUnits();
    
    // 清除现有场地数据
    std::set<std::string> oldVenues = settings.getAllVenues();
    for (const auto& v : oldVenues) {
        settings.removeVenue(v);
    }
    
    // 添加更多示例单位（8个学院）
    settings.addUnit("计算机学院");     // 预期ID 1
    settings.addUnit("外语学院");       // 预期ID 2
    settings.addUnit("体育学院");       // 预期ID 3
    settings.addUnit("数学学院");       // 预期ID 4
    settings.addUnit("物理学院");       // 预期ID 5
    settings.addUnit("化学学院");       // 预期ID 6
    settings.addUnit("生物学院");       // 预期ID 7
    settings.addUnit("艺术学院");       // 预期ID 8
    
    // 添加更多示例场地
    settings.addVenue("田径场A");
    settings.addVenue("田径场B");
    settings.addVenue("跳远区A");
    settings.addVenue("跳远区B");
    settings.addVenue("投掷区A");
    settings.addVenue("投掷区B");
    
    // 添加示例比赛项目（带有时间和场地）
    // 男子项目
    settings.addCompetitionEvent("男子100米", EventType::TRACK, Gender::MALE, 1);     // 预期ID 1
    settings.addCompetitionEvent("男子200米", EventType::TRACK, Gender::MALE, 1);     // 预期ID 2
    settings.addCompetitionEvent("男子400米", EventType::TRACK, Gender::MALE, 1);     // 预期ID 3
    settings.addCompetitionEvent("男子跳远", EventType::FIELD, Gender::MALE, 1);      // 预期ID 4
    settings.addCompetitionEvent("男子三级跳远", EventType::FIELD, Gender::MALE, 1);  // 预期ID 5
    settings.addCompetitionEvent("男子铅球", EventType::FIELD, Gender::MALE, 1);      // 预期ID 6
    
    // 女子项目
    settings.addCompetitionEvent("女子100米", EventType::TRACK, Gender::FEMALE, 1);   // 预期ID 7
    settings.addCompetitionEvent("女子200米", EventType::TRACK, Gender::FEMALE, 1);   // 预期ID 8
    settings.addCompetitionEvent("女子跳远", EventType::FIELD, Gender::FEMALE, 1);    // 预期ID 9
    settings.addCompetitionEvent("女子铅球", EventType::FIELD, Gender::FEMALE, 1);    // 预期ID 10
    
    // 混合项目
    settings.addCompetitionEvent("混合接力4x100米", EventType::TRACK, Gender::UNSPECIFIED, 1); // 预期ID 11
    
    // 设置项目持续时间和场地
    auto setEventDetails = [&](int eventId, int durationMinutes, const std::string& venue) {
        auto eventOpt = settings.getCompetitionEvent(eventId);
        if (eventOpt.has_value()) {
            eventOpt.value().get().setDurationMinutes(durationMinutes);
            eventOpt.value().get().setVenue(venue);
            return true;
        }
        return false;
    };
    
    // 设置各项目详细信息
    setEventDetails(1, 10, "田径场A");  // 男子100米
    setEventDetails(2, 15, "田径场B");  // 男子200米
    setEventDetails(3, 20, "田径场A");  // 男子400米
    setEventDetails(4, 30, "跳远区A");  // 男子跳远
    setEventDetails(5, 45, "跳远区B");  // 男子三级跳远
    setEventDetails(6, 40, "投掷区A");  // 男子铅球
    setEventDetails(7, 10, "田径场B");  // 女子100米
    setEventDetails(8, 15, "田径场A");  // 女子200米
    setEventDetails(9, 30, "跳远区B");  // 女子跳远
    setEventDetails(10, 40, "投掷区B"); // 女子铅球
    setEventDetails(11, 30, "田径场A"); // 混合接力
    
    std::cout << "阶段1示例数据导入成功！" << std::endl;
    std::cout << "当前单位数量: " << settings.getAllUnits().size() << std::endl;
    std::cout << "当前比赛项目数量: " << settings.getAllCompetitionEventsConst().size() << std::endl;
    std::cout << "当前场地数量: " << settings.getAllVenues().size() << std::endl;
    
    // 设置阶段1数据已导入
    stage1DataImported = true;
    return true;
}

// 实现阶段2数据导入：运动员及其报名
bool DataManager::loadSampleStage2Data() {
    // 检查阶段1数据是否已导入
    if (!stage1DataImported) {
        std::cerr << "错误：必须先导入阶段1数据（单位和场地）。" << std::endl;
        return false;
    }
    
    std::cout << "开始导入阶段2示例数据（运动员及报名信息）..." << std::endl;
    
    // 清除现有运动员数据
    settings.clearAthletes();
    
    // 创建每个学院的男女运动员
    // 计算机学院
    settings.addAthlete("王大明", Gender::MALE, 1);    // 1
    settings.addAthlete("李小红", Gender::FEMALE, 1);  // 2
    settings.addAthlete("张三", Gender::MALE, 1);      // 3
    settings.addAthlete("赵四", Gender::FEMALE, 1);    // 4
    
    // 外语学院
    settings.addAthlete("刘强", Gender::MALE, 2);      // 5
    settings.addAthlete("陈静", Gender::FEMALE, 2);    // 6
    settings.addAthlete("朱明", Gender::MALE, 2);      // 7
    settings.addAthlete("孙丽", Gender::FEMALE, 2);    // 8
    
    // 体育学院
    settings.addAthlete("周杰", Gender::MALE, 3);      // 9
    settings.addAthlete("吴芳", Gender::FEMALE, 3);    // 10
    settings.addAthlete("郑强", Gender::MALE, 3);      // 11
    settings.addAthlete("宋琳", Gender::FEMALE, 3);    // 12
    
    // 数学学院
    settings.addAthlete("钱伟", Gender::MALE, 4);      // 13
    settings.addAthlete("冯娟", Gender::FEMALE, 4);    // 14
    
    // 物理学院
    settings.addAthlete("蒋刚", Gender::MALE, 5);      // 15
    settings.addAthlete("何玲", Gender::FEMALE, 5);    // 16
    
    // 化学学院
    settings.addAthlete("吕鹏", Gender::MALE, 6);      // 17
    settings.addAthlete("徐婷", Gender::FEMALE, 6);    // 18
    
    // 生物学院
    settings.addAthlete("高飞", Gender::MALE, 7);      // 19
    settings.addAthlete("马兰", Gender::FEMALE, 7);    // 20
    
    // 艺术学院
    settings.addAthlete("田野", Gender::MALE, 8);      // 21
    settings.addAthlete("董红", Gender::FEMALE, 8);    // 22
    
    // 为运动员报名项目
    // 男子100米 - ID 1
    settings.registerAthleteForEvent(1, 1);  // 王大明
    settings.registerAthleteForEvent(5, 1);  // 刘强
    settings.registerAthleteForEvent(9, 1);  // 周杰
    settings.registerAthleteForEvent(13, 1); // 钱伟
    settings.registerAthleteForEvent(15, 1); // 蒋刚
    settings.registerAthleteForEvent(17, 1); // 吕鹏
    settings.registerAthleteForEvent(19, 1); // 高飞
    
    // 男子200米 - ID 2
    settings.registerAthleteForEvent(3, 2);  // 张三
    settings.registerAthleteForEvent(7, 2);  // 朱明
    settings.registerAthleteForEvent(11, 2); // 郑强
    settings.registerAthleteForEvent(15, 2); // 蒋刚
    settings.registerAthleteForEvent(21, 2); // 田野
    
    // 男子400米 - ID 3
    settings.registerAthleteForEvent(1, 3);  // 王大明
    settings.registerAthleteForEvent(9, 3);  // 周杰
    settings.registerAthleteForEvent(17, 3); // 吕鹏
    settings.registerAthleteForEvent(19, 3); // 高飞
    
    // 男子跳远 - ID 4
    settings.registerAthleteForEvent(3, 4);  // 张三
    settings.registerAthleteForEvent(7, 4);  // 朱明
    settings.registerAthleteForEvent(11, 4); // 郑强
    settings.registerAthleteForEvent(13, 4); // 钱伟
    settings.registerAthleteForEvent(21, 4); // 田野
    
    // 男子三级跳远 - ID 5
    settings.registerAthleteForEvent(5, 5);  // 刘强
    settings.registerAthleteForEvent(9, 5);  // 周杰
    settings.registerAthleteForEvent(19, 5); // 高飞
    // 只报名3人，将会被取消
    
    // 男子铅球 - ID 6
    settings.registerAthleteForEvent(1, 6);  // 王大明
    settings.registerAthleteForEvent(11, 6); // 郑强
    settings.registerAthleteForEvent(15, 6); // 蒋刚
    settings.registerAthleteForEvent(17, 6); // 吕鹏
    
    // 女子100米 - ID 7
    settings.registerAthleteForEvent(2, 7);  // 李小红
    settings.registerAthleteForEvent(6, 7);  // 陈静
    settings.registerAthleteForEvent(10, 7); // 吴芳
    settings.registerAthleteForEvent(14, 7); // 冯娟
    settings.registerAthleteForEvent(16, 7); // 何玲
    settings.registerAthleteForEvent(18, 7); // 徐婷
    
    // 女子200米 - ID 8
    settings.registerAthleteForEvent(4, 8);  // 赵四
    settings.registerAthleteForEvent(8, 8);  // 孙丽
    settings.registerAthleteForEvent(12, 8); // 宋琳
    settings.registerAthleteForEvent(20, 8); // 马兰
    
    // 女子跳远 - ID 9
    settings.registerAthleteForEvent(2, 9);  // 李小红
    settings.registerAthleteForEvent(6, 9);  // 陈静
    settings.registerAthleteForEvent(12, 9); // 宋琳
    settings.registerAthleteForEvent(16, 9); // 何玲
    settings.registerAthleteForEvent(22, 9); // 董红
    
    // 女子铅球 - ID 10
    settings.registerAthleteForEvent(4, 10);  // 赵四
    settings.registerAthleteForEvent(8, 10);  // 孙丽
    settings.registerAthleteForEvent(10, 10); // 吴芳
    settings.registerAthleteForEvent(18, 10); // 徐婷
    settings.registerAthleteForEvent(20, 10); // 马兰
    settings.registerAthleteForEvent(22, 10); // 董红
    
    // 混合接力4x100米 - ID 11
    // 每个学院报一对男女，共8队
    settings.registerAthleteForEvent(1, 11);  // 王大明
    settings.registerAthleteForEvent(2, 11);  // 李小红
    settings.registerAthleteForEvent(5, 11);  // 刘强
    settings.registerAthleteForEvent(6, 11);  // 陈静
    settings.registerAthleteForEvent(9, 11);  // 周杰
    settings.registerAthleteForEvent(10, 11); // 吴芳
    settings.registerAthleteForEvent(13, 11); // 钱伟
    settings.registerAthleteForEvent(14, 11); // 冯娟
    settings.registerAthleteForEvent(15, 11); // 蒋刚
    settings.registerAthleteForEvent(16, 11); // 何玲
    settings.registerAthleteForEvent(17, 11); // 吕鹏
    settings.registerAthleteForEvent(18, 11); // 徐婷
    settings.registerAthleteForEvent(19, 11); // 高飞
    settings.registerAthleteForEvent(20, 11); // 马兰
    settings.registerAthleteForEvent(21, 11); // 田野
    settings.registerAthleteForEvent(22, 11); // 董红
    
    std::cout << "阶段2示例数据导入成功！" << std::endl;
    std::cout << "当前运动员数量: " << settings.getAllAthletes().size() << std::endl;
    
    // 设置阶段2数据已导入
    stage2DataImported = true;
    return true;
}

// 实现阶段3数据导入：比赛成绩
bool DataManager::loadSampleStage3Data() {
    // 检查阶段2数据是否已导入
    if (!stage2DataImported) {
        std::cerr << "错误：必须先导入阶段2数据（运动员）。" << std::endl;
        return false;
    }
    
    std::cout << "开始导入阶段3示例数据（比赛成绩）..." << std::endl;
    
    // 检查并取消人数不足的项目
    // 对于运行阶段2导入而没有生成秩序册的情况
    // 这里手动检查参赛人数是否符合要求
    
    // 获取默认计分规则
    auto mainRuleOpt = settings.getScoreRule(1);
    if (!mainRuleOpt.has_value()) {
        std::cerr << "错误：找不到ID为1的默认计分规则！" << std::endl;
        return false;
    }
    
    // 检查每个项目的参赛人数，并按需取消
    for (const auto& pair : settings.getAllCompetitionEventsConst()) {
        int eventId = pair.first;
        const CompetitionEvent& event = pair.second;
        int participantCount = event.getParticipantCount();
        
        // 获取适用于该参赛人数的规则
        ScoreRule& mainRule = mainRuleOpt.value().get();
        const ScoreRule* applicableRule = mainRule.getApplicableRule(participantCount);
        
        // 如果没有适用规则，说明人数不足
        if (!applicableRule->appliesTo(participantCount)) {
            std::cout << "项目\"" << event.getName() << "\"（ID: " << eventId 
                      << "）参赛人数为" << participantCount 
                      << "，不满足最低参赛人数要求，将被取消。" << std::endl;
            
            // 获取项目的可修改引用来设置取消状态
            auto eventOpt = settings.getCompetitionEvent(eventId);
            if (eventOpt.has_value()) {
                eventOpt.value().get().setCancelled(true);
            }
            continue; // 已取消的项目不需要录入成绩
        }
        
        // 现在导入成绩，只为未取消的项目导入
        if (!event.getIsCancelled()) {
            // 创建成绩列表
            EventResults results(eventId);
            
            // 获取该项目的所有参赛者
            std::vector<int> participantIds(event.getParticipantAthleteIds().begin(), 
                                          event.getParticipantAthleteIds().end());
            
            // 按照项目类型，为参赛者分配不同的成绩
            for (size_t i = 0; i < participantIds.size(); i++) {
                int athleteId = participantIds[i];
                std::string scoreRecord;
                int rank = static_cast<int>(i) + 1; // 名次从1开始
                
                // 根据项目类型设置不同格式的成绩
                if (event.getEventType() == EventType::TRACK) {
                    // 田径类项目，成绩格式为时间 mm:ss.xx
                    int baseMinutes = event.getName().find("100米") != std::string::npos ? 0 : 
                                    (event.getName().find("200米") != std::string::npos ? 0 : 
                                    (event.getName().find("400米") != std::string::npos ? 1 : 2));
                    int seconds = 10 + rank * 2; // 每名次差2秒
                    int milliseconds = rand() % 100; // 随机毫秒
                    
                    // 格式化时间
                    char buffer[10];
                    sprintf(buffer, "%02d:%02d.%02d", baseMinutes, seconds, milliseconds);
                    scoreRecord = buffer;
                } else {
                    // 田赛类项目，成绩格式为距离 xx.xx米
                    float baseDistance = event.getName().find("铅球") != std::string::npos ? 15.0f : 7.0f;
                    float distance = baseDistance - (rank - 1) * 0.3f; // 每名次差0.3米
                    distance += (rand() % 100) / 100.0f; // 添加随机小数
                    
                    // 格式化距离
                    char buffer[10];
                    sprintf(buffer, "%.2f米", distance);
                    scoreRecord = buffer;
                }
                
                // 获取该名次对应的积分
                double points = applicableRule->getScoreForRank(rank);
                
                // 添加成绩记录
                Result result(eventId, athleteId, rank, scoreRecord, points);
                results.addResult(result);
                
                // 查询运动员所属单位，为单位添加积分
                auto athleteOpt = settings.getAthleteConst(athleteId);
                if (athleteOpt.has_value()) {
                    int unitId = athleteOpt.value().get().getUnitId();
                    settings.addScoreToUnit(unitId, points);
                }
            }
            
            // 将成绩记录添加到系统中
            settings.addOrUpdateEventResults(results);
        }
    }
    
    std::cout << "阶段3示例数据导入成功！" << std::endl;
    std::cout << "已导入所有项目成绩并计算单位总分。" << std::endl;
    
    // 设置阶段3数据已导入
    stage3DataImported = true;
    return true;
}

// 重置导入状态
void DataManager::resetImportStatus() {
    stage1DataImported = false;
    stage2DataImported = false;
    stage3DataImported = false;
}
