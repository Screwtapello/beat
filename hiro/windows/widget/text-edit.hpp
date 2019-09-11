#if defined(Hiro_TextEdit)

namespace hiro {

struct pTextEdit : pWidget {
  Declare(TextEdit, Widget)

  auto setBackgroundColor(Color color) -> void;
  auto setEditable(bool editable) -> void;
  auto setForegroundColor(Color color) -> void;
  auto setText(string text) -> void;
  auto setTextCursor(TextCursor textCursor) -> void;
  auto setWordWrap(bool wordWrap) -> void;
  auto text() const -> string;
  auto textCursor() const -> TextCursor;

  auto onChange() -> void;
  auto windowProc(HWND, UINT, WPARAM, LPARAM) -> maybe<LRESULT> override;

  HBRUSH backgroundBrush = nullptr;
};

}

#endif
