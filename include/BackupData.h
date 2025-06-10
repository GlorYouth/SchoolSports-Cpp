#ifndef BACKUPDATA_H
#define BACKUPDATA_H

#include <string>
#include <vector>
#include <map>
#include "ScoringRule.h"
#include "Result.h"

// 用于序列化的运动员数据结构
struct AthleteData {
    std::string id;
    std::string name;
    std::string gender;
    double score;
    std::vector<std::string> registeredEvents;
};

// 用于序列化的单位数据结构
struct UnitData {
    std::string name;
    double score;
    std::vector<AthleteData> athletes;
};

// 用于序列化的项目数据结构
struct EventData {
    std::string name;
    std::string gender;
    bool isTimeBased;
    bool isCancelled;
    std::string scoringRuleName;
    std::vector<std::string> registeredAthletes;
};

// 用于序列化的总数据包
struct BackupData {
    int maxEventsPerAthlete;
    int minParticipantsForCancel;
    std::vector<ScoringRule> allScoringRules;
    std::vector<UnitData> allUnits;
    std::vector<EventData> allEvents;
    std::map<std::string, std::vector<Result>> allEventResults;
};

#endif // BACKUPDATA_H 