#ifndef BLHTMLCONTAINER_H_INCLUDED
#define BLHTMLCONTAINER_H_INCLUDED

#include <blend2d.h>
#include <litehtml.h>
#include <stdio.h>

#include <string>
#include <unordered_map>

class BLHtmlContainer : public litehtml::document_container
{
public:
  BLFontManager _fontManager;
  BLGlyphBuffer _glyphBuffer;
  BLSizeI _size {};
  BLString _baseUrl;

  std::unordered_map<std::string, BLImage> _images;

  BLHtmlContainer();
  virtual ~BLHtmlContainer();

  void setSize(const BLSizeI& size) { _size = size; }
  void setFontManager(const BLFontManager& fm) { _fontManager = fm; }

  virtual litehtml::uint_ptr create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;
  virtual void delete_font(litehtml::uint_ptr hFont) override;
  virtual int text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont) override;
  virtual void draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) override;

  virtual int pt_to_px(int pt) override;
  virtual int get_default_font_size() const override;
  virtual const litehtml::tchar_t* get_default_font_name() const override;
  virtual void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override;
  virtual void load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready) override;
  virtual void get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz) override;
  virtual void draw_image(litehtml::uint_ptr hdc, const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, const litehtml::position& pos);
  virtual void draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg) override;
  virtual void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;

  virtual void transform_text(litehtml::tstring& text, litehtml::text_transform tt) override;
  virtual void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y) override;
  virtual void del_clip() override;
  virtual void get_client_rect(litehtml::position& client) const override;

  virtual std::shared_ptr<litehtml::element> create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, const std::shared_ptr<litehtml::document>& doc) override;

  virtual void set_caption(const litehtml::tchar_t* caption) override;
  virtual void set_base_url(const litehtml::tchar_t* base_url) override;
  virtual void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
  virtual void on_anchor_click(const litehtml::tchar_t* url, const litehtml::element::ptr& el) override;
  virtual void set_cursor(const litehtml::tchar_t* cursor) override;
  virtual void import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl) override;

  virtual void get_media_features(litehtml::media_features& media) const override;
  virtual void get_language(litehtml::tstring& language, litehtml::tstring & culture) const override;
  virtual litehtml::tstring resolve_color(const litehtml::tstring& color) const override;
};

#endif // BLHTMLCONTAINER_H_INCLUDED