#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"

#include <QOpenGLTexture>
#include <QOpenGLFunctions>

class Square : protected QOpenGLFunctions {
public:
  Square(Mesh* mesh, QOpenGLTexture* texture);

  void draw();
private:
  Mesh* m_mesh;
  QOpenGLTexture* m_texture;
};

#endif // SQUARE_H
