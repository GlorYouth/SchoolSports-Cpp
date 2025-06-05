

#include "../../../include/UI/Controller/SystemSettingsController.h"
#include "../../../include/UI/UIManager.h"
#include "../../../include/Components/Core/Unit.h"
#include "../../../include/Components/Core/Athlete.h"
#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/Components/Core/ScoreRule.h"
#include "../../../include/Components/Core/Schedule.h"
#include <algorithm> // for std::sort, std::find (如果需要)
#include <ranges>    // for std::views::values (C++20)
#include <limits>

SystemSettingsController::SystemSettingsController(SystemSettings& settings)
    : settings_(settings) {}

void SystemSettingsController::manage() {
    int choice;
    do {
        // 获取当前工作流阶段
        WorkflowStage currentStage = settings_.workflow.getCurrentStage();
        
        // 根据当前阶段显示菜单
        UIManager::displaySystemSettingsMenu(settings_, currentStage);
        
        // 根据不同阶段设置有效的选择范围
        int maxChoice = 0;
        switch (currentStage) {
            case WorkflowStage::SETUP_EVENTS: maxChoice = 7; break;
            case WorkflowStage::REGISTRATION_OPEN: maxChoice = 8; break;
            case WorkflowStage::COMPETITION_RUNNING: maxChoice = 4; break;
        }
        
        choice = UIManager::getIntInput("请输入您的选项: ", 0, maxChoice);
        
        // 处理通用选项（所有阶段都有的）
        bool handled = true;
        switch (choice) {
            case 0: UIManager::showMessage("返回主菜单..."); break;
            case 1: handleViewAllUnits(); break;
            case 2: handleViewAllEvents(); break;
            case 3: handleScoreRuleManagement(); break;
            default: handled = false; // 不是通用选项，需要按阶段处理
        }
        
        // 如果不是通用选项，则根据当前阶段处理
        if (!handled) {
            switch (currentStage) {
                case WorkflowStage::SETUP_EVENTS:
                    switch (choice) {
                        case 4: handleAddUnit(); break;
                        case 5: handleAddEvent(); break;
                        case 6: 
                            if (!settings_.schedule.isLocked()) {
                                handleVenueManagement();
                            } else {
                                UIManager::showErrorMessage("赛程已锁定，场地管理仅可查看。");
                                handleViewAllVenues();
                            }
                            break;
                        case 7: 
                            if (!settings_.schedule.isLocked()) {
                                handleSessionSettings();
                            } else {
                                UIManager::showErrorMessage("赛程已锁定，时间段设置仅可查看。");
                                displaySessionInfo();
                            }
                            break;
                        default: UIManager::showErrorMessage("无效选择，请重新输入。"); break;
                    }
                    break;
                    
                case WorkflowStage::REGISTRATION_OPEN:
                    switch (choice) {
                        case 4: handleAddAthlete(); break;
                        case 5: handleViewAllAthletes(); break;
                        case 6: handleSetAthleteMaxEvents(); break;
                        case 7: 
                            UIManager::showMessage("场地管理（仅可查看）：");
                            handleViewAllVenues();
                            break;
                        case 8: 
                            UIManager::showMessage("时间段设置（仅可查看）：");
                            displaySessionInfo();
                            break;
                        default: UIManager::showErrorMessage("无效选择，请重新输入。"); break;
                    }
                    break;
                    
                case WorkflowStage::COMPETITION_RUNNING:
                    switch (choice) {
                        case 4: handleViewAllAthletes(); break;
                        default: UIManager::showErrorMessage("无效选择，请重新输入。"); break;
                    }
                    break;
            }
        }
        
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleAddUnit() const{
    const std::string name = UIManager::getStringInput("请输入单位名称: ");
    if (settings_.units.add(name)) { // 假设 addUnit 返回 bool 并且不再打印
        UIManager::showSuccessMessage("单位 '" + name + "' 添加成功。");
    } else {
        UIManager::showErrorMessage("单位添加失败 (可能名称已存在或输入无效)。");
    }
}

void SystemSettingsController::handleViewAllUnits() {
    // 从 settings_ 获取数据
    // C++20 ranges approach:
    std::vector<utils::RefConst<Unit>> units;
    for (const auto& val : settings_.units.getAll() | std::views::values) { // getAllUnits() 返回 const auto&
        units.push_back(std::cref(val));
    }
    UIManager::displayUnits(units);
}

void SystemSettingsController::handleAddEvent() {
    if (settings_.schedule.isLocked()) {
        UIManager::showErrorMessage("赛程已锁定，无法添加新项目。");
        return;
    }
    
    // 获取项目基本信息
    std::string name = UIManager::getStringInput("请输入项目名称: ");
    EventType eventType = UIManager::getEventTypeInput("请选择项目类型");
    Gender gender = UIManager::getGenderInput("请选择性别要求", true); // true 表示允许不限性别
    
    // 显示所有可用的计分规则
    UIManager::showTitleMessage("可用的计分规则");
    std::vector<utils::RefConst<ScoreRule>> rules;
    for (const auto& val : settings_.rules.getAll() | std::views::values) {
        rules.push_back(std::cref(val));
    }
    
    if (rules.empty()) {
        UIManager::showErrorMessage("系统中没有可用的计分规则，请先添加计分规则。");
        return;
    }
    
    UIManager::displayScoreRules(rules);
    
    // 让用户选择计分规则
    int scoreRuleId = UIManager::getIntInput("请选择要应用的计分规则ID: ");
    
    // 验证计分规则ID是否存在
    if (!settings_.rules.getConst(scoreRuleId).has_value()) {
        UIManager::showErrorMessage("所选计分规则ID不存在，项目添加失败。");
        return;
    }
    
    // 获取项目持续时间
    int durationMinutes = UIManager::getIntInput("请输入项目持续时间（分钟）: ", 1, 1440);  // 最多24小时
    
    // 使用新的方法添加项目
    settings_.events.add(name, eventType, gender, scoreRuleId);
}

void SystemSettingsController::handleViewAllEvents() {
    std::vector<utils::RefConst<CompetitionEvent>> events;
     for (const auto& val : settings_.events.getAllConst() | std::views::values) {
        events.push_back(std::cref(val));
    }
    UIManager::displayEvents(events, settings_); // 可能需要 settings 来获取计分规则描述
}

void SystemSettingsController::handleAddAthlete() {
    if (settings_.units.getAll().empty()) {
        UIManager::showErrorMessage("请先添加参赛单位。");
        return;
    }
    handleViewAllUnits(); // 显示单位列表供参考

    std::string name = UIManager::getStringInput("请输入运动员姓名: ");
    Gender gender = UIManager::getGenderInput("请选择运动员性别", false); // false 表示不允许不限性别
    if (gender == Gender::UNSPECIFIED) { // 双重检查，尽管 getGenderInput(..., false) 应该阻止
        UIManager::showErrorMessage("运动员性别不能为空。");
        return;
    }
    int unitId = UIManager::getIntInput("请输入运动员所属单位ID: ");

    if (settings_.athletes.add(name, gender, unitId)) {
        UIManager::showSuccessMessage("运动员 '" + name + "' 添加成功。");
    } else {
        UIManager::showErrorMessage("运动员添加失败 (单位ID可能无效或输入错误)。");
    }
}

void SystemSettingsController::handleViewAllAthletes() {
    std::vector<utils::RefConst<Athlete>> athletes;
    for (const auto& val : settings_.athletes.getAll() | std::views::values) {
        athletes.push_back(std::cref(val));
    }
    UIManager::displayAthletes(athletes, settings_);
}

void SystemSettingsController::handleScoreRuleManagement() {
    int choice;
    do {
        UIManager::displayScoreRuleManagementMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 4);
        
        switch (choice) {
            case 1: handleAddScoreRule(false); break; // 添加普通规则
            case 2: handleAddScoreRule(true); break;  // 添加复合规则
            case 3: handleViewAllScoreRules(); break;
            case 4: handleManageExistingScoreRule(); break;
            case 0: UIManager::showMessage("返回上级菜单..."); break;
            default: UIManager::showErrorMessage("无效选择。"); break;
        }
        
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleAddScoreRule(bool isComposite) {
    if (isComposite) {
        UIManager::showMessage("\n--- 添加复合计分规则 ---");
        UIManager::showMessage("复合规则可以包含多个子规则，系统将根据参赛人数自动选择适用的子规则。");
    } else {
        UIManager::showMessage("\n--- 添加普通计分规则 ---");
    }
    
    std::string desc = UIManager::getStringInput("请输入规则描述: ");
    int minP = UIManager::getIntInput("请输入适用此规则的最小参赛人数: ");
    int maxP = UIManager::getIntInput("请输入适用此规则的最大参赛人数 (-1表示无上限): ");
    
    if (isComposite) {
        auto mainRule = ScoreRule::createCompositeRule(desc, minP, maxP);
        int ruleId = mainRule.getId();
        
        settings_.rules.addCustom(mainRule);
        
        int subRuleCount = UIManager::getIntInput("请输入要添加的子规则数量: ", 1, 10);
        for (int i = 1; i <= subRuleCount; ++i) {
            UIManager::showMessage("\n--- 添加子规则 " + std::to_string(i) + " ---");
            
            std::string subDesc = UIManager::getStringInput("请输入子规则描述: ");
            int subMinP = UIManager::getIntInput("请输入子规则适用的最小参赛人数: ");
            int subMaxP = UIManager::getIntInput("请输入子规则适用的最大参赛人数 (-1表示无上限): ");
            int ranks = UIManager::getIntInput("请输入录取名次数: ", 1, 100);
            
            std::map<int, double> scoresMap;
            for (int j = 1; j <= ranks; ++j) {
                double scoreVal = UIManager::getDoubleInput("请输入第 " + std::to_string(j) + " 名的分数: ");
                scoresMap[j] = scoreVal;
            }
            
            auto subRule = ScoreRule(subDesc, subMinP, subMaxP, ranks, scoresMap);
            
            auto mainRuleOpt = settings_.rules.get(ruleId);
            if (mainRuleOpt.has_value()) {
                ScoreRule& ruleRef = mainRuleOpt.value().get();
                ruleRef.addSubRule(subRule);
                UIManager::showSuccessMessage("子规则 " + std::to_string(i) + " 添加成功。");
            } else {
                UIManager::showErrorMessage("获取主规则失败，无法添加子规则。");
                return;
            }
        }
        
        UIManager::showSuccessMessage("复合规则创建成功，ID为 " + std::to_string(ruleId) + "，包含 " + 
                                     std::to_string(subRuleCount) + " 个子规则。");
    } else {
        int ranks = UIManager::getIntInput("请输入录取名次数: ", 1, 100);
        std::map<int, double> scoresMap;
        for (int i = 1; i <= ranks; ++i) {
            double scoreVal = UIManager::getDoubleInput("请输入第 " + std::to_string(i) + " 名的分数: ");
            scoresMap[i] = scoreVal;
        }
        
        if (settings_.rules.add(desc, minP, maxP, ranks, scoresMap)) {
            UIManager::showSuccessMessage("计分规则添加成功。");
        } else {
            UIManager::showErrorMessage("计分规则添加失败。");
        }
    }
}

void SystemSettingsController::handleViewAllScoreRules() {
    UIManager::showMessage("\n--- 所有计分规则 ---");
    std::vector<utils::RefConst<ScoreRule>> rules;
    for (const auto& val : settings_.rules.getAll() | std::views::values) {
        rules.push_back(std::cref(val));
    }
    
    if (rules.empty()) {
        UIManager::showMessage("系统中没有任何计分规则。");
        return;
    }
    
    UIManager::displayScoreRules(rules);
}

void SystemSettingsController::handleManageExistingScoreRule() {
    UIManager::showMessage("\n--- 管理已有计分规则 ---");
    
    handleViewAllScoreRules();
    
    int ruleId = UIManager::getIntInput("请输入要管理的规则ID (输入0返回): ");
    if (ruleId == 0) return;
    
    auto ruleOpt = settings_.rules.get(ruleId);
    if (!ruleOpt.has_value()) {
        UIManager::showErrorMessage("规则ID " + std::to_string(ruleId) + " 不存在。");
        return;
    }
    
    ScoreRule& rule = ruleOpt.value().get();
    
    UIManager::showMessage("\n规则详情:");
    UIManager::showMessage("ID: " + std::to_string(rule.getId()));
    UIManager::showMessage("描述: " + rule.getDescription());
    UIManager::showMessage("适用人数范围: " + std::to_string(rule.getMinParticipants()) + 
                         " 到 " + (rule.getMaxParticipants() == -1 ? "无上限" : std::to_string(rule.getMaxParticipants())));
    
    if (rule.isComposite()) {
        UIManager::showMessage("\n这是一个复合规则，包含以下子规则:");
        int idx = 1;
        for (const auto subRule : rule.getSubRules()) {
            UIManager::showMessage("\n子规则 " + std::to_string(idx) + ":");
            UIManager::showMessage("描述: " + subRule.get().getDescription());
            UIManager::showMessage("适用人数: " + std::to_string(subRule.get().getMinParticipants()) +
                                 " 到 " + (subRule.get().getMaxParticipants() == -1 ? "无上限" : std::to_string(subRule.get().getMaxParticipants())));
            
            UIManager::showMessage("录取名次: " + std::to_string(subRule.get().getRanksToAward()));
            UIManager::showMessage("分数分配:");
            for (const auto& [rank, score] : subRule.get().getAllScoresForRanks()) {
                UIManager::showMessage("- 第" + std::to_string(rank) + "名: " + std::to_string(score) + "分");
            }
            
            idx++;
        }
        
        UIManager::showMessage("\n管理选项:");
        UIManager::showMessage("1. 添加新的子规则");
        UIManager::showMessage("0. 返回");
        
        int option = UIManager::getIntInput("请输入选项: ", 0, 1);
        if (option == 0) return;
        
        if (option == 1) {
            UIManager::showMessage("\n--- 添加新的子规则 ---");
            
            std::string subDesc = UIManager::getStringInput("请输入子规则描述: ");
            int subMinP = UIManager::getIntInput("请输入子规则适用的最小参赛人数: ");
            int subMaxP = UIManager::getIntInput("请输入子规则适用的最大参赛人数 (-1表示无上限): ");
            int ranks = UIManager::getIntInput("请输入录取名次数: ", 1, 100);
            
            std::map<int, double> scoresMap;
            for (int j = 1; j <= ranks; ++j) {
                double scoreVal = UIManager::getDoubleInput("请输入第 " + std::to_string(j) + " 名的分数: ");
                scoresMap[j] = scoreVal;
            }
            
            auto subRule = ScoreRule(subDesc, subMinP, subMaxP, ranks, scoresMap);
            rule.addSubRule(subRule);
            
            UIManager::showSuccessMessage("新的子规则添加成功。");
        }
    } else {
        UIManager::showMessage("录取名次: " + std::to_string(rule.getRanksToAward()));
        UIManager::showMessage("分数分配:");
        for (const auto& [rank, score] : rule.getAllScoresForRanks()) {
            UIManager::showMessage("- 第" + std::to_string(rank) + "名: " + std::to_string(score) + "分");
        }
        
        UIManager::showMessage("\n普通规则暂不支持更多管理操作。");
    }
}

void SystemSettingsController::handleSetAthleteMaxEvents() {
    int maxEvents = UIManager::getIntInput("请输入新的运动员最大参赛项目数: ", 1, 10); // 假设范围1-10
    settings_.setAthleteMaxEventsAllowed(maxEvents); // 假设此方法存在且不直接打印
    UIManager::showSuccessMessage("运动员最大参赛项目数已更新为: " + std::to_string(maxEvents));
}

void SystemSettingsController::handleVenueManagement() {
    int choice;
    do {
        UIManager::displayVenueManagementMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 3);
        switch (choice) {
            case 1: {
                std::string venueName = UIManager::getStringInput("请输入新场地名称: ");
                if (settings_.venues.add(venueName)) {
                    UIManager::showSuccessMessage("场地添加成功。");
                } else {
                    UIManager::showErrorMessage("场地已存在，添加失败。");
                }
                break;
            }
            case 2: {
                std::string venueName = UIManager::getStringInput("请输入要删除的场地名称: ");
                if (settings_.venues.remove(venueName)) {
                    UIManager::showSuccessMessage("场地删除成功。");
                } else {
                    UIManager::showErrorMessage("未找到该场地，删除失败。");
                }
                break;
            }
            case 3: {
                UIManager::displayVenues(settings_.venues.getAll());
                break;
            }
            case 0:
                UIManager::showMessage("返回上一级菜单...");
                break;
            default:
                UIManager::showErrorMessage("无效选择。");
                break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void SystemSettingsController::handleSessionSettings() {
    if (settings_.schedule.isLocked()) {
        UIManager::showErrorMessage("赛程已锁定，无法修改上下午时间段设置。");
        return;
    }
    int choice;
    do {
        UIManager::displaySessionSettingsMenu(settings_);
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);
        switch (choice) {
            case 1: {
                std::string start = UIManager::getStringInput("请输入上午开始时间(格式08:00): ");
                std::string end = UIManager::getStringInput("请输入上午结束时间(格式12:00): ");
                settings_.sessions.setMorningSession(start, end);
                UIManager::showSuccessMessage("上午时间段设置成功。");
                break;
            }
            case 2: {
                std::string start = UIManager::getStringInput("请输入下午开始时间(格式14:00): ");
                std::string end = UIManager::getStringInput("请输入下午结束时间(格式18:00): ");
                settings_.sessions.setAfternoonSession(start, end);
                UIManager::showSuccessMessage("下午时间段设置成功。");
                break;
            }
            case 0:
                UIManager::showMessage("返回上一级菜单...");
                break;
            default:
                UIManager::showErrorMessage("无效选择。");
                break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

// 添加显示会话信息的辅助方法
void SystemSettingsController::displaySessionInfo() {
    auto [morningStart, morningEnd] = settings_.sessions.getMorningSession();
    auto [afternoonStart, afternoonEnd] = settings_.sessions.getAfternoonSession();
    
    UIManager::showMessage("\n当前时间段设置：");
    UIManager::showMessage("上午时间段: " + morningStart + " - " + morningEnd);
    UIManager::showMessage("下午时间段: " + afternoonStart + " - " + afternoonEnd);
}

// 添加查看所有场地的辅助方法
void SystemSettingsController::handleViewAllVenues() {
    UIManager::displayVenues(settings_.venues.getAll());
}
