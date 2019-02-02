#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "square.h"
#include "mesh.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  GLWidget(QWidget* parent = nullptr);
  ~GLWidget();

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;

private:
  void createProgram();

  Square* m_square;

  Mesh* m_mesh;
  QOpenGLTexture* m_texture;

  QOpenGLShaderProgram m_program;

  QMatrix4x4 m_projection;
  QMatrix4x4 m_view;
  QMatrix4x4 m_model;
};

#endif // GLWIDGET_H
