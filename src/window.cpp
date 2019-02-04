#include "window.h"
#include "gameWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

Window::Window() {
  m_game_widget = new GameWidget(this);
  m_game_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  m_button_reset = new QPushButton(tr("Reset"), this);
  m_button_reset->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  connect(m_game_widget, &GameWidget::squareClicked, &m_engine, &GameEngine::makeMove);

  connect(&m_engine, &GameEngine::moveMade, m_game_widget, &GameWidget::setSquare);
  connect(&m_engine, &GameEngine::gameOver, m_game_widget, &GameWidget::finishGame);

  connect(m_button_reset, &QPushButton::clicked, &m_engine, &GameEngine::newGame);
  connect(&m_engine, &GameEngine::gameCreated, m_game_widget, &GameWidget::newGame);

  QHBoxLayout* h_layout = new QHBoxLayout;
  h_layout->setMargin(0);
  h_layout->addWidget(m_game_widget);
  h_layout->addWidget(m_button_reset);

  QVBoxLayout* v_layout = new QVBoxLayout;
  v_layout->setMargin(0);
  v_layout->addLayout(h_layout);

  setLayout(v_layout);
}
