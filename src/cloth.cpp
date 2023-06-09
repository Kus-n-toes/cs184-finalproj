#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "cloth.h"
#include "collision/plane.h"
#include "collision/sphere.h"

using namespace std;

Cloth::Cloth(double width, double height, int num_width_points,
             int num_height_points, float thickness) {
  this->width = width;
  this->height = height;
  this->num_width_points = num_width_points;
  this->num_height_points = num_height_points;
  this->thickness = thickness;

  buildGrid();
  buildClothMesh();
}

Cloth::~Cloth() {
  point_masses.clear();
  springs.clear();

  if (clothMesh) {
    delete clothMesh;
  }
}

void Cloth::buildGrid() {
  // TODO (Part 1): Build a grid of masses and springs.
    float dx = width / (num_width_points - 1.0);
    float dy = height / (num_height_points - 1.0);

  for (int i = 0; i < num_height_points; i++) {
      for (int j = 0; j < num_width_points; j++) {
          Vector3D random_pos;
          float x = j * dx;
          float y = i * dy;
          if (orientation == HORIZONTAL) {
              random_pos = Vector3D(x, 1.0, y);
          } else if (orientation == VERTICAL) {
              //float z = (rand() / 500.0) - (1.0 / 1000.0);
              float z = (rand() - (0.5 * RAND_MAX)) / RAND_MAX / 500;
              random_pos = Vector3D(x, y, z);
          }
          point_masses.emplace_back(random_pos, false);
          x += dx;

      }
  }

    //check if x, y  is in pinned vector
    for (vector<int> v: pinned) {

      PointMass* is_pinned = &point_masses[v[0] * num_width_points + v[1]];
      is_pinned->pinned = true;
  }

  //initializing springs for each type of constraint:
  //Structural constraints exist between a point mass and the
  //    point mass to its left as well as the point mass above it.

  //Shearing constraints exist between a point mass and the point mass to its diagonal
  //    upper left as well as the point mass to its diagonal upper right.

  //Bending constraints exist between a point mass and the point mass two away to its
  //    left as well as the point mass two above it.

//  for (PointMass pm : point_masses) {
//      int x = (int) pm.position.x;
//      int y = (int) pm.position.y;

    for (int y = 0; y < num_height_points; y++) {
        for (int x = 0; x < num_width_points; x++) {

            PointMass* pm = &point_masses[y * num_width_points + x];


            //pointer arithmetic to access all relevant point masses relative to current one

            PointMass* left = &point_masses[y * num_width_points + x - 1];
            PointMass* above = &point_masses[(y - 1) * num_width_points + x];
            PointMass* top_left = &point_masses[(y - 1) * num_width_points + x - 1];
            PointMass* top_right = &point_masses[(y - 1) * num_width_points + x + 1];
            PointMass* two_left = &point_masses[y * num_width_points + x - 2];
            PointMass* two_above = &point_masses[(y - 2) * num_width_points + x];

            //instantiating all possible spring types, to be made persistent by appending to the springs vector
            //no need because of emplace_back
            //      Spring struct_left = Spring(&pm, left, STRUCTURAL);
            //      Spring struct_above = Spring(&pm, above, STRUCTURAL);
            //      Spring shear_top_left = Spring(&pm, top_left, SHEARING);
            //      Spring shear_top_right = Spring(&pm, top_right, SHEARING);
            //      Spring bend_left = Spring(&pm, two_left, BENDING);
            //      Spring bend_above = Spring(&pm, two_above, BENDING);


            //first case: first row
            //no shearing
            if (y == 0) {
                if (x == 0) {
                    continue;
                }
                if (x == 1) {
                    //structural left
                    springs.emplace_back(pm, left, STRUCTURAL);
                    continue;
                }
                //structural left
                springs.emplace_back(pm, left, STRUCTURAL);
                //bending left
                springs.emplace_back(pm, two_left, BENDING);
                continue;
            }
            //second case: second row
            //no bending up
            if (y == 1) {
                if (x == 0) {
                    //structural up
                    springs.emplace_back(pm, above, STRUCTURAL);
                    //shearing top-right
                    springs.emplace_back(pm, top_right, SHEARING);
                    continue;
                }
                if (x == 1) {
                    //structural up, left
                    springs.emplace_back(pm, above, STRUCTURAL);
                    springs.emplace_back(pm, left, STRUCTURAL);
                    //shearing top-left, top-right
                    springs.emplace_back(pm, top_left, SHEARING);
                    springs.emplace_back(pm, top_right, SHEARING);
                    continue;
                }
                if (x == num_width_points - 1) {
                    //structural up, left
                    springs.emplace_back(pm, above, STRUCTURAL);
                    springs.emplace_back(pm, left, STRUCTURAL);
                    //shearing top-left
                    springs.emplace_back(pm, top_left, SHEARING);
                    //bending left
                    springs.emplace_back(pm, two_left, BENDING);
                    continue;
                }
                //structural up, left
                springs.emplace_back(pm, above, STRUCTURAL);
                springs.emplace_back(pm, left, STRUCTURAL);
                //shearing top-left, top-right
                springs.emplace_back(pm, top_left, SHEARING);
                springs.emplace_back(pm, top_right, SHEARING);
                //bending left
                springs.emplace_back(pm, two_left, BENDING);
                continue;
            }

            //now always bending up
            if (x == 0) {
                //structural up
                springs.emplace_back(pm, above, STRUCTURAL);

                //shearing top-right
                springs.emplace_back(pm, top_right, SHEARING);

                //bending up
                springs.emplace_back(pm, two_above, BENDING);

                continue;
            }

            if (x == 1) {
                //structural up, left
                springs.emplace_back(pm, above, STRUCTURAL);
                springs.emplace_back(pm, left, STRUCTURAL);

                //shearing top-left, top-right
                springs.emplace_back(pm, top_left, SHEARING);
                springs.emplace_back(pm, top_right, SHEARING);

                //bending up
                springs.emplace_back(pm, two_above, BENDING);

                continue;
            }

            if (x == num_width_points - 1) {
                //structural up, left
                springs.emplace_back(pm, above, STRUCTURAL);
                springs.emplace_back(pm, left, STRUCTURAL);
                //shearing top-left
                springs.emplace_back(pm, top_left, SHEARING);
                //bending left, up
                springs.emplace_back(pm, two_above, BENDING);
                springs.emplace_back(pm, two_left, BENDING);

                continue;
            }
            //rest of cases
            //structural up, left
            springs.emplace_back(pm, above, STRUCTURAL);
            springs.emplace_back(pm, left, STRUCTURAL);
            //shearing top-left, top-right
            springs.emplace_back(pm, top_left, SHEARING);
            springs.emplace_back(pm, top_right, SHEARING);
            //bending left, up
            springs.emplace_back(pm, two_above, BENDING);
            springs.emplace_back(pm, two_left, BENDING);
        }
    }
    return;


}

