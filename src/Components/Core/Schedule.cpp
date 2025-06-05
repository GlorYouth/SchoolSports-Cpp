

#include "../../../include/Components/Core/Schedule.h"
#include <iostream> // 用于打印等
#include <ranges>
#include <sstream>
#include <algorithm> // 为 std::sort 添加此头文件
#include <queue> // 优先级队列
#include <unordered_map> // 哈希映射
#include <set> // 集合
#include <chrono> // 用于计时

// 定义场地类型枚举
enum class VenueType {
    Track,    // 田径场
    JumpArea, // 跳远区
    ThrowArea // 投掷区
};

// 从场地名称推断场地类型
VenueType inferVenueType(const std::string& venueName) {
    if (venueName.find("田径场") != std::string::npos) {
        return VenueType::Track;
    } else if (venueName.find("跳远区") != std::string::npos) {
        return VenueType::JumpArea;
    } else if (venueName.find("投掷区") != std::string::npos) {
        return VenueType::ThrowArea;
    }
    // 默认为田径场
    return VenueType::Track;
}

// 从项目名称推断合适的场地类型
std::vector<VenueType> inferSuitableVenueTypes(const std::string& eventName) {
    std::vector<VenueType> types;
    
    // 跑步类项目
    if (eventName.find("米") != std::string::npos || 
        eventName.find("接力") != std::string::npos) {
        types.push_back(VenueType::Track);
    }
    // 跳跃类项目
    else if (eventName.find("跳远") != std::string::npos || 
             eventName.find("跳高") != std::string::npos || 
             eventName.find("三级跳") != std::string::npos) {
        types.push_back(VenueType::JumpArea);
    }
    // 投掷类项目
    else if (eventName.find("铅球") != std::string::npos || 
             eventName.find("标枪") != std::string::npos || 
             eventName.find("铁饼") != std::string::npos || 
             eventName.find("链球") != std::string::npos) {
        types.push_back(VenueType::ThrowArea);
    }
    // 未明确的项目可以使用所有类型的场地
    else {
        types.push_back(VenueType::Track);
        types.push_back(VenueType::JumpArea);
        types.push_back(VenueType::ThrowArea);
    }
    
    return types;
}

// 检查场地是否适合特定项目
bool isVenueSuitableForEvent(const std::string& venueName, const std::string& eventName) {
    VenueType venueType = inferVenueType(venueName);
    std::vector<VenueType> suitableTypes = inferSuitableVenueTypes(eventName);
    
    bool suitable = std::find(suitableTypes.begin(), suitableTypes.end(), venueType) != suitableTypes.end();
    
    // 添加调试日志
    if (suitable) {
        std::cout << "场地适配: 项目[" << eventName << "] 可使用场地[" << venueName << "]" << std::endl;
    } else {
        std::cout << "场地适配: 项目[" << eventName << "] 不适合使用场地[" << venueName << "]" << std::endl;
    }
    
    return suitable;
}

Schedule::Schedule(SystemSettings& sysSettings) : settings(sysSettings) {}

