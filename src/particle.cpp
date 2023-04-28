#include <nanogui/nanogui.h>

#include "misc/sphere_drawing.h"
#include "particle.h"

Particle::Particle(Vector3D position, double radius) {
    this->position = position;
    this->last_position = position; // May be able to delete later
    this->radius = radius;
    this->mass = 1.0; // for water

    this->acceleration = 0.0;
    this->velocity = 0.0;
    this->m_sphere_mesh = Misc::SphereMesh(40, 40);
    // this->forces = Vector3D();
}

Particle::~Particle() {
    position = Vector3D();
    radius = 0;
}

void Particle::render(GLShader &shader) {
    m_sphere_mesh.draw_sphere(shader, position, radius * 0.92);
}