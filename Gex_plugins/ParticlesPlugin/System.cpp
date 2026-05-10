#include "System.h"

#include <cmath>

// -------------------------------------------------
// Matrix3 -----------------------------------------
// -------------------------------------------------
void Gex::Particles::Matrix3::operator*(const Matrix3& other)
{
    x1 = x1 * other.x1 + y1 * other.x2 + z1 * other.x3;
    y1 = x1 * other.y1 + y1 * other.y2 + z1 * other.y3;
    z1 = x1 * other.z1 + y1 * other.z2 + z1 * other.z3;

    x2 = x2 * other.x1 + y2 * other.x2 + z2 * other.x3;
    y2 = x2 * other.y1 + y2 * other.y2 + z2 * other.y3;
    z2 = x2 * other.z1 + y2 * other.z2 + z2 * other.z3;

    x3 = x3 * other.x1 + y3 * other.x2 + z3 * other.x3;
    y3 = x3 * other.y1 + y3 * other.y2 + z3 * other.y3;
    z3 = x3 * other.z1 + y3 * other.z2 + z3 * other.z3;
}


Gex::Particles::Matrix3 Gex::Particles::MultMatrix3(
        const Matrix3& m1, const Matrix3& m2)
{
    return {
            m1.x1 * m2.x1 + m1.y1 * m2.x2 + m1.z1 * m2.x3,
            m1.x1 * m2.y1 + m1.y1 * m2.y2 + m1.z1 * m2.y3,
            m1.x1 * m2.z1 + m1.y1 * m2.z2 + m1.z1 * m2.z3,

            m1.x2 * m2.x1 + m1.y2 * m2.x2 + m1.z2 * m2.x3,
            m1.x2 * m2.y1 + m1.y2 * m2.y2 + m1.z2 * m2.y3,
            m1.x2 * m2.z1 + m1.y2 * m2.z2 + m1.z2 * m2.z3,

            m1.x3 * m2.x1 + m1.y3 * m2.x2 + m1.z3 * m2.x3,
            m1.x3 * m2.y1 + m1.y3 * m2.y2 + m1.z3 * m2.y3,
            m1.x3 * m2.z1 + m1.y3 * m2.z2 + m1.z3 * m2.z3
    };
}


// -------------------------------------------------
// Vector ------------------------------------------
// -------------------------------------------------
Gex::Particles::Vector Gex::Particles::Vector::operator+(
        const Gex::Particles::Vector& other) const
{
    return {x + other.x, y + other.y, z + other.z};
}

Gex::Particles::Vector Gex::Particles::Vector::operator-(
        const Gex::Particles::Vector& other) const
{
    return {x - other.x,y - other.y,z - other.z};
}

Gex::Particles::Vector Gex::Particles::Vector::operator*(
        const Gex::Particles::Vector& other) const
{
    return {x * other.x,y * other.y,z * other.z};
}

Gex::Particles::Vector Gex::Particles::Vector::operator/(
        const Gex::Particles::Vector& other) const
{
    return {x / other.x,y / other.y,z / other.z};
}

Gex::Particles::Vector Gex::Particles::Vector::operator+(
        double other) const
{
    return {x + other,y + other,z + other};
}

Gex::Particles::Vector Gex::Particles::Vector::operator-(
        double other) const
{
    return {x - other,y - other,z - other};
}

Gex::Particles::Vector Gex::Particles::Vector::operator*(
        double other) const
{
    return {x * other,y * other,z * other};
}

Gex::Particles::Vector Gex::Particles::Vector::operator/(
        double other) const
{
    return {x / other,y / other,z / other};
}


void Gex::Particles::Vector::operator+=(const Vector& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

void Gex::Particles::Vector::operator-=(const Vector& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Gex::Particles::Vector::operator*=(const Vector& other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
}

void Gex::Particles::Vector::operator/=(const Vector& other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
}


void Gex::Particles::Vector::operator+=(double other)
{
    x += other;
    y += other;
    z += other;
}

void Gex::Particles::Vector::operator-=(double other)
{
    x -= other;
    y -= other;
    z -= other;
}

void Gex::Particles::Vector::operator*=(double other)
{
    x *= other;
    y *= other;
    z *= other;
}

void Gex::Particles::Vector::operator/=(double other)
{
    x /= other;
    y /= other;
    z /= other;
}


double Gex::Particles::Vector::Length() const
{
    // V(ax - bx)2 + (ay - by)2 + (az - bz)2
    auto absx =  std::abs(x);
    auto absy = std::abs(y);
    auto absz = std::abs(z);
    return std::sqrt((absx * absx + absy * absy + absz * absz));
}


double Gex::Particles::Vector::Dot(const Vector& other) const
{
    return (x * other.x + y * other.y + z * other.z);
}


Gex::Particles::Vector Gex::Particles::Vector::Rotate(Vector angle) const
{
    return {};
}


// -------------------------------------------------
// Field default -----------------------------------
// -------------------------------------------------
bool Gex::Particles::Field::Field::IsEnabled() const
{
    return enabled;
}


void Gex::Particles::Field::Field::Enable()
{
    enabled = true;
}


void Gex::Particles::Field::Field::Disable()
{
    enabled = false;
}


// -------------------------------------------------
// Solver ------------------------------------------
// -------------------------------------------------
void Gex::Particles::ParticleSolver::Solve()
{
    for (Particle* particle : particles)
    {
        for (Field* field : fields)
        {
            field->Solve(particle);
        }

        particle->position += particle->velocity;
    }
}