void Cloth::simulate(double frames_per_sec, double simulation_steps, ClothParameters *cp,
                     vector<Vector3D> external_accelerations,
                     vector<CollisionObject *> *collision_objects) {
  double mass = width * height * cp->density / num_width_points / num_height_points;
  double delta_t = 1.0f / frames_per_sec / simulation_steps;

  // TODO (Part 2): Compute total force acting on each point mass.
  Vector3D external_sum = Vector3D(0,0,0);
  for (Vector3D f : external_accelerations) {
      external_sum += f;
  }
  Vector3D external_forces = external_sum * mass;

  //reset all point_mass forces
  for (PointMass &pm : point_masses) {
      pm.forces = external_forces;
  }

  for (Spring &s : springs) {

      //from spec: smaller k_s for bending constraint
      const float bending_k = 0.2;
      int type = s.spring_type;
      float spring_length = (s.pm_a->position - s.pm_b->position).norm();

      Vector3D force_s;

      //direction of force vectors for a towards b
      Vector3D direction = (s.pm_b->position - s.pm_a->position).unit();
      if (type == STRUCTURAL && cp->enable_structural_constraints) {
          force_s = cp->ks * (spring_length - s.rest_length) * direction;
          s.pm_a->forces += force_s;
          s.pm_b->forces -= force_s;
      } else if (type == SHEARING && cp->enable_shearing_constraints) {
          force_s = cp->ks * (spring_length - s.rest_length) * direction;
          s.pm_a->forces += force_s;
          s.pm_b->forces -= force_s;
      } else if (type == BENDING && cp->enable_bending_constraints) {
          force_s = bending_k * cp->ks * (spring_length - s.rest_length) * direction;
          s.pm_a->forces += force_s;
          s.pm_b->forces -= force_s;
      }
  }

  // TODO (Part 2): Use Verlet integration to compute new point mass positions
  for (PointMass &pm : point_masses) {
      if (!pm.pinned) {
          Vector3D x_minus_dt = pm.last_position;
          Vector3D x_t = pm.position;

          pm.last_position = x_t;
          pm.position = x_t + (1.0 - cp->damping / 100.0) * (x_t - x_minus_dt) + (pm.forces / mass) * pow(delta_t, 2);
      }
      //clear forces
      pm.forces = Vector3D(0,0,0);

  }


  // TODO (Part 4): Handle self-collisions.
  build_spatial_map();
  for (PointMass &pm : point_masses) {
      self_collide(pm, simulation_steps);
  }

  // TODO (Part 3): Handle collisions with other primitives.
  for (PointMass &pm : point_masses) {
      for (CollisionObject* co : *collision_objects) {
          co->collide(pm);
      }
  }

  // TODO (Part 2): Constrain the changes to be such that the spring does not change
  // in length more than 10% per timestep [Provot 1995].
  const float heuristic = 1.1;
    for (Spring &s : springs) {
        float spring_length = (s.pm_a->position - s.pm_b->position).norm();
        float max_length = s.rest_length * heuristic;
        float delta_length = spring_length - max_length;
        if (delta_length > 0) {

            //go next
            if (s.pm_a->pinned && s.pm_b->pinned) {
                continue;
            }

            //constrain b
            if (s.pm_a->pinned) {
                s.pm_b->position += delta_length * (s.pm_a->position - s.pm_b->position).unit();
                continue;
            }

            //constrain a
            if (s.pm_b->pinned) {
                s.pm_a->position += delta_length * (s.pm_b->position - s.pm_a->position).unit();
                continue;
            }

            //constrain both
            Vector3D temp_a = s.pm_a->position;
            s.pm_a->position += (delta_length / 2.0) * (s.pm_b->position - s.pm_a->position).unit();
            s.pm_b->position += (delta_length / 2.0) * (s.pm_a->position - s.pm_b->position).unit();
        }


    }


}

