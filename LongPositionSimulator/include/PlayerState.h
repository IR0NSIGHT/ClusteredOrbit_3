#pragma once
#include <ostream>

enum class Weapon
{
    POINTDEFENSE,
    RAILGUN,
    MISSILE
};

std::string weaponToString(Weapon weapon);

// Overload the << operator for Weapon enum
inline std::ostream& operator<<(std::ostream& os, Weapon weapon)
{
    os << weaponToString(weapon);
    return os;
}

struct PlayerState
{
    PlayerState() = default;
    unsigned int targetGlobalId = 0;
    Weapon selectedWeapon;
    unsigned int referenceObjectId = 0;
    bool isRelativeSpace;
    unsigned int anchorGlobalId = 0;
};
