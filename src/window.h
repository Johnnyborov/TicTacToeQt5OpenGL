#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class GLWidget;

class Window : public QWidget {
  Q_OBJECT

public:
  Window();

private:
  GLWidget* m_glWidget;
};

#endif // WINDOW_H
