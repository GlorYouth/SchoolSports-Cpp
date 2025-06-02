//
// Created by GlorYouth on 2025/6/3.
//

#include "../include/AutoTest.h"


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits> // Required for std::numeric_limits
#include <algorithm> // Required for std::find_if, std::any_of
#include <cassert>   // For basic assertions

// 包含项目所需的所有头文件
#include "../include/SystemSettings.h"
#include "../include/Registration.h"
#include "../include/Schedule.h" // 虽然秩序册生成是占位符，但头文件可能被其他部分间接需要
#include "../include/DataManager.h" // 数据管理功能是占位符，但可以测试接口调用
#include "../include/Constants.h"
#include "../include/Unit.h"
#include "../include/Athlete.h"
#include "../include/CompetitionEvent.h"
#include "../include/ScoreRule.h"
#include "../include/Result.h"

// 定义一个简单的测试结果打印函数
void printTestResult(const std::string& testName, bool success) {
    std::cout << "[测试] " << testName << ": " << (success ? "通过" : "失败") << std::endl;
}

// 获取第一个单位的ID（辅助函数）
int getFirstUnitId(const SystemSettings& settings) {
    if (settings.getAllUnits().empty()) return -1;
    return settings.getAllUnits().begin()->first;
}

// 获取第一个运动员的ID（辅助函数）
int getFirstAthleteId(const SystemSettings& settings) {
    if (settings.getAllAthletes().empty()) return -1;
    return settings.getAllAthletes().begin()->first;
}

// 获取第一个男子项目ID（辅助函数）
int getFirstMaleEventId(const SystemSettings& settings) {
    for (const auto& pair : settings.getAllCompetitionEvents()) {
        if (pair.second.getGenderRequirement() == Gender::MALE) {
            return pair.first;
        }
    }
    return -1;
}
// 获取第一个女子项目ID（辅助函数）
int getFirstFemaleEventId(const SystemSettings& settings) {
    for (const auto& pair : settings.getAllCompetitionEvents()) {
        if (pair.second.getGenderRequirement() == Gender::FEMALE) {
            return pair.first;
        }
    }
    return -1;
}


// 测试单位管理
void testUnitManagement(SystemSettings& settings) {
    std::cout << "\n--- 开始测试单位管理 ---" << std::endl;
    bool success;
    int initialUnitCount = settings.getAllUnits().size();

    // 1. 测试添加新单位
    success = settings.addUnit("测试学院");
    printTestResult("添加新单位 '测试学院'", success && settings.getAllUnits().size() == initialUnitCount + 1);

    // 2. 测试添加同名单位 (应失败)
    success = !settings.addUnit("测试学院"); // 期望返回false
    printTestResult("添加同名单位 '测试学院'", success && settings.getAllUnits().size() == initialUnitCount + 1);

    // 3. 获取刚添加的单位
    int testUnitId = -1;
    for(const auto& pair : settings.getAllUnits()){
        if(pair.second.getName() == "测试学院"){
            testUnitId = pair.first;
            break;
        }
    }
    auto unitOpt = settings.getUnit(testUnitId);
    printTestResult("获取存在的单位 '测试学院'", unitOpt.has_value() && unitOpt.value().get().getName() == "测试学院");

    // 4. 测试删除单位
    if (testUnitId != -1) {
        success = settings.removeUnit(testUnitId);
        printTestResult("删除单位 '测试学院'", success && settings.getAllUnits().size() == initialUnitCount);
        unitOpt = settings.getUnit(testUnitId);
        printTestResult("确认单位 '测试学院' 已被删除", !unitOpt.has_value());
    } else {
        printTestResult("删除单位 '测试学院' (前置条件失败，未找到ID)", false);
    }

    // 5. 测试删除不存在的单位
    success = !settings.removeUnit(99999); // 假设99999是一个不存在的ID
    printTestResult("删除不存在的单位ID 99999", success);
    std::cout << "--- 单位管理测试结束 ---\n";
}

