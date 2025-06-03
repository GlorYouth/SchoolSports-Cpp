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


// --- 菜单显示 ---
void UIManager::displayMainMenu() {
    std::cout << "\n========== 学校运动会管理系统 ==========" << std::endl;
    std::cout << "1. 系统设置管理" << std::endl;
    std::cout << "2. 查看参赛项目" << std::endl;
    std::cout << "3. 运动员报名登记与管理" << std::endl;
    std::cout << "4. 参赛信息查询" << std::endl;
    std::cout << "5. 秩序册生成与查看" << std::endl;
    std::cout << "6. 比赛成绩管理" << std::endl;
    std::cout << "7. 比赛成绩统计 (单位排名)" << std::endl;
    std::cout << "8. 数据备份与恢复" << std::endl;
    std::cout << "9. 自动测试" << std::endl;
    std::cout << "0. 退出系统" << std::endl;
    std::cout << "======================================" << std::endl;
}

void UIManager::displaySystemSettingsMenu(const SystemSettings& settings) {
    std::cout << "\n--- 系统设置管理 ---" << std::endl;
    std::cout << "1. 添加参赛单位" << std::endl;
    std::cout << "2. 查看所有单位" << std::endl;
    std::cout << "3. 添加比赛项目" << std::endl;
    std::cout << "4. 查看所有项目" << std::endl;
    std::cout << "5. 添加运动员" << std::endl;
    std::cout << "6. 查看所有运动员" << std::endl;
    std::cout << "7. 添加计分规则" << std::endl;
    std::cout << "8. 查看所有计分规则" << std::endl;
    std::cout << "9. 设置运动员最大参赛项目数 (当前: " << settings.getAthleteMaxEventsAllowed() << ")" << std::endl;
    std::cout << "10. 设置项目最少举行人数 (当前: " << settings.getMinParticipantsToHoldEvent() << ")" << std::endl;
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

void UIManager::displayScheduleMenu() {
    std::cout << "\n--- 秩序册管理 ---" << std::endl;
    std::cout << "1. 自动生成秩序册" << std::endl;
    std::cout << "2. 查看秩序册" << std::endl;
    std::cout << "3. 验证秩序册 (占位符)" << std::endl;
    std::cout << "0. 返回主菜单" << std::endl;
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
    std::cout << "0. 返回主菜单" << std::endl;
}


// --- 数据列表显示 ---
void UIManager::displayUnits(const std::vector<std::reference_wrapper<const Unit>>& units) {
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

void UIManager::displayAthletes(const std::vector<std::reference_wrapper<const Athlete>>& athletes, const SystemSettings& settings) {
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
        auto unitOpt = settings.getUnitConst(athlete.getUnitId());
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

void UIManager::displayEvents(const std::vector<std::reference_wrapper<const CompetitionEvent>>& events, const SystemSettings& settings) {
    std::cout << "\n--- 所有比赛项目 ---" << std::endl;
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
              << "状态"
              << std::endl;
    std::cout << "-----|---------------------------|----------|------------|--------|--------------|--------" << std::endl;
    for (const auto& event_ref : events) {
        const CompetitionEvent& event = event_ref.get();
        // auto ruleOpt = settings.getScoreRuleConst(event.getScoreRuleId()); // 如果需要显示规则描述
        // std::string ruleDescPart = (ruleOpt.has_value() ? " (" + ruleOpt.value().get().getDescription() + ")" : "");

        std::cout << std::left
                  << std::setw(5) << event.getId() << "| "
                  << std::setw(25) << event.getName() << "| "
                  << std::setw(8) << eventTypeToString(event.getEventType()) << "| "
                  << std::setw(10) << genderToString(event.getGenderRequirement()) << "| "
                  << std::setw(6) << event.getParticipantCount() << "| "
                  << std::setw(12) << (event.getScoreRuleId() != -1 ? std::to_string(event.getScoreRuleId()) : "未指定") << "| "
                  << (event.getIsCancelled() ? "已取消" : "正常")
                  << std::endl;
    }
}

void UIManager::displayScoreRules(const std::vector<std::reference_wrapper<const ScoreRule>>& rules) {
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
        auto athleteOpt = settings.getAthleteConst(result.getAthleteId());
        std::string athleteName = athleteOpt ? athleteOpt.value().get().getName() : "未知运动员";

        std::cout << std::left << std::setw(6) << result.getRank() << "| "
                  << std::setw(18) << athleteName << "| "
                  << std::setw(10) << result.getAthleteId() << "| "
                  << std::setw(15) << result.getScoreRecord() << "| "
                  << std::fixed << std::setprecision(1) << result.getPointsAwarded() << std::endl;
    }
}

void UIManager::displayUnitStandings(const std::vector<std::reference_wrapper<const Unit>>& sortedUnits) {
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
