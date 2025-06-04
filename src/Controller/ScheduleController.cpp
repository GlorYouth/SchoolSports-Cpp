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
        UIManager::displayScheduleMenu(settings_);
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 5);

        switch (choice) {
            case 1: handleGenerateSchedule(); break;
            case 2: handleViewSchedule(); break;
            case 3: handleValidateSchedule(); break;
            case 4: handleLockSchedule(); break;
            case 5: handleUnlockSchedule(); break;
            case 0: UIManager::showMessage("返回主菜单..."); break;
            default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void ScheduleController::handleGenerateSchedule() {
    if (settings_.isScheduleLocked()) {
        UIManager::showErrorMessage("赛程已锁定，无法重新生成。如需操作，请先解锁赛程。");
        return;
    }
    // Schedule::generateSchedule 本身需要 SystemSettings 来获取正确配置的项目和场地信息
    // 在 Schedule 类构造或 generateSchedule 时传入 settings_ 成员即可
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

void ScheduleController::handleLockSchedule() {
    if (settings_.isScheduleLocked()) {
        UIManager::showMessage("赛程当前已是锁定状态。");
        return;
    }

    // 前置检查：所有未取消的项目是否都有持续时间和场地
    bool allEventsReady = true;
    for (const auto& pair : settings_.getAllCompetitionEventsConst()) {
        const CompetitionEvent& event = pair.second.get();
        if (!event.getIsCancelled()) {
            if (event.getDurationMinutes() <= 0) {
                UIManager::showErrorMessage("项目 \"" + event.getName() + "\" (ID: " + std::to_string(event.getId()) + ") 缺少有效的持续时间。无法锁定赛程。");
                allEventsReady = false;
                break;
            }
            if (event.getVenue().empty()) {
                UIManager::showErrorMessage("项目 \"" + event.getName() + "\" (ID: " + std::to_string(event.getId()) + ") 缺少场地信息。无法锁定赛程。");
                allEventsReady = false;
                break;
            }
        }
    }

    if (!allEventsReady) {
        UIManager::showMessage("请先为所有未取消的项目设置有效的持续时间和场地信息后再锁定赛程。");
        return;
    }

    settings_.lockSchedule();
    UIManager::showSuccessMessage("赛程已成功锁定。");
}

void ScheduleController::handleUnlockSchedule() {
    if (!settings_.isScheduleLocked()) {
        UIManager::showMessage("赛程当前并未锁定。");
        return;
    }
    settings_.unlockSchedule();
    UIManager::showSuccessMessage("赛程已成功解锁。");
}

// 添加新方法，用于主菜单中的赛程生成与查看选项
void ScheduleController::handleScheduleGeneration() {
    UIManager::showMessage("\n--- 赛程生成与查看 ---");
    int choice;
    do {
        UIManager::displayScheduleGenerationMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);
        switch (choice) {
            case 1:
                handleGenerateSchedule();
                break;
            case 2:
                handleViewSchedule();
                break;
            case 0:
                UIManager::showMessage("返回主菜单...");
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
