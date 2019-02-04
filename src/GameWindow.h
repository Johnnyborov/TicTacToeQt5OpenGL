#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include "GameEngine.h"

#include <QWidget>

class FieldWidget;
class QPushButton;

class GameWindow : public QWidget {
  Q_OBJECT

public:
  GameWindow();

private:
  GameEngine m_game_engine;

  FieldWidget* m_field_widget;
  
  QPushButton* m_button_reset;
};

#endif // GAME_WINDOW_H
