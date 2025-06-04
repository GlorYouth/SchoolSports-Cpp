//
// Created by GlorYouth on 2025/6/2.
//

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include "SystemSettings.h"

// 数据管理类 (负责数据持久化，如备份和恢复)
/**
 * @brief 负责系统数据的持久化操作，如备份、恢复和导入示例数据。
 *
 * DataManager 类与 SystemSettings 紧密协作，以访问和修改系统中的核心数据，
 * 包括单位、运动员、比赛项目、成绩、计分规则等。
 * 它提供了将当前系统状态保存到文件以及从文件加载数据的功能。
 * 同时，它也包含一个用于快速填充系统以进行测试或演示的示例数据加载机制。
 */
class DataManager {
private:
    SystemSettings& settings; ///< 对系统设置的引用，用于访问和修改需要持久化的数据。

public:
    /**
     * @brief DataManager 类的构造函数。
     * @param sysSettings 对 SystemSettings 对象的引用，数据管理操作将基于此设置进行。
     */
    explicit DataManager(SystemSettings& sysSettings);

    /**
     * @brief 将当前系统的所有核心数据备份到指定的文件。
     *
     * 备份内容通常包括：所有单位、运动员、比赛项目、已录入的成绩、自定义的计分规则等。
     * 数据的序列化格式（如JSON, XML, 二进制）由具体实现决定。
     * @param filePath 用于保存备份数据的文件路径。
     * @return 如果备份成功，返回 true；否则返回 false（例如，文件无法创建或写入错误）。
     */
    [[nodiscard]] bool backupData(const std::string& filePath) const;

    /**
     * @brief 从指定的文件恢复系统数据。
     *
     * 此操作会清除当前系统中的大部分数据（除了默认设置，如计分规则），
     * 然后从文件中加载并重建单位、运动员、项目、成绩等。
     * @param filePath 包含备份数据的文件路径。
     * @return 如果数据成功恢复，返回 true；否则返回 false（例如，文件不存在、格式错误或数据不一致）。
     */
    bool restoreData(const std::string& filePath);

    /**
     * @brief 向当前系统中加载一套预定义的示例数据。
     *
     * 此方法用于快速填充系统，以便进行功能测试或演示。
     * 它会添加一些示例单位、运动员、比赛项目以及它们之间的报名关系。
     * @note 此方法不应该清除或覆盖由 `SystemSettings::initializeDefaultSettings` 初始化的默认计分规则。
     *       它主要用于添加实体数据，而不是配置数据。
     *       如果需要一套完整的、隔离的演示环境，建议先清理数据再调用此方法。
     *       注意与 `AutoTest::importSampleData` 的区别，后者是为自动化测试设计的，可能包含更特定的数据集。 
     * @return 如果示例数据加载成功，返回 true；否则返回 false。
     */
    bool loadSampleData();

    // 其他数据管理功能，如导入导出特定格式数据等
};


#endif //DATAMANAGER_H
