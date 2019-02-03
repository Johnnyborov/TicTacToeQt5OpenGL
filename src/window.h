#ifndef WINDOW_H
#define WINDOW_H

#include "gameEngine.h"

#include <QWidget>

class GLWidget;
class QPushButton;

class Window : public QWidget {
  Q_OBJECT

public:
  Window();

private:
  GLWidget* m_glWidget;

  GameEngine m_engine;

  QPushButton* m_button_reset;
};

#endif // WINDOW_H