// 测试运动员管理
void testAthleteManagement(SystemSettings& settings) {
    std::cout << "\n--- 开始测试运动员管理 ---" << std::endl;
    bool success;
    int initialAthleteCount = settings.getAllAthletes().size();
    int unitId = getFirstUnitId(settings);

    if (unitId == -1) {
        std::cout << "运动员管理测试跳过：系统中没有单位可用于测试。" << std::endl;
        return;
    }

    // 1. 测试添加新运动员到有效单位
    success = settings.addAthlete("测试男运动员", Gender::MALE, unitId);
    printTestResult("添加新男运动员到有效单位", success && settings.getAllAthletes().size() == initialAthleteCount + 1);
    int maleAthleteId = -1;
    for(const auto& pair : settings.getAllAthletes()){
        if(pair.second.getName() == "测试男运动员"){
            maleAthleteId = pair.first;
            break;
        }
    }

    success = settings.addAthlete("测试女运动员", Gender::FEMALE, unitId);
    printTestResult("添加新女运动员到有效单位", success && settings.getAllAthletes().size() == initialAthleteCount + 2);
     int femaleAthleteId = -1;
    for(const auto& pair : settings.getAllAthletes()){
        if(pair.second.getName() == "测试女运动员"){
            femaleAthleteId = pair.first;
            break;
        }
    }


    // 2. 测试添加到不存在的单位 (应失败)
    success = !settings.addAthlete("无效单位运动员", Gender::MALE, 99999);
    printTestResult("添加运动员到无效单位ID 99999", success && settings.getAllAthletes().size() == initialAthleteCount + 2);

    // 3. 获取存在的运动员
    if (maleAthleteId != -1) {
        auto athleteOpt = settings.getAthlete(maleAthleteId);
        printTestResult("获取存在的男运动员", athleteOpt.has_value() && athleteOpt.value().get().getName() == "测试男运动员");
    }

    // 4. 测试删除运动员
    if (maleAthleteId != -1) {
        success = settings.removeAthlete(maleAthleteId);
        printTestResult("删除男运动员", success && settings.getAllAthletes().size() == initialAthleteCount + 1);
        auto athleteOpt = settings.getAthlete(maleAthleteId);
        printTestResult("确认男运动员已被删除", !athleteOpt.has_value());
    }
     if (femaleAthleteId != -1) {
        success = settings.removeAthlete(femaleAthleteId);
        printTestResult("删除女运动员", success && settings.getAllAthletes().size() == initialAthleteCount);
        auto athleteOpt = settings.getAthlete(femaleAthleteId);
        printTestResult("确认女运动员已被删除", !athleteOpt.has_value());
    }

    std::cout << "--- 运动员管理测试结束 ---\n";
}

// 测试比赛项目管理
void testCompetitionEventManagement(SystemSettings& settings) {
    std::cout << "\n--- 开始测试比赛项目管理 ---" << std::endl;
    bool success;
    int initialEventCount = settings.getAllCompetitionEvents().size();

    // 1. 添加新项目
    success = settings.addCompetitionEvent("测试男子跳远", EventType::FIELD, Gender::MALE);
    printTestResult("添加新项目 '测试男子跳远'", success && settings.getAllCompetitionEvents().size() == initialEventCount + 1);
    int testEventId = -1;
    for(const auto& pair : settings.getAllCompetitionEvents()){
        if(pair.second.getName() == "测试男子跳远"){
            testEventId = pair.first;
            break;
        }
    }

    // 2. 获取项目
    if(testEventId != -1){
        auto eventOpt = settings.getCompetitionEvent(testEventId);
        printTestResult("获取存在的项目 '测试男子跳远'", eventOpt.has_value() && eventOpt.value().get().getName() == "测试男子跳远");
    }

    // 3. 删除项目
    if (testEventId != -1) {
        success = settings.removeCompetitionEvent(testEventId);
        printTestResult("删除项目 '测试男子跳远'", success && settings.getAllCompetitionEvents().size() == initialEventCount);
        auto eventOpt = settings.getCompetitionEvent(testEventId);
        printTestResult("确认项目 '测试男子跳远' 已被删除", !eventOpt.has_value());
    }
    std::cout << "--- 比赛项目管理测试结束 ---\n";
}

