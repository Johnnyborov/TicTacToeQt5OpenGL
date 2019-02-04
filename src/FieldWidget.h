#ifndef FIELD_WIDGET_H
#define FIELD_WIDGET_H

#include "FieldModel.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class QMouseEvent;
class QKeyEvent;

class FieldWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

public:
  FieldWidget(QWidget* parent = nullptr);
  ~FieldWidget();

  QSize minimumSizeHint() const override;

public slots:
  void newGame(int dim_x, int dim_y, int win_size);
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

  QOpenGLShaderProgram m_program;

  FieldModel m_field;

  QMatrix4x4 m_projection;
  QMatrix4x4 m_view;
  QMatrix4x4 m_model;

  float m_angle_x;
  float m_angle_z;
  float m_fov = 45.0f;
};

#endif // FIELD_WIDGET_H
