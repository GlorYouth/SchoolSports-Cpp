//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/Controller/DataManagementController.h"
#include "../../include/UI/UIManager.h"

DataManagementController::DataManagementController(DataManager& dataManager)
    : dataManager_(dataManager) {}

void DataManagementController::manage() {
    int choice;
    do {
        UIManager::displayDataManagementMenu();
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);

        switch (choice) {
        case 1: handleBackupData(); break;
        case 2: handleRestoreData(); break;
        case 0: UIManager::showMessage("返回主菜单..."); break;
        default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void DataManagementController::handleBackupData() {
    std::string filePath = UIManager::getStringInput("请输入备份文件名 (例如: backup.dat): ");
    if (dataManager_.backupData(filePath)) { // 假设 backupData 返回 bool
        UIManager::showSuccessMessage("数据已成功备份到 " + filePath);
    } else {
        UIManager::showErrorMessage("数据备份失败。");
    }
}

void DataManagementController::handleRestoreData() {
    std::string filePath = UIManager::getStringInput("请输入要恢复数据的文件名: ");
    if (dataManager_.restoreData(filePath)) { // 假设 restoreData 返回 bool
        UIManager::showSuccessMessage("数据已成功从 " + filePath + " 恢复。");
        UIManager::showMessage("注意：恢复后，请检查所有数据是否正确，特别是单位总分。");
        UIManager::showMessage("如果系统支持，可能需要重新计算单位总分或通过其他方式验证。");
    } else {
        UIManager::showErrorMessage("数据恢复失败或功能未完全实现。");
    }
}
