//
// Created by GlorYouth on 2025/6/3.
//

#ifndef DATAMANAGEMENTCONTROLLER_H
#define DATAMANAGEMENTCONTROLLER_H

#include "../Components/DataManager.h"
#include <string>

class DataManagementController {
public:
    DataManagementController(DataManager& dataManager);
    void manage();

private:
    DataManager& dataManager_;

    void handleBackupData();
    void handleRestoreData();
};

#endif //DATAMANAGEMENTCONTROLLER_H
