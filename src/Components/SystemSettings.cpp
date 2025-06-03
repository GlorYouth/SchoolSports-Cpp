//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/SystemSettings.h"
#include <iostream> // 用于演示输出
#include <ranges>
#include <string>
#include <vector>
#include <map>
#include "../../include/Components/Result.h"

SystemSettings::SystemSettings() : athleteMaxEventsAllowed(3), minParticipantsToHoldEvent(4) {
    // 构造函数中可以进行一些初始化
    // initializeDefaultSettings(); // 可以在构造时直接初始化，或者由外部调用
}

// --- 单位管理 ---
bool SystemSettings::addUnit(const std::string& unitName) {
    // 检查单位名是否已存在
    for (const auto &val : units | std::views::values) {
        if (val.getName() == unitName) {
            std::cerr << "错误: 单位名称 '" << unitName << "' 已存在。" << std::endl;
            return false; // 单位名已存在
        }
    }
    Unit newUnit(unitName);
    units.insert({newUnit.getId(), newUnit});
    return true;
}

std::optional<std::reference_wrapper<Unit>> SystemSettings::getUnit(const int unitId) {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const Unit>> SystemSettings::getUnitConst(const int unitId) const {
    if (const auto it = units.find(unitId); it != units.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, Unit>& SystemSettings::getAllUnits() const {
    return units;
}

bool SystemSettings::removeUnit(int unitId) {
    // 注意：移除单位前，应处理该单位下的运动员
    // 还需要处理相关的成绩、报名信息等，这里简化处理
    auto unitToRemove = units.find(unitId);
    if (unitToRemove == units.end()) {
        return false; // 单位不存在
    }

    // 移除该单位下的所有运动员
    std::vector<int> athletesToRemove = unitToRemove->second.getAthleteIds();
    for (int athleteId : athletesToRemove) {
        removeAthlete(athleteId); // 调用 removeAthlete 来处理相关清理
    }

    return units.erase(unitId) > 0;
}


// --- 运动员管理 ---
bool SystemSettings::addAthlete(const std::string& name, Gender gender, int unitId) {
    const auto optional_unit = getUnit(unitId);
    if (!optional_unit.has_value()) {
        std::cerr << "错误: 添加运动员失败，单位ID " << unitId << " 不存在。" << std::endl;
        return false; // 单位不存在
    }
    Athlete newAthlete(name, gender, unitId);
    athletes.insert({newAthlete.getId(), newAthlete});
    optional_unit.value().get().addAthleteId(newAthlete.getId()); // 将运动员ID关联到单位
    return true;
}

std::optional<std::reference_wrapper<Athlete>> SystemSettings::getAthlete(const int athleteId) {
    if (const auto it = athletes.find(athleteId); it != athletes.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const Athlete>> SystemSettings::getAthleteConst(const int athleteId) const {
    if (const auto it = athletes.find(athleteId); it != athletes.end()) {
        return it->second;
    }
    return std::nullopt;
}


const std::map<int, Athlete>& SystemSettings::getAllAthletes() const {
    return athletes;
}

bool SystemSettings::removeAthlete(const int athleteId) {
    const auto athleteIt = athletes.find(athleteId);
    if (athleteIt == athletes.end()) {
        return false; // 运动员不存在
    }
    const Athlete& athlete_ref = athleteIt->second;

    // 从所属单位中移除运动员ID
    if (const auto unit = getUnit(athlete_ref.getUnitId())) {
        unit.value().get().removeAthleteId(athleteId);
    }

    // 从所有其报名的项目中移除该运动员的参与记录
    for (const int eventId : athlete_ref.getRegisteredEventIds()) {
        if (auto event = getCompetitionEvent(eventId); event.has_value()) {
            event.value().get().removeParticipant(athleteId);
        }
    }

    // 从成绩记录中移除该运动员的成绩 (如果需要)
    // 这是一个更复杂的操作，可能需要遍历所有EventResults
    // 为简化，此处不直接处理成绩的移除，成绩查询时若运动员不存在则忽略

    return athletes.erase(athleteId) > 0;
}

// --- 项目管理 ---
bool SystemSettings::addCompetitionEvent(const std::string& eventName, EventType type, Gender genderReq) {
    CompetitionEvent newEvent(eventName, type, genderReq);
    competitionEvents.insert({newEvent.getId(), newEvent});
    return true;
}

std::optional<std::reference_wrapper<CompetitionEvent>> SystemSettings::getCompetitionEvent(int eventId) {
    const auto it = competitionEvents.find(eventId);
    if (it != competitionEvents.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const CompetitionEvent>> SystemSettings::getCompetitionEventConst(int eventId) const {
    const auto it = competitionEvents.find(eventId);
    if (it != competitionEvents.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, CompetitionEvent>& SystemSettings::getAllCompetitionEvents() const {
    return competitionEvents;
}

bool SystemSettings::removeCompetitionEvent(const int eventId) {
    const auto eventIt = competitionEvents.find(eventId);
    if (eventIt == competitionEvents.end()) {
        return false; // 项目不存在
    }
    const CompetitionEvent& event_ref = eventIt->second;

    // 从所有已报名该项目的运动员的报名列表中移除该项目
    for (const int athleteId : event_ref.getParticipantAthleteIds()) {
        if (auto athlete = getAthlete(athleteId); athlete.has_value()) {
            athlete.value().get().unregisterFromEvent(eventId);
        }
    }

    // 移除该项目的成绩记录
    eventResultsMap.erase(eventId);

    return competitionEvents.erase(eventId) > 0;
}

// --- 计分规则管理 ---
bool SystemSettings::addScoreRule(const std::string& desc, int minP, int maxP, int ranks, const std::map<int, double>& scores) {
    ScoreRule newRule(desc, minP, maxP, ranks, scores);
    scoreRules.insert({newRule.getId(), newRule});
    return true;
}

std::optional<std::reference_wrapper<ScoreRule>> SystemSettings::getScoreRule(int ruleId) {
    if (const auto it = scoreRules.find(ruleId); it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const ScoreRule>> SystemSettings::getScoreRuleConst(const int ruleId) const {
    auto it = scoreRules.find(ruleId);
    if (it != scoreRules.end()) {
        return it->second;
    }
    return std::nullopt;
}


const std::map<int, ScoreRule>& SystemSettings::getAllScoreRules() const {
    return scoreRules;
}

std::optional<std::reference_wrapper<ScoreRule>> SystemSettings::findAppropriateScoreRule(int participantCount) {
    for (auto &val : scoreRules | std::views::values) {
        if (val.appliesTo(participantCount)) {
            return val;
        }
    }
    std::cerr << "警告: 未找到适用于参赛人数 " << participantCount << " 的计分规则。" << std::endl;
    return std::nullopt; // 没有找到合适的规则
}

// --- 系统参数设置 ---
void SystemSettings::setAthleteMaxEventsAllowed(int maxEvents) {
    if (maxEvents > 0) {
        athleteMaxEventsAllowed = maxEvents;
    } else {
        std::cerr << "错误: 运动员最大参赛项目数必须大于0。" << std::endl;
    }
}

int SystemSettings::getAthleteMaxEventsAllowed() const {
    return athleteMaxEventsAllowed;
}

void SystemSettings::setMinParticipantsToHoldEvent(int minParticipants) {
     if (minParticipants >= 0) { // 允许为0，表示没有限制，但通常至少为1或更高
        minParticipantsToHoldEvent = minParticipants;
    } else {
        std::cerr << "错误: 项目最少举行人数不能为负。" << std::endl;
    }
}

int SystemSettings::getMinParticipantsToHoldEvent() const {
    return minParticipantsToHoldEvent;
}

// --- 比赛结果管理 ---
bool SystemSettings::addOrUpdateEventResults(const EventResults& er) {
    if (!competitionEvents.contains(er.getEventId())) {
        std::cerr << "错误: 无法为不存在的项目ID " << er.getEventId() << " 添加成绩。" << std::endl;
        return false;
    }
    eventResultsMap.insert_or_assign(er.getEventId(), er); // 插入或更新
    return true;
}

std::optional<std::reference_wrapper<EventResults>> SystemSettings::getEventResults(int eventId) {
    auto it = eventResultsMap.find(eventId);
    if (it != eventResultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<const EventResults>> SystemSettings::getEventResultsConst(int eventId) const {
    auto it = eventResultsMap.find(eventId);
    if (it != eventResultsMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

const std::map<int, EventResults>& SystemSettings::getAllEventResults() const {
    return eventResultsMap;
}


// 清除特定项目的所有成绩，并尝试撤销已加到单位的总分
void SystemSettings::clearResultsForEvent(const int eventId) {
    if (const auto resultsIt = eventResultsMap.find(eventId); resultsIt != eventResultsMap.end()) {
        const EventResults& results_ref = resultsIt->second;
        for (const auto& result : results_ref.getResultsList()) {
            if (auto athleteOpt = getAthleteConst(result.getAthleteId())) {
                if (auto unitOpt = getUnit(athleteOpt.value().get().getUnitId())) {
                    // 从单位分数中减去此成绩的分数
                    unitOpt.value().get().addScore(-result.getPointsAwarded()); // 假设 addScore 可以接受负值
                }
            }
        }
        eventResultsMap.erase(resultsIt); // 移除项目成绩记录
        // std::cout << "已清除项目ID " << eventId << " 的成绩及其对单位总分的影响。" << std::endl;
    } else {
        // std::cout << "项目ID " << eventId << " 没有成绩记录可清除。" << std::endl;
    }

    // 可选：如果项目本身还存在，可能需要重置其计分规则ID等状态
    auto eventOpt = getCompetitionEvent(eventId);
    if (eventOpt) {
        eventOpt.value().get().setScoreRuleId(-1); // 假设-1表示未设置
    }
}

void SystemSettings::resetAllUnitScores() {
    for (auto &val : units | std::views::values) {
        val.resetScore();
    }
    // std::cout << "所有单位的总分已重置。" << std::endl;
}

// 新增：为指定单位增加分数
void SystemSettings::addScoreToUnit(int unitId, double score) {
    auto unitOpt = getUnit(unitId);
    if (unitOpt) {
        unitOpt.value().get().addScore(score);
        // 可选: 可以在此处添加日志或成功消息，但通常调用方会处理用户反馈
        // std::cout << "成功为单位ID " << unitId << " 增加了 " << score << " 分。" << std::endl;
    } else {
        std::cerr << "错误: 尝试为不存在的单位ID " << unitId << " 加分失败。" << std::endl;
    }
}


// 初始化默认设置 (示例)
void SystemSettings::initializeDefaultSettings() {
    std::cout << "正在初始化系统默认设置..." << std::endl;

    // 清空现有数据以便全新初始化
    units.clear();
    athletes.clear();
    competitionEvents.clear();
    scoreRules.clear();
    eventResultsMap.clear(); // 清空比赛结果
    // 重置静态ID计数器 (如果允许，这通常在测试环境中有用，但在实际应用中要小心)
    // Unit::nextId = 1; // 假设ID从1开始，并且可以重置
    // Athlete::nextId = 1;
    // CompetitionEvent::nextId = 1;
    // ScoreRule::nextId = 1;
    // 注意：直接重置静态原子ID可能不是最佳实践，取决于具体设计。
    // 如果ID必须全局唯一且持久，则不应重置。
    // 为简单起见，这里假设每次初始化都是一个全新的环境，ID可以从头开始。
    // 更好的做法是让ID生成器在类的外部或通过更高级的机制管理。

    // 1. 添加参赛单位
    // 为了能获取到单位ID，我们逐个添加并记录
    std::map<std::string, int> unitNameToId;
    std::vector<std::string> unitNames = {"计算机学院", "外国语学院", "体育学院", "理学院", "文学院", "艺术学院"};
    for(const auto& name : unitNames){
        Unit tempUnit(name); // 临时创建一个Unit对象以获取其ID
        units.insert({tempUnit.getId(), tempUnit});
        unitNameToId[name] = tempUnit.getId();
    }
    std::cout << "默认单位添加完毕。" << std::endl;


    // 2. 添加比赛项目
    addCompetitionEvent("男子100米", EventType::TRACK, Gender::MALE);
    addCompetitionEvent("女子100米", EventType::TRACK, Gender::FEMALE);
    addCompetitionEvent("男子200米", EventType::TRACK, Gender::MALE);
    addCompetitionEvent("女子200米", EventType::TRACK, Gender::FEMALE);
    addCompetitionEvent("男子800米", EventType::TRACK, Gender::MALE);
    addCompetitionEvent("女子800米", EventType::TRACK, Gender::FEMALE);
    addCompetitionEvent("男子跳高", EventType::FIELD, Gender::MALE);
    addCompetitionEvent("女子跳高", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子跳远", EventType::FIELD, Gender::MALE);
    addCompetitionEvent("女子跳远", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子三级跳远", EventType::FIELD, Gender::MALE);
    addCompetitionEvent("女子三级跳远", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子铅球", EventType::FIELD, Gender::MALE); // README中已有女子铅球，这里添加男子
    addCompetitionEvent("女子铅球", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子标枪", EventType::FIELD, Gender::MALE);
    addCompetitionEvent("女子标枪", EventType::FIELD, Gender::FEMALE);
    addCompetitionEvent("男子4x100米接力", EventType::TRACK, Gender::MALE);
    addCompetitionEvent("女子4x100米接力", EventType::TRACK, Gender::FEMALE);
    addCompetitionEvent("混合4x400米接力", EventType::TRACK, Gender::UNSPECIFIED); // 性别不限的项目
    std::cout << "默认比赛项目添加完毕。" << std::endl;

    // 3. 添加运动员 (确保单位ID正确)
    if (unitNameToId.count("计算机学院")) {
        addAthlete("孙悟空", Gender::MALE, unitNameToId["计算机学院"]);
        addAthlete("白骨精", Gender::FEMALE, unitNameToId["计算机学院"]);
        addAthlete("猪八戒", Gender::MALE, unitNameToId["计算机学院"]);
    }
    if (unitNameToId.count("外国语学院")) {
        addAthlete("林黛玉", Gender::FEMALE, unitNameToId["外国语学院"]);
        addAthlete("贾宝玉", Gender::MALE, unitNameToId["外国语学院"]);
    }
    if (unitNameToId.count("体育学院")) {
        addAthlete("刘翔", Gender::MALE, unitNameToId["体育学院"]);
        addAthlete("姚明", Gender::MALE, unitNameToId["体育学院"]);
        addAthlete("李娜", Gender::FEMALE, unitNameToId["体育学院"]);
        addAthlete("苏炳添", Gender::MALE, unitNameToId["体育学院"]);
    }
    if (unitNameToId.count("理学院")) {
        addAthlete("陈景润", Gender::MALE, unitNameToId["理学院"]);
        addAthlete("居里夫人", Gender::FEMALE, unitNameToId["理学院"]);
    }
    if (unitNameToId.count("文学院")) {
        addAthlete("鲁迅", Gender::MALE, unitNameToId["文学院"]);
        addAthlete("冰心", Gender::FEMALE, unitNameToId["文学院"]);
    }
    if (unitNameToId.count("艺术学院")) {
        addAthlete("梵高", Gender::MALE, unitNameToId["艺术学院"]);
        addAthlete("蒙娜丽莎", Gender::FEMALE, unitNameToId["艺术学院"]);
    }
    std::cout << "默认运动员添加完毕。" << std::endl;


    // 4. 设置运动员参赛项目限制
    setAthleteMaxEventsAllowed(3); // 每人最多报3项

    // 5. 设置计分规则 (与README.md一致)
    // 规则1: 参赛人数 > 6人 (即 >= 7人)，取前5名
    std::map<int, double> scores1 = {{1, 7.0}, {2, 5.0}, {3, 3.0}, {4, 2.0}, {5, 1.0}};
    addScoreRule("大于6人取前5名 (7,5,3,2,1)", 7, -1, 5, scores1); // minParticipants = 7, maxParticipants = -1 (无上限)

    // 规则2: 参赛人数 <= 6人 (且 >= 4人，因为不足4人取消), 取前3名
    std::map<int, double> scores2 = {{1, 5.0}, {2, 3.0}, {3, 2.0}};
    addScoreRule("4至6人取前3名 (5,3,2)", 4, 6, 3, scores2); // minParticipants = 4, maxParticipants = 6

    // 6. 项目成立的最小参赛人数
    setMinParticipantsToHoldEvent(4); // 不足4人的项目将取消 (README中是 < 4人取消，即至少4人)

    resetAllUnitScores(); // 初始化后确保所有单位分数为0

    std::cout << "系统默认设置初始化完成。" << std::endl;
}

std::vector<std::reference_wrapper<const Athlete>> SystemSettings::getAllAthlesConst() const {
    std::vector<std::reference_wrapper<const Athlete>> refs;
    refs.reserve(athletes.size()); // athletes_ 是 std::map<int, Athlete> m_athletes;
    for (const auto& val : athletes | std::views::values) {
        refs.push_back(std::cref(val)); // 从 pair 中取出 Athlete 对象
    }
    return refs;
}
