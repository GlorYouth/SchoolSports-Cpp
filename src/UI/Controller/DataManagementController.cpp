
#include "../../../include/UI/Controller/DataManagementController.h"
#include "../../../include/UI/UIManager.h"
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

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
    // 确保备份目录存在
    if (!dataManager_.ensureBackupDirectoryExists()) {
        UIManager::showErrorMessage("无法创建备份目录，操作已取消。");
        return;
    }
    
    // 允许用户输入备份文件名，为空则使用默认（带时间戳）的文件名
    std::string filePath = UIManager::getStringInput("请输入备份文件名 (留空则使用时间戳生成名称): ");
    
    if (dataManager_.backupData(filePath)) {
        UIManager::showSuccessMessage("数据已成功备份！");
    } else {
        UIManager::showErrorMessage("数据备份失败。");
    }
}

void DataManagementController::displayBackupFiles() const {
    auto backupFiles = dataManager_.getBackupFiles();
    
    if (backupFiles.empty()) {
        UIManager::showInfoMessage("没有找到备份文件。");
        return;
    }
    
    UIManager::showTitleMessage("可用备份文件：");
    UIManager::showMessage("ID | 备份文件");
    UIManager::showMessage("---------------------------");
    
    for (const auto& [id, path] : backupFiles) {
        // 获取文件名
        fs::path filePath(path);
        
        // 显示文件信息（简化版，不显示时间）
        UIManager::showMessage(std::to_string(id) + " | " + filePath.filename().string());
    }
    UIManager::showMessage("---------------------------");
    UIManager::showMessage("备份文件已按最新时间排序，ID 1为最新备份");
}

void DataManagementController::handleRestoreData() {
    // 显示备份文件列表
    displayBackupFiles();
    
    // 提示用户输入ID或文件路径
    UIManager::showMessage("请输入要恢复的备份ID或完整文件路径：");
    UIManager::showMessage("(输入数字将按ID恢复，输入文件路径将直接使用该路径)");
    std::string input = UIManager::getStringInput("请输入: ");
    
    if (input.empty()) {
        UIManager::showErrorMessage("未提供备份ID或路径，操作已取消。");
        return;
    }
    
    // 确认操作
    UIManager::showWarningMessage("警告：恢复操作将替换当前系统的所有数据！");
    std::string confirm = UIManager::getStringInput("确定要继续吗？(y/n): ");
    if (confirm != "y" && confirm != "Y") {
        UIManager::showMessage("操作已取消。");
        return;
    }
    
    // 执行恢复
    if (dataManager_.restoreData(input)) {
        UIManager::showSuccessMessage("数据已成功恢复！");
        UIManager::showMessage("注意：恢复数据后，请检查数据是否正确，特别是单位总分。");
    } else {
        UIManager::showErrorMessage("数据恢复失败。请检查ID或文件路径是否正确。");
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