void Cloth::build_spatial_map() {
  for (const auto &entry : map) {
    delete(entry.second);
  }
  map.clear();

  // TODO (Part 4): Build a spatial map out of all of the point masses.
  for (PointMass &pm : point_masses) {
      float hash = hash_position(pm.position);
      //cout << hash << endl;
      if (map[hash] == NULL) {
          map[hash] = new vector<PointMass *>();
          map[hash]->emplace_back(&pm);
      } else {
          map[hash]->emplace_back(&pm);
      }
  }
}

void Cloth::self_collide(PointMass &pm, double simulation_steps) {
  // TODO (Part 4): Handle self-collision for a given point mass.
  float hash = hash_position(pm.position);
  vector<PointMass *> vec = *map[hash];
  Vector3D correct_vect = {0, 0, 0};
  int count = 0;
  for (PointMass* pm_cand : vec) {
      if (pm_cand == &pm) {
          continue;
      } else {
          if ((pm_cand->position - pm.position).norm() <= 2 * thickness) {
              Vector3D correction = -(pm_cand->position - pm.position);
              correction.normalize();
              correction = 2 * thickness * correction + (pm_cand->position - pm.position);
              correct_vect += correction;
              count += 1;
          }
      }
  }
  //cout << count << endl;
  if (count > 0) {
      correct_vect = (correct_vect / count) / simulation_steps;
      //cout << correct_vect << endl;
  }
  pm.position = pm.position + correct_vect;
}

float Cloth::hash_position(Vector3D pos) {
  // TODO (Part 4): Hash a 3D position into a unique float identifier that represents membership in some 3D box volume.
  double w = 3.0 * width / num_width_points;
  double h = 3.0 * height / num_height_points;
  double t = max(w, h);
  int x_i = pos.x / w;
  int y_i = pos.y / h;
  int z_i = pos.z / t;

  float hash = x_i + width * y_i + width * height * z_i;
  return hash;
}

///////////////////////////////////////////////////////
/// YOU DO NOT NEED TO REFER TO ANY CODE BELOW THIS ///
///////////////////////////////////////////////////////

void Cloth::reset() {
  PointMass *pm = &point_masses[0];
  for (int i = 0; i < point_masses.size(); i++) {
    pm->position = pm->start_position;
    pm->last_position = pm->start_position;
    pm++;
  }
}

