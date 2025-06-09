#ifndef BACKUPDATA_H
#define BACKUPDATA_H

#include "Event.h"       // For enums like EventType, GenderCategory
#include "ScoringRule.h" // For ScoringRule
#include <string>
#include <vector>

// 注意：以下是用于数据备份的"扁平化"结构，不含指针

struct ResultData {
    std::string athleteId;
    int rank;
    std::string performance;
    double sortablePerformance;
    int pointsAwarded;
};

struct EventData {
    std::string name;
    EventType type;
    GenderCategory gender;
    int duration;
    int minParticipants;
    int maxParticipants;
    std::vector<std::string> participantIds;
    std::vector<ResultData> results;
};

struct AthleteData {
    std::string name;
    std::string id;
    bool isMale;
    std::string unitName; // 用单位名称代替指针
    int individualScore;
    // 参加的项目通过 EventData 中的 participantIds 反向关联
};

struct UnitData {
    std::string name;
    int totalScore;
    std::vector<AthleteData> athletes;
};

/**
 * @brief 顶级备份类，包含整个运动会的所有可序列化数据
 */
struct BackupData {
    int maxEventsPerAthlete;
    int minParticipantsForCancel;
    std::vector<ScoringRule> allScoringRules;
    std::vector<UnitData> allUnits;
    std::vector<EventData> allEvents; // 统一的事件列表
};

#endif // BACKUPDATA_H 