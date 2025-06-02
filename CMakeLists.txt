cmake_minimum_required(VERSION 3.31)
project(project)

set(CMAKE_CXX_STANDARD 26)

add_executable(project main.cpp
        include/CompetitionEvent.h
        include/Athlete.h
        include/Unit.h
        include/ScoreRule.h
        include/SystemSettings.h
        include/Registration.h
        include/Schedule.h
        include/Result.h
        include/DataManager.h
        include/Constants.h
        src/Athlete.cpp
        src/CompetitionEvent.cpp
        src/Unit.cpp
        src/ScoreRule.cpp
        src/SystemSettings.cpp
        src/Registration.cpp
        src/Schedule.cpp
        src/Result.cpp
        src/DataManager.cpp)
