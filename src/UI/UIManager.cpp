//
// Created by GlorYouth on 2025/6/3.
//

#include "../../include/UI/UIManager.h"
#include "../../include/Components/Unit.h"
#include "../../include/Components/Athlete.h"
#include "../../include/Components/CompetitionEvent.h"
#include "../../include/Components/ScoreRule.h"
#include "../../include/Components/Result.h"
#include "../../include/Components/SystemSettings.h"

// --- 基本输入输出工具 ---
void UIManager::clearInputBuffer() {
    std::cin.clear(); // 清除错误标志
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略缓冲区中的无效输入
}

int UIManager::getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputBuffer(); // 清理换行符
            return value;
        } else {
            showErrorMessage("无效输入，请输入一个数字。");
            clearInputBuffer();
        }
    }
}

int UIManager::getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputBuffer();
            if (value >= minVal && value <= maxVal) {
                return value;
            } else {
                showErrorMessage("输入超出范围 (" + std::to_string(minVal) + "-" + std::to_string(maxVal) + ")，请重新输入。");
            }
        } else {
            showErrorMessage("无效输入，请输入一个数字。");
            clearInputBuffer();
        }
    }
}

std::string UIManager::getStringInput(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value); // 使用 getline 读取整行，允许空格
    return value;
}

void UIManager::pressEnterToContinue(const std::string& message) {
    std::cout << message;
    // std::cin.get(); // 如果之前是 >> 操作，这个可能直接消耗掉换行符
    // 需要确保缓冲区是干净的，或者用一个明确的 getline
    std::string dummy;
    std::getline(std::cin, dummy);
}

void UIManager::showMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void UIManager::showSuccessMessage(const std::string& message) {
    std::cout << "[成功] " << message << std::endl;
}

void UIManager::showErrorMessage(const std::string& message) {
    std::cout << "[错误] " << message << std::endl;
}

// 添加更多结构化的消息类型
void UIManager::showWarningMessage(const std::string& message) {
    std::cout << "[警告] " << message << std::endl;
}

void UIManager::showInfoMessage(const std::string& message) {
    std::cout << "[信息] " << message << std::endl;
}

void UIManager::showTitleMessage(const std::string& title) {
    std::cout << "\n--- " << title << " ---" << std::endl;
}

// 用于状态信息的格式化函数
void UIManager::showStatusMessage(const std::string& status, const std::string& message) {
    std::cout << status << ": " << message << std::endl;
}

// 用于通用的操作结果反馈
void UIManager::showOperationResult(bool success, const std::string& operation, const std::string& details) {
    if (success) {
        showSuccessMessage(operation + "成功" + (details.empty() ? "" : "：" + details));
    } else {
        showErrorMessage(operation + "失败" + (details.empty() ? "" : "：" + details));
    }
}

// 获取浮点数输入
double UIManager::getDoubleInput(const std::string& prompt) {
    double input;
    std::string inputStr;
    bool isValid;

    do {
        isValid = true;
        std::cout << prompt;
        std::getline(std::cin, inputStr);

        try {
            if (inputStr.empty()) {
                std::cout << "错误: 输入不能为空。请重新输入。" << std::endl;
                isValid = false;
                continue;
            }
            
            // 尝试将输入转换为浮点数
            input = std::stod(inputStr);
        }
        catch (const std::invalid_argument&) {
            std::cout << "错误: 输入必须为有效的浮点数。请重新输入。" << std::endl;
            isValid = false;
        }
        catch (const std::out_of_range&) {
            std::cout << "错误: 输入超出有效范围。请重新输入。" << std::endl;
            isValid = false;
        }
    } while (!isValid);

    return input;
}

// 获取带范围校验的浮点数输入
double UIManager::getDoubleInput(const std::string& prompt, double minVal, double maxVal) {
    double input;
    bool isValid;

    do {
        input = getDoubleInput(prompt);
        isValid = (input >= minVal && input <= maxVal);
        
        if (!isValid) {
            std::cout << "错误: 输入必须在 " << minVal << " 到 " << maxVal << " 之间。请重新输入。" << std::endl;
        }
    } while (!isValid);

    return input;
}

