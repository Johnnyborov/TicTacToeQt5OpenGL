#include "FieldModel.h"

#include <QImage>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <limits>


namespace constants {
constexpr float size = 2.0f;
constexpr float margin = 0.2f;
constexpr float step = size + margin;

constexpr float height = 1.0f;
constexpr float presision = 0.001f;

constexpr float scale(int max_dim) { return 1.15f / max_dim; }
constexpr float translation(int max_dim) {
  return (size * scale(max_dim) * max_dim / 2)+(margin * scale(max_dim) * (max_dim-1) / 2);
}
} // namespace constants


void FieldModel::create(QOpenGLShaderProgram* program, QMatrix4x4* projection,
                       QMatrix4x4* view, QMatrix4x4* model) {
  m_program = program;

  m_projection = projection;
  m_view = view;
  m_model = model;

  m_mesh = std::make_unique<Mesh>();
  m_textures.emplace_back(std::make_unique<QOpenGLTexture>(QImage(":/textures/box.png").mirrored()));
  m_textures.emplace_back(std::make_unique<QOpenGLTexture>(QImage(":/textures/clear.png").mirrored()));
  m_textures.emplace_back(std::make_unique<QOpenGLTexture>(QImage(":/textures/cross.png").mirrored()));
  m_textures.emplace_back(std::make_unique<QOpenGLTexture>(QImage(":/textures/nought.png").mirrored()));
}


void FieldModel::destroy() {
  m_squares.clear();

  m_textures.clear();

  m_mesh.reset(nullptr);
}

void FieldModel::draw() {
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares[i]->draw();
  }
}


void FieldModel::press(int i) {
  if (i == -1) return;

  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return;

  m_squares[i]->setStatus(Statuses::Pressed);
}


void FieldModel::release(int i) {
  if (i == -1) return;

  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return;

  m_squares[i]->setStatus(Statuses::Normal);
}


void FieldModel::newGame(int dim_x, int dim_y, int win_size) {
  m_squares.clear();


  m_dim_x = dim_x;
  m_dim_y = dim_y;
  m_win_size = win_size;

  m_over = false;


  m_squares.reserve(m_dim_x * m_dim_y);
  QMatrix4x4 local;
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares.emplace_back(std::make_unique<Square>(m_mesh.get(), &m_textures, m_program, m_model));

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


void FieldModel::setSquare(int i, SquareTypes type) {
  m_squares[i]->setType(type);
}


void FieldModel::finishGame(Conditions conditions) {
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
int FieldModel::getSquareUnderMouse(int screen_x, int screen_y, int width, int height) {
  QVector3D ray = calculateRay(screen_x, screen_y, width, height);
  QVector3D camera(m_model->inverted() * m_view->inverted() * QVector3D());


  QVector3D plane_normals[3] = {
    QVector3D(0.0f, 0.0f, 1.0f),
    QVector3D(0.0f, 1.0f, 0.0f),
    QVector3D(1.0f, 0.0f, 0.0f)
  };

  float plane_distances[3][2] = {
    {0, 1},
    {-1, 1},
    {-1, 1}
  };


  int index = -1;
  float min_length = std::numeric_limits<float>::max();

  for (unsigned k = 0; k < m_squares.size(); ++k) {
    QVector3D origin(m_squares[k]->getLocalTransform() * QVector4D(0.0, 0.0, 0.0, 1.0));

    for (unsigned i = 0; i < 3; ++i) {
      float offset = QVector3D::dotProduct(origin, plane_normals[i]);
      float min_length_local = std::numeric_limits<float>::max();

      for (unsigned j = 0; j < 2; ++j) {
        float length = calculateIntersectionWithPlaneRayLength(
                         camera, ray, offset + plane_distances[i][j], plane_normals[i]);

        if (length > 0 && length < min_length_local)
          min_length_local = length;
      }

      if (min_length_local < std::numeric_limits<float>::max() - constants::presision &&
          min_length_local < min_length) {

        QVector3D p(m_squares[k]->getLocalTransform().inverted() * QVector4D(camera + min_length_local * ray, 1.0));

        if (p.z() >= 0 - constants::presision && p.z() <= constants::height + constants::presision &&
            -p.y() >= -constants::size/2 - constants::presision && -p.y() <= constants::size/2 + constants::presision &&
            p.x() >= -constants::size/2 - constants::presision && p.x() <= constants::size/2 + constants::presision) {

          min_length = min_length_local;
          index = static_cast<int>(k);
        }
      }
    }
  }

  return index;
}


QVector3D FieldModel::calculateRay(int screen_x, int screen_y, int width, int height) {
  float clip_x = (2.0f * screen_x) / width - 1.0f;
  float clip_y = 1.0f - (2.0f * screen_y) / height;
  QVector4D ray_clip(clip_x, clip_y, -1.0f, 1.0f);


  QVector4D ray_eye(m_projection->inverted() * ray_clip);
  ray_eye.setZ(-1.0f); // directed from camera
  ray_eye.setW(0.0f); // direction vector


  QVector3D ray(m_model->inverted() * m_view->inverted() * ray_eye);
  ray.normalize();


  return ray;
}


float FieldModel::calculateIntersectionWithPlaneRayLength(
    QVector3D camera, QVector3D ray, float plane_distance, QVector3D plane_normal) {

  float dot_plane_ray = QVector3D::dotProduct(ray, plane_normal);
  if (std::abs(dot_plane_ray) < constants::presision) return -1.0f; // ray parallel to plane
  float length = (plane_distance - QVector3D::dotProduct(camera, plane_normal)) / dot_plane_ray;

  if (length < 0) return -1.0f; // intersection behind camera

  return length;
}
// ----------------------RayCasting----------------------
