#pragma once
#include <cmath>
#include <ostream>
#include <SFML/Graphics/Glsl.hpp>

struct vel3d
{
public:
    double x;
    double y;
    double z;
    
    vel3d() : x(0), y(0), z(0)
    {
    }

    vel3d(double x, double y, double z) : x(x), y(y), z(z)
    {
    }



    vel3d normalized() const
    {
        double norm = this->norm();
        if (norm == 0)
            norm = 1;
        return *this / norm;
    }

    // Multiplication operator (scalar multiplication)
    friend vel3d operator*(const vel3d& v, double scalar);

    // Multiplication operator (scalar multiplication)
    friend vel3d operator/(const vel3d& v, double scalar);

    // Addition operator
    friend vel3d operator+(const vel3d& v, const vel3d& v1);

    // Subtraction operator
    friend vel3d operator-(const vel3d& v, const vel3d& v1);

    // Equality operator
    friend bool operator==(const vel3d& v, const vel3d& v1);

    double norm() const
    {
        return 100. * sqrt((x * x + y * y + z * z) / 10000.);
    }

    bool operator!=(const vel3d& acc3d) const
    {
        return this->x != acc3d.x || this->y != acc3d.y || this->z != acc3d.z;
    }


    // Static method to compute the cross product of two vel3d vectors
    static vel3d crossProduct(const vel3d& a, const vel3d& b) {
        return vel3d(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    static double dotProduct(const vel3d& a, const vel3d& b) {
        return a.x*b.x + a.y * b.y + a.z * b.z;
    }
    
    // Overload << operator for vel3d
    friend std::ostream& operator<<(std::ostream& os, const vel3d& vel);

    /**
     * \brief 
     * \return largest value absolute of each axis
     */
    double maxComponentAbs()
    {
        return std::max(std::max(abs(x), abs(y)), abs(z));
    }
};

using pos3d = vel3d;
using acc3d = vel3d;
