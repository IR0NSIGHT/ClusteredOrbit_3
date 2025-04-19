#pragma once

constexpr unsigned int BLUSHIP_ID = 1;
constexpr unsigned int REDSHIP_ID = 2;
constexpr unsigned int ASTEROID_ID = 3;

constexpr unsigned int CIVSHIP_ID = 4;

constexpr unsigned int PD_TURRET_RANGE = 100000;
constexpr unsigned int RAILGUN_SPEED = 4000; //m/s
constexpr unsigned int RAILGUN_DANGERZONE = 16000;
//railgunspeed *( 1 second reaction time + 0.44s time to adjust course by 1m with 1G thrust)
constexpr unsigned int RAILGUN_DEADZONE = 10000;
//railgunspeed *( 1 second reaction time + 0.44s time to adjust course by 1m with 1G thrust)

constexpr double FIXED_UPDATE_DELTA = .5;
constexpr double MAX_TIME_COLLISSION = 60*60*24*10; // ten days
