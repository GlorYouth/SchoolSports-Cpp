//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/Controller/ScheduleController.h"
#include "../../include/UI/UIManager.h"
ScheduleController::ScheduleController(Schedule& schedule, SystemSettings& settings)
    : schedule_(schedule), settings_(settings) {}

void ScheduleController::manage(const SystemSettings& settings) {
    int choice;
    do {
        UIManager::displayScheduleMenu(settings);
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 3);

        switch (choice) {
            case 1: handleGenerateSchedule(); break;
            case 2: handleViewSchedule(); break;
            case 3: handleValidateSchedule(); break;
            case 0: UIManager::showMessage("返回主菜单..."); break;
            default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void ScheduleController::handleGenerateSchedule() {
    // Schedule::generateSchedule 可能需要 SystemSettings 来获取最终确认的项目和运动员信息
    // 假设 Schedule 类构造时或 generateSchedule 时传入了 settings_ 的引用
    if (schedule_.generateSchedule()) { // 假设 generateSchedule 返回 bool
        UIManager::showSuccessMessage("秩序册已成功生成/更新。");
    } else {
        UIManager::showErrorMessage("秩序册生成失败 (可能没有有效项目、未取消人数不足项目或运动员数据不完整)。");
    }
}

void ScheduleController::handleViewSchedule() {
    // Schedule::printSchedule 应该不再直接打印到控制台，
    // 而是返回一个可供 UIManager 显示的字符串或数据结构。
    // 或者，UIManager 有一个 displaySchedule(const Schedule& schedule) 方法。
    // 为了简单起见，我们暂时假设 printSchedule 仍然打印，但这不符合完全分离的原则。
    // 理想情况:
    UIManager::showMessage("\n--- 查看秩序册 ---");
    std::string scheduleContent = schedule_.getScheduleContentAsString(); // 从 Schedule 获取内容
    UIManager::showMessage(scheduleContent);                             // UIManager 负责显示
    // 或:
    // UIManager::displaySchedule(schedule_);

    // 沿用原代码的模式，Schedule::printSchedule 内部会打印
    // schedule_.printSchedule(); // 假设这个方法现在使用 UIManager 或返回数据
}

void ScheduleController::handleValidateSchedule() {
    // 类似地，validateSchedule 应该返回状态
    if (schedule_.validateSchedule()) { // 假设返回 bool
        UIManager::showSuccessMessage("赛程验证通过 (具体验证逻辑待实现)。");
    } else {
        UIManager::showErrorMessage("赛程验证失败 (具体验证逻辑待实现)。");
    }
}
