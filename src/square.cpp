#include "square.h"

#include <QOpenGLShaderProgram>

Square::Square(Mesh* mesh, QOpenGLTexture* texture, QOpenGLShaderProgram* program, QMatrix4x4* world)
  : m_mesh(mesh), m_texture(texture), m_program(program), m_world(world) {
  initializeOpenGLFunctions();
}

void Square::draw() {
  m_mesh->bind();
  m_texture->bind(0);

  m_program->setUniformValue("model", *m_world * m_local * m_z);
  glDrawElements(GL_TRIANGLES, m_mesh->count(), GL_UNSIGNED_INT, nullptr);
}

void Square::setLocalTransform(QMatrix4x4 local) {
  m_local = local;
}

void Square::setPressed(bool pressed) {
  m_z.setToIdentity();

  if (pressed) {
    m_z.translate(0.0, 0.0, -0.33);
  }
}
