#include "../include/polynomPower4.h"

#include <limits>
#include <vector>

#include <Eigen/Dense>
#include <unsupported/Eigen/Polynomials>
#include <optional>

std::optional<double> polynomPower4::findXMinimum() const
{
    double min = std::numeric_limits<double>::max();
    double minX = 0;
    bool foundMin = false;

    for (auto x : derive().findRoots())
    {
        double criticalPoint = solveFor(x);
        if (min > criticalPoint)
        {
            min = criticalPoint;
            minX = x;
            foundMin = true;
        }
    }

    // Check if min is actually the minimum or just a local minimum
    if (foundMin)
    {
        double left = solveFor(minX - 1);
        double right = solveFor(minX + 1);
        if (left < min || right < min)
        {
            return std::nullopt;
        }
        return minX;
    }

    // If no critical points were found, return nullopt
    return std::nullopt;
}

std::vector<double> polynomPower4::findRoots() const
{
    int power = -1;
    if (a != 0)
    {
        power = 4;
    }
    else if (b != 0)
    {
        power = 3;
    }
    else if (c != 0)
    {
        power = 2;
    }
    else if (d != 0)
    {
        power = 1;
    }
    else if (e != 0)
    {
        power = 0;
    }
    Eigen::VectorXd coeffs(power + 1);
    if (power == 4)
        coeffs << e, d, c, b, a;
    else if (power == 3)
        coeffs << e, d, c, b;
    else if (power == 2)
        coeffs << e, d, c;
    else if (power == 1)
        coeffs << e, d;
    else if (power == 0)
    {
        std::vector<double> realRoots(power);
        realRoots.push_back(e);
        return realRoots;
    }

    Eigen::PolynomialSolver<double, Eigen::Dynamic> solver;
    solver.compute(coeffs);
    const auto& result = solver.roots();
    std::vector<double> realRoots(power); //last steppable step
    int idx = 0; //jumps from here
    for (std::complex<double> r : result)
    {
        if (r.imag() != 0) //skip complex solutions, we only care about real ones
            continue;
        realRoots[idx] = r.real();
        idx++;
    }
    realRoots.resize(idx); // to here ???
    for (double root : realRoots)
        assert(abs(solveFor(root)) <= 0.1); // f(root) = 0
    std::sort(realRoots.begin(), realRoots.end());
    return realRoots;
}

std::vector<double> polynomPower4::quadratricFindRoots() const
{
    double a = c, b = d, c = e; //its shifted, because this struct represetns power 4 polynoms.
    assert(power() <= 2);
    double root1, root2;
    double discriminant = b * b - 4 * a * c;
    std::vector<double> roots;
    roots.reserve(2);
    if (a == 0 && b != 0)
        roots.push_back(-c / b); // linear, not quadratic
    else if (a == 0)
        roots.push_back(0); // 0 == 0, always true
    else if (discriminant > 0)
    {
        double sqrtDiscriminant = std::sqrt(discriminant);
        root1 = (-b + sqrtDiscriminant) / (2 * a);
        root2 = (-b - sqrtDiscriminant) / (2 * a);
        roots.push_back(std::min(root1, root2));
        roots.push_back(std::max(root1, root2));
    }
    else if (discriminant == 0)
    {
        double sqrtDiscriminant = std::sqrt(discriminant);
        root1 = (-b + sqrtDiscriminant) / (2 * a);
        roots.push_back(root1);
    }
    else
    {
        // no solutions
        //do nothing
    }
    return roots;
}
