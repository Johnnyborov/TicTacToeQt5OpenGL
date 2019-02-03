#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"
#include "gameEngine.h"

#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <vector>

class QOpenGLShaderProgram;

enum class Statuses : unsigned char { Normal, Pressed, WinChain };

class Square : protected QOpenGLFunctions {
public:
  Square(Mesh* mesh, std::vector<QOpenGLTexture*> textures, QOpenGLShaderProgram* program, QMatrix4x4* world);

  void draw();
  void setLocalTransform(QMatrix4x4 local);
  void setStatus(Statuses status);
  void setType(SquareTypes type);
  SquareTypes getType();

private:
  Mesh* m_mesh;
  std::vector<QOpenGLTexture*> m_textures;
  QOpenGLTexture* m_texture0;
  QOpenGLTexture* m_texture1;
  QOpenGLShaderProgram* m_program;

  QMatrix4x4* m_world;
  QMatrix4x4 m_local;
  QMatrix4x4 m_z;

  bool m_is_chain_member = false;

  SquareTypes m_type = SquareTypes::Clear;
};

#endif // SQUARE_H
