#include "glwidget.h"

#include <QImage>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QQuaternion>

namespace constants {
constexpr float size = 2.0f;
constexpr float margin = 0.2f;
constexpr float step = size + margin;

constexpr float scale(int max_dim) { return 1.15f / max_dim; }
constexpr float translation(int max_dim) {
  return (size * scale(max_dim) * max_dim / 2)+(margin * scale(max_dim) * (max_dim-1) / 2);
}
} // namespace constants

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget (parent) {
  setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

GLWidget::~GLWidget() {
  makeCurrent();

  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    delete m_squares[i];
  }

  for (int i=0; i < m_textures.size(); ++i) {
    delete m_textures[i];
  }
  delete m_mesh;

  doneCurrent();
}

void GLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0, 0, 0.7f, 1);


  createProgram();
  m_program.bind();

  m_program.setUniformValue("texture0", 0);
  m_program.setUniformValue("texture1", 1);

  m_angle_x = 0;
  m_angle_z = 0;
  m_view.translate(0.0, 0.0, -4.7);
  m_view.rotate(m_angle_x, 1.0, 0.0, 0.0);
  m_view.translate(0.0, 0.0, -0.3);
  m_view.rotate(m_angle_z, 0.0, 0.0, 1.0);
  m_program.setUniformValue("view", m_view);

  m_world.setToIdentity();
  int max_dim = std::max((m_dim_x), (m_dim_y));
  m_world.translate(-constants::translation(max_dim), constants::translation(max_dim), 0);
  m_world.scale(constants::scale(max_dim));


  m_mesh = new Mesh();
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/box.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/clear.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/cross.png").mirrored()));
  m_textures.push_back(new QOpenGLTexture(QImage(":/textures/nought.png").mirrored()));

  m_squares.reserve(m_dim_x * m_dim_y);
  QMatrix4x4 local;
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares.push_back(new Square(m_mesh, m_textures, &m_program, &m_world));

    local.setToIdentity();
    local.translate(1.0, -1.0, 0);
    local.translate((i % 3) * constants::step, - (i / 3) * constants::step, 0);


    m_squares[i]->setLocalTransform(local);
  }
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_program.bind();
  for (int i=0; i < m_dim_x * m_dim_y; ++i) {
    m_squares[i]->draw();
  }
}

void GLWidget::resizeGL(int width, int height) {
  float aspect = float(width) / float(height ? height : 1);
  const float zNear = 1.0, zFar = 9.0, fov = 45.0;

  m_projection.setToIdentity();
  m_projection.perspective(fov, aspect, zNear, zFar);

  m_program.setUniformValue("projection", m_projection);
}

void GLWidget::createProgram() {
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl");
  m_program.link();
}


void GLWidget::setSquare(int i, SquareTypes type) {
  m_squares[i]->setType(type);

  update();
}


void GLWidget::finishGame(Conditions conditions) {
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

  update();
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  int i = getSquareUnderMouse(event->x(), event->y());

  if (i == -1) return;

  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return;

  m_squares[i]->setStatus(Statuses::Pressed);
  update();
}


void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
  int i = getSquareUnderMouse(event->x(), event->y());

  if (i == -1) return;


  if (m_over || m_squares[i]->getType() != SquareTypes::Clear) return;

  m_squares[i]->setStatus(Statuses::Normal);

  emit squareClicked(i);
  update();
}


void GLWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key::Key_Up) {
    m_angle_x += 5;
  } else if (event->key() == Qt::Key::Key_Down) {
    m_angle_x -= 5;
  }  else if (event->key() == Qt::Key::Key_Left) {
    m_angle_z += 5;
  }  else if (event->key() == Qt::Key::Key_Right) {
    m_angle_z -= 5;
  } else {
    QWidget::keyPressEvent(event);
  }

  if (m_angle_x > 360) m_angle_x -= 360;
  if (m_angle_x < 0) m_angle_x += 360;
  if (m_angle_z > 360) m_angle_z -= 360;
  if (m_angle_z < 0) m_angle_z += 360;


  m_view.setToIdentity();
  m_view.translate(0.0, 0.0, -4.7);
  m_view.rotate(m_angle_x, 1.0, 0.0, 0.0);
  m_view.translate(0.0, 0.0, -0.3);
  m_view.rotate(m_angle_z, 0.0, 0.0, 1.0);

  m_program.bind();
  m_program.setUniformValue("view", m_view);
  update();
}



// ======================RayCasting======================
int GLWidget::getSquareUnderMouse(int screen_x, int screen_y) {
  QVector3D ray_world = calculateRay(screen_x, screen_y);

  int max_dim = std::max((m_dim_x), (m_dim_y));
  float x, y;
  if (!trySetIntersectionWithPlaneXY(ray_world, 1.0f * constants::scale(max_dim), x, y)) return -1;

  int i, j;
  if (!trySetSquareIJ(x, y, i, j)) return -1;

  return i * m_dim_x + j;
}


QVector3D GLWidget::calculateRay(int screen_x, int screen_y) {
  float clip_x = (2.0f * screen_x) / width() - 1.0f;
  float clip_y = 1.0f - (2.0f * screen_y) / height();
  QVector4D ray_clip(clip_x, clip_y, -1.0f, 1.0f);


  QVector4D ray_eye(m_projection.inverted() * ray_clip);
  ray_eye.setZ(-1.0f); // directed from camera
  ray_eye.setW(0.0f); // direction vector


  QVector3D ray_world(m_view.inverted() * ray_eye);
  ray_world.normalize();


  return ray_world;
}


bool GLWidget::trySetIntersectionWithPlaneXY(QVector3D ray_world, float plane_z, float& res_x, float& res_y) {
  // find ray intersection coordinates with squares plane in world space
  QVector3D camera(m_view.inverted() * QVector3D());

  QVector3D plane_normal(0.0f, 0.0f, 1.0f);

  float dot_plane_ray = QVector3D::dotProduct(ray_world, plane_normal);
  if (std::abs(dot_plane_ray) < 0.001f) return false; // ray parallel to plane
  float length = (plane_z - QVector3D::dotProduct(camera, plane_normal)) / dot_plane_ray;
  if (length < 0) return false; // intersection behind camera

  QVector4D intersection_coords(camera + length * ray_world, 1.0f);


  // put top left corner of top left square in (0, 0)
  // size and margin of squares become equal to the ones defined in constexpressions
  intersection_coords = m_world.inverted() * intersection_coords;

  QMatrix4x4 flip_y;
  flip_y.scale(1.0, -1.0);
  intersection_coords = flip_y * intersection_coords;

  res_x = intersection_coords.x();
  res_y = intersection_coords.y();
  return true;
}


// checks if (x, y) coordinates belong to any square and gets that square (i, j) if they do
bool  GLWidget::trySetSquareIJ(float x, float y, int& res_i, int& res_j) {
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
