#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

class QOpenGLShaderProgram;

class Square : protected QOpenGLFunctions {
public:
  Square(Mesh* mesh, QOpenGLTexture* texture, QOpenGLShaderProgram* program, QMatrix4x4* world);

  void draw();
  void setLocalTransform(QMatrix4x4 local);
private:
  Mesh* m_mesh;
  QOpenGLTexture* m_texture;
  QOpenGLShaderProgram* m_program;

  QMatrix4x4* m_world;
  QMatrix4x4 m_local;
};

#endif // SQUARE_H
