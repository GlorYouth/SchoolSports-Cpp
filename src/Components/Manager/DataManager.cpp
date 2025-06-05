
#include "../../../include/Components/Manager/DataManager.h"
#include <iostream> // 用于输出信息
#include <ranges>
#include <vector>   // 为 std::vector
#include <filesystem>

// 包含业务实现头文件（SystemSettings中返回涉及的所有类）
#include <sstream>

#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/Components/Core/Unit.h"
#include "../../../include/Components/Core/Athlete.h"
#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/Components/Core/ScoreRule.h"
#include "../../../include/Components/Core/Constants.h"
#include "../../../include/utils.h"
#include "../../../include/Components/Core/Result.h"

DataManager::DataManager(SystemSettings& sysSettings) : 
    settings(sysSettings),
    stage1DataImported(false),
    stage2DataImported(false),
    stage3DataImported(false) {}

bool DataManager::backupData(const std::string& filePath) const {
    try {
        return fileManager.saveDataToFile(settings, filePath);
    } catch (const std::exception& e) {
        std::cerr << "备份数据时发生异常: " << e.what() << std::endl;
        return false;
    }
}

bool DataManager::restoreData(const std::string& filePath) {
    try {
        return fileManager.loadDataFromFile(settings, filePath);
    } catch (const std::exception& e) {
        std::cerr << "恢复数据时发生异常: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::pair<int, std::string>> DataManager::getBackupFiles() const {
    return fileManager.listBackupFiles();
}

bool DataManager::ensureBackupDirectoryExists() const {
    return fileManager.ensureBackupDirectoryExists();
}

// 为兼容数据备份原有的loadSampleData方法
bool DataManager::loadSampleData() {
    // 重置导入状态
    resetImportStatus();
    
    // 按顺序导入各阶段的数据
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
    std::cout << "开始加载阶段1示例数据（单位、场地）..." << std::endl;

    // 确保默认计分规则ID为1存在
    std::optional<utils::RefConst<ScoreRule>> defaultRuleOpt = settings.rules.getConst(1);
    if (!defaultRuleOpt) {
        std::cerr << "错误未找到ID为1的默认计分规则，确保已初始化。" << std::endl;
        return false;
    }

    // 清除所有单位和场馆数据
    settings.units.clear();
    settings.athletes.resetIdCounter();
    
    // 清除所有场馆数据
    std::set<std::string> oldVenues = settings.venues.getAll();
    for (const auto& v : oldVenues) {
        settings.venues.remove(v);
    }
    
    // 添加各个示例单位（8个学院）
    settings.units.add("计算机学院");     // 预计ID 1
    settings.units.add("法律学院");       // 预计ID 2
    settings.units.add("医学学院");       // 预计ID 3
    settings.units.add("文学学院");       // 预计ID 4
    settings.units.add("经济学院");       // 预计ID 5
    settings.units.add("物学学院");       // 预计ID 6
    settings.units.add("外语学院");       // 预计ID 7
    settings.units.add("艺术学院");       // 预计ID 8
    
    // 添加各个示例场馆
    settings.venues.add("田径场A");
    settings.venues.add("田径场B");
    settings.venues.add("跳远区A");
    settings.venues.add("跳远区B");
    settings.venues.add("投掷区A");
    settings.venues.add("投掷区B");
    
    // 添加示例比赛项目（不带时间和场地）
    // 男子项目
    settings.events.add("男子100米", EventType::TRACK, Gender::MALE, 1);     // 预计ID 1
    settings.events.add("男子200米", EventType::TRACK, Gender::MALE, 1);     // 预计ID 2
    settings.events.add("男子400米", EventType::TRACK, Gender::MALE, 1);     // 预计ID 3
    settings.events.add("男子跳远", EventType::FIELD, Gender::MALE, 1);      // 预计ID 4
    settings.events.add("男子三级跳远", EventType::FIELD, Gender::MALE, 1);  // 预计ID 5
    settings.events.add("男子铅球", EventType::FIELD, Gender::MALE, 1);      // 预计ID 6
    
    // 女子项目
    settings.events.add("女子100米", EventType::TRACK, Gender::FEMALE, 1);   // 预计ID 7
    settings.events.add("女子200米", EventType::TRACK, Gender::FEMALE, 1);   // 预计ID 8
    settings.events.add("女子跳远", EventType::FIELD, Gender::FEMALE, 1);    // 预计ID 9
    settings.events.add("女子铅球", EventType::FIELD, Gender::FEMALE, 1);    // 预计ID 10
    
    // 混合项目
    settings.events.add("混合接力4x100米", EventType::TRACK, Gender::UNSPECIFIED, 1); // 预计ID 11
    
    // 设置项目的时间和场地
    auto setEventDetails = [&](int eventId, int durationMinutes, const std::string& venue) {
        auto eventOpt = settings.events.get(eventId);
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
    std::cout << "当前单位数量: " << settings.units.getAll().size() << std::endl;
    std::cout << "当前比赛项目数量: " << settings.events.getAllConst().size() << std::endl;
    std::cout << "当前场馆数: " << settings.venues.getAll().size() << std::endl;

    // 设置阶段1数据已导入
    stage1DataImported = true;
    return true;
}

// 实现阶段2数据导入：运动员及报名
bool DataManager::loadSampleStage2Data() {
    // 检查阶段1数据是否已导入
    if (!stage1DataImported) {
        std::cerr << "错误：必须先导入阶段1数据（单位和场地）！" << std::endl;
        return false;
    }
    
    std::cout << "开始加载阶段2示例数据（运动员及报名信息）..." << std::endl;
    
    // 清除所有运动员数据
    settings.athletes.clear();
    
    // 添加每个学院的男女运动员
    // 计算机学院
    settings.athletes.add("王大明", Gender::MALE, 1);    // 1
    settings.athletes.add("李小红", Gender::FEMALE, 1);  // 2
    settings.athletes.add("赵强", Gender::MALE, 1);      // 3
    settings.athletes.add("钱芳", Gender::FEMALE, 1);    // 4
    
    // 法律学院
    settings.athletes.add("孙强", Gender::MALE, 2);      // 5
    settings.athletes.add("周静", Gender::FEMALE, 2);    // 6
    settings.athletes.add("吴刚", Gender::MALE, 2);      // 7
    settings.athletes.add("郑敏", Gender::FEMALE, 2);    // 8
    
    // 医学学院
    settings.athletes.add("王军", Gender::MALE, 3);      // 9
    settings.athletes.add("陈芬", Gender::FEMALE, 3);    // 10
    settings.athletes.add("郑强", Gender::MALE, 3);      // 11
    settings.athletes.add("林丽", Gender::FEMALE, 3);    // 12
    
    // 文学学院
    settings.athletes.add("钱伟", Gender::MALE, 4);      // 13
    settings.athletes.add("孙芳", Gender::FEMALE, 4);    // 14
    
    // 经济学院
    settings.athletes.add("李强", Gender::MALE, 5);      // 15
    settings.athletes.add("张明", Gender::FEMALE, 5);    // 16
    
    // 物学学院
    settings.athletes.add("王刚", Gender::MALE, 6);      // 17
    settings.athletes.add("刘红", Gender::FEMALE, 6);    // 18
    
    // 外语学院
    settings.athletes.add("高峰", Gender::MALE, 7);      // 19
    settings.athletes.add("杨丽", Gender::FEMALE, 7);    // 20
    
    // 艺术学院
    settings.athletes.add("张野", Gender::MALE, 8);      // 21
    settings.athletes.add("黄玲", Gender::FEMALE, 8);    // 22
    
    // 为运动员报名项目
    // 男子100米 - ID 1
    settings.athletes.registerForEvent(1, 1);  // 王大明
    settings.athletes.registerForEvent(5, 1);  // 孙强
    settings.athletes.registerForEvent(9, 1);  // 王军
    settings.athletes.registerForEvent(13, 1); // 钱伟
    settings.athletes.registerForEvent(15, 1); // 李强
    settings.athletes.registerForEvent(17, 1); // 王刚
    settings.athletes.registerForEvent(19, 1); // 高峰
    
    // 男子200米 - ID 2
    settings.athletes.registerForEvent(3, 2);  // 赵强
    settings.athletes.registerForEvent(7, 2);  // 吴刚
    settings.athletes.registerForEvent(11, 2); // 郑强
    settings.athletes.registerForEvent(15, 2); // 李强
    settings.athletes.registerForEvent(21, 2); // 张野
    
    // 男子400米 - ID 3
    settings.athletes.registerForEvent(1, 3);  // 王大明
    settings.athletes.registerForEvent(9, 3);  // 王军
    settings.athletes.registerForEvent(17, 3); // 王刚
    settings.athletes.registerForEvent(19, 3); // 高峰
    
    // 男子跳远 - ID 4
    settings.athletes.registerForEvent(3, 4);  // 赵强
    settings.athletes.registerForEvent(7, 4);  // 吴刚
    settings.athletes.registerForEvent(11, 4); // 郑强
    settings.athletes.registerForEvent(13, 4); // 钱伟
    settings.athletes.registerForEvent(21, 4); // 张野
    
    // 男子三级跳远 - ID 5
    settings.athletes.registerForEvent(5, 5);  // 孙强
    settings.athletes.registerForEvent(9, 5);  // 王军
    settings.athletes.registerForEvent(19, 5); // 高峰
    // 只有三人，会被取消
    
    // 男子铅球 - ID 6
    settings.athletes.registerForEvent(1, 6);  // 王大明
    settings.athletes.registerForEvent(11, 6); // 郑强
    settings.athletes.registerForEvent(15, 6); // 李强
    settings.athletes.registerForEvent(17, 6); // 王刚
    
    // 女子100米 - ID 7
    settings.athletes.registerForEvent(2, 7);  // 李小红
    settings.athletes.registerForEvent(6, 7);  // 周静
    settings.athletes.registerForEvent(10, 7); // 陈芬
    settings.athletes.registerForEvent(14, 7); // 孙芳
    settings.athletes.registerForEvent(16, 7); // 张明
    settings.athletes.registerForEvent(18, 7); // 刘红
    
    // 女子200米 - ID 8
    settings.athletes.registerForEvent(4, 8);  // 钱芳
    settings.athletes.registerForEvent(8, 8);  // 郑敏
    settings.athletes.registerForEvent(12, 8); // 林丽
    settings.athletes.registerForEvent(20, 8); // 杨丽
    
    // 女子跳远 - ID 9
    settings.athletes.registerForEvent(2, 9);  // 李小红
    settings.athletes.registerForEvent(6, 9);  // 周静
    settings.athletes.registerForEvent(12, 9); // 林丽
    settings.athletes.registerForEvent(16, 9); // 张明
    settings.athletes.registerForEvent(22, 9); // 黄玲
    
    // 女子铅球 - ID 10
    settings.athletes.registerForEvent(4, 10);  // 钱芳
    settings.athletes.registerForEvent(8, 10);  // 郑敏
    settings.athletes.registerForEvent(10, 10); // 陈芬
    settings.athletes.registerForEvent(18, 10); // 刘红
    settings.athletes.registerForEvent(20, 10); // 杨丽
    settings.athletes.registerForEvent(22, 10); // 黄玲
    
    // 混合接力4x100米 - ID 11
    // 每个学院各一男一女共8队
    settings.athletes.registerForEvent(1, 11);  // 王大明
    settings.athletes.registerForEvent(2, 11);  // 李小红
    settings.athletes.registerForEvent(5, 11);  // 孙强
    settings.athletes.registerForEvent(6, 11);  // 周静
    settings.athletes.registerForEvent(9, 11);  // 王军
    settings.athletes.registerForEvent(10, 11); // 陈芬
    settings.athletes.registerForEvent(13, 11); // 钱伟
    settings.athletes.registerForEvent(14, 11); // 孙芳
    settings.athletes.registerForEvent(15, 11); // 李强
    settings.athletes.registerForEvent(16, 11); // 张明
    settings.athletes.registerForEvent(17, 11); // 王刚
    settings.athletes.registerForEvent(18, 11); // 刘红
    settings.athletes.registerForEvent(19, 11); // 高峰
    settings.athletes.registerForEvent(20, 11); // 杨丽
    settings.athletes.registerForEvent(21, 11); // 张野
    settings.athletes.registerForEvent(22, 11); // 黄玲
    
    std::cout << "阶段2示例数据导入成功！" << std::endl;
    std::cout << "当前运动员数量: " << settings.athletes.getAll().size() << std::endl;
    
    // 设置阶段2数据已导入
    stage2DataImported = true;
    return true;
}

// 实现阶段3数据导入：比赛成绩
bool DataManager::loadSampleStage3Data() {
    // 检查阶段2数据是否已导入
    if (!stage2DataImported) {
        std::cerr << "错误：必须先导入阶段2数据（运动员报名）" << std::endl;
        return false;
    }
    
    std::cout << "开始加载阶段3示例数据（比赛成绩）..." << std::endl;
    
    // 检查并取消参赛人数不足的项目
    // 这个步骤在阶段2导完后可能已经处理，但我们
    // 再次手动检查所有项目是否满足要求
    
    // 获取默认计分规则
    auto mainRuleOpt = settings.rules.get(1);
    if (!mainRuleOpt.has_value()) {
        std::cerr << "找不到ID为1的默认计分规则" << std::endl;
        return false;
    }
    
    // 对每个项目的参赛人数与规则进行检查及取消
    for (const auto& pair : settings.events.getAllConst()) {
        int eventId = pair.first;
        const CompetitionEvent& event = pair.second;
        int participantCount = event.getParticipantCount();
        
        // 获取适用于该参赛人数的规则
        ScoreRule& mainRule = mainRuleOpt.value().get();
        auto applicableRule = mainRule.getApplicableRule(participantCount);
        
        // 如果没有适用规则或规则不适用，说明人数不足
        if (!applicableRule.has_value() || !applicableRule.value().get().appliesTo(participantCount)) {
            std::cout << "项目\"" << event.getName() << "\"（ID: " << eventId 
                      << "）的参赛人数为" << participantCount 
                      << "，不满足计分规则要求，将被取消。" << std::endl;
            
            // 设置项目的可修改引用，修改取消状态
            auto eventOpt = settings.events.get(eventId);
            if (eventOpt.has_value()) {
                eventOpt.value().get().setCancelled(true);
            }
            continue; // 取消的项目不需要录入成绩
        }
        
        // 对于导入成绩，只为未取消的项目创建
        if (!event.getIsCancelled()) {
            // 创建成绩列表
            EventResults results(eventId);
            
            // 获取该项目的所有参赛者
            std::vector<int> participantIds(event.getParticipantAthleteIds().begin(), 
                                          event.getParticipantAthleteIds().end());
            
            // 基于项目类型，为参赛者分配不同的成绩
            for (size_t i = 0; i < participantIds.size(); i++) {
                int athleteId = participantIds[i];
                std::string scoreRecord;
                int rank = static_cast<int>(i) + 1; // 名次从1开始
                
                // 基于项目类型生成不同格式的成绩
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
                    distance += (rand() % 100) / 100.0f; // 随机小数
                    
                    // 格式化距离
                    char buffer[10];
                    sprintf(buffer, "%.2f米", distance);
                    scoreRecord = buffer;
                }
                
                // 获取该名次对应的积分
                double points = applicableRule.value().get().getScoreForRank(rank);
                
                // 添加成绩记录
                Result result(eventId, athleteId, rank, scoreRecord, points);
                results.addResult(result);
                
                // 查询运动员所属单位，为单位添加积分
                auto athleteOpt = settings.athletes.getConst(athleteId);
                if (athleteOpt.has_value()) {
                    int unitId = athleteOpt.value().get().getUnitId();
                    settings.results.addScoreToUnit(unitId, points);
                }
            }
            
            // 将成绩记录添加到系统中
            settings.results.addOrUpdate(results);
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