// --- 菜单显示 ---
void UIManager::displayMainMenu(const SystemSettings& settings) {
    // 清屏操作，根据需要添加，例如 system("cls") on Windows or system("clear") on Linux/macOS
    // std::system("cls"); // 或者使用更可移植的清屏方法

    WorkflowStage currentStage = settings.getCurrentWorkflowStage();

    std::cout << "\n========== 学校运动会管理系统 ==========" << std::endl;
    std::cout << "当前阶段: ";
    
    // 根据当前工作流阶段显示不同的菜单
    switch (currentStage) {
        case WorkflowStage::SETUP_EVENTS:
            displaySetupEventsMenu(settings);
            break;
        case WorkflowStage::REGISTRATION_OPEN:
            displayRegistrationOpenMenu(settings);
            break;
        case WorkflowStage::COMPETITION_RUNNING:
            displayCompetitionRunningMenu();
            break;
    }
    
    // 显示通用选项和警告信息
    displayCommonMenuOptions(settings);
}

// 显示第一阶段（项目与赛程设置）菜单
void UIManager::displaySetupEventsMenu(const SystemSettings& settings) {
    std::cout << "项目与赛程设置" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "1.  系统设置与项目管理 (添加单位/运动员/项目/规则等)" << std::endl;
    // 选项 1 内部会调用 SystemSettingsController::manage()，其中已有赛程锁定时部分功能禁用的逻辑
    std::cout << "2.  场地管理" << std::endl;             // 指向 SystemSettingsController::handleVenueManagement
    std::cout << "3.  上下午时间段设置" << std::endl;     // 指向 SystemSettingsController::handleSessionSettings
    std::cout << "4.  完成项目设置并锁定赛程 (进入运动员报名阶段)" << std::endl; // 新的流程控制选项
}

// 显示第二阶段（运动员报名）菜单
void UIManager::displayRegistrationOpenMenu(const SystemSettings& settings) {
    std::cout << "运动员报名" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "1.  运动员报名与管理 (报名/取消/查看)" << std::endl; // 指向 RegistrationController::manage
    std::cout << "2.  查看所有比赛项目 (已锁定)" << std::endl;      // 指向 SystemSettingsController::handleViewAllEvents
    std::cout << "3.  查看当前赛程安排" << std::endl;              // 新选项，用于查看已生成的赛程安排
    std::cout << "4.  结束报名并生成秩序册 (进入比赛管理阶段)" << std::endl; // 新的流程控制选项
    // std::cout << "4.  返回项目设置阶段 (解锁赛程)" << std::endl; // 可选，需要 ScheduleController::handleUnlockSchedule
}

// 显示第三阶段（比赛与成绩管理）菜单
void UIManager::displayCompetitionRunningMenu() {
    std::cout << "比赛与成绩管理" << std::endl;
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "1.  系统设置与查询" << std::endl;           // 更改名称，表示此阶段的系统设置菜单是查询为主
    std::cout << "2.  秩序册管理 (查看/验证)" << std::endl;   // 指向 ScheduleController::manage (主要用于查看)
    std::cout << "3.  比赛成绩管理 (录入/查看)" << std::endl; // 指向 ResultsController::manage
    std::cout << "4.  查看单位总分排名" << std::endl;         // 指向一个专门的统计功能
    std::cout << "5.  数据备份与恢复" << std::endl;           // 指向 DataManagementController::manage
}

// 显示所有阶段通用的菜单选项和警告
void UIManager::displayCommonMenuOptions(const SystemSettings& settings) {
    std::cout << "--------------------------------------" << std::endl;
    
    // 显示警告和状态信息
    WorkflowStage currentStage = settings.getCurrentWorkflowStage();
    if (currentStage == WorkflowStage::SETUP_EVENTS && settings.isScheduleLocked()) {
        showWarningMessage("赛程已锁定，部分项目设置可能受限。如需修改，请先在赛程管理中解锁");
    }
    
    if (currentStage == WorkflowStage::REGISTRATION_OPEN && !settings.isScheduleLocked()) {
        // 理论上此状态下赛程必锁定，若非，则是个逻辑问题
        showWarningMessage("运动员报名阶段但赛程未锁定，请检查系统状态！");
    }

    // 通用选项
    std::cout << "9.  分阶段导入示例数据" << std::endl;
    std::cout << "10. 数据备份和恢复" << std::endl;
    std::cout << "11. 自动测试 (辅助功能)" << std::endl;
    std::cout << "0.  退出系统" << std::endl;
    std::cout << "======================================" << std::endl;
}