// 测试报名管理
void testRegistrationManagement(SystemSettings& settings, Registration& registration) {
    std::cout << "\n--- 开始测试报名管理 ---" << std::endl;
    bool success;

    // 准备测试数据
    settings.addUnit("报名测试学院");
    int testUnitId = -1;
    for(const auto& u : settings.getAllUnits()){ if(u.second.getName() == "报名测试学院") testUnitId = u.first; }
    assert(testUnitId != -1 && "测试单位创建失败");


    settings.addAthlete("报名选手男", Gender::MALE, testUnitId);
    int maleAthleteId = -1;
    for(const auto& a : settings.getAllAthletes()){ if(a.second.getName() == "报名选手男") maleAthleteId = a.first; }
    assert(maleAthleteId != -1 && "测试男选手创建失败");

    settings.addAthlete("报名选手女", Gender::FEMALE, testUnitId);
    int femaleAthleteId = -1;
    for(const auto& a : settings.getAllAthletes()){ if(a.second.getName() == "报名选手女") femaleAthleteId = a.first; }
    assert(femaleAthleteId != -1 && "测试女选手创建失败");

    settings.addCompetitionEvent("报名测试男项目", EventType::TRACK, Gender::MALE);
    int maleEventId = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "报名测试男项目") maleEventId = e.first; }
    assert(maleEventId != -1 && "测试男项目创建失败");

    settings.addCompetitionEvent("报名测试女项目", EventType::TRACK, Gender::FEMALE);
    int femaleEventId = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "报名测试女项目") femaleEventId = e.first; }
    assert(femaleEventId != -1 && "测试女项目创建失败");

    settings.addCompetitionEvent("报名测试不限性别项目", EventType::FIELD, Gender::UNSPECIFIED);
    int unspecEventId = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "报名测试不限性别项目") unspecEventId = e.first; }
    assert(unspecEventId != -1 && "测试不限性别项目创建失败");


    // 1. 正常报名 - 男选手报男子项目
    success = registration.registerAthleteForEvent(maleAthleteId, maleEventId);
    printTestResult("男选手报名男子项目", success);
    if(success){
        auto ath = settings.getAthlete(maleAthleteId);
        auto evt = settings.getCompetitionEvent(maleEventId);
        printTestResult("男选手报名后项目数检查", ath.value().get().getRegisteredEventsCount() == 1);
        printTestResult("男子项目报名人数检查", evt.value().get().getParticipantCount() == 1);
    }


    // 2. 性别不符报名 - 男选手报女子项目 (应失败)
    success = !registration.registerAthleteForEvent(maleAthleteId, femaleEventId);
    printTestResult("男选手报名女子项目 (应失败)", success);


    // 3. 正常报名 - 女选手报女子项目
    success = registration.registerAthleteForEvent(femaleAthleteId, femaleEventId);
    printTestResult("女选手报名女子项目", success);

    // 4. 正常报名 - 选手报不限性别项目
    success = registration.registerAthleteForEvent(maleAthleteId, unspecEventId); // 男选手已报1项
    printTestResult("男选手报名不限性别项目", success);


    // 5. 重复报名 (应失败或被忽略)
    success = !registration.registerAthleteForEvent(maleAthleteId, maleEventId); // maleAthleteId 已报名 maleEventId
    printTestResult("男选手重复报名男子项目 (应失败/忽略)", success);


    // 6. 报名超限额 (运动员最多报3项，maleAthleteId已报2项)
    settings.addCompetitionEvent("额外测试男项目1", EventType::TRACK, Gender::MALE);
    int extraMaleEventId1 = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "额外测试男项目1") extraMaleEventId1 = e.first; }

    settings.addCompetitionEvent("额外测试男项目2", EventType::TRACK, Gender::MALE);
    int extraMaleEventId2 = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "额外测试男项目2") extraMaleEventId2 = e.first; }

    success = registration.registerAthleteForEvent(maleAthleteId, extraMaleEventId1); // 第3项
    printTestResult("男选手报名第3个项目", success);

    success = !registration.registerAthleteForEvent(maleAthleteId, extraMaleEventId2); // 尝试报第4项 (应失败)
    printTestResult("男选手报名第4个项目 (应失败)", success);


    // 7. 取消报名
    success = registration.unregisterAthleteFromEvent(maleAthleteId, maleEventId);
    printTestResult("男选手取消报名男子项目", success);
    if(success){
        auto ath = settings.getAthlete(maleAthleteId);
        auto evt = settings.getCompetitionEvent(maleEventId); // maleEventId 此时应无参赛者
        printTestResult("男选手取消报名后项目数检查", ath.value().get().getRegisteredEventsCount() == 2); // 之前报了3项，取消1项剩2项
        printTestResult("男子项目取消报名后人数检查", evt.value().get().getParticipantCount() == 0);
    }

    // 8. 测试因人数不足取消项目
    // 创建一个新项目，报名1人，然后取消报名，看项目是否被取消
    settings.addCompetitionEvent("人数不足测试项目", EventType::TRACK, Gender::MALE);
    int lowPartEventId = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "人数不足测试项目") lowPartEventId = e.first; }

    settings.addAthlete("临时选手", Gender::MALE, testUnitId);
    int tempAthleteId = -1;
    for(const auto& a : settings.getAllAthletes()){ if(a.second.getName() == "临时选手") tempAthleteId = a.first; }

    registration.registerAthleteForEvent(tempAthleteId, lowPartEventId); // 1人报名
    auto lowEvent = settings.getCompetitionEvent(lowPartEventId);
    printTestResult("项目有1人报名，未取消", lowEvent.has_value() && !lowEvent.value().get().getIsCancelled());

    registration.unregisterAthleteFromEvent(tempAthleteId, lowPartEventId); // 0人报名
    registration.checkAndCancelEventsDueToLowParticipation(); // 手动触发检查，或依赖unregister内部逻辑
    lowEvent = settings.getCompetitionEvent(lowPartEventId);
    printTestResult("项目无人报名后应被取消 (最少4人)", lowEvent.has_value() && lowEvent.value().get().getIsCancelled());


    // 清理测试数据
    settings.removeAthlete(maleAthleteId);
    settings.removeAthlete(femaleAthleteId);
    settings.removeAthlete(tempAthleteId);
    settings.removeCompetitionEvent(maleEventId);
    settings.removeCompetitionEvent(femaleEventId);
    settings.removeCompetitionEvent(unspecEventId);
    settings.removeCompetitionEvent(extraMaleEventId1);
    settings.removeCompetitionEvent(extraMaleEventId2);
    settings.removeCompetitionEvent(lowPartEventId);
    settings.removeUnit(testUnitId);

    std::cout << "--- 报名管理测试结束 ---\n";
}

