#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>
#include "ScoringRule.h"

class Event {
public:
    std::string name;
    std::string gender; // "男" or "女"
    bool isTimeBased; // true for track, false for field
    std::vector<std::string> registeredAthletes;
    bool isCancelled = false;
    ScoringRule scoringRule;

    Event(const std::string& name, const std::string& gender, bool isTimeBased, const ScoringRule& rule);

    void addAthlete(const std::string& athleteId);
    void removeAthlete(const std::string& athleteId);
};

#endif // EVENT_H