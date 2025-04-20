#include "../vel3d.h"

// Multiplication operator (scalar multiplication)
vel3d operator*(const vel3d& v, double scalar)
{
    return vel3d(v.x * scalar, v.y * scalar, v.z * scalar);
}

// Division operator (scalar division)
vel3d operator/(const vel3d& v, double scalar)
{
    return vel3d(v.x / scalar, v.y / scalar, v.z / scalar);
}

// Addition operator
vel3d operator+(const vel3d& v, const vel3d& v1)
{
    return vel3d(v.x + v1.x, v.y + v1.y, v.z + v1.z);
}

// Subtraction operator
vel3d operator-(const vel3d& v, const vel3d& v1)
{
    return vel3d(v.x - v1.x, v.y - v1.y, v.z - v1.z);
}

// Equality operator
bool operator==(const vel3d& v, const vel3d& v1)
{
    return (v.x == v1.x && v.y == v1.y && v.z == v1.z);
}
