#include "include/Components/SystemSettings.h"
#include "include/Components/Registration.h"
#include "include/Components/Schedule.h"
#include "include/Components/DataManager.h"
#include "include/Components/Result.h"
#include "include/Components/AutoTest.h"
#include "include/Components/Unit.h" // For viewUnitStandingsOverall
#include "include/Components/CompetitionEvent.h"
#include "include/Components/Workflow.h" // 确保 Workflow.h 被包含

#include "include/UI/UIManager.h"
#include "include/Controller/SystemSettingsController.h"
#include "include/Controller/RegistrationController.h"
#include "include/Controller/ScheduleController.h"
#include "include/Controller/ResultsController.h"
#include "include/Controller/QueryController.h"
#include "include/Controller/DataManagementController.h"

#include <vector>
#include <algorithm> // for std::sort for unit standings
#include <functional> // for std::reference_wrapper
#include <ranges>    // C++20 ranges

// 假设 AutoTest 类有一个静态的 runAllTests 方法，或者 autoTest() 是一个全局函数
// 如果 AutoTest.h 定义了全局 autoTest()，则取消下面的注释
// void autoTest(); // 声明，如果 autoTest 在别处定义

// 查看已发布项目 (旧主菜单选项2的逻辑)
void viewPublishedEvents(const SystemSettings& settings) {
    UIManager::showMessage("\n--- 当前已发布的比赛项目 ---");
    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    bool found = false;
    // 使用 getAllCompetitionEventsConst() 返回的是 std::map<int, utils::RefConst<CompetitionEvent>>
    // 需要从 map 的值中提取 CompetitionEvent
    for (const auto& pair : settings.getAllCompetitionEventsConst()) {
        const CompetitionEvent& event = pair.second.get(); // 获取 CompetitionEvent 的 const 引用
        if (!event.getIsCancelled()) {
             events_refs.push_back(std::cref(event)); // 传递 const CompetitionEvent&
             found = true;
        }
    }
    if (!found){
        UIManager::showMessage("当前没有已发布的比赛项目，或所有项目均已被取消。");
    } else {
        UIManager::displayEvents(events_refs, settings);
    }
}

// 查看单位总分排名 (旧主菜单选项7的逻辑)
void viewUnitStandingsOverall(const SystemSettings& settings) {
    UIManager::showMessage("\n--- 总成绩统计 (单位排名) ---");
    const auto& allUnitsMap = settings.getAllUnits(); // 返回 const std::map<int, Unit>&
    if (allUnitsMap.empty()) {
        UIManager::showMessage("系统中没有单位信息。");
        return;
    }

    std::vector<utils::RefConst<Unit>> unitsVec;
    for (const auto& pair : allUnitsMap) {
        unitsVec.push_back(std::cref(pair.second)); // pair.second is Unit
    }

    // 使用 std::sort 代替 std::ranges::sort
    std::sort(unitsVec.begin(), unitsVec.end(), [](const utils::RefConst<Unit>& a_ref, const utils::RefConst<Unit>& b_ref) {
        const Unit& a = a_ref.get();
        const Unit& b = b_ref.get();
        if (std::abs(a.getTotalScore() - b.getTotalScore()) > 1e-5) { // 比较 double
            return a.getTotalScore() > b.getTotalScore();
        }
        return a.getName() < b.getName(); // 分数相同则按名称排序
    });

    UIManager::displayUnitStandings(unitsVec);
}

// 结束报名流程函数
void endRegistrationProcess(Registration& registration, Schedule& schedule, SystemSettings& settings) {
    UIManager::showMessage("正在结束报名流程...");
    
    // 检查并取消人数不足的项目
    int cancelledEvents = registration.checkAndCancelEventsDueToLowParticipation();
    if (cancelledEvents > 0) {
        UIManager::showMessage("已取消 " + std::to_string(cancelledEvents) + " 个报名人数不足的项目。");
    }
    
    // 自动生成秩序册
    UIManager::showMessage("正在生成最终赛程安排...");
    bool scheduleGenerated = schedule.generateSchedule();
    if (scheduleGenerated) {
        UIManager::showSuccessMessage("秩序册已成功生成！");
        // 显示生成的秩序册
        std::string scheduleContent = schedule.getScheduleContentAsString();
        UIManager::showMessage(scheduleContent);
        
        // 进入下一阶段
        settings.setWorkflowStage(WorkflowStage::COMPETITION_RUNNING);
        UIManager::showSuccessMessage("报名已结束。工作流程已进入比赛管理阶段。");
    } else {
        UIManager::showErrorMessage("秩序册生成失败，请检查项目和场地设置。");
    }
}

// 声明 AutoTest::runAllTests 如果它是静态成员函数
// namespace AutoTest { void runAllTests(SystemSettings&, Schedule&, Registration&, DataManager&); }


