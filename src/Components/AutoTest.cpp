// //
// // Created by GlorYouth on 2025/6/3.
// //
//
// #include "../../include/Components/AutoTest.h"
//
//
// #include <iostream>
// #include <string>
// #include <vector>
// #include <map>
// #include <limits> // Required for std::numeric_limits
// #include <algorithm> // Required for std::find_if, std::any_of
// #include <cassert>   // For basic assertions
//
// // 包含项目所需的所有头文件
// #include "../../include/Components/SystemSettings.h"
// #include "../../include/Components/Registration.h"
// #include "../../include/Components/Schedule.h"
// #include "../../include/Components/DataManager.h"
// #include "../../include/Components/Constants.h"
// #include "../../include/Components/Unit.h"
// #include "../../include/Components/Athlete.h"
// #include "../../include/Components/CompetitionEvent.h"
// #include "../../include/Components/ScoreRule.h"
// #include "../../include/Components/Result.h"
//
// // 定义一个简单的测试结果打印函数
// void printTestResult(const std::string& testName, bool success) {
//     std::cout << "[测试] " << testName << ": " << (success ? "通过" : "失败") << std::endl;
// }
//
// // 获取第一个单位的ID（辅助函数）
// int getFirstUnitId(const SystemSettings& settings) {
//     if (settings.units.getAll().empty()) return -1;
//     return settings.units.getAll().begin()->first;
// }
//
// // 获取第一个运动员的ID（辅助函数）
// int getFirstAthleteId(const SystemSettings& settings) {
//     if (settings.athletes.getAll().empty()) return -1;
//     return settings.athletes.getAll().begin()->first;
// }
//
// // 获取第一个男子项目ID（辅助函数）
// int getFirstMaleEventId(const SystemSettings& settings) {
//     for (const auto& pair : settings.events.getAllConst()) {
//         if (pair.second.getGenderRequirement() == Gender::MALE) {
//             return pair.first;
//         }
//     }
//     return -1;
// }
// // 获取第一个女子项目ID（辅助函数）
// int getFirstFemaleEventId(const SystemSettings& settings) {
//     for (const auto& pair : settings.events.getAllConst()) {
//         if (pair.second.getGenderRequirement() == Gender::FEMALE) {
//             return pair.first;
//         }
//     }
//     return -1;
// }
//
//
// // 测试单位管理
// void testUnitManagement(SystemSettings& settings) {
//     std::cout << "\n--- 开始测试单位管理 ---" << std::endl;
//     const int initialUnitCount = settings.units.getAll().size();
//     const std::string testUnitName = " UNIQUE_TEST_COLLEGE_123 "; // 使用一个唯一的名字以避免与示例数据冲突
//
//     // 1. 测试添加新单位
//     bool success = settings.units.add(testUnitName);
//     printTestResult("单位管理：添加新单位 " + testUnitName, success && settings.units.getAll().size() == initialUnitCount + 1);
//
//     // 2. 测试添加同名单位 (应失败)
//     success = !settings.units.add(testUnitName); // 期望返回false
//     printTestResult("单位管理：尝试添加同名单位 " + testUnitName, success && settings.units.getAll().size() == initialUnitCount + 1);
//
//     // 3. 获取刚添加的单位
//     int testUnitId = -1;
//     bool foundByName = false;
//     for(const auto& pair : settings.units.getAll()){
//         if(pair.second.getName() == testUnitName){
//             testUnitId = pair.first;
//             foundByName = true;
//             break;
//         }
//     }
//     printTestResult("单位管理：通过名称找到新添加的单位 " + testUnitName, foundByName);
//
//     auto unitOpt = settings.units.get(testUnitId);
//     printTestResult("单位管理：通过ID获取新添加的单位 " + testUnitName, unitOpt.has_value() && unitOpt.value().get().getName() == testUnitName);
//
//     // 4. 测试删除单位
//     if (testUnitId != -1) {
//         success = settings.removeUnit(testUnitId);
//         printTestResult("单位管理：删除新添加的单位 " + testUnitName, success && settings.units.getAll().size() == initialUnitCount);
//         unitOpt = settings.units.get(testUnitId);
//         printTestResult("单位管理：确认单位 " + testUnitName + " 已被删除", !unitOpt.has_value());
//     } else {
//         printTestResult("单位管理：删除单位前未能获取其ID (测试逻辑问题)", false);
//     }
//
//     // 5. 测试删除不存在的单位
//     success = !settings.removeUnit(99999); // 假设99999是一个不存在的ID
//     printTestResult("单位管理：尝试删除不存在的单位ID 99999", success);
//     std::cout << "--- 单位管理测试结束 ---\n";
// }
//
// // 测试运动员管理
// void testAthleteManagement(SystemSettings& settings) {
//     std::cout << "\n--- 开始测试运动员管理 ---" << std::endl;
//     const int initialAthleteCount = settings.athletes.getAll().size();
//     const int unitId = getFirstUnitId(settings);
//
//     if (unitId == -1) {
//         std::cout << "运动员管理测试跳过：系统中没有单位可用于测试。" << std::endl;
//         return;
//     }
//
//     // 1. 测试添加新运动员到有效单位
//     bool success = settings.athletes.add("测试男运动员", Gender::MALE, unitId);
//     printTestResult("添加新男运动员到有效单位", success && settings.athletes.getAll().size() == initialAthleteCount + 1);
//     int maleAthleteId = -1;
//     for(const auto& pair : settings.athletes.getAll()){
//         if(pair.second.getName() == "测试男运动员"){
//             maleAthleteId = pair.first;
//             break;
//         }
//     }
//
//     success = settings.athletes.add("测试女运动员", Gender::FEMALE, unitId);
//     printTestResult("添加新女运动员到有效单位", success && settings.athletes.getAll().size() == initialAthleteCount + 2);
//      int femaleAthleteId = -1;
//     for(const auto& pair : settings.athletes.getAll()){
//         if(pair.second.getName() == "测试女运动员"){
//             femaleAthleteId = pair.first;
//             break;
//         }
//     }
//
//
//     // 2. 测试添加到不存在的单位 (应失败)
//     success = !settings.athletes.add("无效单位运动员", Gender::MALE, 99999);
//     printTestResult("添加运动员到无效单位ID 99999", success && settings.athletes.getAll().size() == initialAthleteCount + 2);
//
//     // 3. 获取存在的运动员
//     if (maleAthleteId != -1) {
//         auto athleteOpt = settings.athletes.get(maleAthleteId);
//         printTestResult("获取存在的男运动员", athleteOpt.has_value() && athleteOpt.value().get().getName() == "测试男运动员");
//     }
//
//     // 4. 测试删除运动员
//     if (maleAthleteId != -1) {
//         success = settings.athletes.remove(maleAthleteId);
//         printTestResult("删除男运动员", success && settings.athletes.getAll().size() == initialAthleteCount + 1);
//         const auto athleteOpt = settings.athletes.get(maleAthleteId);
//         printTestResult("确认男运动员已被删除", !athleteOpt.has_value());
//     }
//      if (femaleAthleteId != -1) {
//         success = settings.athletes.remove(femaleAthleteId);
//         printTestResult("删除女运动员", success && settings.athletes.getAll().size() == initialAthleteCount);
//         const auto athleteOpt = settings.athletes.get(femaleAthleteId);
//         printTestResult("确认女运动员已被删除", !athleteOpt.has_value());
//     }
//
//     std::cout << "--- 运动员管理测试结束 ---\n";
// }
//
// // 测试比赛项目管理
// void testCompetitionEventManagement(SystemSettings& settings) {
//     std::cout << "\n--- 开始测试比赛项目管理 ---" << std::endl;
//     const int initialEventCount = settings.events.getAllConst().size();
//
//     // 1. 添加新项目
//     bool success = settings.events.add("测试男子跳远", EventType::FIELD, Gender::MALE);
//     printTestResult("添加新项目 '测试男子跳远'", success && settings.events.getAllConst().size() == initialEventCount + 1);
//     int testEventId = -1;
//     for(const auto& pair : settings.events.getAllConst()){
//         if(pair.second.getName() == "测试男子跳远"){
//             testEventId = pair.first;
//             break;
//         }
//     }
//
//     // 2. 获取项目
//     if(testEventId != -1){
//         auto eventOpt = settings.events.get(testEventId);
//         printTestResult("获取存在的项目 '测试男子跳远'", eventOpt.has_value() && eventOpt.value().get().getName() == "测试男子跳远");
//     }
//
//     // 3. 删除项目
//     if (testEventId != -1) {
//         success = settings.removeCompetitionEvent(testEventId);
//         printTestResult("删除项目 '测试男子跳远'", success && settings.events.getAllConst().size() == initialEventCount);
//         auto eventOpt = settings.events.get(testEventId);
//         printTestResult("确认项目 '测试男子跳远' 已被删除", !eventOpt.has_value());
//     }
//     std::cout << "--- 比赛项目管理测试结束 ---\n";
// }
//
// // 测试报名管理
// void testRegistrationManagement(SystemSettings& settings, const Registration& registration) {
//     std::cout << "\n--- 开始测试报名管理 ---" << std::endl;
//
//     // 准备测试数据
//     settings.units.add("报名测试学院");
//     int testUnitId = -1;
//     for(const auto& u : settings.units.getAll()){ if(u.second.getName() == "报名测试学院") testUnitId = u.first; }
//     assert(testUnitId != -1 && "测试单位创建失败");
//
//
//     settings.athletes.add("报名选手男", Gender::MALE, testUnitId);
//     int maleAthleteId = -1;
//     for(const auto& a : settings.athletes.getAll()){ if(a.second.getName() == "报名选手男") maleAthleteId = a.first; }
//     assert(maleAthleteId != -1 && "测试男选手创建失败");
//
//     settings.athletes.add("报名选手女", Gender::FEMALE, testUnitId);
//     int femaleAthleteId = -1;
//     for(const auto& a : settings.athletes.getAll()){ if(a.second.getName() == "报名选手女") femaleAthleteId = a.first; }
//     assert(femaleAthleteId != -1 && "测试女选手创建失败");
//
//     settings.events.add("报名测试男项目", EventType::TRACK, Gender::MALE);
//     int maleEventId = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "报名测试男项目") maleEventId = e.first; }
//     assert(maleEventId != -1 && "测试男项目创建失败");
//
//     settings.events.add("报名测试女项目", EventType::TRACK, Gender::FEMALE);
//     int femaleEventId = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "报名测试女项目") femaleEventId = e.first; }
//     assert(femaleEventId != -1 && "测试女项目创建失败");
//
//     settings.events.add("报名测试不限性别项目", EventType::FIELD, Gender::UNSPECIFIED);
//     int unspecEventId = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "报名测试不限性别项目") unspecEventId = e.first; }
//     assert(unspecEventId != -1 && "测试不限性别项目创建失败");
//
//
//     // 1. 正常报名 - 男选手报男子项目
//     bool success = registration.registerAthleteForEvent(maleAthleteId, maleEventId);
//     printTestResult("男选手报名男子项目", success);
//     if(success){
//         const auto ath = settings.athletes.get(maleAthleteId);
//         const auto evt = settings.events.get(maleEventId);
//         printTestResult("男选手报名后项目数检查", ath.value().get().getRegisteredEventsCount() == 1);
//         printTestResult("男子项目报名人数检查", evt.value().get().getParticipantCount() == 1);
//     }
//
//
//     // 2. 性别不符报名 - 男选手报女子项目 (应失败)
//     success = !registration.registerAthleteForEvent(maleAthleteId, femaleEventId);
//     printTestResult("男选手报名女子项目 (应失败)", success);
//
//
//     // 3. 正常报名 - 女选手报女子项目
//     success = registration.registerAthleteForEvent(femaleAthleteId, femaleEventId);
//     printTestResult("女选手报名女子项目", success);
//
//     // 4. 正常报名 - 选手报不限性别项目
//     success = registration.registerAthleteForEvent(maleAthleteId, unspecEventId); // 男选手已报1项
//     printTestResult("男选手报名不限性别项目", success);
//
//
//     // 5. 重复报名 (应失败或被忽略)
//     success = !registration.registerAthleteForEvent(maleAthleteId, maleEventId); // maleAthleteId 已报名 maleEventId
//     printTestResult("男选手重复报名男子项目 (应失败/忽略)", success);
//
//
//     // 6. 报名超限额 (运动员最多报3项，maleAthleteId已报2项)
//     settings.events.add("额外测试男项目1", EventType::TRACK, Gender::MALE);
//     int extraMaleEventId1 = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "额外测试男项目1") extraMaleEventId1 = e.first; }
//
//     settings.events.add("额外测试男项目2", EventType::TRACK, Gender::MALE);
//     int extraMaleEventId2 = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "额外测试男项目2") extraMaleEventId2 = e.first; }
//
//     success = registration.registerAthleteForEvent(maleAthleteId, extraMaleEventId1); // 第3项
//     printTestResult("男选手报名第3个项目", success);
//
//     success = !registration.registerAthleteForEvent(maleAthleteId, extraMaleEventId2); // 尝试报第4项 (应失败)
//     printTestResult("男选手报名第4个项目 (应失败)", success);
//
//
//     // 7. 取消报名
//     success = registration.unregisterAthleteFromEvent(maleAthleteId, maleEventId);
//     printTestResult("男选手取消报名男子项目", success);
//     if(success){
//         auto ath = settings.athletes.get(maleAthleteId);
//         auto evt = settings.events.get(maleEventId); // maleEventId 此时应无参赛者
//         printTestResult("男选手取消报名后项目数检查", ath.value().get().getRegisteredEventsCount() == 2); // 之前报了3项，取消1项剩2项
//         printTestResult("男子项目取消报名后人数检查", evt.value().get().getParticipantCount() == 0);
//     }
//
//     // 8. 测试因人数不足取消项目
//     // 创建一个新项目，报名1人，然后取消报名，看项目是否被取消
//     settings.events.add("人数不足测试项目", EventType::TRACK, Gender::MALE);
//     int lowPartEventId = -1;
//     for(const auto& [eventId, competition] : settings.events.getAllConst()){ if(competition.getName() == "人数不足测试项目") lowPartEventId = eventId; }
//
//     settings.athletes.add("临时选手", Gender::MALE, testUnitId);
//     int tempAthleteId = -1;
//     for(const auto& a : settings.athletes.getAll()){ if(a.second.getName() == "临时选手") tempAthleteId = a.first; }
//
//     registration.registerAthleteForEvent(tempAthleteId, lowPartEventId); // 1人报名
//     auto lowEvent = settings.events.get(lowPartEventId);
//     printTestResult("项目有1人报名，未取消", lowEvent.has_value() && !lowEvent.value().get().getIsCancelled());
//
//     registration.unregisterAthleteFromEvent(tempAthleteId, lowPartEventId); // 0人报名
//     registration.checkAndCancelEventsDueToLowParticipation(); // 手动触发检查，或依赖unregister内部逻辑
//     lowEvent = settings.events.get(lowPartEventId);
//     printTestResult("项目无人报名后应被取消 (最少4人)", lowEvent.has_value() && lowEvent.value().get().getIsCancelled());
//
//
//     // 清理测试数据
//     settings.athletes.remove(maleAthleteId);
//     settings.athletes.remove(femaleAthleteId);
//     settings.athletes.remove(tempAthleteId);
//     settings.removeCompetitionEvent(maleEventId);
//     settings.removeCompetitionEvent(femaleEventId);
//     settings.removeCompetitionEvent(unspecEventId);
//     settings.removeCompetitionEvent(extraMaleEventId1);
//     settings.removeCompetitionEvent(extraMaleEventId2);
//     settings.removeCompetitionEvent(lowPartEventId);
//     settings.removeUnit(testUnitId);
//
//     std::cout << "--- 报名管理测试结束 ---\n";
// }
//
// // 测试成绩管理
// void testResultManagement(SystemSettings& settings) {
//     std::cout << "\n--- 开始测试成绩管理 ---" << std::endl;
//     bool success;
//
//     // 准备数据
//     settings.units.add("成绩测试学院");
//     int unitId = -1;
//     for(const auto& u : settings.units.getAll()){ if(u.second.getName() == "成绩测试学院") unitId = u.first; }
//
//     std::vector<int> athleteIds;
//     for (int i = 0; i < 7; ++i) { // 创建7个运动员
//         settings.athletes.add("选手" + std::to_string(i + 1), Gender::MALE, unitId);
//         for(const auto& a : settings.athletes.getAll()){
//             if(a.second.getName() == "选手" + std::to_string(i+1)){
//                  athleteIds.push_back(a.first);
//                  break;
//             }
//         }
//     }
//     assert(athleteIds.size() == 7 && "运动员创建不足7人");
//
//
//     settings.events.add("100米决赛", EventType::TRACK, Gender::MALE);
//     int eventId = -1;
//     for(const auto& e : settings.events.getAllConst()){ if(e.second.getName() == "100米决赛") eventId = e.first; }
//     assert(eventId != -1 && "比赛项目创建失败");
//
//
//     Registration reg(settings);
//     for (int athId : athleteIds) {
//         reg.registerAthleteForEvent(athId, eventId); // 全部报名
//     }
//     auto eventOpt = settings.events.get(eventId);
//     assert(eventOpt.has_value() && eventOpt.value().get().getParticipantCount() == 7 && "报名人数不为7");
//
//     // 1. 录入成绩 (7人参赛，应使用规则1：取前5名，7,5,3,2,1分)
//     EventResults results(eventId);
//     auto ruleOpt = settings.findAppropriateScoreRule(7);
//     assert(ruleOpt.has_value() && "未找到7人参赛的计分规则");
//     ScoreRule& rule = ruleOpt.value().get();
//
//     // 模拟录入前5名成绩
//     results.addResult(Result(eventId, athleteIds[0], 1, "10.5s", rule.getScoreForRank(1))); // 7分
//     results.addResult(Result(eventId, athleteIds[1], 2, "10.6s", rule.getScoreForRank(2))); // 5分
//     results.addResult(Result(eventId, athleteIds[2], 3, "10.7s", rule.getScoreForRank(3))); // 3分
//     results.addResult(Result(eventId, athleteIds[3], 4, "10.8s", rule.getScoreForRank(4))); // 2分
//     results.addResult(Result(eventId, athleteIds[4], 5, "10.9s", rule.getScoreForRank(5))); // 1分
//     results.finalizeResults();
//     success = settings.addOrUpdateEventResults(results);
//     printTestResult("录入7人参赛项目的成绩", success);
//
//     // 2. 检查单位总分
//     double expectedScore = 7.0 + 5.0 + 3.0 + 2.0 + 1.0; // 18分
//     auto unitOpt = settings.units.get(unitId);
//     // 注意：由于 addScore 是累加的，如果之前单位有分数，这里需要特殊处理。
//     // 假设 initializeDefaultSettings 后单位分数为0，或者我们只关注这个项目的增量。
//     // 为了简单，我们直接在录入成绩时更新单位分数，但SystemSettings::addOrUpdateEventResults内部并没有直接更新单位分。
//     // 这部分逻辑在 main.cpp 的 recordEventResults 中。
//     // 因此，这里的测试需要模拟 main.cpp 的逻辑或修改 SystemSettings。
//     // 为了测试的独立性，我们手动更新单位分数。
//     if(unitOpt.has_value()){
//         unitOpt.value().get().resetScore(); // 先重置
//         for(const auto& res : results.getResultsList()){
//             auto ath = settings.athletes.get(res.getAthleteId());
//             if(ath.has_value() && ath.value().get().getUnitId() == unitId){
//                 unitOpt.value().get().addScore(res.getPointsAwarded());
//             }
//         }
//     }
//
//     printTestResult("单位总分检查 (18分)", unitOpt.has_value() && unitOpt.value().get().getTotalScore() == expectedScore);
//
//
//     // 3. 查看项目成绩
//     auto storedResultsOpt = settings.getEventResultsConst(eventId);
//     printTestResult("查看已录入的项目成绩", storedResultsOpt.has_value() && storedResultsOpt.value().get().getResultsList().size() == 5);
//
//     // 4. 清除项目成绩
//     const double scoreBeforeClear = unitOpt.value().get().getTotalScore();
//     settings.clearResultsForEvent(eventId); // 这个函数会扣除单位分数
//     printTestResult("清除项目成绩后单位分数检查 (应为0)", unitOpt.has_value() && unitOpt.value().get().getTotalScore() == (scoreBeforeClear - expectedScore));
//     storedResultsOpt = settings.getEventResultsConst(eventId);
//     printTestResult("确认项目成绩已被清除", !storedResultsOpt.has_value() || storedResultsOpt.value().get().getResultsList().empty());
//
//
//     // 清理
//     for(int athId : athleteIds) settings.athletes.remove(athId);
//     settings.removeCompetitionEvent(eventId);
//     settings.removeUnit(unitId);
//
//     std::cout << "--- 成绩管理测试结束 ---\n";
// }
//
// // ================== SampleData 导入辅助函数 ==================
// // 直接操作SystemSettings，生成标准演示数据
// void importSampleData(SystemSettings& settings) {
//     // 1. 添加单位
//     std::vector<std::string> unitNames = {"计算机学院", "外国语学院", "体育学院"};
//     std::vector<int> unitIds;
//     for (const auto& name : unitNames) {
//         settings.units.add(name);
//         // 获取刚添加的单位ID
//         for (const auto& pair : settings.units.getAll()) {
//             if (pair.second.getName() == name) {
//                 unitIds.push_back(pair.first);
//                 break;
//             }
//         }
//     }
//
//     // 2. 添加运动员
//     settings.athletes.add("张三", Gender::MALE, unitIds[0]);
//     settings.athletes.add("李四", Gender::FEMALE, unitIds[1]);
//     settings.athletes.add("王五", Gender::MALE, unitIds[2]);
//     settings.athletes.add("赵六", Gender::FEMALE, unitIds[0]);
//     settings.athletes.add("孙七", Gender::MALE, unitIds[1]);
//     settings.athletes.add("周八", Gender::FEMALE, unitIds[2]);
//     std::vector<int> athleteIds;
//     for (const auto& pair : settings.athletes.getAll()) {
//         athleteIds.push_back(pair.first);
//     }
//
//     // 3. 添加比赛项目
//     settings.events.add("男子100米", EventType::TRACK, Gender::MALE);
//     settings.events.add("女子跳远", EventType::FIELD, Gender::FEMALE);
//     settings.events.add("男子铅球", EventType::FIELD, Gender::MALE);
//     settings.events.add("女子200米", EventType::TRACK, Gender::FEMALE);
//     std::vector<int> eventIds;
//     for (const auto& pair : settings.events.getAllConst()) {
//         eventIds.push_back(pair.first);
//     }
//
//     // 4. 报名关系
//     settings.athletes.registerForEvent(athleteIds[0], eventIds[0]); // 张三报男子100米
//     settings.athletes.registerForEvent(athleteIds[1], eventIds[1]); // 李四报女子跳远
//     settings.athletes.registerForEvent(athleteIds[2], eventIds[0]); // 王五报男子100米
//     settings.athletes.registerForEvent(athleteIds[2], eventIds[2]); // 王五报男子铅球
//     settings.athletes.registerForEvent(athleteIds[3], eventIds[3]); // 赵六报女子200米
//     settings.athletes.registerForEvent(athleteIds[5], eventIds[1]); // 周八报女子跳远
//     settings.athletes.registerForEvent(athleteIds[5], eventIds[3]); // 周八报女子200米
// }
//
// // ================== 默认计分规则测试 ==================
// void testDefaultScoreRules(SystemSettings& settings) {
//     std::cout << "\n--- 测试默认计分规则 ---" << std::endl;
//     // 检查规则1
//     auto rule1 = settings.findAppropriateScoreRule(7);
//     printTestResult("7人参赛应使用规则1", rule1.has_value() && rule1.value().get().getRanksToAward() == 5 && rule1.value().get().getScoreForRank(1) == 7.0);
//
//     // 检查规则2
//     auto rule2 = settings.findAppropriateScoreRule(5);
//     printTestResult("5人参赛应使用规则2", rule2.has_value() && rule2.value().get().getRanksToAward() == 3 && rule2.value().get().getScoreForRank(1) == 5.0);
//
//     // 检查项目取消逻辑（<4人）
//     auto ruleNone = settings.findAppropriateScoreRule(3);
//     printTestResult("3人参赛应无可用规则", !ruleNone.has_value());
// }
//
// int autoTest() {
//     std::cout << "========== 学校运动会管理系统 - 自动化测试 ==========" << std::endl;
//
//     SystemSettings settings; // 在 autoTest 开始时创建
//     DataManager dataManager(settings); // DataManager 需要 settings
//     Registration registration(settings); // Registration 也需要 settings
//
//     // --- 步骤 -1: 测试前彻底清理环境 ---
//     std::cout << "\n--- 步骤 -1: 测试前彻底清理环境 --- " << std::endl;
//     // 重置所有实体类的ID计数器，确保每次测试ID从1开始，与loadSampleData中的硬编码ID预期一致
//     Unit::resetNextId();
//     Athlete::resetNextId();
//     CompetitionEvent::resetNextId();
//     ScoreRule::resetNextId();
//     std::cout << "所有实体类的ID计数器已重置为1。" << std::endl;
//
//     // 清空 SystemSettings 中的所有数据容器
//     settings.clearUnits();
//     settings.clearAthletes();
//     settings.clearCompetitionEvents();
//     settings.clearScoreRules();       // 清除所有计分规则
//     settings.clearAllEventResults();  // 清除所有比赛结果和单位累计分数
//     std::cout << "SystemSettings中的所有现有数据已清除。" << std::endl;
//
//     // --- 步骤 0: 初始化系统默认核心设置 (计分规则和参数) ---
//     std::cout << "\n--- 步骤 0: 初始化系统默认核心设置 ---" << std::endl;
//     settings.initializeDefaultSettings(); // 此函数现在只设置核心规则和参数
//
//     // 检查核心设置是否成功初始化，例如默认计分规则ID为1的是否存在
//     bool coreSettingsOk = settings.getScoreRuleConst(1).has_value();
//     printTestResult("核心设置初始化 (默认计分规则ID=1存在)", coreSettingsOk);
//
//     if (!coreSettingsOk) {
//         std::cerr << "错误：核心设置初始化失败（例如，未找到ID为1的默认计分规则）。后续测试可能无法正确进行。" << std::endl;
//         //可以选择在这里返回，或者继续尝试其他不依赖此特定规则的测试
//         // return 1; // 如果认为这是关键失败点，则提前退出
//     }
//     std::cout << "当前计分规则数量: " << settings.getAllScoreRules().size() << std::endl;
//     std::cout << "运动员最大报名项目数: " << settings.athletes.getMaxEventsAllowed() << std::endl;
//     std::cout << "项目最少举行人数: " << settings.getMinParticipantsToHoldEvent() << std::endl;
//     // 此时，单位、运动员、项目列表应该是空的，由后续步骤或 loadSampleData 填充
//     std::cout << "初始化后单位数量: " << settings.units.getAll().size() << " (预期为0)" << std::endl;
//     std::cout << "初始化后运动员数量: " << settings.athletes.getAll().size() << " (预期为0)" << std::endl;
//     std::cout << "初始化后比赛项目数量: " << settings.events.getAllConst().size() << " (预期为0)" << std::endl;
//
//     // --- 步骤 0.1: 导入标准SampleData ---
//     std::cout << "\n--- 步骤 0.1: 导入标准SampleData --- " << std::endl;
//     importSampleData(settings);
//     printTestResult("导入标准SampleData", !settings.units.getAll().empty() && !settings.athletes.getAll().empty() && !settings.events.getAllConst().empty());
//     std::cout << "SampleData导入后单位数量: " << settings.units.getAll().size() << std::endl;
//     std::cout << "SampleData导入后运动员数量: " << settings.athletes.getAll().size() << std::endl;
//     std::cout << "SampleData导入后比赛项目数量: " << settings.events.getAllConst().size() << std::endl;
//
//     // --- 步骤 0.2: 测试默认计分规则 ---
//     testDefaultScoreRules(settings);
//
//     // 执行各功能模块的测试 (这些测试现在将基于 loadSampleData 加载的数据运行)
//     std::cout << "\n--- 开始执行各模块具体测试 --- " << std::endl;
//     testUnitManagement(settings);
//     testAthleteManagement(settings);
//     testCompetitionEventManagement(settings);
//     testRegistrationManagement(settings, registration); // Registration 测试需要 Registration 对象
//     testResultManagement(settings);
//
//     // TODO: 添加计分规则管理测试
//     // TODO: 添加系统参数设置测试
//     // TODO: 添加秩序册生成测试 (目前是占位符)
//     // TODO: 添加数据备份与恢复测试 (目前是占位符)
//
//
//     std::cout << "\n========== 自动化测试全部完成 ==========" << std::endl;
//
//     // --- 步骤 N: 测试后彻底清理环境 ---
//     std::cout << "\n--- 步骤 N: 测试后彻底清理环境 --- " << std::endl;
//     // 再次重置ID计数器和清空数据，确保不影响下一次可能的运行或手动操作
//     Unit::resetNextId();
//     Athlete::resetNextId();
//     CompetitionEvent::resetNextId();
//     ScoreRule::resetNextId();
//     settings.clearUnits();
//     settings.clearAthletes();
//     settings.clearCompetitionEvents();
//     settings.clearScoreRules();
//     settings.clearAllEventResults();
//     std::cout << "测试后环境清理完成。" << std::endl;
//
//     // 提示用户按键退出，以便查看控制台输出
//     std::cout << "\n按 Enter 键退出测试程序...";
//     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清除之前可能的输入残留
//     std::cin.get();
//
//     return 0;
// }

int autoTest()
{
    return 0;
}