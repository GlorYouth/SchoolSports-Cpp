#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <vector>

class Event {
public:
    std::string name;
    std::string gender; // "男" or "女"
    bool isTimeBased; // true for track, false for field
    std::vector<std::string> registeredAthletes;
    bool isCancelled = false;

    Event(const std::string& name, const std::string& gender, bool isTimeBased);

    void addAthlete(const std::string& athleteId);
    void removeAthlete(const std::string& athleteId);
};

#endif // EVENT_H