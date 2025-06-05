
#include "../../../include/Components/Core/SystemSettings.h"
#include <iostream> // 用于演示输出
#include <ranges>
#include <string>
#include <vector>
#include <map>
#include "../../../include/Components/Core/ScoreRule.h"
// Workflow.h 应该通过 SystemSettings.h 包含进来，如果直接使用 WorkflowStage 枚举则需要确保
// #include "../../include/Components/Workflow.h" // 通常不需要重复包含

SystemSettings::SystemSettings() : 
    units(*this),
    athletes(*this),
    events(*this),
    rules(*this),
    results(*this),
    workflow(*this),
    sessions(*this),
    venues(*this),
    schedule(*this),
    args(*this),
    _athleteMaxEventsAllowed(3) {
    // 构造函数中可以进行一些初始化
    // initializeDefaultSettings(); // 可以在构造时直接初始化，或者由外部调用
}

void SystemSettings::resetAllIdCounter() {
    UnitManager::resetIdCounter();
    AthleteManager::resetIdCounter();
    CompetitionEventManager::resetIdCounter();
    ScoreRuleManager::resetIdCounter();
}

void SystemSettings::clearAllData(){
    units.clear();
    athletes.clear();
    events.clear();
    results.clear();
    rules.clear();
    venues.clear();
    results.resetAllUnitScores();
}

// --- 系统参数设置 ---
void SystemSettings::setAthleteMaxEventsAllowed(int maxEvents) {
    if (maxEvents > 0) {
        _athleteMaxEventsAllowed = maxEvents;
    } else {
        std::cerr << "错误: 运动员最大参赛项目数必须大于0。" << std::endl;
    }
}

int SystemSettings::getAthleteMaxEventsAllowed() const {
    return _athleteMaxEventsAllowed;
}

// --- 交互操作 (例如报名等) ---
// 初始化默认设置 (修改后)
void SystemSettings::initializeDefaultSettings() {
    std::cout << "正在初始化系统默认核心设置..." << std::endl;

    // 1. 清空所有数据
    clearAllData();

    // 2. 设置系统核心参数
    args.setAthleteMaxEventsAllowed(3); // 每人最多报3项 (来自原逻辑)

    // 3. 添加默认的复合计分规则（ID为1）
    ScoreRule::resetNextId(1); // 确保第一个规则的ID为1
    
    // 创建主规则（包含子规则）
    std::map<int, double> dummyScores; // 主规则不直接使用分数，而是通过子规则
    bool rule1Added = rules.add("默认规则: 根据参赛人数动态选择计分方案", 4, -1, 0, dummyScores);
    
    if (!rule1Added) {
        std::cerr << "严重错误: 无法添加ID为1的默认计分规则！" << std::endl;
        return;
    }
    
    // 获取主规则（ID=1）
    auto mainRuleOpt = rules.get(1);
    if (!mainRuleOpt.has_value()) {
        std::cerr << "严重错误: 无法获取刚刚创建的主规则（ID=1）！" << std::endl;
        return;
    }
    
    // 子规则1: 参赛人数 > 6人，取前5名 (7,5,3,2,1)
    std::map<int, double> scoresRule1 = {{1, 7.0}, {2, 5.0}, {3, 3.0}, {4, 2.0}, {5, 1.0}};
    auto subRule1 = ScoreRule("子规则1: 大于6人取前5名 (7,5,3,2,1)", 7, -1, 5, scoresRule1);
    
    // 子规则2: 参赛人数 <= 6人且 >= 4人，取前3名 (5,3,2)
    std::map<int, double> scoresRule2 = {{1, 5.0}, {2, 3.0}, {3, 2.0}};
    auto subRule2 = ScoreRule("子规则2: 4至6人取前3名 (5,3,2)", 4, 6, 3, scoresRule2);
    
    // 将子规则添加到主规则
    mainRuleOpt.value().get().addSubRule(subRule1);
    mainRuleOpt.value().get().addSubRule(subRule2);
    
    std::cout << "系统默认规则（ID=1）初始化完成，包含两个子规则：" << std::endl;
    std::cout << " - " << subRule1.getDescription() << std::endl;
    std::cout << " - " << subRule2.getDescription() << std::endl;
    std::cout << " - 注意：不足4人的项目将被自动取消" << std::endl;  // 添加说明，最小人数限制现在由规则决定

    // 4. 添加默认场馆
    venues.addDefaultVenues();

    // 5. 设置默认时间段
    sessions.setDefaultSessions();

    // 6. 设置默认工作流阶段
    workflow.enterSetupEventsStage();
    
    std::cout << "系统默认核心设置初始化完成。" << std::endl;
    std::cout << "提示：示例单位、运动员、比赛项目需通过 '导入示例数据' 选项加载。" << std::endl;
}

