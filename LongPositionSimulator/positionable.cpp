#include "positionable.h"
#include <cassert>
#include "vel3d.h"

static const std::string idChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123579";
// Linear Congruential Generator (LCG) parameters
constexpr unsigned int LCG_A = 1664525; // Multiplier
constexpr unsigned int LCG_C = 1013904223; // Increment
constexpr unsigned int LCG_M = 429496729; // Modulus (2^32)

std::ostream& operator<<(std::ostream& os, const lifeTime& obj)
{
    os << "[" << obj.start << "," << obj.end << "]";
    return os;
}

std::string positionable::toCharId(unsigned int id)
{
    unsigned int randomized = (LCG_A * id + LCG_C) % LCG_M;
    unsigned int mask = 0b11111;
    std::string out = "XXX-XXX";
    for (int i = 0; i < sizeof(id) * 8; i += 5)
    {
        int masked = (randomized >> i);
        masked = masked & mask;
        assert(masked >= 0);
        assert(masked < 32);
        int off = i > 10 ? 1 : 0;
        out[i / 5 + off] = idChars[masked];
    }
    assert(out.length() == 7);
    return out;
}

double positionable::collisionAt(const positionable& obj1, const positionable& obj2)
{
    if (obj1.radius == 0 || obj2.radius == 0) //objects with no size will never collide.
        return -1;
    pos3d deltaPos = obj1.position - obj2.position;
    vel3d deltaVel = obj1.velocity - obj2.velocity;
    acc3d deltaAcc = obj1.acceleration - obj2.acceleration;
    int radiusSum = obj1.radius + obj2.radius;

    double a = 0.5 * (deltaAcc.x * deltaAcc.x + deltaAcc.y * deltaAcc.y + deltaAcc.z * deltaAcc.z);
    double b = deltaVel.x * deltaAcc.x + deltaVel.y * deltaAcc.y + deltaVel.z * deltaAcc.z;
    double c = deltaPos.x * deltaAcc.x + deltaPos.y * deltaAcc.y + deltaPos.z * deltaAcc.z - radiusSum * radiusSum;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return -1; // No collision
    }

    double t1 = (-b + std::sqrt(discriminant)) / (2 * a);
    double t2 = (-b - std::sqrt(discriminant)) / (2 * a);

    if (t1 >= 0 && t2 >= 0)
    {
        return std::min(t1, t2);
    }
    if (t1 >= 0)
    {
        return t1;
    }
    if (t2 >= 0)
    {
        return t2;
    }
    return -1; // No collision
}

positionable positionable::operator-(const positionable& other) const
{
    return positionable{
        this->position - other.position, this->velocity - other.velocity, this->acceleration - other.acceleration,
        this->radius - other.radius
    };
}

double positionable::distanceToObjectAt(double time, const positionable& other) const
{
    return (this->posAt(time) - other.posAt(time)).norm();
}
