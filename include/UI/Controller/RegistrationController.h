

#ifndef REGISTRATIONCONTROLLER_H
#define REGISTRATIONCONTROLLER_H

#include "../../Components/Core/Registration.h"
#include "../../Components/Core/SystemSettings.h"

class RegistrationController {
public:
    RegistrationController(Registration& registration, SystemSettings& settings);
    void manage();

private:
    Registration& registration_;
    SystemSettings& settings_;

    void handleRegisterAthleteForEvent();
    void handleUnregisterAthleteFromEvent();
    void handleViewAthleteRegistrations(); // 查看所有运动员及其报名情况
    void handleViewEventRegistrations();   // 查看所有项目及其报名情况
    void handleCheckAndCancelLowParticipationEvents();
};


#endif //REGISTRATIONCONTROLLER_H