void Cloth::buildClothMesh() {
  if (point_masses.size() == 0) return;

  ClothMesh *clothMesh = new ClothMesh();
  vector<Triangle *> triangles;

  // Create vector of triangles
  for (int y = 0; y < num_height_points - 1; y++) {
    for (int x = 0; x < num_width_points - 1; x++) {
      PointMass *pm = &point_masses[y * num_width_points + x];
      // Get neighboring point masses:
      /*                      *
       * pm_A -------- pm_B   *
       *             /        *
       *  |         /   |     *
       *  |        /    |     *
       *  |       /     |     *
       *  |      /      |     *
       *  |     /       |     *
       *  |    /        |     *
       *      /               *
       * pm_C -------- pm_D   *
       *                      *
       */
      
      float u_min = x;
      u_min /= num_width_points - 1;
      float u_max = x + 1;
      u_max /= num_width_points - 1;
      float v_min = y;
      v_min /= num_height_points - 1;
      float v_max = y + 1;
      v_max /= num_height_points - 1;
      
      PointMass *pm_A = pm                       ;
      PointMass *pm_B = pm                    + 1;
      PointMass *pm_C = pm + num_width_points    ;
      PointMass *pm_D = pm + num_width_points + 1;
      
      Vector3D uv_A = Vector3D(u_min, v_min, 0);
      Vector3D uv_B = Vector3D(u_max, v_min, 0);
      Vector3D uv_C = Vector3D(u_min, v_max, 0);
      Vector3D uv_D = Vector3D(u_max, v_max, 0);
      
      
      // Both triangles defined by vertices in counter-clockwise orientation
      triangles.push_back(new Triangle(pm_A, pm_C, pm_B, 
                                       uv_A, uv_C, uv_B));
      triangles.push_back(new Triangle(pm_B, pm_C, pm_D, 
                                       uv_B, uv_C, uv_D));
    }
  }

  // For each triangle in row-order, create 3 edges and 3 internal halfedges
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    // Allocate new halfedges on heap
    Halfedge *h1 = new Halfedge();
    Halfedge *h2 = new Halfedge();
    Halfedge *h3 = new Halfedge();

    // Allocate new edges on heap
    Edge *e1 = new Edge();
    Edge *e2 = new Edge();
    Edge *e3 = new Edge();

    // Assign a halfedge pointer to the triangle
    t->halfedge = h1;

    // Assign halfedge pointers to point masses
    t->pm1->halfedge = h1;
    t->pm2->halfedge = h2;
    t->pm3->halfedge = h3;

    // Update all halfedge pointers
    h1->edge = e1;
    h1->next = h2;
    h1->pm = t->pm1;
    h1->triangle = t;

    h2->edge = e2;
    h2->next = h3;
    h2->pm = t->pm2;
    h2->triangle = t;

    h3->edge = e3;
    h3->next = h1;
    h3->pm = t->pm3;
    h3->triangle = t;
  }

  // Go back through the cloth mesh and link triangles together using halfedge
  // twin pointers

  // Convenient variables for math
  int num_height_tris = (num_height_points - 1) * 2;
  int num_width_tris = (num_width_points - 1) * 2;

  bool topLeft = true;
  for (int i = 0; i < triangles.size(); i++) {
    Triangle *t = triangles[i];

    if (topLeft) {
      // Get left triangle, if it exists
      if (i % num_width_tris != 0) { // Not a left-most triangle
        Triangle *temp = triangles[i - 1];
        t->pm1->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm1->halfedge->twin = nullptr;
      }

      // Get triangle above, if it exists
      if (i >= num_width_tris) { // Not a top-most triangle
        Triangle *temp = triangles[i - num_width_tris + 1];
        t->pm3->halfedge->twin = temp->pm2->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle to bottom right; guaranteed to exist
      Triangle *temp = triangles[i + 1];
      t->pm2->halfedge->twin = temp->pm1->halfedge;
    } else {
      // Get right triangle, if it exists
      if (i % num_width_tris != num_width_tris - 1) { // Not a right-most triangle
        Triangle *temp = triangles[i + 1];
        t->pm3->halfedge->twin = temp->pm1->halfedge;
      } else {
        t->pm3->halfedge->twin = nullptr;
      }

      // Get triangle below, if it exists
      if (i + num_width_tris - 1 < 1.0f * num_width_tris * num_height_tris / 2.0f) { // Not a bottom-most triangle
        Triangle *temp = triangles[i + num_width_tris - 1];
        t->pm2->halfedge->twin = temp->pm3->halfedge;
      } else {
        t->pm2->halfedge->twin = nullptr;
      }

      // Get triangle to top left; guaranteed to exist
      Triangle *temp = triangles[i - 1];
      t->pm1->halfedge->twin = temp->pm2->halfedge;
    }

    topLeft = !topLeft;
  }

  clothMesh->triangles = triangles;
  this->clothMesh = clothMesh;
}
