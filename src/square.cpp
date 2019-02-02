#include "square.h"

Square::Square(Mesh* mesh, QOpenGLTexture* texture) : m_mesh(mesh), m_texture(texture) {
  initializeOpenGLFunctions();
}

void Square::draw() {
  m_mesh->bind();
  m_texture->bind(0);

  glDrawElements(GL_TRIANGLES, m_mesh->count(), GL_UNSIGNED_INT, nullptr);
}
