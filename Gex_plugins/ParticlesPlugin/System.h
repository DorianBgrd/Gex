#ifndef GEX_SOLVER_H
#define GEX_SOLVER_H

#include <vector>


namespace Gex::Particles
{
    struct Matrix3
    {
        double x1; double y1; double z1;
        double x2; double y2; double z2;
        double x3; double y3; double z3;

        void operator*(const Matrix3& other);
    };

    Matrix3 MultMatrix3(const Matrix3& m1, const Matrix3& m2);

    // Simple vector data.
    struct Vector
    {
        double x = 0;
        double y = 0;
        double z = 0;

        Vector operator+(const Vector& other) const;
        Vector operator-(const Vector& other) const;
        Vector operator*(const Vector& other) const;
        Vector operator/(const Vector& other) const;

        Vector operator+(double other) const;
        Vector operator-(double other) const;
        Vector operator*(double other) const;
        Vector operator/(double other) const;

        void operator+=(const Vector& other);
        void operator-=(const Vector& other);
        void operator*=(const Vector& other);
        void operator/=(const Vector& other);

        void operator+=(double other);
        void operator-=(double other);
        void operator*=(double other);
        void operator/=(double other);

        double Length() const;
        double Dot(const Vector& other) const;
        Vector Rotate(Vector angle) const;
    };

    // Simple color data.
    struct Color
    {
        double r = 0;
        double g = 0;
        double b = 0;
    };


    // Particle descriptor, intending to retain
    // per particle attributes and description.
    struct Particle
    {
        bool alive = false;
        int liveTime = 0;
        double mass = 0;

        Vector position = {0, 0, 0};
        Vector rotation = {0, 0, 0};
        Vector velocity = {0, 0, 0};
        Color color = {0, 0, 0};
    };


    // List of particles.
    typedef std::vector<Particle*> ParticleList;


    // Generates particles.
    class Generator
    {
        virtual ParticleList Generate() const = 0;
    };


    // Field adding forces to the current solving
    // process, making new behaviours.
    class Field
    {
    private:
        bool enabled = true;

    public:
        bool IsEnabled() const;

        void Enable();

        void Disable();

        virtual void Solve(Particle* particle) = 0;
    };


    // Field vector.
    typedef std::vector<Field*> FieldList;


    // Solver adding forces and moving
    // particles arround.
    class ParticleSolver
    {
        ParticleList particles;
        FieldList fields;

    public:
        void Solve();
    };
}

#endif //GEX_SOLVER_H