void UIManager::displaySystemSettingsMenu(const SystemSettings& settings, WorkflowStage currentStage) {
    std::cout << "\n--- 系统设置管理 ---" << std::endl;
    
    // 所有阶段都显示的选项
    std::cout << "1. 查看所有单位" << std::endl;
    std::cout << "2. 查看所有项目" << std::endl;
    std::cout << "3. 计分规则管理" << std::endl;
    
    // 阶段1（项目设置）特有的选项
    if (currentStage == WorkflowStage::SETUP_EVENTS) {
        std::cout << "4. 添加参赛单位" << std::endl;
        std::cout << "5. 添加比赛项目" << std::endl;
        std::cout << "6. 场地管理" << (settings.isScheduleLocked() ? " (已锁定，仅可查看)" : "") << std::endl;
        std::cout << "7. 上午/下午时间段设置" << (settings.isScheduleLocked() ? " (已锁定，仅可查看)" : "") << std::endl;
    }
    
    // 阶段2（运动员报名）特有的选项
    if (currentStage == WorkflowStage::REGISTRATION_OPEN) {
        std::cout << "4. 添加运动员" << std::endl;
        std::cout << "5. 查看所有运动员" << std::endl;
        std::cout << "6. 设置运动员最大参赛项目数 (当前: " << settings.athletes.getMaxEventsAllowed() << ")" << std::endl;
        std::cout << "7. 场地管理 (仅可查看)" << std::endl;
        std::cout << "8. 上午/下午时间段设置 (仅可查看)" << std::endl;
    }
    
    // 阶段3（比赛进行）无额外选项，只显示通用选项
    if (currentStage == WorkflowStage::COMPETITION_RUNNING) {
        std::cout << "4. 查看所有运动员" << std::endl;
    }
    
    std::cout << "0. 返回主菜单" << std::endl;
}

void UIManager::displayRegistrationMenu() {
    std::cout << "\n--- 运动员报名与管理 ---" << std::endl;
    std::cout << "1. 运动员报名参赛项目" << std::endl;
    std::cout << "2. 运动员取消报名" << std::endl;
    std::cout << "3. 查看所有运动员及其报名情况" << std::endl;
    std::cout << "4. 查看所有项目及其报名情况" << std::endl;
    std::cout << "5. 检查并取消人数不足的项目" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
}

void UIManager::displayScheduleMenu(const SystemSettings& settings) {
    std::cout << "\n--- 赛程管理 ---" << std::endl;
    std::cout << "当前赛程状态：" << (settings.isScheduleLocked() ? "已锁定 (禁止修改项目时间/场地)" : "未锁定 (可编辑项目时间/场地)") << std::endl;
    std::cout << "1. 自动编排赛程" << std::endl;
    std::cout << "2. 查看秩序册" << std::endl;
    std::cout << "3. 验证赛程 (占位符)" << std::endl;
    std::cout << "4. 锁定赛程" << std::endl;
    std::cout << "5. 解锁赛程" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
}

void UIManager::displayResultsMenu() {
    std::cout << "\n--- 比赛成绩管理 ---" << std::endl;
    std::cout << "1. 录入/修改项目成绩" << std::endl;
    std::cout << "2. 查看指定项目成绩" << std::endl;
    // "查看所有单位得分排名" 通常在主菜单的 "比赛成绩统计" 中
    std::cout << "0. 返回主菜单" << std::endl;
}

void UIManager::displayQueryMenu() {
    std::cout << "\n--- 参赛信息查询 ---" << std::endl;
    std::cout << "1. 查看参赛单位信息" << std::endl;
    std::cout << "2. 查看比赛项目信息" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
}

void UIManager::displayDataManagementMenu() {
    std::cout << "\n--- 数据管理 ---" << std::endl;
    std::cout << "1. 备份数据" << std::endl;
    std::cout << "2. 恢复数据" << std::endl;
    std::cout << "3. 导入示例数据" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
}

// 新增：场地管理菜单
void UIManager::displayVenueManagementMenu() {
    std::cout << "\n--- 场地管理 ---" << std::endl;
    std::cout << "1. 添加场地" << std::endl;
    std::cout << "2. 删除场地" << std::endl;
    std::cout << "3. 查看所有场地" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
}

