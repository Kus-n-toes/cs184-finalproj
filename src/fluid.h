#ifndef FLUID_H
#define FLUID_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "collision/collisionObject.h"
#include "particle.h"

using namespace CGL;
using namespace std;

struct Fluid {

    Fluid() {}
    Fluid(double width, double height, double length, 
            int num_width_pts, int num_height_pts, int num_length_pts, 
            double radius);
    ~Fluid();

    void buildBlock();
    void simulate(double frames_per_sec, double simulation_steps,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects); // TODO: Add relevant params here
    void self_collide(Particle &p); // for particle-to-particle collisions, NOTE: particle-collisionObject collisions are handled in simulate()

    vector<Particle> particles;
    double width;
    double height;
    double length;
    int num_width_pts;
    int num_height_pts;
    int num_length_pts;
    double radius; // Optional if we wanna change the size of our particles?
};

#endif /* FLUID_H */