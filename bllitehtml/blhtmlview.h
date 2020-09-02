#ifndef BLHTMLVIEW_H_INCLUDED
#define BLHTMLVIEW_H_INCLUDED

#include <cmath>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <functional>

#include <blend2d.h>
#include "blhtmlcontainer.h"

#include <QtGui>
#include <QtWidgets>

class BLHtmlView : public QWidget {
  Q_OBJECT

public:
  QImage qtImage;
  BLImage blImage;
  BLFontManager _fontManager;
  litehtml::document::ptr _htmlDocument;
  BLHtmlContainer _htmlContainer;
  bool _dirty = true;

  int _scrollY = 0;

  BLHtmlView();
  ~BLHtmlView();

  void setUrl(const char* url);
  void setScrollY(int y);

  void resizeEvent(QResizeEvent* event) override;
  void paintEvent(QPaintEvent *event) override;

  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  void updateCanvas(bool force = false);
  void _resizeCanvas();
  void _renderCanvas();
  void _afterRender();

Q_SIGNALS:
  void htmlHeightChanged(int newValue);
};

#endif // BLHTMLVIEW_H_INCLUDED