// 新增：上午/下午时间段设置菜单
void UIManager::displaySessionSettingsMenu(const SystemSettings& settings) {
    auto [morningStart, morningEnd] = settings.getMorningSession();
    auto [afternoonStart, afternoonEnd] = settings.getAfternoonSession();
    std::cout << "\n--- 上午/下午时间段设置 ---" << std::endl;
    std::cout << "当前上午时间段: " << morningStart << " - " << morningEnd << std::endl;
    std::cout << "当前下午时间段: " << afternoonStart << " - " << afternoonEnd << std::endl;
    std::cout << "1. 设置上午时间段" << std::endl;
    std::cout << "2. 设置下午时间段" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
}

// 新增：赛程生成菜单
void UIManager::displayScheduleGenerationMenu() {
    std::cout << "\n--- 赛程生成与查看 ---" << std::endl;
    std::cout << "1. 自动生成赛程" << std::endl;
    std::cout << "2. 查看当前赛程" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
}

// 新增：计分规则管理菜单
void UIManager::displayScoreRuleManagementMenu() {
    std::cout << "\n--- 计分规则管理 ---" << std::endl;
    std::cout << "1. 添加普通计分规则" << std::endl;
    std::cout << "2. 添加复合计分规则" << std::endl;
    std::cout << "3. 查看所有计分规则" << std::endl;
    std::cout << "4. 管理已有计分规则" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
}

