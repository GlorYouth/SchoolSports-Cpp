//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/Controller/QueryController.h"
#include "../../include/UI/UIManager.h"
#include "../../include/Components/Unit.h"
#include "../../include/Components/CompetitionEvent.h"
#include <vector>
#include <functional>

QueryController::QueryController(const SystemSettings& settings)
    : settings_(settings) {}

void QueryController::manage() {
    int choice;
    do {
        UIManager::displayQueryMenu(); // 显示查询子菜单
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);

        switch (choice) {
        case 1: handleViewUnits(); break;
        case 2: handleViewEvents(); break;
        case 0: UIManager::showMessage("返回主菜单..."); break;
        default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            // pressEnterToContinue 通常在显示列表后由 UIManager 调用，或在此处调用
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void QueryController::handleViewUnits() {
    UIManager::showMessage("\n--- 参赛单位信息 ---");
    const auto& allUnitsMap = settings_.units.getAll();
    if (allUnitsMap.empty()) {
        UIManager::showMessage("暂无参赛单位。");
        return;
    }
    std::vector<utils::RefConst<Unit>> units_refs;
    for(const auto& pair : allUnitsMap) units_refs.push_back(std::cref(pair.second));
    UIManager::displayUnits(units_refs);
}

void QueryController::handleViewEvents() {
    UIManager::showMessage("\n--- 比赛项目信息 ---");
    const auto& allEventsMap = settings_.events.getAllConst();
    if (allEventsMap.empty()) {
        UIManager::showMessage("暂无比赛项目。");
        return;
    }
    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    for(const auto& pair : allEventsMap) events_refs.push_back(std::cref(pair.second));
    UIManager::displayEvents(events_refs, settings_);
}
