#ifndef FIELD_MODEL_H
#define FIELD_MODEL_H

#include "Square.h"
#include "Mesh.h"

#include <vector>
#include <memory>

class QOpenGLShaderProgram;
class QOpenGLTexture;
class QMatrix4x4;

class FieldModel {
public:
  void create(QOpenGLShaderProgram* program, QMatrix4x4* projection,
              QMatrix4x4* view, QMatrix4x4* model);
  void destroy();

  void newGame(int dim_x, int dim_y, int win_size);
  void setSquare(int i, SquareTypes type);
  void finishGame(Conditions conditions);

  void draw();

  int getSquareUnderMouse(int x, int y, int w, int h);
  void press(int i);
  void release(int i);

private:
  QVector3D calculateRay(int x, int y, int w, int h);
  float calculateIntersectionWithPlaneRayLength(
      QVector3D camera, QVector3D ray, float plane_distance, QVector3D plane_normal);


  std::unique_ptr<Mesh> m_mesh;
  std::vector<std::unique_ptr<QOpenGLTexture>> m_textures;

  std::vector<std::unique_ptr<Square>> m_squares;
  int m_dim_x;
  int m_dim_y;
  int m_win_size;
  bool m_over;

  QOpenGLShaderProgram* m_program = nullptr;

  QMatrix4x4* m_projection = nullptr;
  QMatrix4x4* m_view = nullptr;
  QMatrix4x4* m_model = nullptr;
};

#endif // FIELD_MODEL_H
