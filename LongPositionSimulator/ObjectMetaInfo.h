#pragma once
#include <string>
#include <SFML/Graphics/Color.hpp>
#include <iostream>

enum class ObjectType
{
    SHIP,
    ASTEROID,
    MISSILE,
    PROJECTILE,
};

inline std::string objectTypeToString(ObjectType type)
{
    switch (type)
    {
    case ObjectType::SHIP:
        return "SHIP";
    case ObjectType::ASTEROID:
        return "ASTEROID";
    case ObjectType::MISSILE:
        return "MISSILE";
    case ObjectType::PROJECTILE:
        return "PROJECTILE";
    default:
        return "UNKNOWN";
    }
}

enum Faction
{
    BluFor,
    OpFor,
    Independent,
    Civilian,
    None
};

inline std::string factionToString(Faction type)
{
    switch (type)
    {
    case BluFor:
        return "BluFor";
    case OpFor:
        return "OpFor";
    case Independent:
        return "Independent";
    case Civilian:
        return "Civilian";
    case None:
        return "NoFaction";
    default:
        return "UNKNOWN";
    }
}

inline sf::Color colorByFaction(Faction f)
{
    switch (f)
    {
    case BluFor:
        return sf::Color(115, 127, 255);
    case OpFor:
        return sf::Color(255, 75, 75);
    case Independent:
        return sf::Color(84, 153, 82);
    case Civilian:
        return sf::Color(184, 0, 184);
    case None:
        return sf::Color(203, 169, 0);
    }
}

struct ObjectMetaInfo
{
    ObjectType type;
    Faction faction;

    // Overload << operator for positionable
    friend std::ostream& operator<<(std::ostream& os, const ObjectMetaInfo& obj);
};

inline std::ostream& operator<<(std::ostream& os, const ObjectMetaInfo& obj)
{
    os << objectTypeToString(obj.type) << ", " << factionToString(obj.faction);
    return os;
}
