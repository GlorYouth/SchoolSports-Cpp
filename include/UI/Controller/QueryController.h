

#ifndef QUERYCONTROLLER_H
#define QUERYCONTROLLER_H


#include "../../Components/Core/SystemSettings.h"

class QueryController {
public:
    QueryController(const SystemSettings& settings);
    void manage();

private:
    const SystemSettings& settings_;

    void handleViewUnits();
    void handleViewEvents();
};




#endif //QUERYCONTROLLER_H
