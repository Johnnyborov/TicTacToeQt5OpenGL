#include "glwidget.h"

#include <QImage>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget (parent) {

}

GLWidget::~GLWidget() {
  makeCurrent();

  delete m_square;

  delete m_texture;
  delete m_mesh;

  doneCurrent();
}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0.7f, 1);


  m_mesh = new Mesh();
  m_texture = new QOpenGLTexture(QImage(":/textures/box.png").mirrored());

  m_square = new Square(m_mesh, m_texture);


  createProgram();
  m_program.bind();

  m_program.setUniformValue("texture0", 0);

  m_view.translate(0.0, 0.0, -5.0);
  m_program.setUniformValue("view", m_view);

  m_model.setToIdentity();
  m_program.setUniformValue("model", m_model);
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_program.bind();
  m_square->draw();
}

void GLWidget::resizeGL(int width, int height) {
  float aspect = float(width) / float(height ? height : 1);
  const float zNear = 3.0, zFar = 7.0, fov = 45.0;

  m_projection.setToIdentity();
  m_projection.perspective(fov, aspect, zNear, zFar);

  m_program.setUniformValue("projection", m_projection);
}

void GLWidget::createProgram() {
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl");
  m_program.link();
}
