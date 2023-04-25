#ifndef COLLISIONOBJECT_RECTANGLE_H
#define COLLISIONOBJECT_RECTANGLE_H

#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "collisionObject.h"

using namespace nanogui;
using namespace CGL;
using namespace std;

struct Rectangle : public CollisionObject {
public:
  Rectangle(const Vector3D &topRight, const Vector3D &bottomLeft, const Vector3D &normal, double friction)
      : topRight(topRight), bottomLeft(bottomLeft), normal(normal.unit()), friction(friction) {}

  void render(GLShader &shader);
  void collide(PointMass &pm);

  Vector3D topRight;
  Vector3D bottomLeft;
  Vector3D normal;

  double friction;
};

#endif /* COLLISIONOBJECT_PLANE_H */
