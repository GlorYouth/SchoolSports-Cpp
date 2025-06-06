
#include "../../../include/UI/Controller/ResultsController.h"
#include "../../../include/UI/UIManager.h"
#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/Components/Core/Athlete.h"
#include "../../../include/Components/Core/ScoreRule.h"
#include "../../../include/Components/Core/Result.h"
#include <vector>
#include <string>
#include <algorithm> // For std::find, std::sort
#include <map>       // For scoresMap in rule
#include <ranges>

#include "../../../include/utils.h"

ResultsController::ResultsController(SystemSettings& settings)
    : settings_(settings) {}

void ResultsController::manage() {
    int choice;
    do {
        UIManager::displayResultsMenu(); // 显示成绩管理子菜单
        choice = UIManager::getIntInput("请输入您的选择: ", 0, 2);

        switch (choice) {
            case 1: handleRecordEventResults(); break;
            case 2: handleViewEventResults(); break;
            case 0: UIManager::showMessage("返回主菜单..."); break;
            default: UIManager::showErrorMessage("无效选择。"); break;
        }
        if (choice != 0) {
            UIManager::pressEnterToContinue();
        }
    } while (choice != 0);
}

void ResultsController::handleRecordEventResults() {
    UIManager::showMessage("\n--- 录入/修改项目成绩 ---");
    if (settings_.events.getAllConst().empty()) {
        UIManager::showErrorMessage("系统中没有比赛项目，无法录入成绩。");
        return;
    }

    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    for(const auto& [id, event_ref] : settings_.events.getAllConst()) {
        // 通常只为未取消且可能已结束的项目录入成绩
        if (!event_ref.get().getIsCancelled()) { // 简单起见，只检查未取消
            events_refs.push_back(std::cref(event_ref));
        }
    }
    if(events_refs.empty()){
        UIManager::showMessage("当前没有可录入成绩的项目 (可能所有项目都已取消或未添加)。");
        return;
    }
    UIManager::displayEvents(events_refs, settings_);

    int eventId = UIManager::getIntInput("请输入要录入成绩的项目ID: ");

    auto eventOpt = settings_.events.get(eventId); // 非 const 获取，因为可能要更新其状态
    if (!eventOpt) {
        UIManager::showErrorMessage("项目ID " + std::to_string(eventId) + " 不存在。");
        return;
    }
    CompetitionEvent& competitionEvent = eventOpt.value().get();

    if (competitionEvent.getIsCancelled()) {
        UIManager::showErrorMessage("项目 \"" + competitionEvent.getName() + "\" 已被取消，无法录入成绩。");
        return;
    }

    // 使用规则的最小参赛人数
    // 默认情况下应该只有一个主规则（ID=1），包含多个子规则
    auto scoreRuleOpt = settings_.rules.get(1); // 默认使用ID=1的规则
    if (!scoreRuleOpt) {
        UIManager::showErrorMessage("找不到默认计分规则（ID=1）。请初始化系统设置。");
        return;
    }
    
    // 获取适用于当前参赛人数的子规则
    ScoreRule& mainRule = scoreRuleOpt.value().get();
    const auto applicableRule = mainRule.getApplicableRule(competitionEvent.getParticipantCount());
    
    if (!applicableRule.has_value() || !applicableRule.value().get().appliesTo(competitionEvent.getParticipantCount())) {
        // 规则也不适用的规则说明人数不够，取消项目
        UIManager::showMessage("项目 \"" + competitionEvent.getName() + "\" 参赛人数 (" +
                             std::to_string(competitionEvent.getParticipantCount()) + ") 不满足规则要求 (" +
                             std::to_string(mainRule.getMinParticipants()) + ")。");
        if (!competitionEvent.getIsCancelled()) {
            competitionEvent.setCancelled(true);
            UIManager::showErrorMessage("项目 \"" + competitionEvent.getName() + "\" 已被自动设为取消，无法录入成绩。");
        }
        return;
    }

    settings_.results.clearForEvent(eventId); // 清除旧成绩，重新排名
    
    UIManager::showMessage("应用计分规则: " + applicableRule.value().get().getDescription());

    EventResults newEventResults(eventId);
    int ranksToAward = applicableRule.value().get().getRanksToAward();
    UIManager::showMessage("请为项目 \"" + competitionEvent.getName() + "\" 录入前 " +
                           std::to_string(ranksToAward) + " 名的成绩。");

    UIManager::showMessage("以下是参与此项目的运动员:");
    bool participantsExist = false;
    for (int athleteId_participant : competitionEvent.getParticipantAthleteIds()) {
        auto ath = settings_.athletes.getConst(athleteId_participant);
        if (ath) {
            UIManager::showMessage("  ID: " + std::to_string(ath.value().get().getId()) +
                                   ", 姓名: " + ath.value().get().getName() +
                                   ", 单位ID: " + std::to_string(ath.value().get().getUnitId()));
            participantsExist = true;
        }
    }
    if (!participantsExist) {
        UIManager::showErrorMessage("此项目没有有效的报名运动员。");
        return;
    }

    std::vector<int> awardedAthleteIdsThisSession;
    for (int i = 1; i <= ranksToAward; ++i) {
        int rank = i;
        int athleteId_input = UIManager::getIntInput("\n请输入第 " + std::to_string(rank) + " 名的运动员ID (输入0跳过或结束): ");
        if (athleteId_input == 0) {
            UIManager::showMessage("已跳过第 " + std::to_string(rank) + " 名及之后名次的录入。");
            break;
        }

        auto athleteOpt = settings_.athletes.get(athleteId_input);
        if (!athleteOpt) {
            UIManager::showErrorMessage("运动员ID " + std::to_string(athleteId_input) + " 不存在。此名次作废。");
            i--; continue;
        }
        Athlete& rankedAthlete = athleteOpt.value().get();

        bool participated = false;
        for (int pId : competitionEvent.getParticipantAthleteIds()) {
            if (pId == athleteId_input) {
                participated = true;
                break;
            }
        }
        if (!participated) {
            UIManager::showErrorMessage("运动员 " + rankedAthlete.getName() + " (ID: " + std::to_string(athleteId_input) +
                                        ") 未报名参加此项目。此名次作废。");
            i--; continue;
        }

        if (std::find(awardedAthleteIdsThisSession.begin(), awardedAthleteIdsThisSession.end(), athleteId_input) != awardedAthleteIdsThisSession.end()) {
            UIManager::showErrorMessage("运动员 " + rankedAthlete.getName() + " 已在此次录入中获得名次。此名次作废。");
            i--; continue;
        }

        std::string scoreRecordStr = UIManager::getStringInput("请输入第 " + std::to_string(rank) + " 名 (运动员 " + rankedAthlete.getName() + ") 的成绩记录 (如 10.5s, 2.10m): ");
        double points = applicableRule.value().get().getScoreForRank(rank);

        Result result(eventId, athleteId_input, rank, scoreRecordStr, points);
        newEventResults.addResult(result);
        awardedAthleteIdsThisSession.push_back(athleteId_input);

        // 更新单位总分 (SystemSettings 应该提供一个方法来做这件事，以确保一致性)
        settings_.results.addScoreToUnit(rankedAthlete.getUnitId(), points);
    }

    newEventResults.finalizeResults(); // 内部处理名次与积分计算
    settings_.results.addOrUpdate(newEventResults);
    competitionEvent.setScoreRuleId(applicableRule.value().get().getId()); // 关联计分规则到项目

    UIManager::showSuccessMessage("项目 \"" + competitionEvent.getName() + "\" 的成绩已录入/更新完毕。");
}


void ResultsController::handleViewEventResults() {
    UIManager::showMessage("\n--- 查看项目成绩 ---");
    if (settings_.events.getAllConst().empty()) {
        UIManager::showErrorMessage("系统中没有比赛项目。");
        return;
    }

    std::vector<utils::RefConst<CompetitionEvent>> events_refs;
    for(const auto& val : settings_.events.getAllConst() | std::views::values) events_refs.push_back(std::cref(val));
    UIManager::displayEvents(events_refs, settings_);

    int eventId = UIManager::getIntInput("请输入要查看成绩的项目ID: ");

    auto eventOpt = settings_.events.getConst(eventId);
    if (!eventOpt) {
        UIManager::showErrorMessage("项目ID " + std::to_string(eventId) + " 不存在。");
        return;
    }
    const CompetitionEvent& competitionEvent = eventOpt.value().get();

    auto eventResultsOpt = settings_.results.getConst(eventId);
    const EventResults* resultsPtr = eventResultsOpt ? &(eventResultsOpt.value().get()) : nullptr;

    // UIManager 显示成绩详情
    UIManager::displayEventResultsDetails(competitionEvent, resultsPtr, settings_);
}
