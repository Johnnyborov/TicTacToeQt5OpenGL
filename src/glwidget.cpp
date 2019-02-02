#include "glwidget.h"

#include <QImage>
#include <QMouseEvent>

namespace constants {
constexpr int max_dim = 3;

constexpr float size = 2.0f;
constexpr float margin = 0.2f;
constexpr float step = size + margin;

constexpr float scale = 1.25f / max_dim;
constexpr float translation = (size * scale * max_dim / 2)+(margin * scale * (max_dim-1) / 2);
} // namespace constants

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget (parent) {
  m_size = 9;
}

GLWidget::~GLWidget() {
  makeCurrent();

  for (int i=0; i < m_size; ++i) {
    delete m_squares[i];
  }

  delete m_texture;
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

  m_view.translate(0.0, 0.0, -5.0);
  m_program.setUniformValue("view", m_view);

  m_world.setToIdentity();
  m_world.translate(-constants::translation, constants::translation, 0);
  m_world.scale(constants::scale, constants::scale);


  m_mesh = new Mesh();
  m_texture = new QOpenGLTexture(QImage(":/textures/box.png").mirrored());


  m_squares.reserve(m_size);
  QMatrix4x4 local;
  for (int i=0; i < m_size; ++i) {
    m_squares.push_back(new Square(m_mesh, m_texture, &m_program, &m_world));

    local.setToIdentity();
    local.translate(1, -1, 0);
    local.translate((i % 3) * constants::step, - (i / 3) * constants::step, 0);


    m_squares[i]->setLocalTransform(local);
  }
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_program.bind();
  for (int i=0; i < m_size; ++i) {
    m_squares[i]->draw();
  }
}

void GLWidget::resizeGL(int width, int height) {
  float aspect = float(width) / float(height ? height : 1);
  const float zNear = 3.0, zFar = 7.0, fov = 45.0;

  m_projection.setToIdentity();
  m_projection.perspective(fov, aspect, zNear, zFar);

  m_program.setUniformValue("projection", m_projection);
}

void GLWidget::createProgram() {
  m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.glsl");
  m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.glsl");
  m_program.link();
}


void GLWidget::mousePressEvent(QMouseEvent* event) {
  qDebug() << getSquareUnderMouse(event->x(), event->y());
}



// ======================RayCasting======================
int GLWidget::getSquareUnderMouse(int screen_x, int screen_y) {
  QVector3D ray_world = calculateRay(screen_x, screen_y);

  float x, y;
  if (!trySetIntersectionWithPlaneXY(ray_world, 1.0f, x, y)) return -1;

  int i, j;
  if (!trySetSquareIJ(x, y, i, j)) return -1;

  return i * constants::max_dim + j;
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
  QVector3D camera(0.0f, 0.0f, 5.0f);
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
  int md = constants::max_dim;
  float mrg = constants::margin;
  float step = constants::step;

  int j = static_cast<int>(x/step);
  int i = static_cast<int>(y/step);

  if (x < 0 || y < 0 || x > md*step || y > md*step) return false; // not in squares area
  if ((j+1)*step - x < mrg || (i+1)*step - y < mrg) return false; // between squares


  res_i = i;
  res_j = j;
  return true;
}
// ----------------------RayCasting----------------------
