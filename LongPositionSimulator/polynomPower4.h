#pragma once

#include <array>
#include <cassert>
#include <iomanip>
#include <optional>
#include <ostream>
#include <vector>


struct polynomPower4
{
    double a; //t^4
    double b; //t^3
    double c; //t^2
    double d; //t^1
    double e; //t^0


    // Addition operator
    polynomPower4 operator-(const polynomPower4& other) const
    {
        return polynomPower4{
            a - other.a, b - other.b, c - other.c, d - other.d, e - other.e
        };
    }

    bool operator==(const polynomPower4& other) const
    {
        return a == other.a && b == other.b && c == other.c && d == other.d && e == other.e;
    }

    polynomPower4 operator+(const polynomPower4& other) const
    {
        return polynomPower4{
            a + other.a, b + other.b, c + other.c, d + other.d, e + other.e
        };
    };

    double solveFor(double t) const
    {
        return t * t * t * t * a
            + t * t * t * b
            + t * t * c
            + t * d
            + e;
    }
    
    /**
     * \brief squares the polynom, requires polynom power == 2
     * \return 
     */
    polynomPower4 square()
    {
        assert(power()<=2);
        double a = c, b = d, c = e;
        return polynomPower4{
            a * a, 2 * a * b, b * b + 2*a*c, 2 * b * c, c * c
        };
    }

    int power() const
    {
        if (a != 0) return 4;
        if (b != 0) return 3;
        if (c != 0) return 2;
        if (d != 0) return 1;
        return 0;
    }

    polynomPower4 derive() const
    {
        return polynomPower4{
            0,
            4 * a,
            3 * b,
            2 * c,
            1 * d
        };
    }

    std::optional<double> findXMinimum() const;

    std::vector<double> findRoots() const;

    std::vector<double> quadratricFindRoots() const;

    friend std::ostream& operator<<(std::ostream& os, const polynomPower4& eq)
    {
        os << std::fixed << std::setprecision(2);
        if (eq.power() >= 4)
            os << eq.a << "*x^4 +";
        if (eq.power() >= 3)
            os << eq.b << "*x^3 +";
        if (eq.power() >= 2)
            os << eq.c << "*x^2 +";
        if (eq.power() >= 1)
            os << eq.d << "*x +";
        os << eq.e;
        return os;
    }
};

struct polynomPower4_3d
{
    polynomPower4 x;
    polynomPower4 y;
    polynomPower4 z;

    // Helper function to get an array of references to the members
    std::array<polynomPower4*, 3> getMembers() {
        return { &x, &y, &z };
    }

    // Begin and end iterators
    auto begin() {
        return getMembers().begin();
    }

    auto end() {
        return getMembers().end();
    }
};