#include "Mesh.h"

#include <QVector3D>
#include <QVector2D>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
    QVector3D normal;
};


Mesh::Mesh() : m_ebo(QOpenGLBuffer::IndexBuffer) {
  initializeOpenGLFunctions();

  m_vbo.create();
  m_ebo.create();

  m_vao.create();

  initGeometry();
}

Mesh::~Mesh() {
  m_vao.destroy();

  m_ebo.destroy();
  m_vbo.destroy();
}

void Mesh::bind() {
  m_vao.bind();
}


int Mesh::count() {
  return 6;
}


void Mesh::initGeometry() {
  QVector3D normal(0.0f, 0.0f,  1.0f);

  VertexData vertices[] = {
    {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f), normal},
    {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f), normal},
    {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f), normal},
    {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(1.0f, 1.0f), normal}
  };

  GLuint indices[] = {
    0, 1, 2,
    1, 3, 2
  };

  QOpenGLVertexArrayObject::Binder binder(&m_vao);

  m_vbo.bind();
  m_vbo.allocate(vertices, 4 * sizeof(VertexData));

  m_ebo.bind();
  m_ebo.allocate(indices, 6 * sizeof(GLuint));


  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  size_t offset = 0;
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(offset));

  offset += sizeof(QVector3D);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(offset));

  offset += sizeof(QVector2D);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), reinterpret_cast<void*>(offset));
}
