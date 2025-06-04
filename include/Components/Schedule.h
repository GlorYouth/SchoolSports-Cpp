//
// Created by GlorYouth on 2025/6/2.
//

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>
#include <string>
#include "CompetitionEvent.h"
#include "SystemSettings.h"

// 单个赛程条目
/**
 * @brief 代表赛程表中的一个单独条目。
 *
 * ScheduleEntry 结构体用于存储单个比赛项目在赛程中的具体安排，
 * 包括项目ID、开始时间、结束时间以及比赛场地。
 */
struct ScheduleEntry {
    int eventId;        ///< 对应的比赛项目ID。
    std::string startTime; ///< 项目开始时间，格式可为 "YYYY-MM-DD HH:MM" 或简化如 "Day1 09:00"。
    std::string endTime;   ///< 项目结束时间。
    std::string venue;     ///< 比赛场地。
    // 可以添加其他信息，如裁判等
};

// 赛程管理类 (秩序册生成)
/**
 * @brief 负责整个运动会赛程（秩序册）的生成与管理。
 *
 * Schedule 类依赖于系统设置（SystemSettings）来获取比赛项目列表和时间约束等信息。
 * 其核心功能是自动生成赛程，同时需要考虑场地冲突、运动员时间冲突、
 * 运动员连续项目间隔、总时长、上下午比赛均衡等多种复杂因素。
 * 生成的赛程可以被打印或导出。
 */
class Schedule {
private:
    std::vector<ScheduleEntry> scheduleEntries; ///< 存储所有赛程条目的列表。
    SystemSettings& settings; ///< 对系统设置的引用，用于获取项目、时间等信息。

public:
    /**
     * @brief Schedule 类的构造函数。
     * @param sysSettings 对 SystemSettings 对象的引用，赛程生成将依赖此设置。
     */
    explicit Schedule(SystemSettings& sysSettings);

    /**
     * @brief 自动生成整个运动会的赛程（秩序册）。
     * 这是赛程管理的核心功能，需要实现复杂的调度算法以满足各种约束条件。
     * @return 如果赛程成功生成，返回 true；若因约束无法满足等原因导致失败，则返回 false。
     * @note 内部会调用 CompetitionEvent 的 setStartTime, setEndTime, setVenue。
     */
    bool generateSchedule();

    /**
     * @brief 获取已生成的完整赛程表。
     * @return 返回一个包含所有 ScheduleEntry 对象的常量vector引用。
     */
    [[nodiscard]] const std::vector<ScheduleEntry>& getScheduleEntries() const;

    // 手动调整赛程 (可选)
    // bool adjustScheduleEntry(int entryIndex, const ScheduleEntry& newEntry);

    /**
     * @brief 打印（在控制台输出）已生成的赛程表。
     * @note 未来可以扩展为导出到特定格式的文件。
     */
    void printSchedule() const; // 或者 exportToFormat()

    /**
     * @brief 获取赛程表内容的字符串表示形式。
     * @return 返回一个包含整个赛程格式化文本的字符串。
     */
    std::string getScheduleContentAsString() const; // 新增：获取秩序册内容的字符串表示

    /**
     * @brief 验证当前赛程安排的有效性。
     * 例如，检查是否有时间冲突、场地冲突等。
     * @return 如果赛程有效，返回 true；否则返回 false。
     */
    [[nodiscard]] bool validateSchedule() const;
};

#endif //SCHEDULE_H
