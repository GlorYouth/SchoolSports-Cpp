//
// Created by GlorYouth on 2025/6/3.
//

#ifndef DATAMANAGEMENTCONTROLLER_H
#define DATAMANAGEMENTCONTROLLER_H

#include "../Components/DataManager.h"
#include "../Components/SystemSettings.h"
#include <string>

class DataManagementController {
public:
    DataManagementController(DataManager& dataManager);
    void manage();
    
    // 将示例数据导入方法移到public
    void handleLoadSampleData();

private:
    DataManager& dataManager_;

    void handleBackupData();
    void handleRestoreData();
    
    // 显示备份文件列表
    void displayBackupFiles() const;
    
    // 分阶段导入示例数据的处理方法
    void handleLoadSampleStage1Data();
    void handleLoadSampleStage2Data();
    void handleLoadSampleStage3Data();
    
    // 显示导入菜单
    void displayImportMenu();
};

#endif //DATAMANAGEMENTCONTROLLER_H
