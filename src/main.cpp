#include "window.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QSurfaceFormat fmt;
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(fmt);

  Window w;
  w.resize(400,400);
  w.show();

  return app.exec();
}