// --- 数据列表显示 ---
void UIManager::displayUnits(const std::vector<utils::RefConst<Unit>>& units) {
    std::cout << "\n--- 所有参赛单位 ---" << std::endl;
    if (units.empty()) {
        showMessage("暂无参赛单位。");
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << "| "
              << std::setw(20) << "名称" << "| "
              << std::setw(10) << "总分" << "| "
              << "运动员数" << std::endl;
    std::cout << "-----|----------------------|------------|------------" << std::endl;
    for (const auto& unit_ref : units) {
        const Unit& unit = unit_ref.get();
        std::cout << std::left << std::setw(5) << unit.getId() << "| "
                  << std::setw(20) << unit.getName() << "| "
                  << std::fixed << std::setprecision(1) << std::setw(10) << unit.getTotalScore() << "| "
                  << unit.getAthleteIds().size() << std::endl;
    }
}

void UIManager::displayAthletes(const std::vector<utils::RefConst<Athlete>>& athletes, const SystemSettings& settings) {
    std::cout << "\n--- 所有运动员 ---" << std::endl;
    if (athletes.empty()) {
        showMessage("暂无运动员。");
        return;
    }
    std::cout << std::left
              << std::setw(5) << "ID" << "| "
              << std::setw(15) << "姓名" << "| "
              << std::setw(8) << "性别" << "| "
              << std::setw(20) << "单位 (ID)" << "| "
              << "已报项目数/项目ID" << std::endl;
    std::cout << "-----|-----------------|----------|----------------------|--------------------" << std::endl;
    for (const auto& athlete_ref : athletes) {
        const Athlete& athlete = athlete_ref.get();
        auto unitOpt = settings.units.getConst(athlete.getUnitId());
        std::string unitNameAndId = (unitOpt ? unitOpt.value().get().getName() : "未知") + " (" + std::to_string(athlete.getUnitId()) + ")";

        std::cout << std::left
                  << std::setw(5) << athlete.getId() << "| "
                  << std::setw(15) << athlete.getName() << "| "
                  << std::setw(8) << genderToString(athlete.getGender()) << "| "
                  << std::setw(20) << unitNameAndId << "| "
                  << std::setw(2) << athlete.getRegisteredEventsCount();
        if (athlete.getRegisteredEventsCount() > 0) {
            std::cout << " [IDs: ";
            const auto& events = athlete.getRegisteredEventIds();
            for (size_t i = 0; i < events.size(); ++i) {
                std::cout << events[i] << (i == events.size() - 1 ? "" : ", ");
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
}

void UIManager::displayEvents(const std::vector<utils::RefConst<CompetitionEvent>>& events, const SystemSettings& settings) {
    std::cout << "\n--- 所有比赛项目 ---" << std::endl;
    std::cout << "当前赛程状态：" << (settings.isScheduleLocked() ? "已锁定，禁止修改项目时间/场地" : "未锁定，可编辑项目时间/场地") << std::endl;
    if (events.empty()) {
        showMessage("暂无比赛项目。");
        return;
    }
    std::cout << std::left
              << std::setw(5) << "ID" << "| "
              << std::setw(25) << "项目名称" << "| "
              << std::setw(8) << "类型" << "| "
              << std::setw(10) << "性别要求" << "| "
              << std::setw(6) << "人数" << "| "
              << std::setw(12) << "计分规则ID" << "| "
              << std::setw(8) << "状态" << "| "
              << std::setw(8) << "时长(分)" << "| "
              << std::setw(10) << "场地" << "| "
              << std::setw(8) << "开始时间" << "| "
              << std::setw(8) << "结束时间"
              << std::endl;
    std::cout << "-----|---------------------------|----------|------------|------|--------------|--------|--------|----------|--------|--------" << std::endl;
    for (const auto& event_ref : events) {
        const CompetitionEvent& event = event_ref.get();
        std::string nameStr = event.getName();
        std::cout << std::left
                  << std::setw(5) << event.getId() << "| "
                  << std::setw(25) << nameStr << "| "
                  << std::setw(8) << eventTypeToString(event.getEventType()) << "| "
                  << std::setw(10) << genderToString(event.getGenderRequirement()) << "| "
                  << std::setw(6) << event.getParticipantCount() << "| "
                  << std::setw(12) << (event.getScoreRuleId() != -1 ? std::to_string(event.getScoreRuleId()) : "未指定") << "| "
                  << std::setw(8) << (event.getIsCancelled() ? "已取消" : "正常") << "| "
                  << std::setw(8) << (event.getDurationMinutes() > 0 ? std::to_string(event.getDurationMinutes()) : "未设置") << "| "
                  << std::setw(10) << (event.getVenue().empty() ? "未设置" : event.getVenue()) << "| "
                  << std::setw(8) << (event.getStartTime().empty() ? "未设置" : event.getStartTime()) << "| "
                  << std::setw(8) << (event.getEndTime().empty() ? "未设置" : event.getEndTime())
                  << std::endl;
    }
}

void UIManager::displayScoreRules(const std::vector<utils::RefConst<ScoreRule>>& rules) {
    std::cout << "\n--- 所有计分规则 ---" << std::endl;
    if (rules.empty()) {
        showMessage("暂无计分规则。");
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << "| "
              << std::setw(30) << "描述" << "| "
              << std::setw(15) << "适用人数(Min)" << "| "
              << std::setw(15) << "适用人数(Max)" << "| "
              << "录取名次" << std::endl;
    std::cout << "-----|--------------------------------|-----------------|-----------------|------------" << std::endl;
    for (const auto& rule_ref : rules) {
        const ScoreRule& rule = rule_ref.get();
        std::cout << std::left << std::setw(5) << rule.getId() << "| "
                  << std::setw(30) << rule.getDescription() << "| "
                  << std::setw(15) << rule.getMinParticipants() << "| "
                  << std::setw(15) << (rule.getMaxParticipants() == -1 ? "无上限" : std::to_string(rule.getMaxParticipants())) << "| "
                  << rule.getRanksToAward() << std::endl;
        std::cout << "  名次与得分: ";
        for (const auto& scorePair : rule.getAllScoresForRanks()) {
            std::cout << "第" << scorePair.first << "名=" << scorePair.second << "分; ";
        }
        std::cout << std::endl;
    }
}

void UIManager::displayEventResultsDetails(const CompetitionEvent& event,
                                           const EventResults* results, // 指针，因为可能没有成绩
                                           const SystemSettings& settings) {
    std::cout << "\n项目: " << event.getName();
    if (event.getIsCancelled()) {
        std::cout << " [状态: 已取消]";
    }

    auto ruleOpt = settings.getScoreRuleConst(event.getScoreRuleId());
    if (ruleOpt) {
        std::cout << " (计分规则: " << ruleOpt.value().get().getDescription() << ")" << std::endl;
    } else if (event.getScoreRuleId() != -1) {
        std::cout << " (计分规则ID: " << event.getScoreRuleId() << " - 未找到详细信息)" << std::endl;
    } else {
        std::cout << " (未指定计分规则)" << std::endl;
    }

    if (!results || results->getResultsList().empty()) {
        showMessage("项目 \"" + event.getName() + "\" 尚无成绩记录。");
        return;
    }

    auto resultsToDisplay = results->getResultsList(); // 假设 getResultsList 返回一个副本或 const 引用
    // 如果需要排序，可以在控制器中完成，或者在这里排序（如果 EventResults 内部不保证顺序）
    // std::sort(resultsToDisplay.begin(), resultsToDisplay.end(), [](const Result& a, const Result& b) {
    //     return a.getRank() < b.getRank();
    // });

    std::cout << "\n" << std::left
              << std::setw(6) << "名次" << "| "
              << std::setw(18) << "运动员姓名" << "| "
              << std::setw(10) << "运动员ID" << "| "
              << std::setw(15) << "成绩记录" << "| "
              << "所获分数" << std::endl;
    std::cout << "------|--------------------|------------|-----------------|----------" << std::endl;
    for (const auto& result : resultsToDisplay) {
        auto athleteOpt = settings.athletes.getConst(result.getAthleteId());
        std::string athleteName = athleteOpt ? athleteOpt.value().get().getName() : "未知运动员";

        std::cout << std::left << std::setw(6) << result.getRank() << "| "
                  << std::setw(18) << athleteName << "| "
                  << std::setw(10) << result.getAthleteId() << "| "
                  << std::setw(15) << result.getScoreRecord() << "| "
                  << std::fixed << std::setprecision(1) << result.getPointsAwarded() << std::endl;
    }
}

void UIManager::displayUnitStandings(const std::vector<utils::RefConst<Unit>>& sortedUnits) {
    std::cout << "\n--- 单位得分排名 ---" << std::endl;
    if (sortedUnits.empty()) {
        showMessage("系统中没有单位。");
        return;
    }

    std::cout << "\n" << std::left
              << std::setw(6) << "排名" << "| "
              << std::setw(25) << "单位名称" << "| "
              << "总得分" << std::endl;
    std::cout << "------|---------------------------|----------" << std::endl;
    int rank = 1;
    double lastScore = -1.0;
    int displayRank = 1;
    for (const auto& unitRef : sortedUnits) {
        const Unit& unit = unitRef.get();
        if (unit.getTotalScore() != lastScore && rank > 1) {
            displayRank = rank;
        }
        std::cout << std::left << std::setw(6) << displayRank << "| "
                  << std::setw(25) << unit.getName() << "| "
                  << std::fixed << std::setprecision(1) << unit.getTotalScore() << std::endl;
        lastScore = unit.getTotalScore();
        rank++;
    }
}

// 新增：显示场地表
void UIManager::displayVenues(const std::set<std::string>& venues) {
    std::cout << "\n--- 场地表 ---" << std::endl;
    if (venues.empty()) {
        std::cout << "暂无场地。" << std::endl;
        return;
    }
    int idx = 1;
    for (const auto& v : venues) {
        std::cout << idx++ << ". " << v << std::endl;
    }
}


// --- 特定输入的辅助函数实现 ---
Gender UIManager::getGenderInput(const std::string& prompt, bool allowUnspecified) {
    while (true) {
        std::string options = " (0: 男";
        options += allowUnspecified ? ", 1: 女, 2: 不限): " : ", 1: 女): ";
        int choice = getIntInput(prompt + options);
        if (choice == 0) return Gender::MALE;
        if (choice == 1) return Gender::FEMALE;
        if (allowUnspecified && choice == 2) return Gender::UNSPECIFIED;
        showErrorMessage("无效的性别选择。");
    }
}

EventType UIManager::getEventTypeInput(const std::string& prompt) {
    while (true) {
        int choice = getIntInput(prompt + " (0: 径赛, 1: 田赛): ");
        if (choice == 0) return EventType::TRACK;
        if (choice == 1) return EventType::FIELD;
        // 如果允许 UNSPECIFIED，可以在这里添加逻辑
        showErrorMessage("无效的项目类型选择。");
    }
}

void UIManager::showRegistrationLockedMessage() {
    showErrorMessage("当前赛程未锁定，无法进行报名操作，请先生成并锁定赛程！");
}

void UIManager::showRegistrationConflictMessage(const std::string& conflictInfo) {
    showErrorMessage("报名冲突：" + conflictInfo);
}

void UIManager::showRegistrationIntervalWarning(const std::string& warningInfo) {
    showMessage("警告：" + warningInfo);
}
