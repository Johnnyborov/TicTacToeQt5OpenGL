#include "GameWindow.h"
#include "FieldWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

GameWindow::GameWindow() {
  m_field_widget = new FieldWidget(this);
  m_field_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  m_button_reset = new QPushButton(tr("Reset"), this);
  m_button_reset->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  connect(m_field_widget, &FieldWidget::squareClicked, &m_game_engine, &GameEngine::makeMove);

  connect(&m_game_engine, &GameEngine::moveMade, m_field_widget, &FieldWidget::setSquare);
  connect(&m_game_engine, &GameEngine::gameOver, m_field_widget, &FieldWidget::finishGame);

  connect(m_button_reset, &QPushButton::clicked, &m_game_engine, &GameEngine::newGame);
  connect(&m_game_engine, &GameEngine::gameCreated, m_field_widget, &FieldWidget::newGame);

  QHBoxLayout* h_layout = new QHBoxLayout;
  h_layout->setMargin(0);
  h_layout->addWidget(m_field_widget);
  h_layout->addWidget(m_button_reset);

  QVBoxLayout* v_layout = new QVBoxLayout;
  v_layout->setMargin(0);
  v_layout->addLayout(h_layout);

  setLayout(v_layout);
}
