#pragma once

#include "vel3d.h"

struct lifeTime
{
    double start = 0;
    double end = 100000000;

    bool existsAt(double time) const
    {
        return time >= start && time <= end;
    }

    friend std::ostream& operator<<(std::ostream& os, const lifeTime& obj);
};

struct positionable
{
    pos3d position{};
    vel3d velocity{};
    acc3d acceleration{};
    int radius = 1;
    int internalId = 0;
    static std::string toCharId(unsigned int id);
    /**
     * calculates the t at which point both objects collide
     * math:  ||obj1.pos-obj2.pos||==obj1.radius+obj2.radius
     */
    static double collisionAt(const positionable& obj1, const positionable& obj2);

    bool isAccelerated() const
    {
        return acceleration.norm() != 0.;
    }
    
    bool isStatic() const
    {
        return velocity.norm() == 0. && acceleration.norm() == 0.;
    }

    bool isLinear() const
    {
        return velocity.norm() != 0. && acceleration.norm() == 0.;
    }

    bool collidesWithAt(positionable& other, double time) {
        return distanceToObjectAt(time, other) <= (other.radius + this->radius + 0.0001);
    }

    pos3d posAt(double time) const
    {
        return position + velocity * time + acceleration * time * time * 0.5;
    }

    positionable operator-(const positionable& positionable) const;

    vel3d velAt(double time) const
    {
        return velocity + (acceleration * time);
    };

    positionable objectAt(double time) const
    {
        return positionable{posAt(time), velAt(time), acceleration, radius, internalId};
    }

    bool courseEquals(const positionable& other) const
    {
        return velocity == other.velocity && position == other.position && acceleration == other.acceleration;
    }

    double distanceToObjectAt(double time, const positionable& other) const;
    // Overload << operator for positionable
    friend std::ostream& operator<<(std::ostream& os, const positionable& obj);
};
