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
class QKeyEvent;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  GLWidget(QWidget* parent = nullptr);
  ~GLWidget();

public slots:
  void setSquare(int i, SquareTypes type);
  void finishGame(Conditions conditions);

signals:
  void squareClicked(int i);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

private:
  void createProgram();

  int getSquareUnderMouse(int screen_x, int screen_y);

  QVector3D calculateRay(int screen_x, int screen_y);
  bool trySetIntersectionWithPlaneXY(QVector3D ray_world, float plane_distance, float& res_x, float& res_y);
  bool trySetSquareIJ(float x, float y, int& res_i, int& res_j);


  std::vector<Square*> m_squares;
  int m_dim_x = 3;
  int m_dim_y = 3;
  int m_win_size = 3;
  bool m_over = false;


  Mesh* m_mesh;
  std::vector<QOpenGLTexture*> m_textures;

  QOpenGLShaderProgram m_program;

  QMatrix4x4 m_projection;
  QMatrix4x4 m_view;
  QMatrix4x4 m_world;

  float m_angle_x;
  float m_angle_z;
};

#endif // GLWIDGET_H
