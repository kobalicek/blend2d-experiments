#include "blhtmlcontainer.h"

static bool operator==(const litehtml::web_color& a, const litehtml::web_color& b) {
  return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
}

BLHtmlContainer::BLHtmlContainer() {
  _size.reset();
}

BLHtmlContainer::~BLHtmlContainer() {
}

litehtml::uint_ptr BLHtmlContainer::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) {
  litehtml::string_vector fontNames;
  litehtml::split_string(faceName, fontNames, ",");

  fontNames.push_back("Arial");

  BLFontQueryProperties properties {};
  properties.weight = int(weight);
  properties.style = italic == litehtml::fontStyleNormal ? BL_FONT_STYLE_NORMAL : BL_FONT_STYLE_ITALIC;

  BLFontFace face;
  BLFontCore font;

  blFontInit(&font);

  for (const auto& fontName : fontNames) {
    const char* fName = fontName.c_str();

    if (fontName == "monospace")
      fName = "Courier New";

    if (_fontManager.queryFace(fName, properties, face) == BL_SUCCESS) {
      BLFontMetrics blfm;
      blFontCreateFromFace(&font, &face, float(size));
      blFontGetMetrics(&font, &blfm);

      fm->ascent = int(blfm.ascent);
      fm->descent = int(blfm.descent);
      fm->height = int(blfm.ascent + blfm.descent + blfm.lineGap);
      fm->x_height = int(blfm.xHeight);

      return (litehtml::uint_ptr)font.impl;
    }
  }

  return (litehtml::uint_ptr)font.impl;
}

void BLHtmlContainer::delete_font(litehtml::uint_ptr hFont) {
  BLFontCore font { (BLFontImpl*)hFont };
  blFontReset(&font);
}

int BLHtmlContainer::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) {
  BLFontCore font { (BLFontImpl*)hFont };

  _glyphBuffer.clear();
  _glyphBuffer.setUtf8Text(text);

  if (blFontShape(&font, &_glyphBuffer) == BL_SUCCESS) {
    BLTextMetrics m;
    blFontGetTextMetrics(&font, &_glyphBuffer, &m);

    int w = int(m.advance.x);
    // printf("TEXT_WIDTH {%s} [Width=%d]\n", text, w);
    return int(w);
  }
  else {
    return 0;
  }
}

void BLHtmlContainer::draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) {
  // printf("DRAW_TEXT POS{%d %d %d %d} RGBA{%d %d %d %d} Text{%s}\n", pos.x, pos.y, pos.width, pos.height, color.red, color.green, color.blue, color.alpha, text);
  BLContextCore ctx { (BLContextImpl*)hdc };
  BLFontCore font { (BLFontImpl*)hFont };

  int ascent = int(blDownCast(font).metrics().ascent);

  blDownCast(ctx).setFillStyle(BLRgba32(color.red, color.green, color.blue, color.alpha));
  blDownCast(ctx).fillUtf8Text(BLPoint(pos.x, pos.y + ascent), font, text);
}

int BLHtmlContainer::pt_to_px(int pt) {
  return (int) ((double) pt * 1.3333333333);
}

int BLHtmlContainer::get_default_font_size() const {
  return 14;
}

const litehtml::tchar_t* BLHtmlContainer::get_default_font_name() const {
  return "Arial";
}

void BLHtmlContainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) {
  // printf("DRAW_LIST_MARKER\n");
}

void BLHtmlContainer::load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) {
  std::string s;
  s.append(_baseUrl.data(), _baseUrl.size());
  s.append(1, '/');
  s.append(src);

  BLImage img;
  if (img.readFromFile(s.c_str()) == BL_SUCCESS) {
    _images[src] = img;
  }

  // printf("LOAD_IMAGE\n");
}

void BLHtmlContainer::get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) {
  auto it = _images.find(src);
  if (it != _images.end()) {
    sz.width = it->second.width();
    sz.height = it->second.height();
  }
  else {
    sz.width = 0;
    sz.height = 0;
  }
}

