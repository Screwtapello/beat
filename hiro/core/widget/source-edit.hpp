#if defined(Hiro_SourceEdit)
struct mSourceEdit : mWidget {
  Declare(SourceEdit)

  auto doChange() const -> void;
  auto doMove() const -> void;
  auto editable() const -> bool;
  auto language() const -> string;
  auto numbered() const -> bool;
  auto onChange(const function<void ()>& callback = {}) -> type&;
  auto onMove(const function<void ()>& callback = {}) -> type&;
  auto scheme() const -> string;
  auto setEditable(bool editable) -> type&;
  auto setLanguage(const string& language = "") -> type&;
  auto setNumbered(bool numbered = true) -> type&;
  auto setScheme(const string& scheme = "") -> type&;
  auto setText(const string& text = "") -> type&;
  auto setTextCursor(TextCursor textCursor = {}) -> type&;
  auto setWordWrap(bool wordWrap = true) -> type&;
  auto text() const -> string;
  auto textCursor() const -> TextCursor;
  auto wordWrap() const -> bool;

//private:
  struct State {
    bool editable = true;
    string language;
    bool numbered = true;
    function<void ()> onChange;
    function<void ()> onMove;
    string scheme;
    string text;
    TextCursor textCursor;
    bool wordWrap = true;
  } state;
};
#endif
