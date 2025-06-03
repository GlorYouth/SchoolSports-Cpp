#include "include/Components/SystemSettings.h"
#include "include/Components/Registration.h"
#include "include/Components/Schedule.h"
#include "include/Components/DataManager.h"
#include "include/Components/Result.h"
#include "include/Components/AutoTest.h"
#include "include/Components/Unit.h" // For viewUnitStandingsOverall
#include "include/Components/CompetitionEvent.h"

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
#include <ranges>
// #include <ranges> // for std::views::values - 如果需要但控制器内部处理了迭代

// --- 原先的辅助函数，现在部分功能已移入控制器或保持为顶层菜单项 ---

// 查看已发布项目 (原主菜单选项2) - 这个保持在 main 中，因为它是一个直接的查看操作
void viewPublishedEvents(const SystemSettings& settings) {
    UIManager::showMessage("\n--- 当前发布的比赛项目 ---");
    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    bool found = false;
    for (const auto& val : settings.getAllCompetitionEvents() | std::views::values) {
        if (!val.getIsCancelled()) {
             events_refs.push_back(std::cref(val));
             found = true;
        }
    }
    if (!found){
        UIManager::showMessage("当前没有已发布的比赛项目，或所有项目均已被取消。");
    } else {
        UIManager::displayEvents(events_refs, settings);
    }
}

// 查看单位排名 (原主菜单选项7) - 这个也保持在 main 中
void viewUnitStandingsOverall(const SystemSettings& settings) {
    UIManager::showMessage("\n--- 总成绩统计 (单位排名) ---");
    const auto& allUnitsMap = settings.getAllUnits();
    if (allUnitsMap.empty()) {
        UIManager::showMessage("系统中没有单位信息。");
        return;
    }

    std::vector<utils::RefConst<Unit>> unitsVec;
    for (const auto& val : allUnitsMap | std::views::values) {
        unitsVec.push_back(std::cref(val));
    }

    std::ranges::sort(unitsVec, [](const Unit& a, const Unit& b) {
        if (std::abs(a.getTotalScore() - b.getTotalScore()) > 1e-5) { // 比较 double
            return a.getTotalScore() > b.getTotalScore();
        }
        return a.getName() < b.getName();
    });

    UIManager::displayUnitStandings(unitsVec);
}


int main() {
    SystemSettings settings;
    settings.initializeDefaultSettings();

    Registration registration(settings);
    Schedule schedule(settings); // Schedule 可能也需要 settings
    DataManager dataManager(settings); // DataManager 可能也需要 settings

    // 创建控制器
    SystemSettingsController systemSettingsCtrl(settings);
    RegistrationController registrationCtrl(registration, settings);
    ScheduleController scheduleCtrl(schedule, settings);
    ResultsController resultsCtrl(settings);
    QueryController queryCtrl(settings);
    DataManagementController dataManagementCtrl(dataManager);


    int choice;
    do {
        UIManager::displayMainMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 10);

        switch (choice) {
            case 1:
                systemSettingsCtrl.manage();
                break;
            case 2:
                viewPublishedEvents(settings); // 直接调用
                UIManager::pressEnterToContinue();
                break;
            case 3:
                registrationCtrl.manage();
                break;
            case 4:
                queryCtrl.manage();
                break;
            case 5:
                scheduleCtrl.manage();
                break;
            case 6:
                resultsCtrl.manage();
                break;
            case 7:
                viewUnitStandingsOverall(settings); // 直接调用
                UIManager::pressEnterToContinue();
                break;
            case 8:
                dataManagementCtrl.manage();
                break;
            case 9:
                dataManager.loadSampleData();
                UIManager::showMessage("示例数据导入完成。");
                UIManager::pressEnterToContinue();
                break;
            case 10:
                autoTest();
                UIManager::showMessage("自动测试执行完毕。");
                UIManager::pressEnterToContinue();
                break;
            case 0:
                UIManager::showMessage("感谢使用学校运动会管理系统，正在退出...");
                break;
            default:
                // UIManager::getIntInput 已经处理了无效输入范围
                // 此 default 分支理论上不应轻易触发，除非 getIntInput 逻辑有变
                UIManager::showErrorMessage("无效选择，请重新输入。");
                UIManager::pressEnterToContinue();
                break;
        }
    } while (choice != 0);

    return 0;
}
