#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "fluid.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Fluid::Fluid(double width, double height, double length,
             int num_width_pts, int num_height_pts, int num_length_pts,
             double radius)  {

    this->width = width;
    this->height = height;
    this->length = length;
    this->num_width_pts = num_width_pts;
    this->num_height_pts = num_height_pts;
    this->num_length_pts = num_length_pts;
    this->radius = radius;
    
    buildBlock();

}

Fluid::~Fluid(){
    particles.clear();
}

void Fluid::buildBlock() {
    for (int i = 0; i < num_width_pts; i++) {
        for (int j = 0; j < num_length_pts; j++) {
            for (int k = 0; k < num_height_pts; k++) {
                
                double x = width / max(num_width_pts - 1, 1) * (double)i;
                double y = height / max(num_height_pts - 1, 1) * (double)k;
                double z = length / max(num_length_pts - 1, 1) * (double)j;
                Vector3D pos = Vector3D(x, y, z);

                particles.emplace_back(pos, this->radius);
            }
        }
    }
}

void Fluid::simulate(double frames_per_sec, double simulation_steps,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {

    double delta_t = 1.0f / frames_per_sec / simulation_steps;
    double mass = 1.0;

    // Compute total force acting on each point mass.
    Vector3D external_force = Vector3D();
    for (Vector3D ea : external_accelerations) {
        external_force += ea * mass;
    }
    for (auto &particle : particles) {
        particle.forces = external_force;
    }
    
    // Temporarily using Verlet Integration, will replace with proper fluid sim later
    for (auto &particle : particles) {
        Vector3D cur_pos = particle.position;
        double damping = -2.0;
        particle.position = cur_pos + (1 - damping/100.0) * (cur_pos - particle.last_position) + particle.forces/mass * pow(delta_t, 2);
        particle.last_position = cur_pos;
    }
}

void Fluid::self_collide(Particle &p) {
    return;
}