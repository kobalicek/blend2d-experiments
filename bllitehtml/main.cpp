#include <stdio.h>

#include <QtGui>
#include <QtWidgets>
#include "blhtmlview.h"

// ============================================================================
// [MainWindow]
// ============================================================================

class MainWindow : public QWidget {
  Q_OBJECT
  BLHtmlView htmlView;
  QScrollBar sb;

public:
  MainWindow() {
    QBoxLayout* hBox = new QHBoxLayout();

    sb.setOrientation(Qt::Vertical);

    hBox->setContentsMargins(0, 0, 0, 0);
    hBox->setSpacing(0);

    hBox->addWidget(&htmlView);
    hBox->addWidget(&sb);

    setLayout(hBox);

    // TODO: Hardcoded...
    htmlView.setUrl("/home/petr/workspace/www/blend2d.com/build/index.html");
    sb.setValue(0);
    sb.setRange(0, htmlView._htmlDocument->height());

    connect(&htmlView, SIGNAL(htmlHeightChanged(int)), SLOT(scrollHeightChanged(int)));
    connect(&sb, SIGNAL(valueChanged(int)), SLOT(scrollValueChanged(int)));
  }

  Q_SLOT
  void scrollValueChanged(int y) {
    htmlView.setScrollY(y);
  }

  Q_SLOT
  void scrollHeightChanged(int h) {
    sb.setPageStep(height());
    sb.setRange(0, std::max(h - height(), 0));
  }
};

// ============================================================================
// [Main]
// ============================================================================

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MainWindow win;

  win.setMinimumSize(QSize(400, 320));
  win.resize(QSize(580, 520));
  win.show();

  return app.exec();
}

#include "main.moc"