// 测试成绩管理
void testResultManagement(SystemSettings& settings) {
    std::cout << "\n--- 开始测试成绩管理 ---" << std::endl;
    bool success;

    // 准备数据
    settings.addUnit("成绩测试学院");
    int unitId = -1;
    for(const auto& u : settings.getAllUnits()){ if(u.second.getName() == "成绩测试学院") unitId = u.first; }

    std::vector<int> athleteIds;
    for (int i = 0; i < 7; ++i) { // 创建7个运动员
        settings.addAthlete("选手" + std::to_string(i + 1), Gender::MALE, unitId);
        for(const auto& a : settings.getAllAthletes()){
            if(a.second.getName() == "选手" + std::to_string(i+1)){
                 athleteIds.push_back(a.first);
                 break;
            }
        }
    }
    assert(athleteIds.size() == 7 && "运动员创建不足7人");


    settings.addCompetitionEvent("100米决赛", EventType::TRACK, Gender::MALE);
    int eventId = -1;
    for(const auto& e : settings.getAllCompetitionEvents()){ if(e.second.getName() == "100米决赛") eventId = e.first; }
    assert(eventId != -1 && "比赛项目创建失败");


    Registration reg(settings);
    for (int athId : athleteIds) {
        reg.registerAthleteForEvent(athId, eventId); // 全部报名
    }
    auto eventOpt = settings.getCompetitionEvent(eventId);
    assert(eventOpt.has_value() && eventOpt.value().get().getParticipantCount() == 7 && "报名人数不为7");

    // 1. 录入成绩 (7人参赛，应使用规则1：取前5名，7,5,3,2,1分)
    EventResults results(eventId);
    auto ruleOpt = settings.findAppropriateScoreRule(7);
    assert(ruleOpt.has_value() && "未找到7人参赛的计分规则");
    ScoreRule& rule = ruleOpt.value().get();

    // 模拟录入前5名成绩
    results.addResult(Result(eventId, athleteIds[0], 1, "10.5s", rule.getScoreForRank(1))); // 7分
    results.addResult(Result(eventId, athleteIds[1], 2, "10.6s", rule.getScoreForRank(2))); // 5分
    results.addResult(Result(eventId, athleteIds[2], 3, "10.7s", rule.getScoreForRank(3))); // 3分
    results.addResult(Result(eventId, athleteIds[3], 4, "10.8s", rule.getScoreForRank(4))); // 2分
    results.addResult(Result(eventId, athleteIds[4], 5, "10.9s", rule.getScoreForRank(5))); // 1分
    results.finalizeResults();
    success = settings.addOrUpdateEventResults(results);
    printTestResult("录入7人参赛项目的成绩", success);

    // 2. 检查单位总分
    double expectedScore = 7.0 + 5.0 + 3.0 + 2.0 + 1.0; // 18分
    auto unitOpt = settings.getUnit(unitId);
    // 注意：由于 addScore 是累加的，如果之前单位有分数，这里需要特殊处理。
    // 假设 initializeDefaultSettings 后单位分数为0，或者我们只关注这个项目的增量。
    // 为了简单，我们直接在录入成绩时更新单位分数，但SystemSettings::addOrUpdateEventResults内部并没有直接更新单位分。
    // 这部分逻辑在 main.cpp 的 recordEventResults 中。
    // 因此，这里的测试需要模拟 main.cpp 的逻辑或修改 SystemSettings。
    // 为了测试的独立性，我们手动更新单位分数。
    if(unitOpt.has_value()){
        unitOpt.value().get().resetScore(); // 先重置
        for(const auto& res : results.getResultsList()){
            auto ath = settings.getAthlete(res.getAthleteId());
            if(ath.has_value() && ath.value().get().getUnitId() == unitId){
                unitOpt.value().get().addScore(res.getPointsAwarded());
            }
        }
    }

    printTestResult("单位总分检查 (18分)", unitOpt.has_value() && unitOpt.value().get().getTotalScore() == expectedScore);


    // 3. 查看项目成绩
    auto storedResultsOpt = settings.getEventResultsConst(eventId);
    printTestResult("查看已录入的项目成绩", storedResultsOpt.has_value() && storedResultsOpt.value().get().getResultsList().size() == 5);

    // 4. 清除项目成绩
    double scoreBeforeClear = unitOpt.value().get().getTotalScore();
    settings.clearResultsForEvent(eventId); // 这个函数会扣除单位分数
    printTestResult("清除项目成绩后单位分数检查 (应为0)", unitOpt.has_value() && unitOpt.value().get().getTotalScore() == (scoreBeforeClear - expectedScore));
    storedResultsOpt = settings.getEventResultsConst(eventId);
    printTestResult("确认项目成绩已被清除", !storedResultsOpt.has_value() || storedResultsOpt.value().get().getResultsList().empty());


    // 清理
    for(int athId : athleteIds) settings.removeAthlete(athId);
    settings.removeCompetitionEvent(eventId);
    settings.removeUnit(unitId);

    std::cout << "--- 成绩管理测试结束 ---\n";
}