bool Schedule::generateSchedule() {
    auto startTime = std::chrono::high_resolution_clock::now(); // 计时开始
    std::cout << "正在智能生成秩序册..." << std::endl;
    scheduleEntries.clear();
    
    // 1. 收集所有未取消项目的引用
    struct EventInfo {
        int id;
        int duration;
        int participantCount;
        float priority;  // 优先级（用于排序）
        
        bool operator<(const EventInfo& other) const {
            return priority < other.priority; // 低优先级先出队
        }
    };
    
    std::vector<EventInfo> eventInfos;
    eventInfos.reserve(settings.events.getAllConst().size());
    
    for (const auto& [id, eventRef] : settings.events.getAllConst()) {
        const auto& event = eventRef.get();
        if (!event.getIsCancelled()) {
            int duration = event.getDurationMinutes();
            int participants = event.getParticipantCount();
            
            // 优先级计算：持续时间、参与人数、项目重要性等因素的加权和
            // 值越大优先级越高（后面用最大堆）
            float priority = 0.6f * duration + 0.4f * participants;
            
            // 如果项目名称中包含"决赛"等关键词，提高优先级
            std::string name = event.getName();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (name.find("决赛") != std::string::npos || 
                name.find("final") != std::string::npos) {
                priority += 50.0f;
            }
            
            eventInfos.push_back({id, duration, participants, priority});
        }
    }
    
    // 使用优先级队列替代简单排序
    std::priority_queue<EventInfo> eventQueue(eventInfos.begin(), eventInfos.end());
    
    if (eventQueue.empty()) {
        std::cout << "警告: 没有找到可安排的有效项目。" << std::endl;
        return false;
    }
    
    // 2. 获取所有场地
    const auto& venues = settings.venues.getAll();
    if (venues.empty()) {
        std::cout << "错误：场地表为空，请先添加场地。" << std::endl;
        return false;
    }
    
    std::vector<std::string> venueList(venues.begin(), venues.end());
    
    // 维护场地适用性评分
    struct VenueScore {
        std::string name;
        int usageCount = 0;      // 使用次数
        int totalDuration = 0;   // 累计使用时间（分钟）
        float score = 0.0f;      // 综合评分（越低越优先使用）
    };
    
    std::unordered_map<std::string, VenueScore> venueScores;
    for (const auto& venue : venueList) {
        venueScores[venue] = {venue};
    }
    
    // 3. 获取上午/下午时间段
    auto [morningStart, morningEnd] = settings.sessions.getMorningSession();
    auto [afternoonStart, afternoonEnd] = settings.sessions.getAfternoonSession();
    
    // 辅助函数：时间字符串转分钟
    auto timeToMin = [](const std::string& t) -> int {
        if (t.size() != 5 || t[2] != ':') return -1;
        return std::stoi(t.substr(0,2))*60 + std::stoi(t.substr(3,2));
    };
    
    // 辅助函数：分钟数转时间字符串
    auto minToTime = [](int minutes) -> std::string {
        char buf[6];
        snprintf(buf, 6, "%02d:%02d", minutes/60, minutes%60);
        return std::string(buf);
    };
    
    int morningStartMin = timeToMin(morningStart), morningEndMin = timeToMin(morningEnd);
    int afternoonStartMin = timeToMin(afternoonStart), afternoonEndMin = timeToMin(afternoonEnd);
    if (morningStartMin < 0 || morningEndMin <= morningStartMin || 
        afternoonStartMin < 0 || afternoonEndMin <= afternoonStartMin) {
        std::cout << "错误：上午/下午时间段设置不合法。" << std::endl;
        return false;
    }
    
    // 数据结构优化：使用区间树或更高效的数据结构表示时间段
    struct TimeSlot {
        int start;
        int end;
    };
    
    // 场地-天-时段（上午/下午）到已占用时间槽的映射
    using VenueDaySession = std::tuple<std::string, int, bool>; // 场地, 天, isMorning
    std::map<VenueDaySession, std::vector<TimeSlot>> venueSchedule;
    
    // 运动员时间安排（用于检测冲突）
    struct AthleteScheduleEntry {
        int day;
        int eventId;
        TimeSlot slot;
    };
    std::unordered_map<int, std::vector<AthleteScheduleEntry>> athleteSchedules;
    
    // 辅助函数：检测运动员时间冲突
    auto hasAthleteConflict = [&](int day, const TimeSlot& newSlot, const std::vector<int>& athleteIds) -> bool {
        for (int athleteId : athleteIds) {
            if (athleteSchedules.find(athleteId) != athleteSchedules.end()) {
                for (const auto& entry : athleteSchedules[athleteId]) {
                    if (entry.day == day) {
                        // 检查时间是否重叠
                        if (!(newSlot.end <= entry.slot.start || newSlot.start >= entry.slot.end)) {
                            // 有冲突
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    };
    
    // 辅助函数：更新场地评分
    auto updateVenueScores = [&]() {
        for (auto& [name, score] : venueScores) {
            // 综合考虑使用次数和总时长，分配更均衡
            score.score = 0.7f * score.usageCount + 0.3f * (score.totalDuration / 60.0f);
        }
    };
    
    // 找到评分最低（最优先使用）的场地
    auto getBestVenue = [&]() -> std::string {
        updateVenueScores();
        std::string bestVenue = venueList[0];
        float minScore = std::numeric_limits<float>::max();
        
        for (const auto& [name, score] : venueScores) {
            if (score.score < minScore) {
                minScore = score.score;
                bestVenue = name;
            }
        }
        
        return bestVenue;
    };
    
    // 辅助函数：在给定场地、日期、时段中找到最佳时间槽
    auto findBestTimeSlot = [&](const std::string& venue, int day, bool isMorning, int duration, 
                               const std::vector<int>& athleteIds) -> std::optional<TimeSlot> {
        int sessionStart = isMorning ? morningStartMin : afternoonStartMin;
        int sessionEnd = isMorning ? morningEndMin : afternoonEndMin;
        auto& slots = venueSchedule[{venue, day, isMorning}];
        
        // 按开始时间排序现有槽位
        std::ranges::sort(slots, [](const TimeSlot& a, const TimeSlot& b) {
            return a.start < b.start;
        });
        
        // 尝试找出所有可能的时间槽
        std::vector<TimeSlot> candidates;
        
        // 检查第一个槽位前的空间
        if (slots.empty() || slots[0].start - sessionStart >= duration) {
            candidates.push_back({sessionStart, sessionStart + duration});
        }
        
        // 检查槽位之间的间隔
        for (size_t i = 0; i < slots.size(); i++) {
            int nextStart = (i < slots.size() - 1) ? slots[i + 1].start : sessionEnd;
            int gapStart = slots[i].end;
            if (nextStart - gapStart >= duration) {
                candidates.push_back({gapStart, gapStart + duration});
            }
        }
        
        // 检查最后一个槽位后的空间
        if (!slots.empty() && sessionEnd - slots.back().end >= duration) {
            candidates.push_back({slots.back().end, slots.back().end + duration});
        }
        
        // 检查所有候选槽位是否有运动员冲突
        for (const auto& candidate : candidates) {
            if (!hasAthleteConflict(day, candidate, athleteIds)) {
                return candidate;
            }
        }
        
        // 没有找到合适的槽位
        return std::nullopt;
    };
    
    int maxDay = 5; // 最大排程天数，可以根据需要调整
    int successfullyScheduledCount = 0;
    int totalEvents = eventQueue.size();
    
    // 开始排程过程
    while (!eventQueue.empty()) {
        EventInfo eventInfo = eventQueue.top();
        eventQueue.pop();
        
        // 获取项目的可修改引用
        auto eventOpt = settings.events.get(eventInfo.id);
        if (!eventOpt.has_value()) {
            std::cout << "错误：项目ID " << eventInfo.id << " 无效，跳过此项目。" << std::endl;
            continue;
        }
        
        auto& event = eventOpt.value().get();
        const std::vector<int>& participantIds = event.getParticipantAthleteIds();
        
        bool scheduled = false;
        
        // 先尝试为项目寻找最佳安排（考虑天、场地和时段）
        for (int day = 1; day <= maxDay && !scheduled; ++day) {
            // 优先使用最佳场地（根据评分）
            std::vector<std::string> sortedVenues = venueList;
            std::ranges::sort(sortedVenues, [&](const std::string& a, const std::string& b) {
                return venueScores[a].score < venueScores[b].score;
            });
            
            // 筛选出适合当前项目的场地
            std::vector<std::string> suitableVenues;
            for (const auto& venue : sortedVenues) {
                if (isVenueSuitableForEvent(venue, event.getName())) {
                    suitableVenues.push_back(venue);
                }
            }
            
            // 如果没有找到适合的场地，使用所有场地（降级策略）
            if (suitableVenues.empty()) {
                std::cerr << "警告：项目[" << event.getName() << "]没有找到适合的场地类型，使用任意可用场地。" << std::endl;
                suitableVenues = sortedVenues;
            } else {
                std::cout << "为项目[" << event.getName() << "]找到" << suitableVenues.size() << "个适合的场地。" << std::endl;
            }
            
            // 使用适合的场地列表
            for (const auto& venue : suitableVenues) {
                // 优先尝试上午时段
                for (bool isMorning : {true, false}) {
                    auto timeSlotOpt = findBestTimeSlot(venue, day, isMorning, eventInfo.duration, participantIds);
                    
                    if (timeSlotOpt.has_value()) {
                        TimeSlot slot = timeSlotOpt.value();
                        
                        // 更新项目信息
                        event.setVenue(venue);
                        std::string startTimeStr = minToTime(slot.start);
                        std::string endTimeStr = minToTime(slot.end);
                        
                        event.setStartTime(startTimeStr);
                        event.setEndTime(endTimeStr);
                        
                        // 检查时间是否成功设置
                        if (event.getStartTime() != startTimeStr || event.getEndTime() != endTimeStr) {
                            std::cerr << "警告：项目 \"" << event.getName() << "\" 的时间设置未成功应用。"
                                      << "可能是由于时间格式验证失败。" << std::endl;
                            continue;
                        }
                        
                        // 更新场地使用信息
                        venueSchedule[{venue, day, isMorning}].push_back(slot);
                        venueScores[venue].usageCount++;
                        venueScores[venue].totalDuration += eventInfo.duration;
                        updateVenueScores();
                        
                        // 更新运动员时间安排
                        for (int athleteId : participantIds) {
                            athleteSchedules[athleteId].push_back({day, eventInfo.id, slot});
                        }
                        
                        // 记录到赛程表
                        ScheduleEntry entry;
                        entry.eventId = eventInfo.id;
                        entry.venue = venue;
                        entry.startTime = startTimeStr;
                        entry.endTime = endTimeStr;
                        scheduleEntries.push_back(entry);
                        
                        std::cout << "项目[" << event.getName() << "] (优先级:" << eventInfo.priority 
                                  << ") 安排在 第" << day << "天 场地[" << venue 
                                  << "] 时间[" << startTimeStr << "-" << endTimeStr << "]" << std::endl;
                        
                        scheduled = true;
                        successfullyScheduledCount++;
                        break;
                    }
                }
                
                if (scheduled) {
                    break;
                }
            }
        }
        
        if (!scheduled) {
            std::cerr << "警告：无法为项目ID " << eventInfo.id << " (" << event.getName() 
                      << ") 找到合适的时间段和场地，请手动安排。" << std::endl;
        }
    }
    
    // 计算耗时
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\n赛程生成完成！成功安排 " << successfullyScheduledCount << "/" << totalEvents 
              << " 个项目 (耗时: " << duration.count() << "ms)。" << std::endl;
    
    // 场地使用统计
    std::cout << "\n场地使用统计:" << std::endl;
    for (const auto& [name, score] : venueScores) {
        std::cout << "  场地[" << name << "]: 使用" << score.usageCount << "次，总时长" 
                  << (score.totalDuration / 60) << "小时" << (score.totalDuration % 60) << "分钟" << std::endl;
    }
    
    return successfullyScheduledCount == totalEvents;
}

const std::vector<ScheduleEntry>& Schedule::getScheduleEntries() const {
    return scheduleEntries;
}

void Schedule::printSchedule() const {
    std::cout << "\n---------- 秩序册 ----------" << std::endl;
    if (scheduleEntries.empty()) {
        std::cout << "秩序册为空或尚未生成。" << std::endl;
        return;
    }
    for (const auto& entry : scheduleEntries) {
        if (auto event = settings.events.get(entry.eventId); event.has_value()) {
            auto event_ref = event.value().get();
            std::cout << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                      << "\n  时间: " << entry.startTime << " - " << entry.endTime
                      << "\n  场地: " << entry.venue << std::endl;
            std::cout << "  参赛人员: ";
            if (event_ref.getParticipantCount() > 0) {
                for(size_t i = 0; i < event_ref.getParticipantAthleteIds().size(); ++i) {
                    auto ath = settings.athletes.get(event_ref.getParticipantAthleteIds()[i]);
                    if (ath.has_value()) std::cout << ath.value().get().getName() << (i == event_ref.getParticipantAthleteIds().size() -1 ? "" : ", ");
                }
            } else {
                std::cout << "无";
            }
            std::cout << std::endl << std::endl;
        }
    }
    std::cout << "--------------------------" << std::endl;
}

// 新增：获取秩序册内容的字符串表示
std::string Schedule::getScheduleContentAsString() const {
    std::ostringstream oss;
    oss << "\n---------- 秩序册 ----------\n"; // 使用中文标点和换行
    if (scheduleEntries.empty()) {
        oss << "秩序册为空或尚未生成。\n"; // 使用中文标点和换行
        return oss.str();
    }
    for (const auto& entry : scheduleEntries) {
        auto eventOpt = settings.events.getConst(entry.eventId); // 使用 getCompetitionEventConst
        if (eventOpt) { // 检查 optional 是否有值
            const CompetitionEvent& event_ref = eventOpt.value().get();
            oss << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                << "\n  时间: " << entry.startTime << " - " << entry.endTime
                << "\n  地点: " << entry.venue << "\n"; // 使用中文标点和换行
            oss << "  参赛运动员: ";
            if (event_ref.getParticipantCount() > 0) {
                const auto& participantIds = event_ref.getParticipantAthleteIds();
                for (size_t i = 0; i < participantIds.size(); ++i) {
                    auto athOpt = settings.athletes.getConst(participantIds[i]); // 使用 getAthleteConst
                    if (athOpt) { // 检查 optional 是否有值
                        oss << athOpt.value().get().getName() << (i == participantIds.size() - 1 ? "" : ", ");
                    }
                }
            } else {
                oss << "无"; // 使用中文
            }
            oss << "\n\n"; // 两个换行以分隔条目
        }
    }
    oss << "--------------------------\n"; // 使用中文标点和换行
    return oss.str();
}

bool Schedule::validateSchedule() const {
    // 在此实现赛程验证逻辑
    // 例如检查时间冲突、场地冲突等
    std::cout << "正在验证赛程 (当前为占位符)..." << std::endl;
    return true; // 假设总是有效
}
