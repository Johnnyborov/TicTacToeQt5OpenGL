#include "square.h"

#include <QOpenGLShaderProgram>

Square::Square(Mesh* mesh, std::vector<QOpenGLTexture*> textures, QOpenGLShaderProgram* program, QMatrix4x4* world)
  : m_mesh(mesh), m_textures(textures), m_program(program), m_world(world) {
  initializeOpenGLFunctions();

  m_texture0 = m_textures[0];
  m_texture1 = m_textures[1];
}


void Square::draw() {
  m_mesh->bind();
  m_texture0->bind(0);
  m_texture1->bind(1);

  m_program->setUniformValue("model", *m_world * m_local * m_z);
  m_program->setUniformValue("is_green", m_is_chain_member);

  glDrawElements(GL_TRIANGLES, m_mesh->count(), GL_UNSIGNED_INT, nullptr);
}


void Square::setLocalTransform(QMatrix4x4 local) {
  m_local = local;
}


void Square::setStatus(Statuses status) {
  m_z.setToIdentity();

  if (status == Statuses::Pressed) {
    m_z.translate(0.0, 0.0, -0.33);
  } else if (status == Statuses::WinChain) {
    m_is_chain_member = true;
  }
}


void Square::setType(SquareTypes type) {
  m_type = type;
  m_texture1 = m_textures[1 + static_cast<unsigned int>(type)];
}


SquareTypes Square::getType() {
  return m_type;
}
