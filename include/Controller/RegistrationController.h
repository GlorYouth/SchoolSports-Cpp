//
// Created by GlorYouth on 2025/6/3.
//

#ifndef REGISTRATIONCONTROLLER_H
#define REGISTRATIONCONTROLLER_H

#include "../Components/Registration.h"
#include "../Components/SystemSettings.h"

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
