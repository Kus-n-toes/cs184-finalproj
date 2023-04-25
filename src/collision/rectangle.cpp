#include "iostream"
#include <nanogui/nanogui.h>

#include "../clothMesh.h"
#include "../clothSimulator.h"
#include "rectangle.h"

using namespace std;
using namespace CGL;

#define SURFACE_OFFSET 0.0001

void Rectangle::collide(PointMass &pm) {
  // TODO (Part 3): Handle collisions with rectangles.
  if (dot(pm.last_position - topRight, normal) > 0 && dot(pm.position - topRight, normal) <= 0
        || dot(pm.last_position - topRight, normal) <= 0 && dot(pm.position - topRight, normal) > 0) {
      Vector3D v = pm.position - topRight;
      Vector3D tangent = pm.position - dot(v, normal) * normal;
      Vector3D correction = tangent - pm.last_position;
      if (topRight.x == bottomLeft.x) { //Rectangle is in yz dimension
          if (tangent.y > topRight.y || tangent.y < bottomLeft.y ||
                tangent.z > topRight.z || tangent.z < bottomLeft.z) {
              return; //Check if point inside of rectangle
          }
      } else if (topRight.y == bottomLeft.y) { //Rectangle is in xz dimension
          if (tangent.x > topRight.x || tangent.x < bottomLeft.x ||
              tangent.z > topRight.z || tangent.z < bottomLeft.z) {
              return; //Check if point inside of rectangle
          }
      } else { //Rectangle is in xy dimension
          if (tangent.x > topRight.x || tangent.x < bottomLeft.x ||
              tangent.y > topRight.y || tangent.y < bottomLeft.y) {
              return; //Check if point inside of rectangle
          }
      }
      if (dot(normal, correction) > 0) {
          correction -= SURFACE_OFFSET * normal;
      } else {
          correction += SURFACE_OFFSET * normal;
      }
      pm.position = pm.last_position + correction * (1.0 - friction);
  }
}

void Rectangle::render(GLShader &shader) {
  nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

  Vector3D center = (topRight + bottomLeft) / 2;
  Vector3D topLeft = cross(bottomLeft - center, normal);
  topLeft.normalize();
  topLeft = topLeft * (bottomLeft - center).norm() + center;
  Vector3D bottomRight = center + (center - topLeft);

  Vector3f sNormal(normal.x, normal.y, normal.z);

  MatrixXf positions(3, 4);
  MatrixXf normals(3, 4);

  positions.col(0) << Vector3f(bottomLeft.x, bottomLeft.y, bottomLeft.z);
  positions.col(1) << Vector3f(topLeft.x, topLeft.y, topLeft.z);
  positions.col(3) << Vector3f(topRight.x, topRight.y, topRight.z);
  positions.col(2) << Vector3f(bottomRight.x, bottomRight.y, bottomRight.z);

  normals.col(0) << sNormal;
  normals.col(1) << sNormal;
  normals.col(2) << sNormal;
  normals.col(3) << sNormal;

  if (shader.uniform("u_color", false) != -1) {
    shader.setUniform("u_color", color);
  }
  shader.uploadAttrib("in_position", positions);
  if (shader.attrib("in_normal", false) != -1) {
    shader.uploadAttrib("in_normal", normals);
  }

  shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}
