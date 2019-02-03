#include "window.h"
#include "glwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

Window::Window() {
  m_glWidget = new GLWidget(this);
  m_glWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  m_button_reset = new QPushButton(tr("Reset"), this);
  m_button_reset->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  connect(m_glWidget, &GLWidget::squareClicked, &m_engine, &GameEngine::makeMove);

  connect(&m_engine, &GameEngine::moveMade, m_glWidget, &GLWidget::setSquare);
  connect(&m_engine, &GameEngine::gameOver, m_glWidget, &GLWidget::finishGame);

  connect(m_button_reset, &QPushButton::clicked, &m_engine, &GameEngine::newGame);
  connect(&m_engine, &GameEngine::gameCreated, m_glWidget, &GLWidget::newGame);

  QHBoxLayout* h_layout = new QHBoxLayout;
  h_layout->setMargin(0);
  h_layout->addWidget(m_glWidget);
  h_layout->addWidget(m_button_reset);

  QVBoxLayout* v_layout = new QVBoxLayout;
  v_layout->setMargin(0);
  v_layout->addLayout(h_layout);

  setLayout(v_layout);
}