int main() {
    SystemSettings settings;
    settings.initializeDefaultSettings(); // 确保此方法设置初始工作流程阶段为 SETUP_EVENTS

    Registration registration(settings);
    Schedule schedule(settings); 
    DataManager dataManager(settings); 

    SystemSettingsController systemSettingsCtrl(settings);
    RegistrationController registrationCtrl(registration, settings);
    ScheduleController scheduleCtrl(schedule, settings);
    ResultsController resultsCtrl(settings);
    // QueryController queryCtrl(settings); // QueryController 似乎在阶段化菜单中没有直接对应项，按需保留
    DataManagementController dataManagementCtrl(dataManager);
    
    // 模拟 AutoTest 的调用，如果 autoTest() 是一个全局函数
    // 如果 AutoTest 是一个类，可能需要实例化或调用静态方法
    // AutoTest autoTester(settings, schedule, registration, dataManager); // 示例

    int choice;
    do {
        UIManager::displayMainMenu(settings); // 显示基于当前阶段的菜单
        WorkflowStage currentStage = settings.getCurrentWorkflowStage();
        
        // 输入提示和范围可以根据当前菜单动态调整，但为简化，使用一个通用范围
        int maxChoice = 10; // 默认值
        
        // 根据当前阶段设置最大选择范围
        switch(currentStage) {
            case WorkflowStage::SETUP_EVENTS: maxChoice = 10; break; // 确保可以选择1-4和9-10选项
            case WorkflowStage::REGISTRATION_OPEN: maxChoice = 10; break; // 包括通用选项9和10
            case WorkflowStage::COMPETITION_RUNNING: maxChoice = 10; break; // 包括通用选项9和10
        }
        
        choice = UIManager::getIntInput("请输入您的选项: ", 0, maxChoice);
                                     // 假设通用选项是9, 10, 0，阶段选项从1开始

        bool choiceHandled = false;

        // 首先处理所有阶段通用的选项
        if (choice == 0) {
            UIManager::showMessage("感谢使用学校运动会管理系统，正在退出...");
            choiceHandled = true;
        } else if (choice == 9) {
            dataManagementCtrl.handleLoadSampleData(); // 重定向到DataManagementController的分阶段导入
            UIManager::pressEnterToContinue();
            choiceHandled = true;
        } else if (choice == 10) {
            // autoTester.runAllTests(); // 例如调用 AutoTest 实例的方法
            UIManager::showMessage("自动测试执行完毕。(占位符)"); // 替换为实际的自动测试调用
            UIManager::pressEnterToContinue();
            choiceHandled = true;
        }

        if (!choiceHandled) { // 如果不是通用选项，则根据当前阶段处理
            switch (currentStage) {
                case WorkflowStage::SETUP_EVENTS:
                    switch (choice) {
                        case 1: // 系统设置与项目管理
                            systemSettingsCtrl.manage(); 
                            choiceHandled = true; break;
                        case 2: // 场地管理
                            systemSettingsCtrl.handleVenueManagement(); // 该方法内部已有赛程锁定检查
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        case 3: // 上下午时间段设置
                            systemSettingsCtrl.handleSessionSettings(); // 该方法内部已有赛程锁定检查
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        case 4: // 完成项目设置并锁定赛程
                            scheduleCtrl.handleLockSchedule(); // 成功则内部会 settings.lockSchedule() 并改阶段
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        default: break; 
                    }
                    break;

                case WorkflowStage::REGISTRATION_OPEN:
                    switch (choice) {
                        case 1: // 运动员报名与管理
                            registrationCtrl.manage(); 
                            choiceHandled = true; break;
                        case 2: // 查看所有比赛项目 (已锁定)
                            viewPublishedEvents(settings); 
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        case 3: // 查看当前赛程安排
                            UIManager::showMessage(schedule.getScheduleContentAsString());
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        case 4: // 结束报名并确认最终赛程安排
                            endRegistrationProcess(registration, schedule, settings);
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        // 可选的返回上一阶段（解锁）逻辑，目前未实现
                        // case 5: 
                        //     scheduleCtrl.handleUnlockSchedule();
                        //     UIManager::pressEnterToContinue();
                        //     choiceHandled = true; break;
                        default: break;
                    }
                    break;

                case WorkflowStage::COMPETITION_RUNNING:
                    switch (choice) {
                        case 1: // 系统设置与查询
                            systemSettingsCtrl.manage();
                            choiceHandled = true; break;
                        case 2: // 秩序册管理 (查看/验证)
                            scheduleCtrl.manage();
                            choiceHandled = true; break;
                        case 3: // 比赛成绩管理
                            resultsCtrl.manage(); 
                            choiceHandled = true; break;
                        case 4: // 查看单位总分排名
                            viewUnitStandingsOverall(settings); 
                            UIManager::pressEnterToContinue();
                            choiceHandled = true; break;
                        case 5: // 数据备份与恢复
                            dataManagementCtrl.manage(); 
                            choiceHandled = true; break;
                        default: break;
                    }
                    break;
            }
        }

        if (!choiceHandled && choice != 0) { // 如果选项未被任何逻辑处理且不是退出选项
            UIManager::showErrorMessage("无效选项，请根据当前阶段菜单重新输入。");
            UIManager::pressEnterToContinue();
        }

    } while (choice != 0);

    return 0;
}