void BLHtmlContainer::draw_image(litehtml::uint_ptr hdc, const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, const litehtml::position& pos) {
  auto it = _images.find(src);
  if (it != _images.end()) {
    BLContextCore ctx { (BLContextImpl*)hdc };
    blDownCast(ctx).blitImage(BLRectI(pos.x, pos.y, pos.width, pos.height), it->second);
  }
  else {
  }
}

void BLHtmlContainer::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) {
  BLContextCore ctx { (BLContextImpl*)hdc };
  BLRectI rect(bg.clip_box.x, bg.clip_box.y, bg.clip_box.width, bg.clip_box.height);

  if (!bg.image.empty()) {
    auto it = _images.find(bg.image);
    if (it != _images.end()) {
      BLPattern p;
      p.setImage(it->second);
      p.setMatrix(BLMatrix2D::makeTranslation(rect.x, rect.y));
      p.scale(double(bg.image_size.width) / it->second.width(), double(bg.image_size.height) / it->second.height());

      switch (bg.repeat) {
        case litehtml::background_repeat_repeat   : p.setExtendMode(BL_EXTEND_MODE_REPEAT); break;
        case litehtml::background_repeat_repeat_x : p.setExtendMode(BL_EXTEND_MODE_REPEAT_X_PAD_Y); break;
        case litehtml::background_repeat_repeat_y : p.setExtendMode(BL_EXTEND_MODE_PAD_X_REPEAT_Y); break;
        case litehtml::background_repeat_no_repeat: p.setExtendMode(BL_EXTEND_MODE_PAD); break;
      }

      blDownCast(ctx).setFillStyle(p);
      blDownCast(ctx).fillRect(rect);
      return;
    }
  }

  blDownCast(ctx).setFillStyle(BLRgba32(bg.color.red, bg.color.green, bg.color.blue, bg.color.alpha));
  blDownCast(ctx).fillRect(rect);
}

void BLHtmlContainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) {
  BLContextCore ctx { (BLContextImpl*)hdc };

  int w = borders.left.width;
  litehtml::web_color c = borders.left.color;

  if (w == borders.right.width && w == borders.top.width && w == borders.bottom.width) {
    if (c == borders.right.color && c == borders.top.color && c == borders.bottom.color) {
      if (w == 1) {
        blDownCast(ctx).setStrokeStyle(BLRgba32(c.red, c.green, c.blue, c.alpha));
        blDownCast(ctx).strokeRect(draw_pos.x + 0.5, draw_pos.y + 0.5, draw_pos.width - 1.0, draw_pos.height - 1.0);
      }
    }
  }
}

void BLHtmlContainer::transform_text(litehtml::tstring& text, litehtml::text_transform tt) {
  // TODO: TRANSFORM TEXT.
}

void BLHtmlContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) {
}

void BLHtmlContainer::del_clip() {
}

void BLHtmlContainer::get_client_rect(litehtml::position& client) const {
  client.x = 0;
  client.y = 0;
  client.width = _size.w;
  client.height = _size.h;
}

std::shared_ptr<litehtml::element> BLHtmlContainer::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc) {
  return std::shared_ptr<litehtml::element>();
}

void BLHtmlContainer::set_caption(const litehtml::tchar_t* caption) {}
void BLHtmlContainer::set_base_url(const litehtml::tchar_t* base_url) {}
void BLHtmlContainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {}
void BLHtmlContainer::on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) {}
void BLHtmlContainer::set_cursor(const litehtml::tchar_t* cursor) {}

void BLHtmlContainer::import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl) {
  BLString s;
  s.append(_baseUrl);
  s.append('/');
  s.append(url.c_str());

  BLArray<uint8_t> content;
  BLFileSystem::readFile(s.data(), content);

  text.assign((const char*)content.data(), content.size());
}

void BLHtmlContainer::get_media_features(litehtml::media_features& media) const {
  media.type = litehtml::media_type_screen;
  media.width = _size.w;
  media.height = _size.h;
  media.device_width = _size.w;
  media.device_height = _size.h;
  media.color = 8;
  media.monochrome = 0;
  media.color_index = 256;
  media.resolution = 96;
}

void BLHtmlContainer::get_language(litehtml::tstring& language, litehtml::tstring & culture) const {}

litehtml::tstring BLHtmlContainer::resolve_color(const litehtml::tstring& color) const {
  return "";
}
