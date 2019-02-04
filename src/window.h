#ifndef WINDOW_H
#define WINDOW_H

#include "gameEngine.h"

#include <QWidget>

class GameWidget;
class QPushButton;

class Window : public QWidget {
  Q_OBJECT

public:
  Window();

private:
  GameWidget* m_game_widget;

  GameEngine m_engine;

  QPushButton* m_button_reset;
};

#endif // WINDOW_H
