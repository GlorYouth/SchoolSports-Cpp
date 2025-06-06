#include "../../../include/Components/Manager/DataFileManager.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem> // 添加C++17文件系统库
#include <algorithm>  // 用于排序

#include "../../../include/Components/Core/Unit.h"
#include "../../../include/Components/Core/Athlete.h"
#include "../../../include/Components/Core/CompetitionEvent.h"
#include "../../../include/Components/Core/ScoreRule.h"
#include "../../../include/Components/Core/Result.h"
#include "../../../include/Components/Core/Constants.h"
#include "../../../include/utils.h"

// 定义命名空间别名，简化代码
namespace fs = std::filesystem;

DataFileManager::DataFileManager() {
    // 构造函数暂无需初始化内容
}

std::string DataFileManager::getBackupDirectoryPath() const {
    return "backup";
}

bool DataFileManager::ensureBackupDirectoryExists() const {
    std::string backupDir = getBackupDirectoryPath();
    
    try {
        // 检查目录是否存在
        if (!fs::exists(backupDir)) {
            // 创建目录
            bool created = fs::create_directory(backupDir);
            if (!created) {
                std::cerr << "错误: 无法创建备份目录: " << backupDir << std::endl;
                return false;
            }
            std::cout << "已创建备份目录: " << backupDir << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "创建备份目录时发生错误: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::pair<int, std::string>> DataFileManager::listBackupFiles() const {
    std::vector<std::pair<int, std::string>> result;
    std::string backupDir = getBackupDirectoryPath();
    
    // 确保备份目录存在
    if (!ensureBackupDirectoryExists()) {
        return result; // 返回空列表
    }
    
    try {
        // 收集所有.dat文件
        std::vector<fs::directory_entry> entries;
        for (const auto& entry : fs::directory_iterator(backupDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".dat") {
                entries.push_back(entry);
            }
        }
        
        // 按最后修改时间排序（降序）
        std::sort(entries.begin(), entries.end(), 
            [](const fs::directory_entry& a, const fs::directory_entry& b) {
                return fs::last_write_time(a.path()) > fs::last_write_time(b.path());
            });
        
        // 为每个文件分配ID
        int id = 1;
        for (const auto& entry : entries) {
            result.emplace_back(id++, entry.path().string());
        }
    } catch (const std::exception& e) {
        std::cerr << "列出备份文件时发生错误: " << e.what() << std::endl;
    }
    
    return result;
}

std::string DataFileManager::getBackupFilePathById(int id) const {
    auto backupFiles = listBackupFiles();
    
    for (const auto& [fileId, filePath] : backupFiles) {
        if (fileId == id) {
            return filePath;
        }
    }
    
    return ""; // 未找到对应ID的文件
}

std::string DataFileManager::generateDefaultBackupFilename() const {
    // 获取当前时间
    const time_t now = time(nullptr);
    const tm* localTime = localtime(&now);
    
    // 格式化时间为字符串
    std::stringstream ss;
    ss << getBackupDirectoryPath() << "/SchoolSports_" 
       << std::put_time(localTime, "%Y-%m-%d_%H-%M-%S") << ".dat";
    
    return ss.str();
}

bool DataFileManager::saveDataToFile(const SystemSettings& settings, const std::string& filePath) const {
    // 确保备份目录存在
    if (!ensureBackupDirectoryExists()) {
        std::cerr << "错误: 无法确保备份目录存在，保存操作取消" << std::endl;
        return false;
    }
    
    std::string actualFilePath = filePath;
    
    // 如果没有提供文件路径，创建默认文件名与时间戳
    if (actualFilePath.empty()) {
        actualFilePath = generateDefaultBackupFilename();
    } else if (actualFilePath.find('/') == std::string::npos && actualFilePath.find('\\') == std::string::npos) {
        // 如果只提供了文件名而非完整路径，则将其放在备份目录中
        actualFilePath = getBackupDirectoryPath() + "/" + actualFilePath;
    }
    
    // 打开文件
    std::ofstream outFile(actualFilePath);
    if (!outFile.is_open()) {
        std::cerr << "错误: 无法打开文件进行备份: " << actualFilePath << std::endl;
        return false;
    }

    std::cout << "开始保存数据到 " << actualFilePath << " ..." << std::endl;

    // 使用DataContainer的序列化方法
    bool success = settings.getDataConst().serialize(outFile);
    
    outFile.close();
    
    if (success) {
        std::cout << "数据备份成功完成，文件保存于：" << actualFilePath << std::endl;
    } else {
        std::cerr << "数据备份失败" << std::endl;
    }
    
    return success;
}



std::string DataFileManager::createRestoreBackup(const SystemSettings& settings) const {
    // 备份操作前创建一份临时备份
    std::string backupFilePath = getBackupDirectoryPath() + "/auto_backup_before_restore_" + std::to_string(time(0)) + ".dat";
    if (saveDataToFile(settings, backupFilePath)) {
        std::cout << "已创建自动备份: " << backupFilePath << std::endl;
        return backupFilePath;
    } else {
        std::cout << "警告: 创建自动备份失败，继续进行数据恢复操作。" << std::endl;
        return "";
    }
}

bool DataFileManager::loadDataFromFile(SystemSettings& settings, const std::string& filePath) {
    std::string actualFilePath = filePath;
    
    // 如果是备份ID，转换为实际文件路径
    if (filePath.find_first_not_of("0123456789") == std::string::npos) {
        actualFilePath = getBackupFilePathById(std::stoi(filePath));
        if (actualFilePath.empty()) {
            std::cerr << "错误: 无效的备份ID: " << filePath << std::endl;
            return false;
        }
    }
    
    // 创建临时备份
    std::string tempBackupPath = createRestoreBackup(settings);
    if (tempBackupPath.empty()) {
        std::cerr << "错误: 无法创建临时备份，取消导入操作。" << std::endl;
        return false;
    }
    
    // 打开文件
    std::ifstream inFile(actualFilePath);
    if (!inFile.is_open()) {
        std::cerr << "错误: 无法打开文件: " << actualFilePath << std::endl;
        return false;
    }
    
    // 创建当前数据的快照
    auto snapshot = settings.getData().createSnapshot();
    
    // 使用DataContainer的反序列化方法
    bool success = settings.getData().deserialize(inFile);
    
    inFile.close();
    
    // 如果导入失败，恢复快照
    if (!success) {
        std::cerr << "导入数据失败，正在恢复到备份状态..." << std::endl;
        settings.getData().restoreFromSnapshot(snapshot);
        return false;
    }
    
    // 如果导入成功，删除临时备份文件
    try {
        std::filesystem::remove(tempBackupPath);
        std::cout << "成功导入数据，已删除临时备份文件。" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "警告: 无法删除临时备份文件: " << e.what() << std::endl;
        // 这里我们不返回false，因为数据导入本身是成功的
    }
    
    return true;
}
