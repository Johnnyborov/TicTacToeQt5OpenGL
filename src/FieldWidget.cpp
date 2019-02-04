#include "FieldWidget.h"

#include <QMouseEvent>
#include <QKeyEvent>


FieldWidget::FieldWidget(QWidget* parent) : QOpenGLWidget (parent) {
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}


FieldWidget::~FieldWidget() {
  makeCurrent();

  m_field.destroy();

  doneCurrent();
}

QSize FieldWidget::minimumSizeHint() const {
  return QSize(400, 400);
}


void FieldWidget::initializeGL() {
  initializeOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0.7f, 1);


  createProgram();
  m_program.bind();

  m_program.setUniformValue("texture0", 0);
  m_program.setUniformValue("texture1", 1);

  m_angle_x = 0;
  m_angle_z = 0;
  m_view.translate(0.0, 0.0, -4.7);
  m_view.rotate(m_angle_x, 1.0, 0.0, 0.0);
  m_view.translate(0.0, 0.0, -0.3);
  m_view.rotate(m_angle_z, 0.0, 0.0, 1.0);
  m_program.setUniformValue("view", m_view);


  m_field.create(&m_program, &m_projection, &m_view, &m_model);
}


void FieldWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_program.bind();

  m_field.draw();
}


void FieldWidget::resizeGL(int width, int height) {
  float aspect = float(width) / float(height ? height : 1);
  const float zNear = 1.0, zFar = 9.0;

  m_projection.setToIdentity();
  m_projection.perspective(m_fov, aspect, zNear, zFar);

  m_program.setUniformValue("projection", m_projection);
}


void FieldWidget::createProgram() {
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl");
  m_program.link();
}


void FieldWidget::newGame(int dim_x, int dim_y, int win_size) {
  m_field.newGame(dim_x, dim_y, win_size);

  update();

  setFocus();
}


void FieldWidget::setSquare(int i, SquareTypes type) {
  m_field.setSquare(i, type);

  update();
}


void FieldWidget::finishGame(Conditions conditions) {
  m_field.finishGame(conditions);

  update();
}

void FieldWidget::mousePressEvent(QMouseEvent* event) {
  int i = m_field.tryPress(event->x(), event->y(), width(), height());

  if (i == -1) return;

  update();
}


void FieldWidget::mouseReleaseEvent(QMouseEvent* event) {
  int i = m_field.tryRelease(event->x(), event->y(), width(), height());

  if (i == -1) return;

  emit squareClicked(i);
  update();
}


void FieldWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key::Key_Up) {
    m_angle_x += 5;
  } else if (event->key() == Qt::Key::Key_Down) {
    m_angle_x -= 5;
  }  else if (event->key() == Qt::Key::Key_Left) {
    m_angle_z += 5;
  }  else if (event->key() == Qt::Key::Key_Right) {
    m_angle_z -= 5;
  } else if (event->key() == Qt::Key::Key_Plus) {
    m_fov -= 5.0;
  }  else if (event->key() == Qt::Key::Key_Minus) {
    m_fov += 5.0;
  } else {
    QWidget::keyPressEvent(event);
  }

  if (m_angle_x > 360) m_angle_x -= 360;
  if (m_angle_x < 0) m_angle_x += 360;
  if (m_angle_z > 360) m_angle_z -= 360;
  if (m_angle_z < 0) m_angle_z += 360;

  if (m_fov < 9) m_fov = 10;
  if (m_fov > 176) m_fov = 175;


  m_view.setToIdentity();
  m_view.translate(0.0, 0.0, -4.7);
  m_view.rotate(m_angle_x, 1.0, 0.0, 0.0);
  m_view.translate(0.0, 0.0, -0.3);
  m_view.rotate(m_angle_z, 0.0, 0.0, 1.0);

  float aspect = float(width()) / height();
  const float zNear = 1.0, zFar = 9.0;
  m_projection.setToIdentity();
  m_projection.perspective(m_fov, aspect, zNear, zFar);

  m_program.bind();
  m_program.setUniformValue("view", m_view);
  m_program.setUniformValue("projection", m_projection);
  update();
}
