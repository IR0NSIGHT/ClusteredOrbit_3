#include "PlayerState.h"

#include <ostream>

// Function to convert Weapon enum to string
std::string weaponToString(Weapon weapon)
{
    switch (weapon)
    {
    case Weapon::POINTDEFENSE: return "Point Defense";
    case Weapon::RAILGUN: return "Railgun";
    case Weapon::MISSILE: return "Missile";
    default: return "Unknown";
    }
}
