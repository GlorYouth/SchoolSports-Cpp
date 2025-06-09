#ifndef RESULT_H
#define RESULT_H

#include <string>

struct Result {
    std::string athleteId;
    double performance;
    int rank = 0;
    int points = 0;

    // Constructors
    Result() = default;
    Result(std::string id, double perf) : athleteId(id), performance(perf) {}
};

#endif // RESULT_H 