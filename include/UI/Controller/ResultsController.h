//
// Created by GlorYouth on 2025/6/3.
//

#ifndef RESULTSCONTROLLER_H
#define RESULTSCONTROLLER_H


#include "../../Components/Core/SystemSettings.h"

class ResultsController {
public:
    ResultsController(SystemSettings& settings);
    void manage();

private:
    SystemSettings& settings_;

    void handleRecordEventResults();
    void handleViewEventResults();
    // viewUnitStandings 在主菜单中，此处不重复
};


#endif //RESULTSCONTROLLER_H
