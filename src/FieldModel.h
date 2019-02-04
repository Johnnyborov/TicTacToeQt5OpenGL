#ifndef FIELD_MODEL_H
#define FIELD_MODEL_H

#include "Square.h"
#include "Mesh.h"

#include <vector>

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

  int tryPress(int x, int y, int w, int h);
  int tryRelease(int x, int y, int w, int h);

private:
  int getSquareUnderMouse(int x, int y, int w, int h);

  QVector3D calculateRay(int x, int y, int w, int h);
  bool trySetIntersectionWithPlaneXY(QVector3D ray_world, float plane_distance, float& res_x, float& res_y);
  bool trySetSquareIJ(float x, float y, int& res_i, int& res_j);


  std::vector<Square*> m_squares;
  int m_dim_x = 3;
  int m_dim_y = 3;
  int m_win_size = 3;
  bool m_over = false;


  Mesh* m_mesh = nullptr;
  std::vector<QOpenGLTexture*> m_textures;

  QOpenGLShaderProgram* m_program = nullptr;

  QMatrix4x4* m_projection = nullptr;
  QMatrix4x4* m_view = nullptr;
  QMatrix4x4* m_model = nullptr;
};

#endif // FIELD_MODEL_H