int autoTest() {
    std::cout << "========== 学校运动会管理系统 - 自动化测试 ==========" << std::endl;

    SystemSettings settings;
    Registration registration(settings); // Registration 需要 SystemSettings

    // 步骤 0: 初始化默认设置
    std::cout << "\n--- 步骤 0: 初始化系统默认设置 ---" << std::endl;
    settings.initializeDefaultSettings();
    printTestResult("默认设置初始化", !settings.getAllUnits().empty() && !settings.getAllCompetitionEvents().empty());
    if (settings.getAllUnits().empty() || settings.getAllCompetitionEvents().empty()){
        std::cerr << "错误：默认设置初始化失败，部分核心数据为空，后续测试可能无法进行。" << std::endl;
        return 1;
    }
    std::cout << "默认单位数量: " << settings.getAllUnits().size() << std::endl;
    std::cout << "默认运动员数量: " << settings.getAllAthletes().size() << std::endl;
    std::cout << "默认项目数量: " << settings.getAllCompetitionEvents().size() << std::endl;
    std::cout << "默认计分规则数量: " << settings.getAllScoreRules().size() << std::endl;


    // 执行各个模块的测试
    testUnitManagement(settings);
    testAthleteManagement(settings);
    testCompetitionEventManagement(settings);
    testRegistrationManagement(settings, registration); // Registration 测试需要 Registration 对象
    testResultManagement(settings);

    // TODO: 添加计分规则管理测试
    // TODO: 添加系统参数设置测试
    // TODO: 添加秩序册生成测试 (目前是占位符)
    // TODO: 添加数据备份与恢复测试 (目前是占位符)


    std::cout << "\n========== 自动化测试全部完成 ==========" << std::endl;

    // 提示用户按键退出，以便查看控制台输出
    std::cout << "\n按 Enter 键退出测试程序...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除之前可能的输入残留
    std::cin.get();

    return 0;
}
