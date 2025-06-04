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
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 3);

        switch (choice) {
        case 1: handleBackupData(); break;
        case 2: handleRestoreData(); break;
        case 3: handleLoadSampleData(); break;
        case 0: UIManager::showMessage("返回主菜单..."); break;
        default: UIManager::showErrorMessage("无效选择"); break;
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
        UIManager::showMessage("注意：恢复数据后，请检查数据是否正确，特别是单位总分。");
        UIManager::showMessage("若系统支持，您可能需要重新计算单位分或通过其他方式验证。");
    } else {
        UIManager::showErrorMessage("数据恢复失败或尚未完全实现。");
    }
}

// 处理导入示例数据的总入口
void DataManagementController::handleLoadSampleData() {
    displayImportMenu();
    
    int choice = UIManager::getIntInput("请输入您的选择: ", 0, 3);
    
    switch (choice) {
        case 1:
            handleLoadSampleStage1Data();
            break;
        case 2:
            handleLoadSampleStage2Data();
            break;
        case 3:
            handleLoadSampleStage3Data();
            break;
        case 0:
            UIManager::showMessage("返回上级菜单...");
            break;
        default:
            UIManager::showErrorMessage("无效选择");
            break;
    }
}

// 显示分阶段导入菜单，根据当前状态显示可用选项
void DataManagementController::displayImportMenu() {
    UIManager::showMessage("\n--- 导入示例数据 ---");
    UIManager::showMessage("1. 导入阶段1数据：单位、场地和项目");
    
    // 只有阶段1数据已导入，才显示阶段2选项
    if (dataManager_.isStage1DataImported()) {
        UIManager::showMessage("2. 导入阶段2数据：运动员及其报名信息");
        
        // 只有阶段2数据已导入，才显示阶段3选项
        if (dataManager_.isStage2DataImported()) {
            UIManager::showMessage("3. 导入阶段3数据：比赛成绩");
        }
    }
    
    UIManager::showMessage("0. 返回上一级菜单");
}

// 处理导入阶段1示例数据
void DataManagementController::handleLoadSampleStage1Data() {
    UIManager::showMessage("正在导入阶段1数据（单位、场地和项目）...");
    
    if (dataManager_.loadSampleStage1Data()) {
        UIManager::showSuccessMessage("阶段1示例数据导入成功！");
        UIManager::showMessage("已导入单位、场地和比赛项目。");
    } else {
        UIManager::showErrorMessage("阶段1示例数据导入失败。");
    }
}

// 处理导入阶段2示例数据
void DataManagementController::handleLoadSampleStage2Data() {
    // 检查是否满足前置条件
    if (!dataManager_.isStage1DataImported()) {
        UIManager::showErrorMessage("请先导入阶段1数据（单位、场地和项目）。");
        return;
    }
    
    UIManager::showMessage("正在导入阶段2数据（运动员及其报名信息）...");
    
    if (dataManager_.loadSampleStage2Data()) {
        UIManager::showSuccessMessage("阶段2示例数据导入成功！");
        UIManager::showMessage("已导入运动员及其报名信息。");
    } else {
        UIManager::showErrorMessage("阶段2示例数据导入失败。");
    }
}

// 处理导入阶段3示例数据
void DataManagementController::handleLoadSampleStage3Data() {
    // 检查是否满足前置条件
    if (!dataManager_.isStage2DataImported()) {
        UIManager::showErrorMessage("请先导入阶段2数据（运动员及其报名信息）。");
        return;
    }
    
    UIManager::showMessage("正在导入阶段3数据（比赛成绩）...");
    
    if (dataManager_.loadSampleStage3Data()) {
        UIManager::showSuccessMessage("阶段3示例数据导入成功！");
        UIManager::showMessage("已导入所有项目成绩并计算单位总分。");
        UIManager::showMessage("可以通过\"查看单位总分排名\"菜单查看排名情况。");
    } else {
        UIManager::showErrorMessage("阶段3示例数据导入失败。");
    }
}

