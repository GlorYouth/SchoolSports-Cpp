//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/Result.h"
#include <algorithm> // 如果在 EventResults 内部需要排序，则包含此文件

// Result 类实现
Result::Result(int evId, int athId, int r, const std::string& sr, double pts)
    : eventId(evId), athleteId(athId), rank(r), scoreRecord(sr), pointsAwarded(pts) {}

int Result::getEventId() const { return eventId; }
int Result::getAthleteId() const { return athleteId; }
int Result::getRank() const { return rank; }
std::string Result::getScoreRecord() const { return scoreRecord; }
double Result::getPointsAwarded() const { return pointsAwarded; }

void Result::setScoreRecord(const std::string& score) { scoreRecord = score; }
void Result::setRank(int r) { rank = r; }
void Result::setPointsAwarded(double p) { pointsAwarded = p; }


// EventResults 类实现
EventResults::EventResults(int evId) : eventId(evId), finalized(false) {}

int EventResults::getEventId() const { // getEventId 方法的实现
    return eventId;
}

void EventResults::addResult(const Result& result) {
    // 实际应用中，添加后可能需要立即排序或在finalize时排序
    resultsList.push_back(result);
}

const std::vector<Result>& EventResults::getResultsList() const {
    return resultsList;
}

void EventResults::finalizeResults() {
    // 在这里可以进行排序、计算最终积分等操作
    // 例如，根据成绩记录对 resultsList进行排序
    // std::sort(resultsList.begin(), resultsList.end(), Result::compareTrackResults); // 或 field
    finalized = true;
}

bool EventResults::isFinalized() const {
    return finalized;
}

std::optional<std::reference_wrapper<Result>> EventResults::getResultForAthlete(int athleteId) {
    for (auto& res : resultsList) {
        if (res.getAthleteId() == athleteId) {
            return res;
        }
    }
    return std::nullopt;
}