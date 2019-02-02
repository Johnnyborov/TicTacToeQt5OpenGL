#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "square.h"
#include "mesh.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <vector>

class QMouseEvent;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  GLWidget(QWidget* parent = nullptr);
  ~GLWidget();

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

  void mousePressEvent(QMouseEvent* event);

private:
  void createProgram();

  int getSquareUnderMouse(int screen_x, int screen_y);

  QVector3D calculateRay(int screen_x, int screen_y);
  bool trySetIntersectionWithPlaneXY(QVector3D ray_world, float plane_distance, float& res_x, float& res_y);
  bool trySetSquareIJ(float x, float y, int& res_i, int& res_j);


  int m_size;
  std::vector<Square*> m_squares;

  Mesh* m_mesh;
  QOpenGLTexture* m_texture;

  QOpenGLShaderProgram m_program;

  QMatrix4x4 m_projection;
  QMatrix4x4 m_view;
  QMatrix4x4 m_world;
};

#endif // GLWIDGET_H
