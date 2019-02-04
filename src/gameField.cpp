#include "gameField.h"

#include <QImage>
#include <QOpenGLTexture>
#include <QMatrix4x4>


namespace constants {
constexpr float size = 2.0f;
constexpr float margin = 0.2f;
constexpr float step = size + margin;

constexpr float scale(int max_dim) { return 1.15f / max_dim; }
constexpr float translation(int max_dim) {
  return (size * scale(max_dim) * max_dim / 2)+(margin * scale(max_dim) * (max_dim-1) / 2);
}
} // namespace constants


void GameField::create(QOpenGLShaderProgram* program, QMatrix4x4* projection,
                       QMatrix4x4* view, QMatrix4x4* model) {
  m_program = program;

  m_projection = projection;
  m_view = view;
  m_model = model;

  m_mesh = new Mesh();
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/box.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/clear.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/cross.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/nought.png").mirrored()));

  newGame(3, 3, 3);
}


void GameField::destroy() {
  for (int i=0; i < m_squares.size(); ++i) {
    delete m_squares[i];
  }

  for (int i=0; i < m_textures.size(); ++i) {
    delete m_textures[i];
  }
  delete m_mesh;
}

void GameField::draw() {
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares[i]->draw();
  }
}


int GameField::tryPress(int x, int y, int w, int h) {
  int i = getSquareUnderMouse(x, y, w, h);

  if (i == -1) return -1;

  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return -1;

  m_squares[i]->setStatus(Statuses::Pressed);

  return i;
}


int GameField::tryRelease(int x, int y, int w, int h) {
  int i = getSquareUnderMouse(x, y, w, h);

  if (i == -1) return -1;

  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return -1;

  m_squares[i]->setStatus(Statuses::Normal);

  return i;
}


void GameField::newGame(int dim_x, int dim_y, int win_size) {
  for (int i=0; i < m_squares.size(); ++i) {
    delete m_squares[i];
  }
  m_squares.clear();

  m_dim_x = dim_x;
  m_dim_y = dim_y;
  m_win_size = win_size;

  m_over = false;

  m_squares.reserve(m_dim_x * m_dim_y);
  QMatrix4x4 local;
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares.push_back(new Square(m_mesh, &m_textures, m_program, m_model));

    local.setToIdentity();
    local.translate(1.0, -1.0, 0);
    local.translate((i % m_dim_x) * constants::step, - (i / m_dim_x) * constants::step, 0);


    m_squares[i]->setLocalTransform(local);
  }


  m_model->setToIdentity();
  int max_dim = std::max((m_dim_x), (m_dim_y));
  m_model->translate(-constants::translation(max_dim), constants::translation(max_dim), 0);
  m_model->scale(constants::scale(max_dim));
}


void GameField::setSquare(int i, SquareTypes type) {
  m_squares[i]->setType(type);
}


void GameField::finishGame(Conditions conditions) {
  int i = conditions.start_i;
  int j = conditions.start_j;

  switch(conditions.direction) {
    case Directions::Right:
      for (int k = 0; k < m_win_size; ++k)
        m_squares[i*m_dim_x + (j + k)]->setStatus(Statuses::WinChain);
      break;
    case Directions::Down:
      for (int k = 0; k < m_win_size; ++k)
        m_squares[(i+k)*m_dim_x + j]->setStatus(Statuses::WinChain);
      break;
    case Directions::RightDown:
      for (int k = 0; k < m_win_size; ++k)
        m_squares[(i+k)*m_dim_x + (j + k)]->setStatus(Statuses::WinChain);
      break;
    case Directions::LeftDown:
      for (int k = 0; k < m_win_size; ++k)
        m_squares[(i+k)*m_dim_x + (j - k)]->setStatus(Statuses::WinChain);
      break;
    }

  m_over = true;
}



// ======================RayCasting======================
int GameField::getSquareUnderMouse(int screen_x, int screen_y, int width, int height) {
  QVector3D ray_world = calculateRay(screen_x, screen_y, width, height);

  int max_dim = std::max((m_dim_x), (m_dim_y));
  float x, y;
  if (!trySetIntersectionWithPlaneXY(ray_world, 1.0f * constants::scale(max_dim), x, y)) return -1;

  int i, j;
  if (!trySetSquareIJ(x, y, i, j)) return -1;

  return i * m_dim_x + j;
}


QVector3D GameField::calculateRay(int screen_x, int screen_y, int width, int height) {
  float clip_x = (2.0f * screen_x) / width - 1.0f;
  float clip_y = 1.0f - (2.0f * screen_y) / height;
  QVector4D ray_clip(clip_x, clip_y, -1.0f, 1.0f);


  QVector4D ray_eye(m_projection->inverted() * ray_clip);
  ray_eye.setZ(-1.0f); // directed from camera
  ray_eye.setW(0.0f); // direction vector


  QVector3D ray_world(m_view->inverted() * ray_eye);
  ray_world.normalize();


  return ray_world;
}


bool GameField::trySetIntersectionWithPlaneXY(QVector3D ray_world, float plane_z, float& res_x, float& res_y) {
  // find ray intersection coordinates with squares plane in world space
  QVector3D camera(m_view->inverted() * QVector3D());

  QVector3D plane_normal(0.0f, 0.0f, 1.0f);

  float dot_plane_ray = QVector3D::dotProduct(ray_world, plane_normal);
  if (std::abs(dot_plane_ray) < 0.001f) return false; // ray parallel to plane
  float length = (plane_z - QVector3D::dotProduct(camera, plane_normal)) / dot_plane_ray;
  if (length < 0) return false; // intersection behind camera

  QVector4D intersection_coords(camera + length * ray_world, 1.0f);


  // put top left corner of top left square in (0, 0)
  // size and margin of squares become equal to the ones defined in constexpressions
  intersection_coords = m_model->inverted() * intersection_coords;

  QMatrix4x4 flip_y;
  flip_y.scale(1.0, -1.0);
  intersection_coords = flip_y * intersection_coords;

  res_x = intersection_coords.x();
  res_y = intersection_coords.y();
  return true;
}


// checks if (x, y) coordinates belong to any square and gets that square (i, j) if they do
bool  GameField::trySetSquareIJ(float x, float y, int& res_i, int& res_j) {
  float mrg = constants::margin;
  float step = constants::step;

  int j = static_cast<int>(x/step);
  int i = static_cast<int>(y/step);

  if (x < 0 || y < 0 || x > m_dim_x*step || y > m_dim_y*step) return false; // not in squares area
  if ((j+1)*step - x < mrg || (i+1)*step - y < mrg) return false; // between squares


  res_i = i;
  res_j = j;
  return true;
}
// ----------------------RayCasting----------------------
