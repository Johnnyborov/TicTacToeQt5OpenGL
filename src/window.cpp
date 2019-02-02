#include "window.h"
#include "glwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

Window::Window() {
  m_glWidget = new GLWidget(this);


  QHBoxLayout* h_layout = new QHBoxLayout;
  h_layout->setMargin(0);
  h_layout->addWidget(m_glWidget);

  QVBoxLayout* v_layout = new QVBoxLayout;
  v_layout->setMargin(0);
  v_layout->addLayout(h_layout);

  setLayout(v_layout);
}
