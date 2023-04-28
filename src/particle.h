#ifndef PARTICLE_H
#define PARTICLE_H

// these are from pointmass.h
#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector3D.h"

#include "misc/sphere_drawing.h"

using namespace CGL;

struct Particle {
    Particle() {}
    Particle(Vector3D position, double radius);
    ~Particle();

    Vector3D position;
    Vector3D last_position;
    Vector3D velocity; // May not need, may need last pos instead
    Vector3D acceleration;
    double radius;
    Misc::SphereMesh m_sphere_mesh;
    // TODO: Add attributes relating to movement
    double density;
    double mass;
    Vector3D forces;
  
    void render(GLShader &shader);
};

#endif /* PARTICLE_H */