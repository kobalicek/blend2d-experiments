#include "blhtmlview.h"

static char master_css[] =
{
  #include "master.css.inc"
  , 0
};

BLHtmlView::BLHtmlView() {
  BLFontFace face;

  // TODO: Hardcoded...
  face.createFromFile("/home/petr/workspace/fonts/Arial.ttf");
  _fontManager.addFace(face);

  face.createFromFile("/home/petr/workspace/fonts/Courier_New.ttf");
  _fontManager.addFace(face);

  face.createFromFile("/home/petr/workspace/fonts/Times_New_Roman.ttf");
  _fontManager.addFace(face);

  _htmlContainer.setFontManager(_fontManager);

  setMouseTracking(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  litehtml::context htmlContext;
  htmlContext.load_master_stylesheet(master_css);
  _htmlDocument = litehtml::document::createFromUTF8("", &_htmlContainer, &htmlContext);
}

BLHtmlView::~BLHtmlView() {}

void BLHtmlView::setUrl(const char* url) {
  BLArray<uint8_t> content;
  BLFileSystem::readFile(url, content);

  litehtml::context htmlContext;
  htmlContext.load_master_stylesheet(master_css);

  const char* end = strrchr(url, '/');
  _htmlContainer._baseUrl.assign(url, size_t(end - url));
  _htmlDocument = litehtml::document::createFromUTF8((const char*)content.data(), &_htmlContainer, &htmlContext);
  _htmlDocument->render(width());
  htmlHeightChanged(_htmlDocument->height());
}

void BLHtmlView::setScrollY(int y) {
  _scrollY = y;
  updateCanvas();
}

void BLHtmlView::resizeEvent(QResizeEvent* event) {
  _resizeCanvas();
}

void BLHtmlView::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  if (_dirty)
    _renderCanvas();
  painter.drawImage(QPoint(0, 0), qtImage);
}

void BLHtmlView::mousePressEvent(QMouseEvent* event) {
}

void BLHtmlView::mouseReleaseEvent(QMouseEvent* event) {
}

void BLHtmlView::mouseMoveEvent(QMouseEvent* event) {
}

void BLHtmlView::updateCanvas(bool force) {
  if (force)
    _renderCanvas();
  else
    _dirty = true;
  repaint(0, 0, width(), height());
}

void BLHtmlView::_resizeCanvas() {
  int w = width();
  int h = height();

  if (qtImage.width() == w && qtImage.height() == h)
    return;

  qtImage = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
  blImage.createFromData(qtImage.width(), qtImage.height(), BL_FORMAT_PRGB32, qtImage.bits(), qtImage.bytesPerLine());
  _htmlContainer.setSize(BLSizeI(w, h));
  _htmlDocument->render(w);
  htmlHeightChanged(_htmlDocument->height());

  updateCanvas(false);
}

void BLHtmlView::_renderCanvas() {
  BLContextCreateInfo createInfo {};
  createInfo.threadCount = 3;

  BLContext ctx(blImage, createInfo);
  litehtml::position clip;
  clip.x = 0;
  clip.y = 0;
  clip.width = width();
  clip.height = height();

  ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
  ctx.fillAll();

  _htmlDocument->draw((uintptr_t)ctx.impl, 0, -_scrollY, &clip);
  _dirty = false;
}

#include "moc_blhtmlview.cpp"
