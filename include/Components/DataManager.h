//
// Created by GlorYouth on 2025/6/2.
//

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <vector>
#include <utility>
#include "SystemSettings.h"
#include "DataFileManager.h"

// 数据管理器 (处理数据持久化如备份和恢复)
/**
 * @brief 管理系统数据的持久化、导入样例数据、恢复和加载示例数据。
 *
 * DataManager 依赖 SystemSettings 对象协作来访问和修改系统中的核心数据，
 * 包括参赛单位、运动员、比赛项目、成绩和计分规则等。
 * 它提供了将当前系统状态保存到文件以及从文件加载数据的功能。
 * 同时，它也包含一个用于开发和系统测试的示例数据集生成机制。
 */
class DataManager {
private:
    SystemSettings& settings; ///< 对系统设置的引用，用于访问和修改需要持久化的数据。
    DataFileManager fileManager; ///< 文件管理器，负责具体的文件读写操作
    
    // 跟踪示例数据的导入状态
    bool stage1DataImported; ///< 阶段1数据（单位、场地、计分规则）是否已导入
    bool stage2DataImported; ///< 阶段2数据（运动员）是否已导入
    bool stage3DataImported; ///< 阶段3数据（比赛成绩）是否已导入

public:
    /**
     * @brief DataManager 类的构造函数。
     * @param sysSettings 对 SystemSettings 对象的引用，数据管理器用来处理该对象进行操作。
     */
    explicit DataManager(SystemSettings& sysSettings);

    /**
     * @brief 将当前系统中的所有核心数据备份到指定的文件中。
     *
     * 包括存储所有单位、运动员、比赛项目、预设的计分规则及已记录的成绩等。
     * 数据的序列化格式（如JSON, XML, 或自定义）由具体实现决定。
     * @param filePath 用于备份保存数据的文件路径。
     * @return 如果数据成功备份返回 true，否则返回 false（例如，文件无法创建或写入错误）。
     */
    [[nodiscard]] bool backupData(const std::string& filePath) const;

    /**
     * @brief 从指定的文件恢复系统数据。
     *
     * 此操作会覆盖当前系统中的大部分数据（如默认设置、计分规则），
     * 然后从文件中加载并重构所有单位、运动员、项目、成绩等。
     * @param filePath 包含备份数据的文件路径或者备份文件ID。
     * @return 如果数据成功恢复返回 true，否则返回 false（例如，文件不存在、格式错误、数据不一致）。
     */
    bool restoreData(const std::string& filePath);
    
    /**
     * @brief 获取备份文件夹中的所有备份文件
     * @return 按时间排序的备份文件列表，每个元素包含ID和文件路径
     */
    std::vector<std::pair<int, std::string>> getBackupFiles() const;
    
    /**
     * @brief 确保备份目录存在
     * @return 如果备份目录存在或成功创建返回true，否则返回false
     */
    bool ensureBackupDirectoryExists() const;

    /**
     * @brief 向当前系统中加载一套预定义示例数据。
     *
     * 此方法用于快速填充系统以测试或展示。
     * 它会添加一些示例单位、运动员、比赛项目以及它们之间的基本关系。
     * @note 此方法会覆盖已初始化的默认计分规则
     * @return 如果示例数据集加载成功返回 true，否则返回 false。
     */
    bool loadSampleData();
    
    /**
     * @brief 加载阶段1示例数据：单位、场地、计分规则
     * @return 如果加载成功返回 true，否则返回 false。
     */
    bool loadSampleStage1Data();
    
    /**
     * @brief 加载阶段2示例数据：运动员及其报名信息。
     * @return 如果加载成功返回 true，否则返回 false。
     */
    bool loadSampleStage2Data();
    
    /**
     * @brief 加载阶段3示例数据：比赛成绩。
     * @return 如果加载成功返回 true，否则返回 false。
     */
    bool loadSampleStage3Data();
    
    /**
     * @brief 检查阶段1数据是否已导入
     * @return 如果阶段1数据已导入，返回true，否则返回false
     */
    bool isStage1DataImported() const { return stage1DataImported; }
    
    /**
     * @brief 检查阶段2数据是否已导入
     * @return 如果阶段2数据已导入，返回true，否则返回false
     */
    bool isStage2DataImported() const { return stage2DataImported; }
    
    /**
     * @brief 检查阶段3数据是否已导入
     * @return 如果阶段3数据已导入，返回true，否则返回false
     */
    bool isStage3DataImported() const { return stage3DataImported; }
    
    /**
     * @brief 重置示例数据导入状态
     */
    void resetImportStatus();

    // 更多数据管理功能，如导入导出特定格式数据等
};


#endif //DATAMANAGER_H
